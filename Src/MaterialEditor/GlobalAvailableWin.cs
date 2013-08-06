using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace MaterialEditor
{
    public partial class GlobalAvailableWin : Form
    {
        public GlobalAvailableWin()
        {
            InitializeComponent();
        }

        public DataGridView DataGrid
        {
            get
            {
                return dataGrid;
            }
        }
    }
}