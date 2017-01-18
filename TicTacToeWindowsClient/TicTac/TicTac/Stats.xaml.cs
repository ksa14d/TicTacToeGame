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
    /// Interaction logic for Stats.xaml
    /// </summary>
    public partial class Stats : Window
    {
        private SocketConnector s;
        
        public Stats()
        {
            InitializeComponent();
        }

        public Stats(SocketConnector s)
        {
            InitializeComponent();
            // TODO: Complete member initialization
            this.s = s;
        }

        private void btn_stat_Click(object sender, RoutedEventArgs e)
        {
            tb_status.Text += s.ReadStatus();
            s.SendData((string.Format("stats {0}", tb_uname.Text)));
            tb_stats.Text = s.GetData();
        }

        private void btn_who_Click(object sender, RoutedEventArgs e)
        {
            tb_status.Text += s.ReadStatus();
            s.SendData("who");
            tb_stats.Text = s.GetData();
        }

       
    }
}
