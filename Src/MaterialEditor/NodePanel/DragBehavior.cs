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
    public interface DragBehavior
    {
        void BeginDrag(Point pos, float scale);
        void PerformDrag(Point pos, float scale);
        void EndDrag(Point pos, float scale);
        void Draw(Graphics graphics);
    }
}
