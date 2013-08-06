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
    public partial class NodePanel : UserControl
    {
        protected const int MARGIN = 256;
        protected DragBehavior m_DragBehavior;
        protected MainWin m_Main;
        protected float m_Scale = 1.0f;
        
        public NodePanel(MainWin main)
        {
            m_Main = main; 
            InitializeComponent();
            DoubleBuffered = true;
        }

        public float GraphScale
        {
            get
            {
                return m_Scale;
            }

            set
            {
                m_Scale = (float)value;
                Invalidate(true);
            }
        }

        protected void UpdateAstractionProxy(AbstractionNodeProxy absProxy)
        {
            if (absProxy.RectLabel.X < -999)
            {
                // Reset if invalid position
                absProxy.RectLabel.X = (int)((ClientRectangle.Width / 2 - AutoScrollPosition.X) / GraphScale);
                absProxy.RectLabel.Y = (int)((ClientRectangle.Height / 2 - AutoScrollPosition.Y) / GraphScale);
            }

            // Inputs
            int centerX = absProxy.RectLabel.X + absProxy.RectLabel.Rect.Width / 2;
            Size totalSize = new Size(0, 0);
            foreach (AbstractionNodeProxyIn absIn in absProxy.Inputs)
            {
                totalSize.Width += absIn.Input.RectLabel.Rect.Width;
                totalSize.Height = System.Math.Max(absIn.Input.RectLabel.Rect.Height, totalSize.Height);
            }

            int widthSoFar = centerX - totalSize.Width / 2;
            int top = absProxy.RectLabel.Y - totalSize.Height;
            foreach (AbstractionNodeProxyIn absIn in absProxy.Inputs)
            {
                absIn.Input.RectLabel.X = widthSoFar;
                absIn.Input.RectLabel.Y = top - 4;
                widthSoFar += absIn.Input.RectLabel.Rect.Width;

                if (absIn.Input.UseAltBinding)
                {
                    absIn.Input.EllipseLabel.X = absIn.Input.RectLabel.X + absIn.Input.RectLabel.Rect.Width / 2 - absIn.Input.EllipseLabel.Width / 2 - 2;
                    absIn.Input.EllipseLabel.Y = absIn.Input.RectLabel.Y - absIn.Input.EllipseLabel.Rect.Height / 2 - 4;

                    absIn.Input.AltEllipseLabel.X = absIn.Input.RectLabel.X + absIn.Input.RectLabel.Rect.Width / 2 + absIn.Input.EllipseLabel.Width / 2 + 2;
                    absIn.Input.AltEllipseLabel.Y = absIn.Input.RectLabel.Y - absIn.Input.EllipseLabel.Rect.Height / 2 - 4;

                }
                else
                {
                    absIn.Input.EllipseLabel.X = absIn.Input.RectLabel.X + absIn.Input.RectLabel.Rect.Width / 2;
                    absIn.Input.EllipseLabel.Y = absIn.Input.RectLabel.Y - absIn.Input.EllipseLabel.Rect.Height / 2 - 4;

                    absIn.Input.AltEllipseLabel.X = -999;
                    absIn.Input.AltEllipseLabel.Y = -999;
                }

                //absIn.Input.EllipseLabel.X = absIn.Input.RectLabel.X + absIn.Input.RectLabel.Rect.Width / 2;
                //absIn.Input.EllipseLabel.Y = absIn.Input.RectLabel.Y - absIn.Input.EllipseLabel.Rect.Height / 2 - 4;
            }

            // Outputs
            totalSize = new Size(0, 0);
            foreach (AbstractionNodeProxyOut absOut in absProxy.Outputs)
            {
                totalSize.Width += absOut.RectLabel.Rect.Width;
                totalSize.Height = System.Math.Max(absOut.RectLabel.Rect.Height, totalSize.Height);
            }

            widthSoFar = centerX - totalSize.Width / 2;
            int bottom = absProxy.RectLabel.Rect.Bottom;
            foreach (AbstractionNodeProxyOut absOut in absProxy.Outputs)
            {
                absOut.RectLabel.X = widthSoFar;
                absOut.RectLabel.Y = bottom + 4;
                widthSoFar += absOut.RectLabel.Rect.Width;

                absOut.Output.EllipseLabel.X = absOut.RectLabel.X + absOut.RectLabel.Rect.Width / 2;
                absOut.Output.EllipseLabel.Y = absOut.RectLabel.Rect.Bottom + absOut.Output.EllipseLabel.Rect.Height / 2 + 4;
            }
        }

        protected void UpdateFunctionOp(FunctionOpNode op)
        {
            if (op.RectLabel.X < -999)
            {
                // Reset if invalid position
                op.RectLabel.X = (int)((ClientRectangle.Width / 2 - AutoScrollPosition.X) / GraphScale);
                op.RectLabel.Y = (int)((ClientRectangle.Height / 2 - AutoScrollPosition.Y) / GraphScale);
            }

            int centerX = op.RectLabel.X + op.RectLabel.Rect.Width / 2;
            Size totalSize = new Size(0, 0);
            foreach (InputNode input in op.Inputs)
            {
                totalSize.Width += input.RectLabel.Rect.Width;
                totalSize.Height = System.Math.Max(input.RectLabel.Rect.Height, totalSize.Height);
            }

            int widthSoFar = centerX - totalSize.Width / 2;
            int top = op.RectLabel.Y - totalSize.Height;
            foreach (InputNode input in op.Inputs)
            {
                input.RectLabel.X = widthSoFar;
                input.RectLabel.Y = top - 4;
                widthSoFar += input.RectLabel.Rect.Width;

                if (input.UseAltBinding)
                {
                    input.EllipseLabel.X = input.RectLabel.X + input.RectLabel.Rect.Width / 2 - input.EllipseLabel.Width/2 - 2;
                    input.EllipseLabel.Y = input.RectLabel.Y - input.EllipseLabel.Rect.Height / 2 - 4;

                    input.AltEllipseLabel.X = input.RectLabel.X + input.RectLabel.Rect.Width / 2 + input.EllipseLabel.Width / 2 + 2;
                    input.AltEllipseLabel.Y = input.RectLabel.Y - input.EllipseLabel.Rect.Height / 2 - 4;
 
                }
                else
                {
                    input.EllipseLabel.X = input.RectLabel.X + input.RectLabel.Rect.Width / 2;
                    input.EllipseLabel.Y = input.RectLabel.Y - input.EllipseLabel.Rect.Height / 2 - 4;

                    input.AltEllipseLabel.X = -999;
                    input.AltEllipseLabel.Y = -999;
                }
            }

            op.Output.EllipseLabel.X = centerX;
            op.Output.EllipseLabel.Y = op.RectLabel.Rect.Bottom + op.Output.EllipseLabel.Height / 2 + 4;
        }


        protected void UpdateVertexOut(VertexOutNode vertOut)
        {
            //int centerX = vertOut.RectLabel.X + vertOut.RectLabel.Rect.Width / 2;
            //Size totalSize = new Size(0, 0);

            //totalSize.Width += vertOut.Input.RectLabel.Rect.Width;
            //totalSize.Height = System.Math.Max(vertOut.Input.RectLabel.Rect.Height, totalSize.Height);

            //int widthSoFar = centerX - totalSize.Width / 2;
            //int top = vertOut.RectLabel.Y - totalSize.Height;

            //vertOut.Input.RectLabel.X = widthSoFar;
            //vertOut.Input.RectLabel.Y = top - 4;
            //widthSoFar += vertOut.Input.RectLabel.Rect.Width;

            if (vertOut.RectLabel.X < -999)
            {
                // Reset if invalid position
                vertOut.RectLabel.X = (int)((ClientRectangle.Width / 2 - AutoScrollPosition.X) / GraphScale);
                vertOut.RectLabel.Y = (int)((ClientRectangle.Height / 2 - AutoScrollPosition.Y) / GraphScale);
            }

            vertOut.Input.EllipseLabel.X = vertOut.RectLabel.X + vertOut.RectLabel.Rect.Width / 2;
            vertOut.Input.EllipseLabel.Y = vertOut.RectLabel.Y - vertOut.Input.EllipseLabel.Rect.Height / 2 - 4;
        }

        protected void UpdateVertexIn(VertexInNode vertIn)
        {
            if (vertIn.RectLabel.X < -999)
            {
                // Reset if invalid position
                vertIn.RectLabel.X = (int)((ClientRectangle.Width / 2 - AutoScrollPosition.X) / GraphScale);
                vertIn.RectLabel.Y = (int)((ClientRectangle.Height / 2 - AutoScrollPosition.Y) / GraphScale);
            }

            int centerX = vertIn.RectLabel.X + vertIn.RectLabel.Rect.Width / 2;
            vertIn.Output.EllipseLabel.X = centerX;
            vertIn.Output.EllipseLabel.Y = vertIn.RectLabel.Rect.Bottom + vertIn.Output.EllipseLabel.Height / 2 + 4;
        }

        protected void UpdatePixelOut(PixelOutNode pixOut)
        {
            //int centerX = pixOut.RectLabel.X + pixOut.RectLabel.Rect.Width / 2;
            //Size totalSize = new Size(0, 0);

            //totalSize.Width += pixOut.Input.RectLabel.Rect.Width;
            //totalSize.Height = System.Math.Max(pixOut.Input.RectLabel.Rect.Height, totalSize.Height);

            //int widthSoFar = centerX - totalSize.Width / 2;
            //int top = pixOut.RectLabel.Y - totalSize.Height;

            //pixOut.Input.RectLabel.X = widthSoFar;
            //pixOut.Input.RectLabel.Y = top - 4;
            //widthSoFar += pixOut.Input.RectLabel.Rect.Width;
            if (pixOut.RectLabel.X < -999)
            {
                // Reset if invalid position
                pixOut.RectLabel.X = (int)((ClientRectangle.Width / 2 - AutoScrollPosition.X) / GraphScale);
                pixOut.RectLabel.Y = (int)((ClientRectangle.Height / 2 - AutoScrollPosition.Y) / GraphScale);
            }

            pixOut.Input.EllipseLabel.X = pixOut.RectLabel.X + pixOut.RectLabel.Rect.Width / 2;
            pixOut.Input.EllipseLabel.Y = pixOut.RectLabel.Y - pixOut.Input.EllipseLabel.Rect.Height / 2 - 4;
        }

        protected void UpdatePixelIn(PixelInNode pixIn)
        {
            if (pixIn.RectLabel.X < -999)
            {
                // Reset if invalid position
                pixIn.RectLabel.X = (int)((ClientRectangle.Width / 2 - AutoScrollPosition.X) / GraphScale);
                pixIn.RectLabel.Y = (int)((ClientRectangle.Height / 2 - AutoScrollPosition.Y) / GraphScale);
            }

            int centerX = pixIn.RectLabel.X + pixIn.RectLabel.Rect.Width / 2;
            pixIn.Output.EllipseLabel.X = centerX;
            pixIn.Output.EllipseLabel.Y = pixIn.RectLabel.Rect.Bottom + pixIn.Output.EllipseLabel.Height / 2 + 4;
        }

        protected void UpdateAbstractionOut(AbstractionOutNode absOut)
        {
            if (absOut.RectLabel.X < -999)
            {
                // Reset if invalid position
                absOut.RectLabel.X = (int)((ClientRectangle.Width / 2 - AutoScrollPosition.X) / GraphScale);
                absOut.RectLabel.Y = (int)((ClientRectangle.Height / 2 - AutoScrollPosition.Y) / GraphScale);
            }

            absOut.Input.EllipseLabel.X = absOut.RectLabel.X + absOut.RectLabel.Rect.Width / 2;
            absOut.Input.EllipseLabel.Y = absOut.RectLabel.Y - absOut.Input.EllipseLabel.Rect.Height / 2 - 4;
        }

        protected void UpdateAbstractionIn(AbstractionInNode absIn)
        {
            if (absIn.RectLabel.X < -999)
            {
                // Reset if invalid position
                absIn.RectLabel.X = (int)((ClientRectangle.Width / 2 - AutoScrollPosition.X) / GraphScale);
                absIn.RectLabel.Y = (int)((ClientRectangle.Height / 2 - AutoScrollPosition.Y) / GraphScale);
            }

            int centerX = absIn.RectLabel.X + absIn.RectLabel.Rect.Width / 2;
            absIn.Output.EllipseLabel.X = centerX;
            absIn.Output.EllipseLabel.Y = absIn.RectLabel.Rect.Bottom + absIn.Output.EllipseLabel.Height / 2 + 4;
        }

        protected void UpdateGlobalProxy(GlobalNodeProxy proxy)
        {
            if (proxy.RectLabel.X < -999)
            {
                // Reset if invalid position
                proxy.RectLabel.X = (int)((ClientRectangle.Width / 2 - AutoScrollPosition.X) / GraphScale);
                proxy.RectLabel.Y = (int)((ClientRectangle.Height / 2 - AutoScrollPosition.Y) / GraphScale);
            }

            int centerX = proxy.RectLabel.X + proxy.RectLabel.Rect.Width / 2;
            proxy.Output.EllipseLabel.X = centerX;
            proxy.Output.EllipseLabel.Y = proxy.RectLabel.Rect.Bottom + proxy.Output.EllipseLabel.Height / 2 + 4;
        }


        protected Rectangle GetScaledRectangle(Rectangle rect)
        {
            Rectangle newRect = new Rectangle((int)(rect.X * GraphScale),
                                            (int)(rect.Y * GraphScale),
                                            (int)(rect.Width * GraphScale),
                                            (int)(rect.Height * GraphScale));
            return newRect;
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);

            Point point = new Point(e.Location.X - AutoScrollPosition.X, e.Location.Y - AutoScrollPosition.Y);
                    
            if(!DoMouseDown(e))
            {
                if (e.Button == MouseButtons.Right)
                    m_DragBehavior = new ScrollDragBehavior(this);
            }

            if(m_DragBehavior != null)
                m_DragBehavior.BeginDrag(point, GraphScale);

            Invalidate();
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);

            if (m_DragBehavior != null)
            {
                Point point = new Point(e.Location.X - AutoScrollPosition.X, e.Location.Y - AutoScrollPosition.Y);
                m_DragBehavior.PerformDrag(point, GraphScale);
                Invalidate();
            }
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);
            Point point = new Point(e.Location.X - AutoScrollPosition.X, e.Location.Y - AutoScrollPosition.Y);
            if (m_DragBehavior != null)
            {
                m_DragBehavior.EndDrag(point, GraphScale);
                m_DragBehavior = null;
            }

            Invalidate();
        }

        protected void UpdateAutoScrollMinSize()
        {
            Size size = GetAutoScrollMinSize();
            if (AutoScrollMinSize != size)
                AutoScrollMinSize = size;            
        }

        protected override void OnEnter(EventArgs e)
        {
            base.OnEnter(e);
        }

        protected override void OnSizeChanged(EventArgs e)
        {
            SuspendLayout();

            base.OnSizeChanged(e);
            UpdateAutoScrollMinSize();

            ResumeLayout();

            Invalidate(true);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            SuspendLayout();

            UpdatePositions();
            UpdateAutoScrollMinSize();

            ResumeLayout();
            
            base.OnPaint(e);
   
            Graphics graphics = e.Graphics;
            graphics.SmoothingMode = SmoothingMode.AntiAlias;

            Matrix offset = new Matrix(GraphScale, 0, 0, GraphScale, AutoScrollPosition.X, AutoScrollPosition.Y);
            graphics.Transform = offset;

            Draw(graphics);

            if (m_DragBehavior != null)
                 m_DragBehavior.Draw(graphics);
        }

        protected virtual Size GetAutoScrollMinSize() { return new Size(0, 0); }
        protected virtual void UpdatePositions() {}
        protected virtual void Draw(Graphics graphics) {}
        protected virtual bool DoMouseDown(MouseEventArgs e) { return false; }
    }
}
