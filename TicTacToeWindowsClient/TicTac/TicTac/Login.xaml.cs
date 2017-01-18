using System;
using System.Collections.Generic;
using System.Linq;
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
using System.Windows.Shapes;

namespace TicTac
{
    /// <summary>
    /// Interaction logic for Login.xaml
    /// </summary>
    public partial class Login : Window
    {
        private SocketConnector s;
        
        public Login()
        {
            InitializeComponent();
        }

        public Login(SocketConnector sock)
        {
            InitializeComponent();
            // TODO: Complete member initialization
            this.s = sock;
        }

        private void btn_login_Click(object sender, RoutedEventArgs e)
        {
            tb_status.Text += s.ReadStatus();

            s.SendData(tb_uname.Text);


            tb_status.Text += s.ReadStatus();

            s.SendData(tb_password.Text);
            tb_status.Text += s.ReadStatus();
            MessageBox.Show("Welcome !!");            
        }

       
    }
}
