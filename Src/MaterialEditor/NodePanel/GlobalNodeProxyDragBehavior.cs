using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using Common.PropertyGrid;

namespace MaterialEditor
{
    public class GlobalNodeProxyDragBehavior : DragBehavior
    {
        private GlobalNodeProxy m_Proxy;
        private Point m_Offset;

        public GlobalNodeProxyDragBehavior(GlobalNodeProxy proxy)
        {
            m_Proxy = proxy;
        }

        public virtual void BeginDrag(Point pos, float scale)
        {
            pos.X = (int)(pos.X / scale);
            pos.Y = (int)(pos.Y / scale);

            m_Offset = new Point(m_Proxy.RectLabel.X - pos.X, m_Proxy.RectLabel.Y - pos.Y);
        }

        public virtual void PerformDrag(Point pos, float scale)
        {
            m_Proxy.RectLabel.X = (int)((pos.X) / scale) + m_Offset.X;
            m_Proxy.RectLabel.Y = (int)((pos.Y) / scale) + m_Offset.Y;
        }

        public virtual void EndDrag(Point pos, float scale)
        {
        }

        public virtual void Draw(Graphics graphics)
        {
        }
    }
}
