using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace TicTac
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        // Create a TCP/IP  socket.
        SocketConnector s;
        
        
        public MainWindow()
        {
            InitializeComponent();
        }

        

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            s = new SocketConnector();            
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            s.close();
        }

        private void btn_register_Click(object sender, RoutedEventArgs e)
        {
            Register wnd_register = new Register(s);
            wnd_register.Show();
        }
       
        private void btn_login_Click(object sender, RoutedEventArgs e)
        {
            Login wnd_login = new Login(s);
            wnd_login.Show();
        }

        private void btn_game_Click(object sender, RoutedEventArgs e)
        {
            tb_status.Text += s.ReadStatus();
            s.SendData("game");
            MessageBox.Show(s.GetData());
        }

        private void btn_mail_Click(object sender, RoutedEventArgs e)
        {
            Email wnd_email = new Email(s);
            wnd_email.Show();
        }

        private void btn_que_Click(object sender, RoutedEventArgs e)
        {
            tb_status.Text += s.ReadStatus();
            s.SendData("help");
            MessageBox.Show(s.GetData());
        }

        private void btn_help_Click(object sender, RoutedEventArgs e)
        {
            tb_status.Text += s.ReadStatus();
            s.SendData("help");
            MessageBox.Show(s.GetData());
        }

        private void btn_who_Click(object sender, RoutedEventArgs e)
        {
            Stats wnd_stat = new Stats(s);
            wnd_stat.Show();
        }

        private void btn_stats_Click(object sender, RoutedEventArgs e)
        {
            Stats wnd_stat = new Stats(s);
            wnd_stat.Show();
        }

        private void btn_match_Click(object sender, RoutedEventArgs e)
        {
            Match wnd_match = new Match(s);
            wnd_match.Show();
        }

        private void btn_comment_Click(object sender, RoutedEventArgs e)
        {
            Comment wnd_comment = new Comment(s);
            wnd_comment.Show();
        }

        private void btn_quit_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }

       
       
        private void btn_unblock_Click(object sender, RoutedEventArgs e)
        {
            tb_status.Text += s.ReadStatus();
            s.SendData(string.Format("unblock {0}", tb_uname.Text));
            MessageBox.Show(s.GetData());
        }

        private void btn_block_Click(object sender, RoutedEventArgs e)
        {
            tb_status.Text += s.ReadStatus();
            s.SendData(string.Format("block {0}",tb_uname.Text));
            MessageBox.Show(s.GetData());
        }

        private void cb_quiet_Click(object sender, RoutedEventArgs e)
        {
            tb_status.Text += s.ReadStatus();
            if (cb_quiet.IsChecked == true)
            {
                
                s.SendData("quiet");
                MessageBox.Show(s.GetData());
            }
            else
            {
           
                s.SendData("nonquiet");
                MessageBox.Show(s.GetData());
            }
        }


    }
}
