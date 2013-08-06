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
    public class RenderStateNodePanel : NodePanel
    {
        private RenderStateNode m_State;
        
        public RenderStateNodePanel(MainWin main)
            : base(main)
        {   
        }

        public RenderStateNode State
        {
            get
            {
                return m_State;
            }

            set
            {
                m_State = (RenderStateNode)value;

                if (m_State.RectLabel.X == 0 &&
                    m_State.RectLabel.Y == 0)
                {
                    m_State.RectLabel.X = ClientRectangle.Width / 2;
                    m_State.RectLabel.Y = ClientRectangle.Height / 2;
                }
            }
        }

        protected override void UpdatePositions()
        {
            int centerX = m_State.RectLabel.X + m_State.RectLabel.Rect.Width / 2;
            Size totalSize = new Size(0, 0);
            foreach (InputNode input in m_State.Inputs)
            {
                totalSize.Width += input.RectLabel.Rect.Width;
                totalSize.Height = System.Math.Max(input.RectLabel.Rect.Height, totalSize.Height);
            }

            int widthSoFar = centerX - totalSize.Width / 2;
            int top = m_State.RectLabel.Y - totalSize.Height;
            foreach (InputNode input in m_State.Inputs)
            {
                input.RectLabel.X = widthSoFar;
                input.RectLabel.Y = top - 4;
                widthSoFar += input.RectLabel.Rect.Width;

                input.EllipseLabel.X = input.RectLabel.X + input.RectLabel.Rect.Width / 2;
                input.EllipseLabel.Y = input.RectLabel.Y - input.EllipseLabel.Rect.Height / 2 - 4;
            }

            foreach (GlobalNodeProxy dependent in m_State.Dependents)
            {
                UpdateGlobalProxy(dependent);
            }

            // global.Output.EllipseLabel.X = centerX;
            // global.Output.EllipseLabel.Y = global.RectLabel.Rect.Bottom + global.Output.EllipseLabel.Height / 2 + 4;
        }

        protected override Size GetAutoScrollMinSize()
        {
            Size size = new Size(0, 0);

            if (m_State != null)
            {
                size.Width = System.Math.Max(m_State.RectLabel.Rect.Right, size.Width);
                size.Height = System.Math.Max(m_State.RectLabel.Rect.Bottom, size.Height);
                foreach (InputNode input in m_State.Inputs)
                {
                    size.Width = System.Math.Max(input.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(input.RectLabel.Rect.Bottom, size.Height);
                    size.Width = System.Math.Max(input.EllipseLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(input.EllipseLabel.Rect.Bottom, size.Height);
                }

                foreach (GlobalNodeProxy dependent in m_State.Dependents)
                {
                    size.Width = System.Math.Max(dependent.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(dependent.RectLabel.Rect.Bottom, size.Height);
                    size.Width = System.Math.Max(dependent.Output.EllipseLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(dependent.Output.EllipseLabel.Rect.Bottom, size.Height);
                }

                size.Width += MARGIN;
                size.Height += MARGIN;
                size.Width = (int)(size.Width * GraphScale);
                size.Height = (int)(size.Height * GraphScale);
                size.Width = System.Math.Max(ClientRectangle.Width, size.Width);
                size.Height = System.Math.Max(ClientRectangle.Height, size.Height);
            }

            return size;
        }

        protected override bool DoMouseDown(MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                Point point = new Point(e.Location.X - AutoScrollPosition.X, e.Location.Y - AutoScrollPosition.Y);
                if (GetScaledRectangle(m_State.RectLabel.Rect).Contains(point))
                {
                    m_Main.Explorer.SelectNode(m_State);
                    m_DragBehavior = new RenderStateNodeDragBehavior(m_State);
                    return true;
                }

                foreach (InputNode input in m_State.Inputs)
                {
                    if (GetScaledRectangle(input.RectLabel.Rect).Contains(point))
                    {
                        m_Main.Explorer.SelectNode(input);
                        return true;
                    }
                    else if (GetScaledRectangle(input.EllipseLabel.Rect).Contains(point))
                    {
                        List<OutputNode> allOutputs = new List<OutputNode>();
                        foreach (GlobalNodeProxy proxy in m_State.Dependents)
                        {
                            allOutputs.Add(proxy.Output);
                        }

                        m_DragBehavior = new InputNodeDragBehavior(input, allOutputs);
                        return true;
                    }
                    else if (input.UseAltBinding && GetScaledRectangle(input.AltEllipseLabel.Rect).Contains(point))
                    {
                        List<OutputNode> allOutputs = new List<OutputNode>();
                        foreach (GlobalNodeProxy proxy in m_State.Dependents)
                        {
                            allOutputs.Add(proxy.Output);
                        }

                        m_DragBehavior = new InputNodeAltDragBehavior(input, allOutputs);
                        return true;
                    }
                }

                foreach (GlobalNodeProxy proxy in m_State.Dependents)
                {
                    if (GetScaledRectangle(proxy.RectLabel.Rect).Contains(point))
                    {
                        m_Main.Explorer.SelectNode(proxy);
                        m_DragBehavior = new GlobalNodeProxyDragBehavior(proxy);
                        return true;
                    }
                }
            }

            return false;
        }

        protected override void Draw(Graphics graphics)
        {
            foreach (InputNode input in m_State.Inputs)
            {
                input.RectLabel.Draw(graphics);
                input.EllipseLabel.Draw(graphics);
                if (input.UseAltBinding)
                    input.AltEllipseLabel.Draw(graphics);

                if (input.Binding != null)
                {
                    Pen pen = new Pen(Color.Black, 2);
                    AdjustableArrowCap arrowCap = new AdjustableArrowCap(6 * GraphScale, 8 * GraphScale, true);
                    CustomLineCap custom = arrowCap;
                    pen.CustomStartCap = custom;
                    graphics.DrawLine(pen, input.EllipseLabel.X, input.EllipseLabel.Y, input.Binding.EllipseLabel.X, input.Binding.EllipseLabel.Y);
                }

                if (input.AltBinding != null)
                {
                    Pen pen = new Pen(Color.Black, 2);
                    AdjustableArrowCap arrowCap = new AdjustableArrowCap(6 * GraphScale, 8 * GraphScale, true);
                    CustomLineCap custom = arrowCap;
                    pen.CustomStartCap = custom;
                    graphics.DrawLine(pen, input.AltEllipseLabel.X, input.AltEllipseLabel.Y, input.AltBinding.EllipseLabel.X, input.AltBinding.EllipseLabel.Y);
                }
            }

            foreach (GlobalNodeProxy proxy in m_State.Dependents)
            {
                proxy.RectLabel.Draw(graphics);
                proxy.Output.EllipseLabel.Draw(graphics);
            }

            m_State.RectLabel.Draw(graphics);
            
        }
    }
}
