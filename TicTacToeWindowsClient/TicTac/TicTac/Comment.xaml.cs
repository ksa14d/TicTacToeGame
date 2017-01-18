using System;
using System.Collections.Generic;
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
    /// Interaction logic for Comment.xaml
    /// </summary>
    public partial class Comment : Window
    {
        private SocketConnector s;
        
        public Comment()
        {
            InitializeComponent();
        }

        public Comment(SocketConnector sock)
        {
            InitializeComponent();
            // TODO: Complete member initialization
            this.s = sock;
        }

        
        private void btn_tell_Click(object sender, RoutedEventArgs e)
        {
            tb_status.Text += s.ReadStatus();
            s.SendData(string.Format("tell {0} {1}", tb_uname.Text, tb_message.Text));
            s.GetData();
        }

        private void btn_shout_Click(object sender, RoutedEventArgs e)
        {
            tb_status.Text += s.ReadStatus();
            s.SendData(string.Format("shout {0}", tb_message.Text));
            s.GetData();
        }

        private void btn_kibitz_Click(object sender, RoutedEventArgs e)
        {
            tb_status.Text += s.ReadStatus();
            s.SendData(string.Format("kibitz {0}", tb_message.Text));
            s.GetData();
        }

        private void btn_info_Click(object sender, RoutedEventArgs e)
        {
            tb_status.Text += s.ReadStatus();
            s.SendData(string.Format("info {0}", tb_message.Text));
            s.GetData();
        }

      
    }
}
