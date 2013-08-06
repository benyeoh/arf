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
    public class VertexOutNodeDragBehavior : DragBehavior
    {
        private VertexOutNode m_VertOut;
        private Point m_Offset;

        public VertexOutNodeDragBehavior(VertexOutNode vertOut)
        {
            m_VertOut = vertOut;
        }

        public virtual void BeginDrag(Point pos, float scale)
        {
            pos.X = (int)(pos.X / scale);
            pos.Y = (int)(pos.Y / scale);

            m_Offset = new Point(m_VertOut.RectLabel.X - pos.X, m_VertOut.RectLabel.Y - pos.Y);
        }

        public virtual void PerformDrag(Point pos, float scale)
        {
            m_VertOut.RectLabel.X = (int)((pos.X) / scale) + m_Offset.X;
            m_VertOut.RectLabel.Y = (int)((pos.Y) / scale) + m_Offset.Y;
        }

        public virtual void EndDrag(Point pos, float scale)
        {
        }

        public virtual void Draw(Graphics graphics)
        {
        }
    }
}
