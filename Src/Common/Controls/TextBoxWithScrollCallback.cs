using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace Common.Controls
{
    public partial class TextBoxWithScrollCallback : TextBox
    {
        private const int WM_HSCROLL = 0x114;
        private const int WM_VSCROLL = 0x115;

        public event ScrollEventHandler OnHorizontalScroll;
        public event ScrollEventHandler OnVerticalScroll;

        private static ScrollEventType[] scrollEventType =
            new ScrollEventType[] { ScrollEventType.SmallDecrement,
            ScrollEventType.SmallIncrement,
            ScrollEventType.LargeDecrement,
            ScrollEventType.LargeIncrement,
            ScrollEventType.ThumbPosition,
            ScrollEventType.ThumbTrack,
            ScrollEventType.First,
            ScrollEventType.Last,
            ScrollEventType.EndScroll};

        public TextBoxWithScrollCallback()
            : base()
        {
            InitializeComponent();
        }

        private ScrollEventType GetEventType(uint wParam)
        {
            if (wParam < scrollEventType.Length)
                return scrollEventType[wParam];
            else
                return ScrollEventType.EndScroll;
        }

        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);

            if (m.Msg == WM_HSCROLL)
            {
                if (OnHorizontalScroll != null)
                {
                    uint wParam = (uint)m.WParam.ToInt32();
                    OnHorizontalScroll(this,
                        new ScrollEventArgs(
                        GetEventType(wParam & 0xffff), (int)(wParam >> 16)));
                }
            }
            else if (m.Msg == WM_VSCROLL)
            {
                if (OnVerticalScroll != null)
                {
                    uint wParam = (uint)m.WParam.ToInt32();
                    OnVerticalScroll(this,
                        new ScrollEventArgs(
                        GetEventType(wParam & 0xffff), (int)(wParam >> 16)));
                }
            }
        }
    }
}
