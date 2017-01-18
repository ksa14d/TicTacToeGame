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
    /// Interaction logic for Email.xaml
    /// </summary>
    public partial class Email : Window
    {
        
        private SocketConnector s;

        public Email()
        {
            InitializeComponent();
        }

        public Email(SocketConnector sock)
        {
            // TODO: Complete member initialization
            InitializeComponent();
            this.s = sock;
        }


      

        private void btn_listmail_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                tb_status.Text += s.ReadStatus();
                s.SendData("listmail");
                tb_message.Text += s.GetData();
                tb_message.Text += s.GetData();
            }
            catch (Exception)
            {
                
              
            }
        }

        private void btn_readmail_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                tb_status.Text += s.ReadStatus();
                s.SendData(string.Format("readmail {0}", tb_msgnum.Text));
                string maildata = s.GetData();
                maildata += s.GetData();
                List<string> lines = maildata.Split('\n').ToList();
                tb_uname.Text = lines[1];
                tb_subject.Text = lines[2];
                int i = 3;
                tb_message.Text = string.Empty;
                while (i < lines.Count)
                {
                    tb_message.Text += lines[i++];
                }
            }
            catch (Exception)
            {
                
                
            }
        }

        private void btn_deletemail_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                tb_status.Text += s.ReadStatus();
                s.SendData(string.Format("deletemail {0}", tb_msgnum.Text));
                MessageBox.Show(s.GetData());
            }
            catch (Exception)
            {
                
           
            }
        }

        private void btn_sendmail_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                tb_status.Text += s.ReadStatus();
                s.SendData(string.Format("mail {0} {1}", tb_uname.Text, tb_subject.Text));

                MessageBox.Show(s.GetData());
                s.SendData(string.Format("{0}", tb_message.Text), false);
                s.SendData(".", false);
                MessageBox.Show(s.GetData());
            }
            catch (Exception)
            {
                
            
            }
        }
    }
}
