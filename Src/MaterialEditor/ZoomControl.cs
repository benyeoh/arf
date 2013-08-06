using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace MaterialEditor
{
    public partial class ZoomControl : UserControl
    {
        public ZoomControl()
        {
            InitializeComponent();
        }

        private void trackBar_ValueChanged(object sender, EventArgs e)
        {
            percentLabel.Text = trackBar.Value.ToString() + "%";
        }

        public TrackBar TrackBar
        {
            get
            {
                return trackBar;
            }
        }

        public float Zoom
        {
            get
            {
                return trackBar.Value / 100.0f;
            }

            set
            {
                trackBar.Value = (int)((float)value * 100.0f);
            }
        }
    }
}
