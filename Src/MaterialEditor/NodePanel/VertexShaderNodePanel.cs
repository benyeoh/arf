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
    public class VertexShaderNodePanel : NodePanel
    {
        private VertexShaderNode m_VS;

        public VertexShaderNodePanel(MainWin main)
            : base(main)
        {
        }

        public VertexShaderNode VS
        {
            get
            {
                return m_VS;
            }

            set
            {
                m_VS = (VertexShaderNode)value;
            }
        }

        private List<OutputNode> GetAllOutputs(object exceptThis)
        {
            List<OutputNode> allOutputs = new List<OutputNode>();
            foreach (GlobalNodeProxy proxy in m_VS.GlobalProxies)
            {
                if (proxy != exceptThis)
                    allOutputs.Add(proxy.Output);
            }

            foreach (FunctionOpNode op in m_VS.FunctionOps)
            {
                if (op != exceptThis)
                    allOutputs.Add(op.Output);
            }

            foreach (VertexInNode vertIn in m_VS.VertexIns)
            {
                if (vertIn != exceptThis)
                    allOutputs.Add(vertIn.Output);
            }

            foreach (AbstractionNodeProxy absProxy in m_VS.AbstractionProxies)
            {
                if (absProxy != exceptThis)
                {
                    foreach(AbstractionNodeProxyOut absOut in absProxy.Outputs)
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
            foreach (FunctionOpNode op in m_VS.FunctionOps)
            {
                if (op != exceptThis)
                {
                    foreach (InputNode input in op.Inputs)
                    {
                        allInputs.Add(input);
                    }
                }
            }

            foreach (VertexOutNode vertOut in m_VS.VertexOuts)
            {
                if (vertOut != exceptThis)
                {
                    allInputs.Add(vertOut.Input);
                }
            }

            foreach (AbstractionNodeProxy absProxy in m_VS.AbstractionProxies)
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
            foreach (GlobalNodeProxy proxy in m_VS.GlobalProxies)
            {
                UpdateGlobalProxy(proxy);
            }

            foreach (FunctionOpNode op in m_VS.FunctionOps)
            {
                UpdateFunctionOp(op);
            }

            foreach (VertexInNode vertIn in m_VS.VertexIns)
            {
                UpdateVertexIn(vertIn);
            }

            foreach (VertexOutNode vertOut in m_VS.VertexOuts)
            {
                UpdateVertexOut(vertOut);
            }

            foreach(AbstractionNodeProxy absProxy in m_VS.AbstractionProxies)
            {
                UpdateAstractionProxy(absProxy);
            }
            // global.Output.EllipseLabel.X = centerX;
            // global.Output.EllipseLabel.Y = global.RectLabel.Rect.Bottom + global.Output.EllipseLabel.Height / 2 + 4;
        }

        protected override Size GetAutoScrollMinSize()
        {
            Size size = new Size(0, 0);

            if (m_VS != null)
            {
                foreach (GlobalNodeProxy proxy in m_VS.GlobalProxies)
                {
                    size.Width = System.Math.Max(proxy.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(proxy.RectLabel.Rect.Bottom, size.Height);
                    size.Width = System.Math.Max(proxy.Output.EllipseLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(proxy.Output.EllipseLabel.Rect.Bottom, size.Height);
                }

                foreach (FunctionOpNode op in m_VS.FunctionOps)
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

                foreach (VertexInNode vertIn in m_VS.VertexIns)
                {
                    size.Width = System.Math.Max(vertIn.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(vertIn.RectLabel.Rect.Bottom, size.Height);
                    size.Width = System.Math.Max(vertIn.Output.EllipseLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(vertIn.Output.EllipseLabel.Rect.Bottom, size.Height);
                }

                foreach (VertexOutNode vertOut in m_VS.VertexOuts)
                {
                    size.Width = System.Math.Max(vertOut.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(vertOut.RectLabel.Rect.Bottom, size.Height);

                    size.Width = System.Math.Max(vertOut.Input.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(vertOut.Input.RectLabel.Rect.Bottom, size.Height);
                    size.Width = System.Math.Max(vertOut.Input.EllipseLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(vertOut.Input.EllipseLabel.Rect.Bottom, size.Height);
                }

                foreach (AbstractionNodeProxy absProxy in m_VS.AbstractionProxies)
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

                foreach (GlobalNodeProxy proxy in m_VS.GlobalProxies)
                {
                    if (GetScaledRectangle(proxy.RectLabel.Rect).Contains(point))
                    {
                        m_Main.Explorer.SelectNode(proxy);
                        m_DragBehavior = new GlobalNodeProxyDragBehavior(proxy);
                        return true;
                    }
                }

                foreach (FunctionOpNode op in m_VS.FunctionOps)
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

                foreach (AbstractionNodeProxy absProxy in m_VS.AbstractionProxies)
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

                foreach (VertexInNode vertIn in m_VS.VertexIns)
                {
                    if (GetScaledRectangle(vertIn.RectLabel.Rect).Contains(point))
                    {
                        m_Main.Explorer.SelectNode(vertIn);
                        m_DragBehavior = new VertexInNodeDragBehavior(vertIn);
                        return true;
                    }
                }

                foreach (VertexOutNode vertOut in m_VS.VertexOuts)
                {
                    if (GetScaledRectangle(vertOut.RectLabel.Rect).Contains(point))
                    {
                        m_Main.Explorer.SelectNode(vertOut);
                        m_DragBehavior = new VertexOutNodeDragBehavior(vertOut);
                        m_DragBehavior.BeginDrag(point, GraphScale);
                        return true;
                    }
                    else if (GetScaledRectangle(vertOut.Input.EllipseLabel.Rect).Contains(point))
                    {
                        List<OutputNode> allOutputs = GetAllOutputs(null);

                        m_DragBehavior = new InputNodeDragBehavior(vertOut.Input, allOutputs);
                        return true;
                    }
                }
            }

            return false;
        }

        protected override void Draw(Graphics graphics)
        {
            if (m_VS != null)
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

                foreach (GlobalNodeProxy proxy in m_VS.GlobalProxies)
                {
                    proxy.RectLabel.Draw(graphics);
                    proxy.Output.EllipseLabel.Draw(graphics);
                }

                foreach (FunctionOpNode op in m_VS.FunctionOps)
                {
                    op.RectLabel.Draw(graphics);
                    op.Output.EllipseLabel.Draw(graphics);

                    foreach (InputNode input in op.Inputs)
                    {
                        input.RectLabel.Draw(graphics);
                        input.EllipseLabel.Draw(graphics);
                        input.AltEllipseLabel.Draw(graphics);
                    }
                }

                foreach (VertexInNode vertIn in m_VS.VertexIns)
                {
                    vertIn.RectLabel.Draw(graphics);
                    vertIn.Output.EllipseLabel.Draw(graphics);
                }

                foreach (VertexOutNode vertOut in m_VS.VertexOuts)
                {
                    vertOut.RectLabel.Draw(graphics);
                    vertOut.Input.EllipseLabel.Draw(graphics);
                }

                foreach(AbstractionNodeProxy absProxy in m_VS.AbstractionProxies)
                {
                    absProxy.RectLabel.Draw(graphics);

                    foreach(AbstractionNodeProxyIn absIn in absProxy.Inputs)
                    {
                        absIn.Input.RectLabel.Draw(graphics);
                        absIn.Input.EllipseLabel.Draw(graphics);
                        if (absIn.Input.UseAltBinding)
                            absIn.Input.AltEllipseLabel.Draw(graphics);
                    }

                    foreach(AbstractionNodeProxyOut absOut in absProxy.Outputs)
                    {
                        absOut.RectLabel.Draw(graphics);
                        absOut.Output.EllipseLabel.Draw(graphics);
                    }
                }

                
            }
        }

    }
}
