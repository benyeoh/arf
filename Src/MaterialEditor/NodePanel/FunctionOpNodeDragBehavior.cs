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
    public class FunctionOpNodeProxyDragBehavior : DragBehavior
    {
        private FunctionOpNode m_Op;
        private Point m_Offset;
       
        public FunctionOpNodeProxyDragBehavior(FunctionOpNode op)
        {
            m_Op = op;
        }

        public virtual void BeginDrag(Point pos, float scale)
        {
            pos.X = (int)(pos.X / scale);
            pos.Y = (int)(pos.Y / scale);

            m_Offset = new Point(m_Op.RectLabel.X - pos.X, m_Op.RectLabel.Y - pos.Y);
        }

        public virtual void PerformDrag(Point pos, float scale)
        {
            m_Op.RectLabel.X = (int)((pos.X) / scale) + m_Offset.X;
            m_Op.RectLabel.Y = (int)((pos.Y) / scale) + m_Offset.Y;
        }

        public virtual void EndDrag(Point pos, float scale)
        {
        }

        public virtual void Draw(Graphics graphics)
        {
        }
    }
}
