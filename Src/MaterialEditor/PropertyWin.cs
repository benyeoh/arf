using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace MaterialEditor
{
    public partial class PropertyWin : DockContent
    {
        private MainWin m_Main;

        public PropertyWin(MainWin main)
        {
            m_Main = main;
            InitializeComponent();
        }

        public PropertyGrid PropertyGrid
        {
            get { return propertyGrid; }
        }
    }
}