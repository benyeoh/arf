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
    public partial class DrawWin : DockContent
    {
        private MainWin m_Main;
        private NodePanel m_Panel;

        public DrawWin(MainWin main)
        {
            InitializeComponent();
            m_Main = main;

            EventManager.GlobalEventManager.AddEventHandler(RefreshEventArgs.EVENT_TYPE, m_Main_OnRefresh);

           // Application.Idle += new EventHandler(Application_Idle);
            zoomControl.TrackBar.ValueChanged += new EventHandler(TrackBar_ValueChanged);            
        }

        void TrackBar_ValueChanged(object sender, EventArgs e)
        {
            if(m_Panel != null)
            {
                m_Panel.GraphScale = zoomControl.Zoom;
            }
        }
        
        void m_Main_OnRefresh(object sender, EventArgs args)
        {
            //if (!IsHidden && Visible)
            m_Panel.Invalidate();
        }

        public NodePanel Panel
        {
            get
            {
                return m_Panel; 
            }

            set
            {
                m_Panel = (NodePanel)value;
                panel.Controls.Clear();
                panel.Controls.Add(m_Panel);
            }
        }

        protected override void OnDockStateChanged(EventArgs e)
        {
            base.OnDockStateChanged(e);
        }
       
    }
}