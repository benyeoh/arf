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
    public class AbstractionNodePanel : NodePanel
    {
        private AbstractionNode m_Abstraction;

        public AbstractionNodePanel(MainWin main)
            : base(main)
        {
        }

        public AbstractionNode Abstraction
        {
            get
            {
                return m_Abstraction;
            }

            set
            {
                m_Abstraction = (AbstractionNode)value;
            }
        }

        private List<OutputNode> GetAllOutputs(object exceptThis)
        {
            List<OutputNode> allOutputs = new List<OutputNode>();

            foreach (FunctionOpNode op in m_Abstraction.Ops)
            {
                if (op != exceptThis)
                    allOutputs.Add(op.Output);
            }

            foreach (AbstractionInNode absIn in m_Abstraction.Ins)
            {
                if (absIn != exceptThis)
                    allOutputs.Add(absIn.Output);
            }

            foreach (AbstractionNodeProxy absProxy in m_Abstraction.Proxies)
            {
                if (absProxy != exceptThis)
                {
                    foreach (AbstractionNodeProxyOut absOut in absProxy.Outputs)
                    {
                        allOutputs.Add(absOut.Output);
                    }
                }
            }

            return allOutputs;
        }

        private List<InputNode> GetAllInputs(object exceptThis)
        {
            List<InputNode> allInputs = new List<InputNode>();
            foreach (FunctionOpNode op in m_Abstraction.Ops)
            {
                if (op != exceptThis)
                {
                    foreach (InputNode input in op.Inputs)
                    {
                        allInputs.Add(input);
                    }
                }
            }

            foreach (AbstractionOutNode absOut in m_Abstraction.Outs)
            {
                if (absOut != exceptThis)
                {
                    allInputs.Add(absOut.Input);
                }
            }

            foreach (AbstractionNodeProxy absProxy in m_Abstraction.Proxies)
            {
                if (absProxy != exceptThis)
                {
                    foreach (AbstractionNodeProxyIn absIn in absProxy.Inputs)
                    {
                        allInputs.Add(absIn.Input);
                    }
                }
            }

            return allInputs;
        }

        protected override void UpdatePositions()
        {
            foreach (AbstractionInNode absIn in m_Abstraction.Ins)
            {
                UpdateAbstractionIn(absIn);
            }

            foreach (FunctionOpNode op in m_Abstraction.Ops)
            {
                UpdateFunctionOp(op);
            }

            foreach (AbstractionOutNode absOut in m_Abstraction.Outs)
            {
                UpdateAbstractionOut(absOut);
            }

            foreach (AbstractionNodeProxy absProxy in m_Abstraction.Proxies)
            {
                UpdateAstractionProxy(absProxy);
            }
        }

        protected override Size GetAutoScrollMinSize()
        {
            Size size = new Size(0, 0);

            if (m_Abstraction != null)
            {
                foreach (FunctionOpNode op in m_Abstraction.Ops)
                {
                    size.Width = System.Math.Max(op.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(op.RectLabel.Rect.Bottom, size.Height);
                    size.Width = System.Math.Max(op.Output.EllipseLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(op.Output.EllipseLabel.Rect.Bottom, size.Height);
                    foreach (InputNode input in op.Inputs)
                    {
                        size.Width = System.Math.Max(input.RectLabel.Rect.Right, size.Width);
                        size.Height = System.Math.Max(input.RectLabel.Rect.Bottom, size.Height);
                        size.Width = System.Math.Max(input.EllipseLabel.Rect.Right, size.Width);
                        size.Height = System.Math.Max(input.EllipseLabel.Rect.Bottom, size.Height);
                    }
                }

                foreach (AbstractionInNode absIn in m_Abstraction.Ins)
                {
                    size.Width = System.Math.Max(absIn.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(absIn.RectLabel.Rect.Bottom, size.Height);
                    size.Width = System.Math.Max(absIn.Output.EllipseLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(absIn.Output.EllipseLabel.Rect.Bottom, size.Height);
                }

                foreach (AbstractionOutNode absOut in m_Abstraction.Outs)
                {
                    size.Width = System.Math.Max(absOut.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(absOut.RectLabel.Rect.Bottom, size.Height);

                    size.Width = System.Math.Max(absOut.Input.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(absOut.Input.RectLabel.Rect.Bottom, size.Height);
                    size.Width = System.Math.Max(absOut.Input.EllipseLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(absOut.Input.EllipseLabel.Rect.Bottom, size.Height);
                }

                foreach (AbstractionNodeProxy absProxy in m_Abstraction.Proxies)
                {
                    size.Width = System.Math.Max(absProxy.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(absProxy.RectLabel.Rect.Bottom, size.Height);

                    foreach (AbstractionNodeProxyIn absIn in absProxy.Inputs)
                    {
                        size.Width = System.Math.Max(absIn.Input.RectLabel.Rect.Right, size.Width);
                        size.Height = System.Math.Max(absIn.Input.RectLabel.Rect.Bottom, size.Height);
                        size.Width = System.Math.Max(absIn.Input.EllipseLabel.Rect.Right, size.Width);
                        size.Height = System.Math.Max(absIn.Input.EllipseLabel.Rect.Bottom, size.Height);
                    }

                    foreach (AbstractionNodeProxyOut absOut in absProxy.Outputs)
                    {
                        size.Width = System.Math.Max(absOut.RectLabel.Rect.Right, size.Width);
                        size.Height = System.Math.Max(absOut.RectLabel.Rect.Bottom, size.Height);
                        size.Width = System.Math.Max(absOut.Output.EllipseLabel.Rect.Right, size.Width);
                        size.Height = System.Math.Max(absOut.Output.EllipseLabel.Rect.Bottom, size.Height);
                    }
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

                foreach (FunctionOpNode op in m_Abstraction.Ops)
                {
                    if (GetScaledRectangle(op.RectLabel.Rect).Contains(point))
                    {
                        m_Main.Explorer.SelectNode(op);
                        m_DragBehavior = new FunctionOpNodeProxyDragBehavior(op);
                        return true;
                    }

                    foreach (InputNode input in op.Inputs)
                    {
                        if (GetScaledRectangle(input.RectLabel.Rect).Contains(point))
                        {
                            m_Main.Explorer.SelectNode(input);
                            return true;
                        }
                        else if (GetScaledRectangle(input.EllipseLabel.Rect).Contains(point))
                        {
                            List<OutputNode> allOutputs = GetAllOutputs(op);

                            m_DragBehavior = new InputNodeDragBehavior(input, allOutputs);
                            return true;
                        }
                        else if (input.UseAltBinding && GetScaledRectangle(input.AltEllipseLabel.Rect).Contains(point))
                        {
                            List<OutputNode> allOutputs = GetAllOutputs(op);

                            m_DragBehavior = new InputNodeAltDragBehavior(input, allOutputs);
                            return true;
                        }
                    }
                }

                foreach (AbstractionNodeProxy absProxy in m_Abstraction.Proxies)
                {
                    if (GetScaledRectangle(absProxy.RectLabel.Rect).Contains(point))
                    {
                        m_Main.Explorer.SelectNode(absProxy);
                        m_DragBehavior = new AbstractionNodeProxyDragBehavior(absProxy);
                        return true;
                    }

                    foreach (AbstractionNodeProxyIn absIn in absProxy.Inputs)
                    {
                        if (GetScaledRectangle(absIn.Input.RectLabel.Rect).Contains(point))
                        {
                            m_Main.Explorer.SelectNode(absIn.Input);
                            return true;
                        }
                        else if (GetScaledRectangle(absIn.Input.EllipseLabel.Rect).Contains(point))
                        {
                            List<OutputNode> allOutputs = GetAllOutputs(absProxy);

                            m_DragBehavior = new InputNodeDragBehavior(absIn.Input, allOutputs);
                            return true;
                        }
                        else if (absIn.Input.UseAltBinding && GetScaledRectangle(absIn.Input.AltEllipseLabel.Rect).Contains(point))
                        {
                            List<OutputNode> allOutputs = GetAllOutputs(absProxy);

                            m_DragBehavior = new InputNodeAltDragBehavior(absIn.Input, allOutputs);
                            return true;
                        }
                    }

                    foreach (AbstractionNodeProxyOut absOut in absProxy.Outputs)
                    {
                        if (GetScaledRectangle(absOut.RectLabel.Rect).Contains(point))
                        {
                            m_Main.Explorer.SelectNode(absOut);
                            return true;
                        }
                    }
                }

                foreach (AbstractionInNode absIn in m_Abstraction.Ins)
                {
                    if (GetScaledRectangle(absIn.RectLabel.Rect).Contains(point))
                    {
                        m_Main.Explorer.SelectNode(absIn);
                        m_DragBehavior = new AbstractionInDragBehavior(absIn);
                        return true;
                    }
                }

                foreach (AbstractionOutNode absOut in m_Abstraction.Outs)
                {
                    if (GetScaledRectangle(absOut.RectLabel.Rect).Contains(point))
                    {
                        m_Main.Explorer.SelectNode(absOut);
                        m_DragBehavior = new AbstractionOutDragBehavior(absOut);
                        return true;
                    }
                    else if (GetScaledRectangle(absOut.Input.EllipseLabel.Rect).Contains(point))
                    {
                        List<OutputNode> allOutputs = GetAllOutputs(null);

                        m_DragBehavior = new InputNodeDragBehavior(absOut.Input, allOutputs);
                        return true;
                    }
                }
            }

            return false;
        }

        protected override void Draw(Graphics graphics)
        {
            if (m_Abstraction != null)
            {
                Pen linePen = new Pen(Color.Black, 2);
                AdjustableArrowCap arrowCap = new AdjustableArrowCap(6 * GraphScale, 8 * GraphScale, true);
                CustomLineCap custom = arrowCap;
                linePen.CustomStartCap = custom;

                List<InputNode> allInputs = GetAllInputs(null);
                foreach (InputNode input in allInputs)
                {
                    if (input.Binding != null)
                    {
                        graphics.DrawLine(linePen, input.EllipseLabel.X, input.EllipseLabel.Y,
                            input.Binding.EllipseLabel.X, input.Binding.EllipseLabel.Y);
                    }

                    if (input.AltBinding != null)
                    {
                        graphics.DrawLine(linePen, input.AltEllipseLabel.X, input.AltEllipseLabel.Y,
                            input.AltBinding.EllipseLabel.X, input.AltBinding.EllipseLabel.Y);
                    }
                }

                foreach (FunctionOpNode op in m_Abstraction.Ops)
                {
                    op.RectLabel.Draw(graphics);
                    op.Output.EllipseLabel.Draw(graphics);

                    foreach (InputNode input in op.Inputs)
                    {
                        input.RectLabel.Draw(graphics);
                        input.EllipseLabel.Draw(graphics);
                        if (input.UseAltBinding)
                            input.AltEllipseLabel.Draw(graphics);
                    }
                }

                foreach (AbstractionInNode absIn in m_Abstraction.Ins)
                {
                    absIn.RectLabel.Draw(graphics);
                    absIn.Output.EllipseLabel.Draw(graphics);
                }

                foreach (AbstractionOutNode absOut in m_Abstraction.Outs)
                {
                    absOut.RectLabel.Draw(graphics);
                    absOut.Input.EllipseLabel.Draw(graphics);
                }

                foreach (AbstractionNodeProxy absProxy in m_Abstraction.Proxies)
                {
                    absProxy.RectLabel.Draw(graphics);

                    foreach (AbstractionNodeProxyIn absIn in absProxy.Inputs)
                    {
                        absIn.Input.RectLabel.Draw(graphics);
                        absIn.Input.EllipseLabel.Draw(graphics);
                        if (absIn.Input.UseAltBinding)
                            absIn.Input.AltEllipseLabel.Draw(graphics);
                    }

                    foreach (AbstractionNodeProxyOut absOut in absProxy.Outputs)
                    {
                        absOut.RectLabel.Draw(graphics);
                        absOut.Output.EllipseLabel.Draw(graphics);
                    }
                }


            }
        }

    }
}
