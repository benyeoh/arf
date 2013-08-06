using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace MaterialEditor
{
    public partial class EditorTextBox : UserControl
    {
        public EditorTextBox()
        {
            InitializeComponent();
        }

        public TextBox TextArea
        {
            get { return textBox; }
        }

        private void textBox_TextChanged(object sender, EventArgs e)
        {
            // Auto hide and display scrollbars
            int scrollBarVal = 0;
            Size textSize = TextRenderer.MeasureText(TextArea.Text, TextArea.Font);
            if(textSize.Height > TextArea.ClientSize.Height)
            {
                scrollBarVal = (int)ScrollBars.Vertical;
            }

            if (textSize.Width > TextArea.ClientSize.Width)
            {
                scrollBarVal += (int)ScrollBars.Horizontal;
            }

            if (TextArea.ScrollBars != (ScrollBars)scrollBarVal)
                TextArea.ScrollBars = (ScrollBars)scrollBarVal;
        }
    }
}
