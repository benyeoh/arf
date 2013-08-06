using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Design;
using System.ComponentModel;
using System.Windows.Forms;
using System.Windows.Forms.Design;

namespace MaterialEditor
{
    class DropDownTextAreaEditor : UITypeEditor
    {
        private EditorTextBox m_TextArea;

        public DropDownTextAreaEditor()
        {
        }

        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return UITypeEditorEditStyle.DropDown;
        }

        public override object EditValue(ITypeDescriptorContext context, 
            IServiceProvider provider, 
            object value)
        {
            IWindowsFormsEditorService service = 
                (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
            
            if(m_TextArea == null)
                m_TextArea = new EditorTextBox();

            m_TextArea.TextArea.Text = (string)value;

            service.DropDownControl(m_TextArea);

            return m_TextArea.TextArea.Text.Trim();

            //if(m_TextArea == null || m_TextArea.IsDisposed())
            //{
            //    m_TextArea = new TextAreaWin();
            //    TextAreaCreated.Invoke(this, m_TextArea);
            //}

            //m_TextArea.Show(m_Owner);

            //return base.EditValue(context, provider, value);
        }
    }
}
