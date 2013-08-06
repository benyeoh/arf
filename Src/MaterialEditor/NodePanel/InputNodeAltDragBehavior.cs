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
    public class InputNodeAltDragBehavior : DragBehavior
    {
        private InputNode m_Input;
        private List<OutputNode> m_Outputs;
        private Point m_CurPoint;
        private OutputNode m_OldOutput;
        private OutputNode m_CurOutput;

        public InputNodeAltDragBehavior(InputNode input, List<OutputNode> allOutputs)
        {
            m_Input = input;
            m_Outputs = allOutputs;
        }

        public virtual void BeginDrag(Point pos, float scale)
        {
            m_CurPoint = pos;

            m_OldOutput = m_Input.AltBinding;
            m_Input.AltBinding = null;
        }

        private Rectangle GetScaledRectangle(Rectangle rect, float scale)
        {
            Rectangle newRect = new Rectangle((int)(rect.X * scale),
                                            (int)(rect.Y * scale),
                                            (int)(rect.Width * scale),
                                            (int)(rect.Height * scale));
            return newRect;
        }

        public virtual void PerformDrag(Point pos, float scale)
        {
            m_CurPoint = pos;
            m_CurOutput = null;

            foreach (OutputNode output in m_Outputs)
            {
                if (GetScaledRectangle(output.EllipseLabel.Rect, scale).Contains(pos))
                {
                    if (output.Type.Equals(m_Input.Type))
                    {
                        m_CurPoint = new Point((int)(output.EllipseLabel.X * scale), (int)(output.EllipseLabel.Y * scale));
                        m_CurOutput = output;
                        break;
                    }

                    // Else draw an X ?
                }
            }
        }

        public virtual void EndDrag(Point pos, float scale)
        {
            if (m_CurOutput != null)
                m_Input.AltBinding = m_CurOutput;
            else if (GetScaledRectangle(m_Input.AltEllipseLabel.Rect, scale).Contains(pos))
                m_Input.AltBinding = null;
            else
                m_Input.AltBinding = m_OldOutput;
        }

        public virtual void Draw(Graphics graphics)
        {
            Pen pen = new Pen(Color.LightGray, 2);
            if (m_CurOutput != null)
            {
                pen.DashStyle = DashStyle.Solid;
                AdjustableArrowCap arrowCap = new AdjustableArrowCap(6 * graphics.Transform.Elements[0], 8 * graphics.Transform.Elements[0], true);
                CustomLineCap custom = arrowCap;
                pen.CustomStartCap = custom;
            }
            else
            {
                pen.DashStyle = DashStyle.Dash;
            }

            //pen.EndCap = LineCap.ArrowAnchor;
            //pen.StartCap = LineCap.ArrowAnchor;

            graphics.DrawLine(pen, m_Input.AltEllipseLabel.X, m_Input.AltEllipseLabel.Y,
                (int)(m_CurPoint.X / graphics.Transform.Elements[0]), (int)(m_CurPoint.Y / graphics.Transform.Elements[0]));
        }
    }
}
