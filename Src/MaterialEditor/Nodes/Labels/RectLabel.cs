using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.Runtime.Serialization;

namespace MaterialEditor
{
    [Serializable]
    public class RectLabel : ISerializable, ICloneable
    {
        private Rectangle m_Rect;
        private Font m_Font;
        private string m_Text;
        private Color m_BackColor;
        private Color m_FontColor;

        public RectLabel()
        {
            m_Rect = new Rectangle(0, 0, 32, 32);
            m_Font = new Font("Verdana", 12);
            m_Text = "unknown";
            m_BackColor = Color.Beige;
            m_FontColor = Color.Black;

            UpdateBounds();
        }

        protected RectLabel(SerializationInfo info, StreamingContext context)
        {
            m_Rect = (Rectangle) info.GetValue("Rect", typeof(Rectangle));
            m_Font = (Font)info.GetValue("Font", typeof(Font));
            m_Text = info.GetString("Text");
            m_BackColor = (Color)info.GetValue("BackColor", typeof(Color));
            m_FontColor = (Color)info.GetValue("FontColor", typeof(Color));
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Rect", m_Rect);
            info.AddValue("Font", m_Font);
            info.AddValue("Text", m_Text);
            info.AddValue("BackColor", m_BackColor);
            info.AddValue("FontColor", m_FontColor);
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
                return m_Rect.X;
            }

            set
            {
                m_Rect.X = (int)value;
                UpdateBounds();
            }
        }

        public int Y
        {
            get
            {
                return m_Rect.Y;
            }

            set
            {
                m_Rect.Y = (int)value;
                UpdateBounds();
            }
        }
        
        public Font Font
        {
            get
            {
                return m_Font;
            }

            set
            {
                m_Font = (Font)value;
                UpdateBounds();
            }
        }

        public string Text
        {
            get
            {
                return m_Text;
            }

            set
            {
                m_Text = (string)value;
                UpdateBounds();
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

        public Color FontColor
        {
            get
            {
                return m_FontColor;
            }

            set
            {
                m_FontColor = (Color)value;
            }
        }

        public void UpdateBounds()
        {
            Size fontSize = TextRenderer.MeasureText(Text, Font, new Size(0, 0),
                TextFormatFlags.HorizontalCenter | TextFormatFlags.VerticalCenter);
            m_Rect.Width = fontSize.Width + 2 * (int) Font.SizeInPoints / 3;
            m_Rect.Height = fontSize.Height + 2 * (int) Font.SizeInPoints / 3;
        }

        public void Draw(Graphics graphics)
        {            
            Pen outlinePen = new Pen(FontColor, 2);
            SolidBrush fontBrush = new SolidBrush(FontColor);
            SolidBrush fillBrush = new SolidBrush(BackColor);

            graphics.FillRectangle(fillBrush, Rect);
            graphics.DrawRectangle(outlinePen, Rect);
            float fontX = Rect.Left + Font.SizeInPoints / 2;
            float fontY = Rect.Top + Font.SizeInPoints / 2;

            //Rectangle rect = new Rectangle(m_Rect.X, m_Rect.Y, m_Rect.Width, m_Rect.Height);

            //StringFormat stringFormat = new StringFormat();
            //stringFormat.Alignment = StringAlignment.Center;
            //stringFormat.LineAlignment = StringAlignment.Center;
            graphics.DrawString(Text, Font, fontBrush, fontX, fontY);// stringFormat);

            //rect.X += (int)graphics.Transform.OffsetX;
            //rect.Y += (int)graphics.Transform.OffsetY;
            //TextRenderer.DrawText(graphics, Text, Font, rect, FontColor, 
            //    TextFormatFlags.HorizontalCenter | TextFormatFlags.VerticalCenter);

            //graphics.DrawString(Text, Font, fontBrush, fontX, fontY, new StringFormat(StringFormatFlags.));            
        }

        public object Clone()
        {
            RectLabel label = new RectLabel();

            label.X = X;
            label.Y = Y;
            label.Text = (string)Text.Clone();
            label.BackColor = BackColor;
            label.FontColor = FontColor;
            label.Font = (Font) Font.Clone();

            return label;
        }
    }
}
