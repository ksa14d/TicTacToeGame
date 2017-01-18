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
    /// Interaction logic for Register.xaml
    /// </summary>
    public partial class Register : Window
    {
        
        private SocketConnector s;

        public Register()
        {
            InitializeComponent();
        }

        public Register(SocketConnector sock)
        {
            // TODO: Complete member initialization
            InitializeComponent();
            this.s = sock;
        }

        private void btn_register_Click(object sender, RoutedEventArgs e)
        {

            tb_status.Text += s.ReadStatus();
           
            s.SendData("");
            s.SendData(string.Format("register {0} {1}", tb_uname.Text, tb_password.Text));

            tb_status.Text += s.ReadStatus();

            s.SendData("quit");
            tb_status.Text += s.ReadStatus();

            MessageBox.Show("User Register !!");
            Application.Current.Shutdown();
            
           
            
        }

       
    }
}
