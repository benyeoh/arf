using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.Runtime.Serialization;

namespace MaterialEditor
{
    [Serializable]
    public class EllipseLabel : ISerializable, ICloneable
    {   
        private Rectangle m_Rect;
        private Color m_BackColor;  
        private Point m_Centre;

        public EllipseLabel()
        {
            m_Rect = new Rectangle(0, 0, 32, 32);
            m_BackColor = Color.Beige;
            m_Centre = new Point(16, 16);
        }


        protected EllipseLabel(SerializationInfo info, StreamingContext context)
        {
            m_Rect = (Rectangle) info.GetValue("Rect", typeof(Rectangle));
            m_Centre = (Point)info.GetValue("Centre", typeof(Point));
            m_BackColor = (Color)info.GetValue("BackColor", typeof(Color));
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Rect", m_Rect);
            info.AddValue("Centre", m_Centre);
            info.AddValue("BackColor", m_BackColor);
        }

        public Rectangle Rect
        {
            get
            {
                return m_Rect;
            }
        }

        public int X
        {
            get
            {
                return m_Centre.X;
            }

            set
            {
                m_Centre.X = (int)value;
                m_Rect.X = m_Centre.X - m_Rect.Width / 2;
            }
        }

        public int Y
        {
            get
            {
                return m_Centre.Y;
            }

            set
            {
                m_Centre.Y = (int)value;
                m_Rect.Y = m_Centre.Y - m_Rect.Height / 2;
            }
        }

        public int Width
        {
            get
            {
                return m_Rect.Width;
            }

            set
            {
                m_Rect.Width = (int)value;
            }
        }

        public int Height
        {
            get
            {
                return m_Rect.Height;
            }

            set
            {
                m_Rect.Height = (int)value;
            }
        }

        public Color BackColor
        {
            get
            {
                return m_BackColor;
            }

            set
            {
                m_BackColor = (Color)value;
            }
        }

        public void Draw(Graphics graphics)
        {
            Pen outlinePen = new Pen(Color.Black, 2);
            SolidBrush fillBrush = new SolidBrush(BackColor);

            graphics.FillEllipse(fillBrush, Rect);
            graphics.DrawEllipse(outlinePen, Rect);
        }

        public object Clone()
        {
            EllipseLabel label = new EllipseLabel();
            label.Width = Width;
            label.Height = Height;             
            label.X = X;
            label.Y = Y;
            label.BackColor = BackColor;

            return label;
        }
    }
}
