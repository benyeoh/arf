using System;
using System.Collections.Generic;
using System.Text;
using HarmonyManaged.Common;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Drawing;
using HarmonyManaged.MaterialGenManaged;

namespace MaterialEditor
{
    [Serializable]
    public class InputAbstractionNodeNode : PropertyNode, ISerializable, IInput
    {
        private string m_Name;
        private string m_Type;
        private string m_DefaultValue;

        private OutputNode m_Binding;

        private EllipseLabel m_EllipseLabel;
        private RectLabel m_RectLabel;

        public InputAbstractionNodeNode(string defaultName)
            : base()
        {
            m_Name = defaultName;
            m_Type = "";
            m_DefaultValue = "";

            m_EllipseLabel = new EllipseLabel();
            m_EllipseLabel.BackColor = Color.OrangeRed;
            m_EllipseLabel.Width = 12;
            m_EllipseLabel.Height = 12;

            m_RectLabel = new RectLabel();
            m_RectLabel.Font = new Font("Verdana", 8);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = m_DefaultValue.Length > 0 ? Color.PapayaWhip : Color.PeachPuff;
            m_RectLabel.Text = m_Type + "\n" + m_Name;
        }

        #region Serialization
        protected InputNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Name = info.GetString("Name");
            m_Type = info.GetString("Type");
            m_DefaultValue = info.GetString("Default Value");
            m_Binding = (OutputNode)info.GetValue("Binding", typeof(OutputNode));
            m_EllipseLabel = (EllipseLabel)info.GetValue("EllipseLabel", typeof(EllipseLabel));
            m_RectLabel = (RectLabel)info.GetValue("RectLabel", typeof(RectLabel));
            m_RectLabel.Font = new Font("Verdana", 8);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = m_DefaultValue.Length > 0 ? Color.PapayaWhip : Color.PeachPuff;

            ReadOnly = info.GetBoolean("ReadOnly");
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Type", Type);
            info.AddValue("Default Value", DefaultValue);
            info.AddValue("Binding", Binding);
            info.AddValue("EllipseLabel", m_EllipseLabel);
            info.AddValue("RectLabel", m_RectLabel);
            info.AddValue("ReadOnly", m_ReadOnly);
        }
        #endregion

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Input", "Name of the input", m_Name,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Type", typeof(string), "Input", "Type of the input", m_Type,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Default Value", typeof(string), "Input", "Default value of the input", m_DefaultValue,
                typeof(DropDownTextAreaEditor), (string)null));
        }

        protected override void PropertyGetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertyGetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": e.Value = m_Name; break;
                case "Type": e.Value = m_Type; break;
                case "Default Value": e.Value = m_DefaultValue; break;
            }
        }

        protected override void PropertySetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertySetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": m_Name = (string)e.Value; m_RectLabel.Text = m_Type + "\n" + m_Name; break;
                case "Type": m_Type = (string)e.Value; m_RectLabel.Text = m_Type + "\n" + m_Name; break;
                case "Default Value": m_DefaultValue = (string)e.Value; m_RectLabel.BackColor = m_DefaultValue.Length > 0 ? Color.PapayaWhip : Color.PeachPuff; break;
            }
        }

        public string Name
        {
            get { return m_Name; }
            set
            {
                m_Name = (string)value;
                m_RectLabel.Text = m_Type + "\n" + m_Name;
            }
        }

        public string Type
        {
            get { return m_Type; }
            set
            {
                m_Type = (string)value;
                m_RectLabel.Text = m_Type + "\n" + m_Name;
            }
        }

        public string DefaultValue
        {
            get { return m_DefaultValue; }
            set
            {
                m_DefaultValue = (string)value;
                m_RectLabel.BackColor = m_DefaultValue.Length > 0 ? Color.PapayaWhip : Color.PeachPuff;
            }
        }

        public OutputNode Binding
        {
            get
            {
                return m_Binding;
            }

            set
            {
                m_Binding = (OutputNode)value;
            }
        }

        public EllipseLabel EllipseLabel
        {
            get
            {
                return m_EllipseLabel;
            }
        }

        public RectLabel RectLabel
        {
            get
            {
                return m_RectLabel;
            }
        }

        #region Material Gen
        private InputResource m_Impl;
        public void BeginMaterialGen(InputResource input)
        {
            m_Impl = input;
            //m_Proxy.BeginMaterialGen(gen);
        }

        public void ProcessBinding()
        {
            m_Impl.BindValue(Binding.Impl);
        }

        public void EndMaterialGen()
        {
            m_Impl.Dispose();
            m_Impl = null;
        }

        public InputResource Impl
        {
            get
            {
                return m_Impl;
            }
        }
        #endregion
    }
}
