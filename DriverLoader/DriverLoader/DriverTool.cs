using DriverOperateInterface;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO;
using System.Threading;
using Infrastructure.FormAPI;

namespace DriverLoader
{
    public partial class DriverTool : Form
    {
        public static readonly object MLOCK = new object();
        string filePath = string.Empty;
        string ServiceName = string.Empty;
        string DisplayName = string.Empty;

        long dwDriverStatus = 0;
        public DriverTool()
        {
            InitializeComponent();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(textBox1.Text.Trim()))
            {
                MessageBox.Show("请选择文件！", "选择文件提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                ShowErrorMessage(DriverAPIInterface.Create(ServiceName, DisplayName, textBox1.Text).ToInt32());
                RefreshStatus();
            }
            
        }

        private void RefreshStatus()
        {
            bool bCreate = false, bStart = false, bStop = false, bDelete = false;
            switch (dwDriverStatus = DriverAPIInterface.GetStatus(ServiceName).ToInt64())
            {
                case DriverAPIInterface.DRIVER_CLOSED:
                    bCreate = true;
                    this.Invoke(new Action(() => { label5.Text = "已关闭"; label5.ForeColor = Color.Red; }));
                    break;
                case DriverAPIInterface.DRIVER_OPENED:
                    bStart = true;
                    bDelete = true;
                    this.Invoke(new Action(() => { label5.Text = "已指定驱动"; label5.ForeColor = Color.Blue; }));
                    break;
                case DriverAPIInterface.DRIVER_STARTED:
                    bStop = true;
                    this.Invoke(new Action(() => { label5.Text = "驱动运行中"; label5.ForeColor = Color.Green; }));
                    break;
                case DriverAPIInterface.DRIVER_PAUSED:
                    bStop = true;
                    this.Invoke(new Action(() => { label5.Text = "驱动已被暂停"; label5.ForeColor = Color.Yellow; }));
                    break;
                case DriverAPIInterface.DRIVER_BUSY:
                    this.Invoke(new Action(() => { label5.Text = "驱动正忙"; label5.ForeColor = Color.RoyalBlue; }));
                    break;
            }
            this.Invoke(new Action(() => button1.Enabled = bCreate));
            this.Invoke(new Action(() => button2.Enabled = bStart));
            this.Invoke(new Action(() => button3.Enabled = bStop));
            this.Invoke(new Action(() => button4.Enabled = bDelete));
        }

        private void ShowErrorMessage(long dwErrCode)
        {
            long dwLastError = 0;
            switch (dwErrCode)
            {
                case DriverAPIInterface.NDER_GETLASTERROR:
                    dwLastError = Marshal.GetLastWin32Error();
                    break;
                case DriverAPIInterface.NDER_FILE_NO_FOUND:
                    dwLastError = DriverAPIInterface.ERROR_FILE_NOT_FOUND;
                    break;
                case DriverAPIInterface.NDER_SERVICE_NOT_OPEN:
                    MessageBox.Show("驱动还未打开！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    break;
                case DriverAPIInterface.NDER_SERVICE_NOT_STARTED:
                    dwLastError = DriverAPIInterface.ERROR_SERVICE_NOT_ACTIVE;
                    break;
                case DriverAPIInterface.NDER_SERVICE_ALREADY_OPENED:
                    MessageBox.Show("驱动已经打开！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    break;
                case DriverAPIInterface.NDER_SERVICE_ALREADY_STARTED:
                    dwLastError = DriverAPIInterface.ERROR_SERVICE_ALREADY_RUNNING;
                    break;
                case DriverAPIInterface.NDER_SERVICE_IO_PENDING:
                    MessageBox.Show("驱动正忙！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    break;
            }
            if (dwLastError > 0)
            {
                Win32Exception exc = new Win32Exception((int)dwLastError);
                MessageBox.Show($"{exc.Message}", "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
                
        }

        private void Form1_DragDrop(object sender, DragEventArgs e)
        {
            textBox1.Text = ((System.Array)e.Data.GetData(DataFormats.FileDrop)).GetValue(0).ToString();
            filePath = textBox1.Text;
            ServiceName = Path.GetFileNameWithoutExtension(filePath);
            DisplayName = Path.GetFileNameWithoutExtension(filePath);
            textBox2.Text = ServiceName;
            textBox3.Text = DisplayName;
        }

        private void Form1_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                e.Effect = DragDropEffects.Link;
            }
            else
            {
                e.Effect = DragDropEffects.None;
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            OpenFileDialog fileDialog = new OpenFileDialog();
            fileDialog.Multiselect = true;
            fileDialog.Title = "请选择文件";
            fileDialog.Filter = "所有文件(*.*)|*.*";
            if (fileDialog.ShowDialog() == DialogResult.OK)
            {
                string file = fileDialog.FileName;
                textBox1.Text = file;
                filePath = file;
                ServiceName = Path.GetFileNameWithoutExtension(filePath);
                DisplayName = Path.GetFileNameWithoutExtension(filePath);
                textBox2.Text = ServiceName;
                textBox3.Text = DisplayName;
                //MessageBox.Show("已选择文件:" + file, "选择文件提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            ShowErrorMessage(DriverAPIInterface.Load().ToInt32());
            RefreshStatus();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            ShowErrorMessage(DriverAPIInterface.Unload(1000).ToInt32());
            RefreshStatus();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            ShowErrorMessage(DriverAPIInterface.Delete().ToInt32());
            RefreshStatus();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // 在LinkLable控件中可以添加多个链接
            this.linkLabel1.Links.Add(0,this.linkLabel1.Text.Length, @"https://blog.csdn.net/black_bad1993");
            var IsAdmin= DriverAPIInterface.IsRunasAdmin();
            if (IsAdmin)
            {
                this.Text = $"管理员：{this.Text}";
            }
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            if (DriverAPIInterface.GetStatus(ServiceName).ToInt32()!=0&&string.Compare(textBox2.Text,ServiceName)!=0)
            {
                if(MessageBox.Show($"当前正在操作【{ServiceName}】驱动，是否取消操作！", "警告", MessageBoxButtons.OKCancel, MessageBoxIcon.Asterisk) == DialogResult.OK)
                {
                    DriverAPIInterface.Close();
                }
                else
                {

                }
                return;
            }
            if (!string.IsNullOrEmpty(ServiceName))
            {
                if (DriverAPIInterface.Open(ServiceName).ToInt32() == 0)
                {
                    if (MessageBox.Show($"驱动【{ServiceName}】已存在，是否要操作此驱动？", "警告", MessageBoxButtons.OKCancel, MessageBoxIcon.Asterisk) == DialogResult.Cancel)
                    {
                        DriverAPIInterface.Close();
                    }
                    RefreshStatus();
                }
            }
        }

        private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            this.linkLabel1.Links[this.linkLabel1.Links.IndexOf(e.Link)].Visited = true;
            string targetUrl = e.Link.LinkData as string;
            if (string.IsNullOrEmpty(targetUrl))
                MessageBox.Show("没有链接地址！");
            else
                System.Diagnostics.Process.Start(targetUrl);
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked)
            {
                FormAPI.SetFormTop(this.Handle);
            }
            else
            {
                FormAPI.SetFormNoTop(this.Handle);
            }
        }

        private void button6_Click(object sender, EventArgs e)
        {
            System.Environment.Exit(0);
        }

        private void DriverTool_Shown(object sender, EventArgs e)
        {
            Task getDriverState = new Task((object obj) =>
            {
                while (true)
                {
                    lock (MLOCK)
                    {
                        try
                        {
                            RefreshStatus();
                            Thread.Sleep(1500);
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine(ex.ToString());
                        }
                    }
                }
            }, null, TaskCreationOptions.LongRunning);
            getDriverState.Start();
            checkBox1.Checked = true;
        }

        private void button7_Click(object sender, EventArgs e)
        {
            //C:\HanJunJun\辅助SVN\Code\C++\MyfirstDriver\x64\Debug\MyfirstDriver\MyfirstDriver.sys
            DriverAPIInterface.OperateDriver();
        }

        private void button8_Click(object sender, EventArgs e)
        {
            textBox1.Text = AppDomain.CurrentDomain.BaseDirectory+ "MyfirstDriver.sys";
            filePath = textBox1.Text;
            ServiceName = Path.GetFileNameWithoutExtension(filePath);
            DisplayName = Path.GetFileNameWithoutExtension(filePath);
            textBox2.Text = ServiceName;
            textBox3.Text = DisplayName;
            DriverAPIInterface.Unload(1000).ToInt32();//停止驱动
            DriverAPIInterface.Delete().ToInt32();//卸载驱动
            ShowErrorMessage(DriverAPIInterface.Create(ServiceName, DisplayName, textBox1.Text).ToInt32());
            ShowErrorMessage(DriverAPIInterface.Load().ToInt32());
            RefreshStatus();
        }
    }
}
