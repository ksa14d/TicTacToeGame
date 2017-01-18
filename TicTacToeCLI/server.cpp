#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <vector>
#include <ctype.h>
#include <map>
#include <exception>
using namespace std;

pthread_mutex_t mtx_users = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t size_threshold_cv;
const unsigned MAXBUFLEN = 10240;
FILE *fp;

class Player;
class Game;
map<string,string> credentials;
int n_scmds =22 ;
string singlecommand[] =
  { "who", "stats", "game", "unobserve", "resign", "refresh", "quiet","A1", "A2", "A3", "B1", "B2", "B3", "C1", "C2", "C3",
      "nonquiet", "listmail", "exit", "quit", "help", "?" };

int n_dcmds =12 ;
string doublecommand[] = 
  { "stats", "match", "observe", "block", "unblock", "readmail",
      "deletemail",  "passwd" };

int n_tcmds =4 ;
string threecommand[] =
  { "match", "register" };

int n_fcmds =1 ;
string fourcommand[] =
  { "match" };

string Get_banner ()
{
      string s;
      s +=  "Commands supported:\n";
      s+= "who                     # List all online users\n";
      s+= "stats [name]            # Display user information\n";
      s+= "game                    # list all current games\n";
      s+= "observe <game_num>      # Observe a game\n";
      s+= "unobserve               # Unobserve a game\n";
      s+= "match <name> <b|w> [t]  # Try to start a game\n";
      s+= "<A|B|C><1|2|3>          # Make a move in a game\n";
      s+= "resign                  # Resign a game\n";
      s+= "refresh                 # Refresh a game\n";
      s+= "shout <msg>             # shout <msg> to every one online\n";
      s+= "tell <name> <msg>       # tell user <name> message\n";
      s+= "kibitz <msg>            # Comment on a game when observing\n";
      s+= "' <msg>                 # Comment on a game\n";
      s+= "quiet                   # Quiet mode, no broadcast messages\n";
      s+= "nonquiet                # Non-quiet mode\n";
      s+= "block <id>              # No more communication from <id>\n";
      s+= "unblock <id>            # Allow communication from <id>\n";
      s+= "listmail                # List the header of the mails\n";
      s+= "readmail <msg_num>      # Read the particular mail\n";
      s+= "deletemail <msg_num>    # Delete the particular mail\n";
      s+= "mail <id> <title>       # Send id a mail\n";
      s+= "info <msg>              # change your information to <msg>\n";
      s+= "passwd <new>            # change password\n";
      s+= "exit                    # quit the system\n";
      s+= "quit                    # quit the system\n";
      s+= "help                    # print this message\n";
      s+= "?                       # print this message\n";
      return s;
}

vector<Player*> players;
vector<Game*> games ;
vector<Player> playersToUpdate;

void printplayers();

class Mail
{
  public:
  bool IsRead;
  string Sender;
  string Header;
  string SentTime; 
  string Content;
  Mail()
  { 
      IsRead = false;
      time_t now;
      struct tm *timeinfo;
      time(&now);
      timeinfo = localtime(&now);
      SentTime = asctime(timeinfo);
  }
 ~Mail() {}
};



class Player
{

private:

  ssize_t n;
  char buf[MAXBUFLEN];
  string storeclient[10];
  int n_storeclient;
  int counter ;
  vector<Game*> ObservedGames;
  void Move();
  void RemoveGame();
  void WriteAllGames();
  void Refresh();
  void Resign();
  void Observe(int);
  void UnObserve();
  void UnObserve(int);
  bool CheckBlocked(Player *p);
  void Kibitz(string);
  bool ValidateCommand (const char *Cmd)
  {
    int i, j = 0;
    char *token;
    char Command[500];
    n_storeclient = 0;

    strcpy (Command, Cmd);
    token = strtok (Command, " ");

    if(token == NULL)return false;
    if(strcmp(token,"shout")==0||strcmp(token,"info")==0||strcmp(token,"\'")==0||strcmp(token,"kibitz")==0)
    {
       storeclient[0]= token;
       char *s = strtok(NULL,"\n");
       if(s!=NULL)storeclient[1]=s;
       else storeclient[1] = " "; 
       return true;
    }
  
    if(strcmp(token,"tell")==0||strcmp(token,"mail")==0)
    {
       storeclient[0]= token;
       char *s = strtok(NULL," ");
       if(s!=NULL)storeclient[1]=s;
       else return false; 
       s = strtok(NULL,"\n");
       if(s!=NULL)storeclient[2]=s;
       else storeclient[2] = " "; 
       return true;
    }

    while (token != NULL)
      {
        storeclient[j] = token;
        j++;
        token = strtok (NULL, " ");
      }
    n_storeclient = j;
    cout<<"store"<<n_storeclient<<endl; 
    if (j == 1)
      {
        for (i = 0; i < n_scmds ; i++)
  	{
  	  if (singlecommand[i].compare(storeclient[0])==0 )
  	    {
	      if((storeclient[0].length()) == 2)
		{
		   storeclient[1] = storeclient[0];
		   storeclient[0] = "move";
		}
  	      return true;
  	    }
  	}
        return false;
      }

    if (j == 2)
      {
        for (i = 0; i < n_dcmds; i++)
  	{
  	  if (doublecommand[i].compare(storeclient[0])==0)
  	    {
  	      return true;
  	    }
  	}
        return false;
      }

    if (j == 3)
      {
        for (i = 0; i < n_tcmds; i++)
  	{
  	  if (threecommand[i].compare(storeclient[0]) ==0)
  	    {
  	      return true;
  	    }
  	}
        return false;
      }

    if (j == 4)
      {
        for (i = 0; i < n_fcmds; i++)
  	{
  	  if (fourcommand[i].compare(storeclient[0]) ==0)
  	    {
  	      return true;
  	    }
  	}
        return false;
      }
    return false;
  }
  bool quit()
  {
    if(game != NULL)Resign();
    close(sockfd);
    IsOnline = false;
    counter = 0;
   pthread_mutex_lock( &mtx_users );
    for(unsigned i = 0 ; i < playersToUpdate.size() ; i++)
    {
        Player p =  playersToUpdate[i];
        if(p.username == username)
        {
          playersToUpdate.erase(playersToUpdate.begin()+i);
          break;
        }
    }
    playersToUpdate.push_back(*this); 
   pthread_cond_signal( &size_threshold_cv);
   pthread_mutex_unlock( &mtx_users );
    pthread_exit(0);
  }
public:
  string username;
  string invite;
  int sockfd;
  string password;
  double Rating;
  vector<Mail*> emails ;
  int Wins,Losses;
  string Info;
  bool IsQuiet;
  vector<string> BlockedUsers;
  int time;
  bool IsOnline;
  char color;
  void CalcRating()
  {
     if((Wins+Losses)!=0)
         Rating = ((Wins)/(Wins+Losses));
      else
         Rating = 0;
      Rating *= 5;
  }
  Game* game;
  Player (int sid)
  {
    sockfd = sid;
    game =NULL;
    Wins = Losses= Rating = 0;
    time = 600;
    IsOnline = true;
    color = 'b';
    invite = "";
    counter = 0;
    Info = "<none>";
    IsQuiet = false;
   
  }

  ~Player ()
  {

  }
  void WriteString(string str)
  {
    sprintf(buf,"%s",str.c_str());
    ssize_t sz = write (sockfd, buf, strlen (buf));
    if(sz == -1)
    {
      perror("write error\n");
    }
  }
  
void WriteStringPrompt(string str)
  {
    char prompt[20];
    sprintf(buf,"%s",str.c_str());
    ssize_t sz = write (sockfd, buf, strlen (buf));
    if(sz == -1)
    {
      perror("write error\n");
    }
     sprintf(prompt,"\n<%s: %d>",username.c_str(),counter++);
     WriteString(prompt);
  }
 
  void SendMail(string sendto,string subject)
  {
     Player *receiver = GetPlayerByName(sendto);
     if(receiver ==NULL)
     {
        WriteStringPrompt(sendto.append(" is not a user\n"));
        return;
     }
     if(CheckBlocked(receiver))
     {
        WriteStringPrompt(sendto.append(" has blocked you. cannot email\n"));
        return;
     }
     Mail* message = new Mail();
     message->Sender = username;
     message->Header = subject;
     WriteString("\nPlease input mail body,finishing with \'.\' at the beginning of a line\n\n");
    while ((n = read (sockfd, buf, MAXBUFLEN)) > 0)
        {
	 
          buf[n] = '\0';
          if(buf[0] == '.')break;
          message->Content.append(buf);
        }
       receiver->emails.push_back(message);
       WriteStringPrompt("Mail Sent Successfully\n"); 	   
       if(receiver->IsOnline)
       {
         receiver->WriteStringPrompt("A new message just arrived\n");
       }
  }

  void ListMail()
  {
    char temp[100];
    WriteString("Your messages : \n");
    for(unsigned i = 0 ; i < emails.size() ; i++)
    {
       Mail *m = emails[i];
       string flg = "N";
       if(m->IsRead)
       {
          flg = " ";
       }
       sprintf(temp,"%d\t%s\t%s\t\"%s\"\t%s\n",i,flg.c_str(),m->Sender.c_str(),m->Header.c_str(),m->SentTime.c_str());
       WriteString(temp);
    }
    WriteStringPrompt("\n");
  }
  
  void ReadMail(unsigned mnum)
  {
     if(mnum >= emails.size())
     {
        WriteStringPrompt("Message Number invalid\n");
        return;
     } 
     Mail *m = emails[mnum];
     m->IsRead = true;
     char temp[200];
     sprintf(temp,"\nFrom: %s\nTitle: %s\nTime: %s\n\n",m->Sender.c_str(),m->Header.c_str(),m->SentTime.c_str());
     WriteString(temp);
     WriteStringPrompt(m->Content);
  }

 
  void DeleteMail(unsigned mnum)
  {
     if(mnum >= emails.size())
     {
        WriteStringPrompt("Message Number invalid\n");
        return;
     }
     Mail *m = emails[mnum];
     emails.erase(emails.begin() + mnum);
     WriteStringPrompt("Message deleted successfully\n");
     delete m;
  } 

  void ProcessCommands()
  {
    int n ;
    string s;
         s = Get_banner();
         int unreadcount = 0;
         for(unsigned i = 0; i < emails.size();i++)
         {
            if(!emails[i]->IsRead)
            {
               unreadcount++;
            }
         }
         if(unreadcount == 0)
         {
           s.append("you have no unread messages\n");
         }
         else
         {
           s.append("you have ");
           s.append(to_string(unreadcount));
           s.append(" unread messages\n");
         }
         WriteStringPrompt(s);
    while ((n = read (sockfd, buf, MAXBUFLEN)) > 0)
        {
	  n = n -2;
          buf[n] = '\0';
          bool Verify = ValidateCommand (buf);
          cout<<Verify<<"verify";
          cout<<"In Process Commands"<<buf<<endl;
          cout<<"in while username "<<username <<" sock "<<sockfd<< endl ;
          printplayers();
                  cout<<"sock in after print players"<<sockfd<<endl;
        //   cout<<storeclient[0]<<" "<<storeclient[1]<<endl;
          //if else for all commands
          
          if(Verify)
            {
              //process
              if(storeclient[0].compare("who") == 0)
        	{
        	    WriteAllPlayers();
        	}
              else if(storeclient[0].compare("stats") == 0)
        	{
                  cout<<"sock in stats"<<sockfd<<endl;
        	  if(n_storeclient == 1)
        	    {
        	       PrintStats("");
        	    }
        	  else
        	    {
        	      PrintStats(storeclient[1]);
        	    }

        	}
              else if(storeclient[0].compare("game") == 0)
                      	{
        	             WriteAllGames();
                      	}
              else if(storeclient[0].compare("observe") == 0)
                      	{
                             Observe(atoi(storeclient[1].c_str()));                     	  
                      	}
              else if(storeclient[0].compare("unobserve") == 0)
                      	{
                      	     UnObserve();
                      	}
              else if(storeclient[0].compare("match") == 0)
                      	{
                             if(game != NULL )
                             {
                                 WriteStringPrompt("Please finish a match before starting a new one\n");
                                 continue;
                             }
                             char *colr = NULL;
			     if(n_storeclient == 3)                           
                             colr = &(storeclient[2])[0];
                             try
                             {
                                 if(n_storeclient == 4)
                                 {
                                     time  =atoi(storeclient[3].c_str());
                                     colr = &(storeclient[2])[0];
                                 }
                                 BeginGame(storeclient[1],colr);   
                             }
                             catch(exception &e)
                             {
                                WriteStringPrompt("invalid timing\n");
                             }
                      	}
	      else if(storeclient[0].compare("move") == 0)
			{
			     Move();
			}
              else if(storeclient[0].compare("resign") == 0)
                      	{
                             Resign();
                      	}
              else if(storeclient[0].compare("refresh") == 0)
                      	{
                             Refresh();
                      	}
              else if(storeclient[0].compare("shout") == 0)
                      	{
                      	     Shout(storeclient[1]);
                      	}
              else if(storeclient[0].compare("tell") == 0)
                      	{
                      	     Tell(storeclient[2],storeclient[1]);
                      	}
              else if(storeclient[0].compare("kibitz") == 0)
                      	{
                      	    Kibitz(storeclient[1]);
                      	}
              else if(storeclient[0].compare("\'") == 0)
                      	{
                      	    Kibitz(storeclient[1]);
                      	}
              else if(storeclient[0].compare("quiet") == 0)
                      	{
                             IsQuiet = true;
			     WriteStringPrompt("User in quiet mode\n"); 	   
                      	}
              else if(storeclient[0].compare("nonquiet") == 0)
                      	{
                      	     IsQuiet = false;
			     WriteStringPrompt("User in non-quiet mode\n"); 	   
                      	}
              else if(storeclient[0].compare("block") == 0)
                      	{
                            if(username.compare(storeclient[1])==0)
                             {
                               WriteStringPrompt("You cannot block your self\n");
                             }
                             else 
                             {
                                char temp[50];
                                Player* p = GetPlayerByName(storeclient[1]);
                                if(p!=NULL)
                                {
                                    sprintf(temp,"user %s blocked.\n",p->username.c_str());
                                    BlockedUsers.push_back(string(storeclient[1].c_str()));        
                                }
                                else
                                {
                                    sprintf(temp,"user %s does not exist.\n",p->username.c_str());
                                }
                                WriteStringPrompt(temp);
                                 
                             }      	  
                      	}
              else if(storeclient[0].compare("unblock") == 0)
                      	{
                                char temp[50];
                                bool IsUserBlocked = true;
                                for(unsigned i = 0 ; i< BlockedUsers.size() ; i++) 
                                {
   			           if(BlockedUsers[i].compare(storeclient[1])==0)
                                   {
                                       sprintf(temp,"user %s unblocked.\n",BlockedUsers[i].c_str());
                                       IsUserBlocked = false;
                                       BlockedUsers.erase(BlockedUsers.begin()+ i);
                                       WriteStringPrompt(temp);
                                   }
                                }
                                if(IsUserBlocked)
                                {
                                    sprintf(temp,"user %s was never Blocked.\n",storeclient[1].c_str());
                                    WriteStringPrompt(temp);
                                }
                      	}
              else if(storeclient[0].compare("listmail") == 0)
                      	{
                           ListMail();
                      	}
              else if(storeclient[0].compare("readmail") == 0)
                      	{
                      	    ReadMail(atoi(storeclient[1].c_str()));
                      	}
              else if(storeclient[0].compare("deletemail") == 0)
                      	{
                      	    DeleteMail(atoi(storeclient[1].c_str()));
                      	}
              else if(storeclient[0].compare("mail") == 0)
                      	{
                           SendMail(storeclient[1],storeclient[2]);
                      	}
              else if(storeclient[0].compare("info") == 0)
                      	{
                            Info = storeclient[1];
			    WriteStringPrompt("Info Updated Successfully\n"); 	   
                      	}
              else if(storeclient[0].compare("passwd") == 0)
                      	{
                          cout<<"Password" <<storeclient[1]<<endl;
			  credentials[username] = storeclient[1];
                          password = storeclient[1];
                          WriteStringPrompt("Password updated successfully\n");
                      	}
              else if(storeclient[0].compare("exit") == 0)
                      	{
        		  quit();
                      	}
              else if(storeclient[0].compare("quit") == 0)
                      	{
                      	  quit();
                      	}
              else if(storeclient[0].compare("help") == 0)
                      	{
                           string helpBanner = Get_banner();
                           WriteStringPrompt(helpBanner);                      	  
                      	}
              else if(storeclient[0].compare("?") == 0)
                      	{
                           string helpBanner = Get_banner();
                           WriteStringPrompt(helpBanner);                      	  
                      	}
              else if(storeclient[0].compare("register") == 0)
                      	{
                            WriteStringPrompt("please use guest login to register new users\n");
                        }
            }
            else
            {
               WriteStringPrompt("command not valid\n");
            }
          //nbytes = write (sockfd, buf, strlen (buf));
          //if(nbytes == -1) perror("Write Failed");
        }
      if (n == 0)
        {
          cout << "client closed" << endl;
           quit(); 
        }
      else
        {
          cout << "client terminated" << endl;
            quit();
        }
  }
 
  void Shout(string message)
  {
    char temp[100];
    sprintf(temp,"!shout! *%s* : %s\n",username.c_str(),message.c_str());
    for(unsigned i = 0 ;i <players.size() ; i++ )
    {
       if(players[i]->IsOnline && !players[i]->IsQuiet)   // check block
       {
           bool IsBlocked = CheckBlocked(players[i]);
           if(!IsBlocked)
             players[i]->WriteStringPrompt(temp);
       }
    }
  }
  
  void Tell(string message, string name)
  {
    char temp[100];
    Player* p = GetPlayerByName(name);
    if(p == NULL)
    {
       sprintf(temp,"user %s does not exist.\n",name.c_str());
       WriteStringPrompt(temp);
       return;
    }
    bool IsBlocked = CheckBlocked(p);
    if(IsBlocked)
    {
       sprintf(temp,"you cannot talk to %s. you are Blocked\n",name.c_str());
       WriteStringPrompt(temp);
       return;
    }
    sprintf(temp,"%s : %s\n",username.c_str(),message.c_str());
    if( p->IsOnline)
    {
      p->WriteStringPrompt(temp);
    }
    else 
    {
       sprintf(temp,"user %s is not on line\n",p->username.c_str());
       WriteStringPrompt(temp);
      
    }
    WriteStringPrompt("");
  }
 
  
 
  bool Authenticate ()
  {
    ssize_t nbytes;
    string s;
    strcpy (buf, "username (guest):");
    nbytes = write (sockfd, buf, strlen (buf));
    if(nbytes == -1) perror("Write Failed");
    cout<<"I am before read";
    if ((n = read (sockfd, buf, MAXBUFLEN)) > 0)
      {
        
    cout<<"I am after read";
	n = n - 2;
	buf[n] = '\0';
        cout<<"In Authenticate"<<buf<<"@@"<<endl;
	if(strlen(buf) == 0)
	  {
	    s = Get_banner();
	    s += "You login as a guest. The only command that you can use is\n'register username password'\n";
	    WriteStringPrompt(s);
	    while ((n = read (sockfd, buf, MAXBUFLEN)) > 0)
	      {
		n = n-2;
		buf[n] = '\0';
                cout<<"In Authenticate i "<<buf<<"@@"<<endl;
		string Command = buf;
		if(Command.find("register") == 0)
		  {

		    bool Verify = ValidateCommand (Command.c_str ());
		    if(Verify)
		      {
			//store into database
			username = storeclient[1];
			password = storeclient[2];
                        if(credentials.find(username)!= credentials.end())
                        {
			   WriteStringPrompt("User is already registered");
                           continue;
                        }
			credentials[username] = password;
			IsOnline = true;
			s = "User Registered\n";
			WriteStringPrompt(s);
                        printplayers();  
			players.push_back(this);
                        printplayers();
                  
		      }
		    else
		      {
			s = "You are not supposed to do this.\nYou can only use 'register username password' as a guest.\n";
			WriteStringPrompt(s);
		      }
		  }
		else if(Command.compare("quit") == 0)
		  {
		    s ="Thank you for using Online Tic-tac-toe Server.\nSee you next time.\n";
		    WriteStringPrompt(s);
                    BrokenPipe();
		  }
		else
		  {
		    s = "You are not supposed to do this.\nYou can only use 'register username password' as a guest.\n";
		    WriteStringPrompt(s);
		  }
	      }
              if (n == 0)
              {
                 cout << "client closed" << endl;
                 BrokenPipe();
              }
              else
              {
                 cout << "client terminated" << endl;
                 BrokenPipe();
              }
	  }
	username = buf;
      }
    
      else if (n == 0)
       {
          cout << "client closed" << endl;
          BrokenPipe();
       }
       else
        {
            cout << "client terminated" << endl;
            BrokenPipe();
        }
    strcpy (buf, "password:");
    WriteString(string(buf));
    if ((n = read (sockfd, buf, MAXBUFLEN)) > 0)
      {
	n = n-2;
	buf[n] = '\0';
	password = buf;
      }
      else if (n == 0)
       {
          cout << "client closed" << endl;
          BrokenPipe();
       }
       else
       {
          cout << "client terminated wrong" << endl;
          BrokenPipe();
       }
      cout<<"Auth:@@"<<username<<"@@"<<password<<"@@"<<credentials[username]<<"@@"<<endl;
    if(password.length() == 0)
    {
	strcpy (buf, "Authentication failed");
	nbytes = write (sockfd, buf, strlen (buf));
        BrokenPipe();
    }
    if(credentials[username].compare(password) != 0 )
    {
	strcpy (buf, "Authentication failed");
	nbytes = write (sockfd, buf, strlen (buf));
        BrokenPipe();
    }
    return true;
  }
 
  Player* GetPlayerByName(string name)
  {
    
      for(vector<Player*>::iterator it = players.begin(); it != players.end(); ++it)
      {
	   if((*it)->username.compare(name) == 0)
           {
              return *it;
           }
      }
      return NULL;
  }
  
  void BrokenPipe()
  {
      IsOnline = false;
      counter = 0;
      close(sockfd);
      pthread_exit(0);
  }
  void BeginGame(string name,char* colr);
 
  void WriteAllPlayers()
   {
  	 //create mutex
      string onlineList ;
      int onlineCount = 0;
      for(vector<Player*>::iterator it = players.begin(); it != players.end(); ++it)
      {
        if((*it)->IsOnline)
	{
           onlineCount++;
	   onlineList += (*it)->username;
           onlineList += " ";
	}
      }
      string s = string("Total ")+ to_string(onlineCount) +string(" user(s) online :\n") ;
      s += onlineList ;
      WriteStringPrompt(s);
      //create mutex
   }
  void PrintStats(string user)
  {
    Player *p = NULL;
    string s;
    char frame[100];
    if(user.compare("") == 0)
      {
	p = this;
      }
    else
      {
         p = GetPlayerByName(user);
         if(p == NULL)
         {
            string temp = "user ";
            temp.append(user);
            temp.append(" does not exist\n");
            WriteStringPrompt(temp);
            return;
         }
      }
  sprintf(frame,"\nUser: %s\nInfo:%s\nRating:%lf\nWins:%d, Loses:%d\n",p->username.c_str(),p->Info.c_str(),p->Rating,
	p->Wins, p->Losses);
  s.append(frame);
  if(p->IsQuiet)
    {
      s.append("Quiet: Yes\nBlocked Users: ");
    }
  else
    {
      s.append("Quiet: No\nBlocked Users: ");
    }
  if(p->BlockedUsers.size() == 0)
    {
      s += string("<none>\n");
    }
  for(vector<string>::iterator it = p->BlockedUsers.begin(); it != p->BlockedUsers.end(); it++) {
		s.append(*it);
		s.append(" ");
  	      }
  s.append("\n\n");
  s.append(p->username);
  if(p->IsOnline)
    {
      s.append(" is on-line\n");
    }
  else
    {
      s.append(" is off-line\n");
    }
  WriteStringPrompt(s);
}
};

class Game
{

private:
    
    char alternate;
  map<char,int> moves ;
public:
  int game_num;
  time_t GameStartTime ;
  vector<Player*> Observers;
  void Refresh(Player* P)
  {
    char temp[100];
    string s;
    sprintf(temp,"\nBlack:\t%s\tWhite:\t%s\nTime:\t%d seconds\tTime:\t%d seconds\n",Player2->username.c_str(),Player1->username.c_str(),Player2->time,Player1->time);    
    s = temp;
    for(int i =0 ; i < 4 ; i++)
    {
      for(int j = 0 ; j <  4 ; j++)
      {
        sprintf(temp,"%c ",Board[i][j]);
        s.append(temp);
      }
      s+= "\n";
    }
    P->WriteStringPrompt(s);
  }
  
  void RefreshObservers()
  {
    char temp[100];
    string s;
    sprintf(temp,"\nBlack:\t%s\tWhite:\t%s\nTime:\t%d seconds\tTime:\t%d seconds\n",Player2->username.c_str(),Player1->username.c_str(),Player2->time,Player1->time);    
    s = temp;
    for(int i =0 ; i < 4 ; i++)
    {
      for(int j = 0 ; j <  4 ; j++)
      {
        sprintf(temp,"%c ",Board[i][j]);
        s.append(temp);
      }
      s+= "\n";
    }
    for(unsigned i = 0 ; i< Observers.size() ; i++)
    {
       Observers[i]->WriteStringPrompt(s);
    }
  }
  bool IsGameOver = false;
  bool IsDrawMatch = false;
  int  steps;
  string winner;
  Player *Player1;
  Player *Player2;
  Game (Player *p1, Player *p2)
  {
    IsGameOver = false;
    time(&GameStartTime);
    steps = 0;
    winner = "";
    moves['A'] = 1;
    moves['B'] = 2;
    moves['C'] = 3;
    Player1 = p1;
    Player2 = p2;
    alternate = 'b';
    Board[0][0] = ' ';
    for(int i =1 ; i < 4 ; i++)
    {
      sprintf((*(Board+0)+1),"%s","123");
      for(int j = 1 ; j <  4 ; j++)
      {
         Board[i][j] = '.';
      }
    }
    Board[1][0] = 'A',Board[2][0] = 'B', Board[3][0] = 'C';
  }
  ~Game ()
  {
  }
  char Board[4][4];
  
  void Publish()
  {  
     time_t currentTime;
     time(&currentTime);
     double elapsedTime =  difftime(currentTime,GameStartTime);
     GameStartTime += elapsedTime;
     int eTime = (int)elapsedTime;
     if(Player1->color!= alternate)
        Player1->time -= eTime;
     else
        Player2->time -= eTime;
     if(Player1->time < 0)
     {
       Player1->time = 0;
       DeclareWinner(Player1);
       string temp = Player1->username;
       temp+=" lost by time.\n";
       Player1->WriteStringPrompt(temp);
       Player2->WriteStringPrompt(temp);
       for(unsigned i= 0;  i< Observers.size() ; i++)
       {
         Observers[i]->WriteStringPrompt(temp);
       } 
     }
     
     if(Player2->time < 0)
     {
       Player2->time = 0;
       DeclareWinner(Player2);
       string temp = Player2->username;
       temp+=" lost by time.\n";
       Player2->WriteStringPrompt(temp);
       Player1->WriteStringPrompt(temp);
       for(unsigned i= 0;  i< Observers.size() ; i++)
       {
         Observers[i]->WriteStringPrompt(temp);
       } 
     }
     Refresh(Player1);
     Refresh(Player2);
     RefreshObservers();
  }

  bool CheckWin()
  {
     steps++;
     int bwslash = 0, fwslash = 0;
     int bbslash = 0, fbslash = 0;
     for(int i = 1 ; i < 4 ; i++)
     {
       int rwcount = 0,cwcount = 0;
       int rbcount = 0,cbcount = 0;
       for(int j = 1; j < 4 ; j++)
       {
          if(Board[i][j] == '#')rwcount++;
          else if(Board[i][j] == 'o')rbcount++;
          if(Board[j][i] == '#')cwcount++;
          else if(Board[j][i] == 'o')cbcount++;
          if(i == j && Board[i][j] ==  '#') bwslash++;
          else if(i == j && Board[i][j] ==  'o') bbslash++;
          if(i + j == 4 && Board[i][j] == '#') fwslash++;
          else if(i + j == 4 && Board[i][j] == 'o') fbslash++;
       }
       if( rwcount == 3 || cwcount == 3 || bwslash == 3 ||  fwslash == 3)
       {
           if(Player1->color == 'w') 
           {
              winner = Player1->username;
           }
           else
           {
              winner = Player2->username;
           }  
           return true;
       }
       if( rbcount == 3 || cbcount == 3 || bbslash == 3 ||  fbslash == 3)
       {
           if(Player1->color == 'b') 
           {
              winner = Player1->username;
           }
           else
           {
              winner = Player2->username;
           }  
           return true;
       }
     }
     return false; 
  }

  void DeclareWinner(Player* p)
  {
     IsGameOver = true;
     if(p->username.compare(Player1->username)==0)
     {
        winner = Player2->username;
     }
     else
     {
        winner = Player1->username;
     }
     string temp = winner;
     temp.append(" won the game!!\n");
     Player1->WriteStringPrompt(temp);
     Player2->WriteStringPrompt(temp);
     for(unsigned i= 0;  i< Observers.size() ; i++)
     {
        Observers[i]->WriteStringPrompt(temp);
     }
     if(winner.compare(Player1->username)==0)
     {
          Player1->Wins++;
          Player2->Losses++;
          Player1->CalcRating();
          Player2->CalcRating();
     }
     else
     {
          Player2->Wins++;
          Player1->Losses++;    
          Player2->CalcRating();
          Player1->CalcRating();  
     }
     Player1->time = 600;
     Player2->time = 600;
  }
 
  bool MakeMove(string index,Player *p)
  {
    if(p->color != alternate)
    {
        p->WriteStringPrompt("it is not your turn.\n");
        return false;
    }
    char pawn = '#';
    cout<<pawn<<endl;
    int r = moves[index[0]];
    int c = atoi(&(index[1]));
    if(r < 1 || r > 3 || c < 1 || c > 3 )
    {
        p->WriteStringPrompt("invalid move.\n");
    }
    //cout<<"r :"<<r<<" c:"<<c<<"index[0]:"<<index[0]<<"index[1]:"<<index[1]<<endl;
    cout << "P->color :"<<p->username<<"color : "<<p->color<<endl;
    if(p->color == 'b')
    {
      pawn = 'o';
    }
    if(Board[r][c] == '.')
    {
        Board[r][c] = pawn;
    }
    else
    {
        string s = index;
        s.append(" is occupied\n");
        p->WriteStringPrompt(s);
        return false;
    }
     if(alternate =='b')alternate = 'w';
     else alternate = 'b';
     if(CheckWin())
     { 
       IsGameOver = true;
       
       if(winner.compare(Player1->username)==0)
        {
           Player1->Wins++;
           Player2->Losses++;      
           Player2->CalcRating();
           Player1->CalcRating();  
        }
        else
        {
            Player2->Wins++;
            Player1->Losses++;      
            Player2->CalcRating();
            Player1->CalcRating();  
        }
     }
    Publish();
    if(IsGameOver || steps ==9)
    {
           if(!IsGameOver && steps == 9)
           { 
               winner.append("Game is Tied");
               IsGameOver = true;
           }
           else
           {
              winner.append(" won the game!!\n");
           }
           for(unsigned i= 0;  i< Observers.size() ; i++)
           {
               Observers[i]->WriteStringPrompt(winner);
           }
           Player1->WriteStringPrompt(winner);
           Player2->WriteStringPrompt(winner);
           Player1->time = 600;
           Player2->time = 600;
           
    }
    return true; 
  }
};


Game* GetGameByNum(int game_num)
{
    for(unsigned i = 0 ; i < games.size(); i++)
    {
       if(games[i]->game_num == game_num)
       {
          return games[i];
       }
    }
    return NULL;
}

bool Player::CheckBlocked(Player *p)
{

    bool IsBlocked = false;
    for(unsigned i =0 ; i <p->BlockedUsers.size() ; i++)
    {
      if(p->BlockedUsers[i].compare(username)==0)
      {
         IsBlocked = true;
         break;
      }
    }
    return IsBlocked;
}

void Player::Kibitz(string message)
{
    if(ObservedGames.size() == 0)
    {
       WriteStringPrompt("You are not observing any game\n");
    }
    for(unsigned i = 0 ; i < ObservedGames.size(); i++)  //later based on prof input
    {
       for(unsigned j = 0; j < ObservedGames[i]->Observers.size() ; j++) // i->0
       {
            if(!CheckBlocked(ObservedGames[i]->Observers[j]))
            {
               ObservedGames[i]->Observers[j]->WriteStringPrompt(message);
            }
       }
    }
}

void Player::Observe(int game_num)
{
   Game* game = GetGameByNum(game_num);
   if(game == NULL)
   { 
     WriteStringPrompt("The game does not exist.\n");
     return;
   }
   bool IsObserving = false;
   for(unsigned i = 0; i< game->Observers.size() ; i++)
   {
       if(game->Observers[i]->username.compare(username)==0)
       {
           IsObserving = true;
           break;
       }
   }
   if(IsObserving)
   {
       WriteString("you are already observing this game\n");
       return;
   }
   game->Observers.push_back(this);
   game->Refresh(this);
   this->ObservedGames.push_back(game);
}

void Player::UnObserve(int game_num)  // -1
{
   Game* game = GetGameByNum(game_num);
   if(game == NULL)
   {
     WriteStringPrompt("The game does not exist.\n");
     return;
   }
   bool IsObserving = true;
   for(unsigned i = 0; i< game->Observers.size() ; i++)
   {
       if(game->Observers[i]->username.compare(username)==0)
       {
           IsObserving = false;
           game->Observers.erase(game->Observers.begin() + i);
           break;
       }
   }
   if(!IsObserving)
   {
    IsObserving = true;
    for(unsigned i = 0 ; i < ObservedGames.size(); i++)
    {
       if(ObservedGames[i]->game_num == game_num)
       {
           IsObserving = false;
           ObservedGames.erase(ObservedGames.begin() + i);
           break;
       }
    }
   }
   if(IsObserving)
   {
      WriteStringPrompt("You are not observing this game\n");
   }
}

void Player::UnObserve() 
{
    if(ObservedGames.size() > 0)
    {
       
       for(unsigned i = 0; i< ObservedGames[0]->Observers.size() ; i++)
       {
          if(ObservedGames[0]->Observers[i]->username.compare(username)==0)
          {
             ObservedGames[0]->Observers.erase(ObservedGames[0]->Observers.begin() + i);
             break;
          }
       }
       ObservedGames.erase(ObservedGames.begin());
    }
   else
   {
      WriteStringPrompt("You are not observing anything\n");
   }
}
  void Player::WriteAllGames()
  {
      string s = string("Total ")+ to_string(games.size()) +string(" Games(s) :\n") ;
       char temp[50];
            for(unsigned i = 0; i < games.size() ; i++ )
            {
               sprintf(temp,"Game %u(%u) : %s .vs. %s, %d moves\n ",games[i]->game_num,i,games[i]->Player1->username.c_str(),games[i]->Player2->username.c_str(),games[i]->steps);
           s.append(temp);
            }
      /*for(vector<Game*>::iterator it = games.begin(); it != games.end(); ++it)
      {
          //print game details here
      }*/
      WriteStringPrompt(s);
      //create mutex
  }
void Player::Move()
{
  
  if(game != NULL && !game->IsGameOver)
  {
   game->MakeMove(storeclient[1],this);
  }
  else
  {
        WriteStringPrompt("not a valid command.\n");
  }
 if(game!= NULL &&game->IsGameOver)
     RemoveGame();
}

void Player::RemoveGame()
{

          if(game != NULL && game->IsGameOver)
          { 
            for(unsigned i = 0; i < game->Observers.size() ; i++)
            {
                game->Observers[i]->UnObserve(game->game_num); // please game is over un observe
            }
          
            for(unsigned i = 0; i < games.size() ; i++ )
            {
               if( games[i]->Player1->username.compare( game->Player1->username) == 0 &&
                   games[i]->Player2->username.compare( game->Player2->username) == 0)
               {
                    games.erase(games.begin() + i);
               } 
                   
            }
            if(game->Player1->username.compare(username)==0)
            {
               game->Player2->game = NULL;
            }
            else
            {
               game->Player1->game = NULL;
            }
            delete game;
            game = NULL;
          }
}

void Player::Refresh()
{
  
  if(game != NULL && !game->IsGameOver)
  {
   game->Refresh(this);
  }
  else if(game == NULL)
  {
        WriteStringPrompt("cannot refresh without playing a game.\n");
  }
}
void Player::Resign()
{
  
  if(game != NULL && !game->IsGameOver)
  {
        game->DeclareWinner(this);
  }
  else if(game == NULL)
  {
        WriteStringPrompt("cannot resign without playing a game.\n");
  }
 
     RemoveGame();
}

void Player::BeginGame(string name,char*  colr )
{
      cout<<"Begin Game\n";
       Player *p ;
       string s;
       p = GetPlayerByName(name);
      cout<<"Begin Game\n";
       if(p == NULL)
       {
          WriteStringPrompt("user does not exist\n");
          return;
       }
      cout<<p->username<<"Begin Game\n";
       if(!p->IsOnline)
       {
        string s = "user ";
        s.append(name);
        s.append(" is not online\n");
        WriteStringPrompt(s);
          return;
       }
       if(name.compare(username)==0)
       {
        WriteStringPrompt("you cannot have match with yourself\n");
        return;
       }
       invite = name;
        char invitation[1000] ;
       if(p->invite.compare(username)==0)
       {
          if(colr != NULL)color = *colr;
          else color = 'w';
          if(p->color == color)
          {
             sprintf(invitation,"%s wants <match %s %s %d %d> ; %s wants <match %s %s %d %d>\n",username.c_str(),p->username.c_str(),&color,time,p->time,p->username.c_str(),username.c_str(),&color,time,p->time); // change later
	     p->WriteStringPrompt(string(invitation));
	     WriteStringPrompt(string(invitation));
             return;
          }
          game = new Game(this,p);
          game->game_num = (int)games.size();
          games.push_back(game);
          p->game = game;
          game->Publish();
          invite = "";
          p->invite = "";
       }
       else
       {
          if(colr != NULL)color = *colr;
          else color = 'b';
          sprintf(invitation,"%s invites you for a game <match %s %s %d %d>",username.c_str(),username.c_str(),&color,time,p->time); // change later
	  p->WriteStringPrompt(string(invitation));
          WriteStringPrompt("");
       }
  }

void printplayers()
{
  for(unsigned i = 0; i < players.size(); i++)
  {
    cout<<"uname : " << players[i]->username<<"soc : "<< players[i]->sockfd<<endl;
  }
}



/*
 vector<Game> GetAllGame()
 {

 }


 bool SentMessageToPlayer()
 {

 }*/
void *process_connection (void *arg)
{
  int sockfd  = *((int *)arg);
  free(arg);
  pthread_detach (pthread_self ());
  Player *p = new Player (sockfd);
   p->username ="Guest";
  bool IsReg = p->Authenticate ();
//  cout<<"username "<<p->username <<" sock "<<p->sockfd<< endl ;
  if(IsReg)
    {
         Player *temp = p->GetPlayerByName(p->username);
         if(temp->IsOnline)
	 {
            cout<<"flag set"<<endl;
            close(sockfd);
            pthread_exit(0);
	 }
         delete p;
         p = temp;
         cout<<"Player assigned"<<endl;
         p->IsOnline = true;
    }
      cout<<"socket mapped"<<endl;
      p->sockfd = sockfd;
      p->ProcessCommands();
   close(sockfd);
   p->IsOnline = false;
  cout<<"closed socket"<<endl;
  return (NULL);
}

void SaveEmails(Player p,FILE* fp)
{
   for(unsigned i= 0;i < p.emails.size(); i++)
   {
     Mail* m = p.emails[i];
       
    fprintf(fp,"username : %s\nIsRead : %d\nSender : %s\nHeader : %s\nSentTime : %s\nContent : %s\n%%%%\n",p.username.c_str(),m->IsRead,m->Sender.c_str(),m->Header.c_str(),m->SentTime.c_str(),m->Content.c_str());fflush(fp);
   }
}
void *RecordUpdaterThread(void* arg)
{
   FILE *fp1;
   while(1)
   {
       pthread_mutex_lock( &mtx_users );
       cout<<"Inside mutex\n";
       pthread_cond_wait(&size_threshold_cv ,&mtx_users );
       if((fp = fopen("Users","w"))== NULL)
       {
         cout<<"Error opening thee file users"<<endl;
       }
       if((fp1 = fopen("Emails","w"))== NULL)
       {
         cout<<"Error opening thee file users"<<endl;
       }
       if(playersToUpdate.size() > 0)
       {
        
          for(unsigned i = 0; i < playersToUpdate.size() ; i++ )
          {
             string blkusr= "";
             Player p = playersToUpdate[i];
             if(p.BlockedUsers.size()  == 0)blkusr = "<none>";
             for(unsigned j = 0 ; j < p.BlockedUsers.size() ; j++)
             {
                blkusr+=p.BlockedUsers[j];
                blkusr+= ",";
             }       
            cout<<"Above fprintf\n";
             fprintf(fp,"username : %s\npassword : %s\ninfo : %s\nrating : %lf\nwins : %d\nlosses : %d\nquiet : %d\nblocked : %s\n%%%%\n",p.username.c_str(),p.password.c_str(),p.Info.c_str(),p.Rating,p.Wins,p.Losses,p.IsQuiet,blkusr.c_str());fflush(fp);
         
                SaveEmails(p,fp1);
           }
       }
       fclose(fp);
       fclose(fp1);
       pthread_mutex_unlock(&mtx_users );
   }
   return NULL;
}


void ReloadEmails()
{
   FILE* inFile = fopen("Emails", "r");
  if(inFile == NULL)
  {
    return;
  }
   cout<<"hello"<<endl;
   char input[1000];
   Player helper(-1); 
   Player *p = NULL;
   char * ptr = input;
     Mail* m = new Mail();
   while (fgets(ptr, 1000, inFile))
   {
     string s = ptr;
     if(s.find("username")==0)    
     {
        string uname = s.substr(11);
        uname.erase(uname.end()-1);
        printf("uname : %s\n",uname.c_str());
        if(p != NULL )
        {
          if(uname.compare(p->username)!=0)
          {
             p = helper.GetPlayerByName(uname.c_str());
          } 
        }
        else  
           p = helper.GetPlayerByName(uname);
     } 
     else if(s.find("IsRead")==0)
     {
        string temp = s.substr(9);
        temp.erase(temp.end()-1);
        m->IsRead = atoi(temp.c_str()); 
     }
     else if(s.find("Sender")==0)
     {
        m->Sender = s.substr(9);
        m->Sender.erase(m->Sender.end()-1);
     }
     else if(s.find("Header")==0)
     {
        m->Header = s.substr(9);
        m->Header.erase(m->Header.end()-1);
     }
     else if(s.find("SentTime")==0)
     {
        m->SentTime = s.substr(11);
        m->SentTime.erase(m->SentTime.end()-1);
     }
     else if(s.find("Content")==0)
     {
        m->Content = s.substr(10);
        if(fgets(ptr, 1000, inFile)==NULL)
            return;
         s = ptr;
        while(s.find("%%")!=0)
        {
          m->Content += s;
          if(fgets(ptr, 1000, inFile)==NULL)
              return;
          s = ptr;
        }
         if(p!=NULL)
         {
            cout<<"user name"<<p->username<<"mail sneder"<<m->Sender<<endl; 
            p->emails.push_back(m);
            m = new Mail();
         } 
       }
    }
   fclose(inFile);
   cout<<"Exit"<<endl;
}


void ReloadDatabase()
{
   cout<<"Entering"<<endl;
  char uname[50];
  char pwd[50];
  char inf[50];
  float rat;
  int w,l,q;
  char blk[500];

       if((fp = fopen("Users","r"))== NULL)
       {
         cout<<"Error opening thee file users"<<endl;
         return;
       }
while(!feof(fp))
  {
   blk[0] = uname[0] = pwd[0] = inf[0] = '\0';
   int n_read=fscanf(fp,"username : %s\npassword : %s\ninfo : %s\nrating : %g\nwins : %i\nlosses : %i\nquiet : %i\nblocked : %s\n%%%%\n",uname,pwd,inf,&rat,&w,&l,&q,blk);
  cout<<"nread"<<n_read;
  if(n_read != 8) continue;
   cout<<uname<<" "<<pwd<<" "<<blk<<endl;
   Player* p = new Player(-1);
   p->username = uname;
   p->password = pwd;
   p->Info = inf;
   p->Rating = rat;
   p->Wins = w;
   p->Losses = l;
   p->IsQuiet = q;
   p->game = NULL;
   p->invite = " ";
   p->IsOnline = false;
   p->color = ' ';
   credentials[p->username]= p->password;
   cout<<uname<<" "<<credentials[p->username]<<" "<<blk<<endl;
   if(strcmp(blk,"<none>")!=0)
   {
      char *token = strtok (blk, ",");
      while(token != NULL)
      {
         cout<<"token :"<<token<<endl;
         p->BlockedUsers.push_back(string(token));
         token = strtok(NULL, ",");
      }
   }
   players.push_back(p);
   playersToUpdate.push_back(*p);
   
  }
  
       fclose(fp);
   
   cout<<"Exit"<<endl;
}


 
class Server
{
public:
  int port;
  Server ()
  {
  }
  ~Server ()
  {
  }
  void Start (void)
  {
    int serv_sockfd, cli_sockfd, *sock_ptr,option = 1;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t sock_len;
    pthread_t tid1;
    pthread_t tid;
    serv_sockfd = socket (AF_INET, SOCK_STREAM, 0);

    bzero ((void*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    serv_addr.sin_port = htons (port);
     
    pthread_cond_init(&size_threshold_cv,NULL);
    pthread_create (&tid1, NULL, &RecordUpdaterThread,(void*)NULL);
    bind (serv_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if(setsockopt(serv_sockfd,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
    {
      printf("setsockopt failed\n");
      close(serv_sockfd);
      exit(2);
    }
    ReloadDatabase();
   ReloadEmails();
    listen (serv_sockfd, 40);

    for (;;)
      {
	sock_len = sizeof(cli_addr);
	cli_sockfd = accept (serv_sockfd, (struct sockaddr *) &cli_addr,
			     &sock_len);

	cout << "remote client IP == " << inet_ntoa (cli_addr.sin_addr);
	cout << ", port == " << ntohs (cli_addr.sin_port) << endl;

	sock_ptr = (int *) malloc (sizeof(int));
	*sock_ptr = cli_sockfd;

	pthread_create (&tid, NULL, &process_connection, (void *) sock_ptr);
      }

  }
};

int
main (int argc, char **argv)
{
  Server *serv = new Server ();
  serv->port = atoi (argv[1]);
  serv->Start ();
  pthread_cond_destroy(&size_threshold_cv);
  delete serv;
  return 0;
}
