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
    public class ScrollDragBehavior : DragBehavior
    {
        private Point m_Offset;
        private NodePanel m_Panel;
        private Point m_Scroll;
        private Cursor m_Old;

        public ScrollDragBehavior(NodePanel panel)
        {
            m_Panel = panel;
        }

        public virtual void BeginDrag(Point pos, float scale)
        {
            m_Old = Cursor.Current;
            Cursor.Current = Cursors.Hand;

            m_Offset = m_Panel.PointToClient(Cursor.Position);
            m_Scroll.X = m_Panel.HorizontalScroll.Value;
            m_Scroll.Y = m_Panel.VerticalScroll.Value;
        }

        public virtual void PerformDrag(Point pos, float scale)
        {           
            Point relPos = m_Panel.PointToClient(Cursor.Position);
            Point diff = new Point(relPos.X - m_Offset.X, relPos.Y - m_Offset.Y);
           // m_Offset = relPos;

            m_Panel.HorizontalScroll.Value = 
                System.Math.Min(
                    System.Math.Max(m_Scroll.X - (int)(diff.X * scale * 1.5f), 
                        m_Panel.HorizontalScroll.Minimum), m_Panel.HorizontalScroll.Maximum);
            m_Panel.VerticalScroll.Value = 
                System.Math.Min(
                    System.Math.Max(m_Scroll.Y - (int)(diff.Y * scale * 1.5f), 
                    m_Panel.HorizontalScroll.Minimum), m_Panel.HorizontalScroll.Maximum);
        }


        public virtual void EndDrag(Point pos, float scale)
        {
            Cursor.Current = m_Old;
        }

        public virtual void Draw(Graphics graphics)
        {
        }
    }
}
