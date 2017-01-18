using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace TicTac
{
    public class SocketConnector
    {
        byte[] bytes = new byte[10024];
        private Socket sock;
        public SocketConnector()
        {
            try
            {

                IPAddress ia = IPAddress.Parse("10.0.0.1");
                IPEndPoint remoteEP = new IPEndPoint(ia, 10000);
                try
                {
                    sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                    sock.Connect(remoteEP);

                }
                catch (ArgumentNullException ane)
                {
                    Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
                }
                catch (SocketException se)
                {
                    Console.WriteLine("SocketException : {0}", se.ToString());
                }
                catch (Exception e)
                {
                    Console.WriteLine("Unexpected exception : {0}", e.ToString());
                }

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public string GetData()
        {
            Array.Clear(bytes, 0, bytes.Length);
            int n = sock.Receive(bytes);
            return Encoding.ASCII.GetString(bytes,0,n);
        }

        public string ReadStatus()
        {
            string str = string.Empty;
            sock.ReceiveTimeout = 1000;
            try
            {
                str = GetData();
            }
            catch (Exception)
            {
            }
            sock.ReceiveTimeout = 0;
            return str;
        }

        public void SendData(string message)
        {
            message += "  ";
            byte[] msg = Encoding.ASCII.GetBytes(message);
            sock.Send(msg);
        }

        public void close()
        {
            sock.Shutdown(SocketShutdown.Both);
            sock.Close();
        }

        internal void SendData(string message, bool app)
        {
            if(app)
            message += "  ";
            byte[] msg = Encoding.ASCII.GetBytes(message);
            sock.Send(msg);
        }
    }
}
