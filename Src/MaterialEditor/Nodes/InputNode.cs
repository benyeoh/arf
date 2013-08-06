using System;
using System.Collections.Generic;
using System.Text;
using Common.PropertyGrid;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Drawing;
using ARFManaged.MaterialGenManaged;
using System.ComponentModel;

namespace MaterialEditor
{
    [Serializable]
    public class InputNode : PropertyNode, ISerializable, IInput, ICloneable
    {
        private string m_Name;
        private string m_Type;
        private string m_DefaultValue;
        private bool m_UseAltBinding;
        private bool m_IsInputInstanced;

        private OutputNode m_Binding;
        private OutputNode m_AltBinding;

        private EllipseLabel m_EllipseLabel;
        private EllipseLabel m_AltEllipseLabel;
        private RectLabel m_RectLabel;
        
        public InputNode(string defaultName)
            : base()
        {
            m_Name = defaultName;
            m_Type = "";
            m_DefaultValue = "";
            m_UseAltBinding = false;

            m_EllipseLabel = new EllipseLabel();
            m_EllipseLabel.BackColor = Color.OrangeRed;
            m_EllipseLabel.Width = 12;
            m_EllipseLabel.Height = 12;

            m_AltEllipseLabel = new EllipseLabel();
            m_AltEllipseLabel.BackColor = Color.PaleVioletRed;
            m_AltEllipseLabel.Width = 12;
            m_AltEllipseLabel.Height = 12;

            m_RectLabel = new RectLabel();
            m_RectLabel.Font = new Font("Verdana", 8);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = m_DefaultValue.Length > 0 ? Color.PapayaWhip : Color.PeachPuff;
            m_RectLabel.Text = m_Type + "\n" + m_Name;

            IsInputInstanced = false;
        }

        #region Serialization
        protected InputNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Name = info.GetString("Name");
            m_Type = info.GetString("Type");
            m_DefaultValue = info.GetString("Default Value");
            m_Binding = (OutputNode) info.GetValue("Binding", typeof(OutputNode));
            m_AltBinding = (OutputNode)info.GetValue("AltBinding", typeof(OutputNode));
            m_UseAltBinding = info.GetBoolean("UseAltBinding");
            m_EllipseLabel = (EllipseLabel)info.GetValue("EllipseLabel", typeof(EllipseLabel));
            m_EllipseLabel.BackColor = Color.OrangeRed;
            m_EllipseLabel.Width = 12;
            m_EllipseLabel.Height = 12;

           // m_AltEllipseLabel = (EllipseLabel)info.GetValue("AltEllipseLabel", typeof(EllipseLabel));
            m_AltEllipseLabel = new EllipseLabel();
            m_AltEllipseLabel.BackColor = Color.PaleVioletRed;
            m_AltEllipseLabel.Width = 12;
            m_AltEllipseLabel.Height = 12;

            m_RectLabel = (RectLabel)info.GetValue("RectLabel", typeof(RectLabel));
            m_RectLabel.Font = new Font("Verdana", 8);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = m_DefaultValue.Length > 0 ? Color.PapayaWhip : Color.PeachPuff;

            IsInputInstanced = info.GetBoolean("IsInputInstanced");
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Type", Type);
            info.AddValue("Default Value", DefaultValue);
            info.AddValue("Binding", Binding);
            info.AddValue("AltBinding", AltBinding); 
            info.AddValue("EllipseLabel", m_EllipseLabel);
            info.AddValue("AltEllipseLabel", m_AltEllipseLabel);
            info.AddValue("RectLabel", m_RectLabel);
            info.AddValue("IsInputInstanced", m_IsInputInstanced);
            info.AddValue("UseAltBinding", m_UseAltBinding);
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
            m_Properties.Properties.Add(new PropertySpec(
                "Use Alt Binding", typeof(bool), "Instanced Options", "Use alt binding?", m_UseAltBinding,
                (string)null, (string)null));
        }

        protected override void PropertyGetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertyGetValue(sender, e);
            
            switch (e.Property.Name)
            {
                case "Name": e.Value = m_Name; break;
                case "Type": e.Value = m_Type; break;
                case "Default Value": e.Value = m_DefaultValue; break;
                case "Use Alt Binding": e.Value = m_UseAltBinding; break;
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
                case "Use Alt Binding": m_UseAltBinding = (bool) e.Value; break;
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

        public bool UseAltBinding
        {
            get
            {
                return m_UseAltBinding;
            }
            set
            {
                m_UseAltBinding = (bool)value;
                if (!m_UseAltBinding)
                    m_AltBinding = null;
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

        public OutputNode AltBinding
        {
            get
            {
                return m_AltBinding;
            }

            set
            {
                m_AltBinding = (OutputNode)value;
            }
        }

        public EllipseLabel EllipseLabel
        {
            get
            {
                return m_EllipseLabel;
            }
        }

        public EllipseLabel AltEllipseLabel
        {
            get
            {
                return m_AltEllipseLabel;
            }
        }
        
        public RectLabel RectLabel
        {
            get
            {
                return m_RectLabel;
            }
        }
        
        public bool IsInputInstanced
        {
            get
            {
                return m_IsInputInstanced;
            }

            set
            {
                m_IsInputInstanced = (bool)value;
                if(m_IsInputInstanced)
                {
                    MakeReadOnly("Name", ReadOnlyAttribute.Yes);
                    MakeReadOnly("Type", ReadOnlyAttribute.Yes);
                    MakeReadOnly("Default Value", ReadOnlyAttribute.Yes);
                    MakeReadOnly("Use Alt Binding", ReadOnlyAttribute.No);
                }
                else
                {
                    MakeReadOnly("Name", ReadOnlyAttribute.No);
                    MakeReadOnly("Type", ReadOnlyAttribute.No);
                    MakeReadOnly("Default Value", ReadOnlyAttribute.No);
                    MakeReadOnly("Use Alt Binding", ReadOnlyAttribute.Yes);
                }
            }
        }

        public object Clone()
        {
            InputNode input = new InputNode(m_Name);
            input.DefaultValue = DefaultValue;
            input.Type = Type;
            input.Binding = Binding;
            input.UseAltBinding = UseAltBinding;
            input.AltBinding = AltBinding;
            input.EllipseLabel.X = EllipseLabel.X;
            input.EllipseLabel.Y = EllipseLabel.Y;
            input.AltEllipseLabel.X = AltEllipseLabel.X;
            input.AltEllipseLabel.Y = AltEllipseLabel.Y;
            input.RectLabel.X = RectLabel.X;
            input.RectLabel.Y = RectLabel.Y;

            return input;
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
            if (Binding != null)
                m_Impl.BindValue(Binding.Impl);
            if (AltBinding != null)
                m_Impl.BindAltValue(AltBinding.Impl);
        }

        public void EndMaterialGen()
        {
            if (Impl != null)
            {
                m_Impl.Dispose();
                m_Impl = null;
            }
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
