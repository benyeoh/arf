using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using ARFManaged.CoreManaged;
using ARFManaged.RendererManaged;

namespace Common.Controls
{
    public partial class RendererWin : UserControl
    {
        private Renderer m_Renderer;

        public RendererWin()
        {
            InitializeComponent();
        }

        public void Initialize(Renderer renderer, AppCallback callback)
        {
            InitInfo info;
            info.presentInfo.backBufferWidth = (uint) Width;
            info.presentInfo.backBufferHeight = (uint) Height;
            info.presentInfo.fullScreen = false;
            info.presentInfo.hWnd = Handle;

            info.hApplication = callback;

            renderer.Initialize(info);
            m_Renderer = renderer;
        }

        public Renderer Renderer
        {
            get { return m_Renderer; }
        }

        private void RendererWin_SizeChanged(object sender, EventArgs e)
        {
            DisplayInfo dispInfo;
            dispInfo.backBufferWidth = (uint)Width;
            dispInfo.backBufferHeight = (uint)Height;
            dispInfo.fullScreen = false;
            dispInfo.hWnd = Handle;

            Renderer.SetDisplayInfo(dispInfo);
        }
    }
}
