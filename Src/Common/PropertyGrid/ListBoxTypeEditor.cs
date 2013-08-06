using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Design;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.ComponentModel;

namespace Common.PropertyGrid
{
    public abstract class ListBoxTypeEditor : UITypeEditor
    {
        private IWindowsFormsEditorService m_Service;

        private int GetMaxHeight(ListBox listBox)
        {
            int maxHeight = listBox.GetItemHeight(0) * 25 + 4;
            if (maxHeight > listBox.PreferredHeight)
                maxHeight = listBox.PreferredHeight;
            return maxHeight;
        }
        
        public abstract object[] GetListBoxValues();

        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return UITypeEditorEditStyle.DropDown;
        }

        public override object EditValue(System.ComponentModel.ITypeDescriptorContext context, System.IServiceProvider provider, object value)
        {
            m_Service = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));

            if (m_Service != null)
            {
                ListBox listBox = new ListBox();
                listBox.SelectionMode = SelectionMode.One;
                listBox.IntegralHeight = true;

                listBox.Items.AddRange(GetListBoxValues());
                listBox.Height = GetMaxHeight(listBox);

                if (listBox.Items.Contains(value))
                {
                    listBox.SetSelected(listBox.Items.IndexOf(value), true);
                }

                listBox.SelectedIndexChanged += new EventHandler(listBox_SelectedIndexChanged);
                m_Service.DropDownControl(listBox);
                
                if (listBox.SelectedItem == null) 
                { 
                    return value; 
                }
               
                return listBox.SelectedItem;
            }

            return value;

        }

        void listBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            // Once there is something selected, close the listbox
            m_Service.CloseDropDown();
        }

    }
}
