using System;
using System.Collections.Generic;
using System.Text;
using Common.PropertyGrid;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Drawing;
using ARFManaged.MaterialGenManaged;

namespace MaterialEditor
{
    [Serializable]
    public class VertexOutNode : PropertyNode, ISerializable, ICloneable
    {
        private string m_Name;
        private string m_Type;
        private string m_OptionalName;
        private string m_DefaultValue;

        private InputNode m_Input;

        private RectLabel m_RectLabel;

        public VertexOutNode (string defaultName)
            : base()
        {
            m_Name = defaultName;
            m_Type = "";
            m_OptionalName = "";
            m_DefaultValue = "";

            m_RectLabel = new RectLabel();
            m_RectLabel.X = -9999;
            m_RectLabel.Y = -9999;
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.Thistle;
            UpdateRectLabel();

            m_Input = new InputNode(m_Name);
            m_Input.Type = m_Type;
            m_Input.IsInputInstanced = true;
            m_Input.DefaultValue = m_DefaultValue;
        }

        #region Serialization
        protected VertexOutNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Name = info.GetString("Name");
            m_OptionalName = info.GetString("OptionalName");
            m_DefaultValue = info.GetString("DefaultValue");
            m_Type = info.GetString("Type");
            m_Input = (InputNode)info.GetValue("Input", typeof(InputNode));

            m_RectLabel = (RectLabel)info.GetValue("RectLabel", typeof(RectLabel));
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.Thistle;
            UpdateRectLabel();
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Type", Type);
            info.AddValue("Input", Input);
            info.AddValue("RectLabel", m_RectLabel);
            info.AddValue("OptionalName", m_OptionalName);
            info.AddValue("DefaultValue", m_DefaultValue);

        }
        #endregion

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Vertex Out", "Name of the input", m_Name,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Type", typeof(string), "Vertex Out", "Type of the input", m_Type,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Optional Name", typeof(string), "Vertex Out", "Optional name for readability purposes", m_OptionalName,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Default Value", typeof(string), "Vertex Out", "Default value of the input", m_DefaultValue,
                typeof(DropDownTextAreaEditor), (string)null));
        }

        protected override void PropertyGetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertyGetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": e.Value = m_Name; break;
                case "Type": e.Value = m_Type; break;
                case "Optional Name": e.Value = m_OptionalName; break;
                case "Default Value": e.Value = m_DefaultValue; break;
            }
        }

        protected override void PropertySetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertySetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": m_Name = (string)e.Value; m_Input.Name = m_Name; UpdateRectLabel(); break;
                case "Type": m_Type = (string)e.Value; m_Input.Type = m_Type; UpdateRectLabel(); break;
                case "Optional Name": m_OptionalName = (string)e.Value; UpdateRectLabel(); break;
                case "Default Value": m_DefaultValue = (string)e.Value; m_Input.DefaultValue = m_DefaultValue; break;
            }
        }

        public string Name
        {
            get { return m_Name; }
            set
            {
                m_Name = (string)value;
                UpdateRectLabel();
                m_Input.Name = m_Name;
            }
        }

        public string Type
        {
            get { return m_Type; }
            set
            {
                m_Type = (string)value;
                UpdateRectLabel();
                m_Input.Type = m_Type;
            }
        }

        public InputNode Input
        {
            get
            {
                return m_Input;
            }

            //set
            //{
            //    m_Input = (InputNode)value;
            //}
        }

        public RectLabel RectLabel
        {
            get
            {
                return m_RectLabel;
            }
        }

        public string OptionalName
        {
            get
            {
                return m_OptionalName;
            }

            set
            {
                m_OptionalName = (string)value;
                UpdateRectLabel();
            }
        }

        private void UpdateRectLabel()
        {
            m_RectLabel.Text = m_OptionalName != null && m_OptionalName.Length > 0 ? m_Type + " " + m_Name + "\n( " + m_OptionalName + " )" :
                m_Type + " " + m_Name;
        }

        public object Clone()
        {
            VertexOutNode vertOut = new VertexOutNode(Name);
            vertOut.OptionalName = OptionalName;

            vertOut.Type = Type;
            vertOut.Input.Binding = Input.Binding;
            vertOut.Input.AltBinding = Input.AltBinding;
            vertOut.Input.EllipseLabel.X = Input.EllipseLabel.X;
            vertOut.Input.EllipseLabel.Y = Input.EllipseLabel.Y;
            vertOut.Input.RectLabel.X = Input.RectLabel.X;
            vertOut.Input.RectLabel.Y = Input.RectLabel.Y;

            vertOut.RectLabel.X = RectLabel.X;
            vertOut.RectLabel.Y = RectLabel.Y;

            return vertOut;
        }


        #region Material Gen
        private VertexOut m_Impl;
        public void BeginMaterialGen(MaterialGen gen)
        {
            if (Impl == null)
            {
                ParamSig sig;
                sig.hName = Name;
                sig.hType = Type;
                m_Impl = gen.GetResourceMgr().CreateVertexOut(sig);

                m_Input.BeginMaterialGen(m_Impl.GetInput());
            }
            //m_Proxy.BeginMaterialGen(gen);
        }

        public void ProcessBinding()
        {
            m_Input.ProcessBinding();
        }

        public void EndMaterialGen()
        {
            if (Impl != null)
            {
                m_Input.EndMaterialGen();
                m_Impl.Dispose();
                m_Impl = null;
            }
        }

        public VertexOut Impl
        {
            get
            {
                return m_Impl;
            }
        }
        #endregion
    }
}
