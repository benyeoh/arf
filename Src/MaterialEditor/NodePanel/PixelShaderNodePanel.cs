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
    public class PixelShaderNodePanel : NodePanel
    {
        private PixelShaderNode m_PS;
        
        public PixelShaderNodePanel(MainWin main)
            : base(main)
        {
        }

        public PixelShaderNode PS
        {
            get
            {
                return m_PS;
            }

            set
            {
                m_PS = (PixelShaderNode)value;
            }
        }

        private List<OutputNode> GetAllOutputs(object exceptThis)
        {
            List<OutputNode> allOutputs = new List<OutputNode>();
            foreach (GlobalNodeProxy proxy in m_PS.GlobalProxies)
            {
                if (proxy != exceptThis)
                    allOutputs.Add(proxy.Output);
            }

            foreach (FunctionOpNode op in m_PS.FunctionOps)
            {
                if (op != exceptThis)
                    allOutputs.Add(op.Output);
            }

            foreach (PixelInNode pixIn in m_PS.PixelIns)
            {
                if (pixIn != exceptThis)
                    allOutputs.Add(pixIn.Output);
            }

            foreach (AbstractionNodeProxy absProxy in m_PS.AbstractionProxies)
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
            foreach (FunctionOpNode op in m_PS.FunctionOps)
            {
                if (op != exceptThis)
                {
                    foreach (InputNode input in op.Inputs)
                    {
                        allInputs.Add(input);
                    }
                }
            }

            foreach (PixelOutNode pixOut in m_PS.PixelOuts)
            {
                if (pixOut != exceptThis)
                {
                    allInputs.Add(pixOut.Input);
                }
            }

            foreach (AbstractionNodeProxy absProxy in m_PS.AbstractionProxies)
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
            foreach (GlobalNodeProxy proxy in m_PS.GlobalProxies)
            {
                UpdateGlobalProxy(proxy);
            }

            foreach (FunctionOpNode op in m_PS.FunctionOps)
            {
                UpdateFunctionOp(op);
            }

            foreach (PixelInNode pixIn in m_PS.PixelIns)
            {
                UpdatePixelIn(pixIn);
            }

            foreach (PixelOutNode pixOut in m_PS.PixelOuts)
            {
                UpdatePixelOut(pixOut);
            }

            foreach (AbstractionNodeProxy absProxy in m_PS.AbstractionProxies)
            {
                UpdateAstractionProxy(absProxy);
            }

            // global.Output.EllipseLabel.X = centerX;
            // global.Output.EllipseLabel.Y = global.RectLabel.Rect.Bottom + global.Output.EllipseLabel.Height / 2 + 4;
        }

        protected override Size GetAutoScrollMinSize()
        {
            Size size = new Size(0, 0);

            if (m_PS != null)
            {

                foreach (GlobalNodeProxy proxy in m_PS.GlobalProxies)
                {
                    size.Width = System.Math.Max(proxy.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(proxy.RectLabel.Rect.Bottom, size.Height);
                    size.Width = System.Math.Max(proxy.Output.EllipseLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(proxy.Output.EllipseLabel.Rect.Bottom, size.Height);
                }

                foreach (FunctionOpNode op in m_PS.FunctionOps)
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

                foreach (PixelInNode pixIn in m_PS.PixelIns)
                {
                    size.Width = System.Math.Max(pixIn.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(pixIn.RectLabel.Rect.Bottom, size.Height);
                    size.Width = System.Math.Max(pixIn.Output.EllipseLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(pixIn.Output.EllipseLabel.Rect.Bottom, size.Height);
                }

                foreach (PixelOutNode pixOut in m_PS.PixelOuts)
                {
                    size.Width = System.Math.Max(pixOut.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(pixOut.RectLabel.Rect.Bottom, size.Height);

                    size.Width = System.Math.Max(pixOut.Input.RectLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(pixOut.Input.RectLabel.Rect.Bottom, size.Height);
                    size.Width = System.Math.Max(pixOut.Input.EllipseLabel.Rect.Right, size.Width);
                    size.Height = System.Math.Max(pixOut.Input.EllipseLabel.Rect.Bottom, size.Height);
                }


                foreach (AbstractionNodeProxy absProxy in m_PS.AbstractionProxies)
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

                foreach (GlobalNodeProxy proxy in m_PS.GlobalProxies)
                {
                    if (GetScaledRectangle(proxy.RectLabel.Rect).Contains(point))
                    {
                        m_Main.Explorer.SelectNode(proxy);
                        m_DragBehavior = new GlobalNodeProxyDragBehavior(proxy);
                        return true;
                    }
                }

                foreach (AbstractionNodeProxy absProxy in m_PS.AbstractionProxies)
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

                foreach (FunctionOpNode op in m_PS.FunctionOps)
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

                foreach (PixelInNode pixIn in m_PS.PixelIns)
                {
                    if (GetScaledRectangle(pixIn.RectLabel.Rect).Contains(point))
                    {
                        m_Main.Explorer.SelectNode(pixIn);
                        m_DragBehavior = new PixelInNodeDragBehavior(pixIn);
                        return true;
                    }
                }

                foreach (PixelOutNode pixOut in m_PS.PixelOuts)
                {
                    if (GetScaledRectangle(pixOut.RectLabel.Rect).Contains(point))
                    {
                        m_Main.Explorer.SelectNode(pixOut);
                        m_DragBehavior = new PixelOutNodeDragBehavior(pixOut);
                        return true;
                    }
                    else if (GetScaledRectangle(pixOut.Input.EllipseLabel.Rect).Contains(point))
                    {
                        List<OutputNode> allOutputs = GetAllOutputs(null);

                        m_DragBehavior = new InputNodeDragBehavior(pixOut.Input, allOutputs);
                        return true;
                    }
                }
            }

            return false;
        }

        protected override void Draw(Graphics graphics)
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

            foreach (GlobalNodeProxy proxy in m_PS.GlobalProxies)
            {
                proxy.RectLabel.Draw(graphics);
                proxy.Output.EllipseLabel.Draw(graphics);
            }

            foreach (FunctionOpNode op in m_PS.FunctionOps)
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

            foreach (PixelInNode pixIn in m_PS.PixelIns)
            {
                pixIn.RectLabel.Draw(graphics);
                pixIn.Output.EllipseLabel.Draw(graphics);
            }

            foreach (PixelOutNode pixOut in m_PS.PixelOuts)
            {
                pixOut.RectLabel.Draw(graphics);
                pixOut.Input.EllipseLabel.Draw(graphics);
            }

            foreach (AbstractionNodeProxy absProxy in m_PS.AbstractionProxies)
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
