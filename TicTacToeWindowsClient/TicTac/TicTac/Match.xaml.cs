using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace TicTac
{
    /// <summary>
    /// Interaction logic for Match.xaml
    /// </summary>
    public partial class Match : Window
    {
        private SocketConnector s;
        private List<Button> board;
        public Match()
        {
            InitializeComponent();
            
        }


       
        
        public Match(SocketConnector sock)
        {
            InitializeComponent();
            // TODO: Complete member initialization
            try
            {
                this.s = sock;
                cmb_color.ItemsSource = new ObservableCollection<string> { "b", "w" };
                board = Board.Children.OfType<Button>().ToList();
                for (int i = 0; i < board.Count; i++)
                {
                    board[i].Click += (sender, e) =>
                    {
                        Button b = sender as Button;
                        tb_status.Text += s.ReadStatus();
                        s.SendData(b.Name);
                        //process game data
                        string gamedata = s.GetData();
                        UpdateBoard(gamedata);
                    };
                }
            }
            catch (Exception)
            {
                
       
            }
        }

        

        private void btn_invite_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                tb_status.Text += s.ReadStatus();
                s.SendData((string.Format("match {0} {1} {2}", tb_uname.Text, cmb_color.Text, tb_time.Text)));
                //may begin game
                string gamedata = s.GetData();
                UpdateBoard(gamedata);
            }
            catch (Exception)
            {
                
        
            }
        }

        private void btn_refresh_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                tb_status.Text += s.ReadStatus();
                s.SendData("refresh");
                //process game data
                string gamedata = s.GetData();
                UpdateBoard(gamedata);
            }
            catch (Exception)
            {
                
               
            }
        }

        private void btn_observe_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                tb_status.Text += s.ReadStatus();
                s.SendData((string.Format("observe {0}", tb_gamenum.Text)));
                tb_status.Text += s.ReadStatus();
            }
            catch (Exception)
            {
                
            
            }
        }

        private void btn_unobserve_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                tb_status.Text += s.ReadStatus();
                s.SendData("unobserve");
                tb_status.Text += s.ReadStatus();
            }
            catch (Exception)
            {
                
          
            }
        }

        private void btn_resign_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                tb_status.Text += s.ReadStatus();
                s.SendData("resign");
                //process game
                string gamedata = s.GetData();
                UpdateBoard(gamedata);
            }
            catch (Exception)
            {
                
          
            }
        }

       

        private void UpdateBoard(string GameData)
        {
            try
            {
                if (GameData.Contains("won") || GameData.Contains("want") || GameData.Contains("cannot") || GameData.Contains("turn"))
                {
                    tb_dashboard.Text = GameData;
                    return;
                }

                List<string> lines = GameData.Split('\n').ToList<string>();
                tb_dashboard.Text = lines[1] + "\n" + lines[2];
                List<string> r1 = lines[4].Split(' ').ToList<string>();
                List<string> r2 = lines[5].Split(' ').ToList<string>();
                List<string> r3 = lines[6].Split(' ').ToList<string>();
                r1.RemoveAt(0);
                r1.RemoveAt(3);
                r2.RemoveAt(0);
                r2.RemoveAt(3);
                r3.RemoveAt(0);
                r3.RemoveAt(3);
                List<string> lst = new List<string>();
                lst.AddRange(r1);
                lst.AddRange(r2);
                lst.AddRange(r3);
                for (int i = 0; i < board.Count; i++)
                {
                    board[i].Content = lst[i];
                }
            }
            catch (Exception)
            {
                
       
            }
        }

        private void btn_Update_Notifications_Click(object sender, RoutedEventArgs e)
        {
            tb_status.Text += s.ReadStatus();
        }
    }
}
