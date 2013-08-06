using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace MaterialEditor
{
    public partial class CodeOutputWin : DockContent
    {
        private MainWin m_Main;

        public CodeOutputWin(MainWin main)
        {
            m_Main = main;
            InitializeComponent();

            numberLabel.Font = new Font(textBox.Font.FontFamily,
                              textBox.Font.Size);
        }

        public void SetText(string text)
        {
            textBox.Text = text;
            UpdateNumberLabel();
        }

        private void UpdateNumberLabel()
        {
            //we get index of first visible char and 

            //number of first visible line

            Point pos = new Point(0, 0);
            //int firstIndex = textBox.GetCharIndexFromPosition(pos);
            int firstLine = 0;// textBox.GetLineFromCharIndex(0);

            //now we get index of last visible char 

            //and number of last visible line

            pos.X = ClientRectangle.Width;
            pos.Y = ClientRectangle.Height;
            //int lastIndex = textBox.GetCharIndexFromPosition(pos);
            int lastLine = textBox.GetLineFromCharIndex(textBox.TextLength-1);

            //this is point position of last visible char, we'll 

            //use its Y value for calculating numberLabel size

            //pos = textBox.GetPositionFromCharIndex(lastIndex);

            //finally, renumber label

            numberLabel.SuspendLayout();
            numberLabel.Text = "";
            for (int i = firstLine; i <= lastLine + 1; i++)
            {
                numberLabel.Text += i + 1 + "\n";
            }
            numberLabel.ResumeLayout();
            numberLabel.Location = new Point(numberLabel.Parent.ClientSize.Width - numberLabel.Size.Width, 0);
        }

        private void textBox_VScroll(object sender, EventArgs e)
        {
            int d = textBox.GetPositionFromCharIndex(0).Y;
            numberLabel.Location = new Point(numberLabel.Parent.ClientSize.Width - numberLabel.Size.Width, d);

           // UpdateNumberLabel();
        }

        //public TextBox TextBox
        //{
        //    get
        //    {
        //        return textBox;
        //    }
        //}
    }
}