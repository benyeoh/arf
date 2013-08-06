using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using WeifenLuo.WinFormsUI.Docking;
using Common.PropertyGrid;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace MaterialEditor
{
    public partial class OutputWin : DockContent
    {
        private MainWin m_Main;

        public OutputWin(MainWin main)
        {
            m_Main = main;
            InitializeComponent();
        }

        public void WriteLine(string text)
        {
            TextBox.AppendText(text + "\r\n");
            TextBox.ScrollToCaret();
        }

        public void Clear()
        {
            TextBox.Text = "";
        }

        public RichTextBox TextBox
        {
            get
            {
                return textBox;
            }
        }
    }
}