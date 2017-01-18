README



Tic tac toe server will provide all the functionalities listed and the
commands will perform the following operations:

1. Any user can register using guest account and quit. He cannot perform any
logic commands when he is registering.(register username password)[Only one
registration per quest account]

2. Once registering he can login with the user id and password which he
entered.

3. he can then perform few commands listed on  "help" or "?" command.

4. User information is saved when he quits in case if server crashes we have
the backup of all the user information.(Users and Emails are the file names)

5. If an user is on-line he cannot login through other client or terminal.

6. Other user options will behave as listed in the "help" description.



We have a make file which is complied by running "make" command and make demo
will run the server from 10000 port



./server 10000



We can connect to the server using telnet or we have a window based client by
which we can connect to.



telnet 127.0.0.1 10000



Window based Client: 

1. We have connected the server and the client through the Ethernet cable on a
fixed IP 10.0.0.1 and we connect to the server using that IP address and 10000
port. This is visible only to the server and the client and not visible to the
outside world.  

2. We receive notifications on clicking any buttons on the window client and
upon doing that we get all the messages like tell, shout in the message text
box placed below.

3. We have to refresh when playing a game to view other persons moves.

4. Window clients has buttons for all commands through which he can do regular
client operations and we have a radio button for entering quiet and non quiet
mode.

   

   We have a normal multiplexed client through which it can connect to the
   server and perform the operations exactly as telnet behaves. It can be run
   by executing the below command.



   ./client 127.0.0.1 10000



   SERVER:



   We are using a multi-threaded server which accepts connections whenever a
   client gets connected to it and create a separate thread for that client. 
