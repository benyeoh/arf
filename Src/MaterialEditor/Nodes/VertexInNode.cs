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
    public class VertexInNode : PropertyNode, ISerializable, IOutputNodeOwner, ICloneable
    {
        private string m_Name;
        private string m_Type;
        private string m_OptionalName;

        private OutputNode m_Output;

        private RectLabel m_RectLabel;

        public VertexInNode(string defaultName)
            : base()
        {
            m_Name = defaultName;
            m_Type = "";
            m_Output = new OutputNode(this);
            m_OptionalName = "";

            m_RectLabel = new RectLabel();
            m_RectLabel.X = -9999;
            m_RectLabel.Y = -9999;
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.Lavender;
            UpdateRectLabel();

        }

        #region Serialization
        protected VertexInNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Name = info.GetString("Name");
            m_Type = info.GetString("Type");
            m_OptionalName = info.GetString("OptionalName");
            
            m_Output = (OutputNode)info.GetValue("Output", typeof(OutputNode));
            m_RectLabel = (RectLabel)info.GetValue("RectLabel", typeof(RectLabel));
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.Lavender;
            UpdateRectLabel();
           
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Type", Type);
            info.AddValue("Output", Output);
            info.AddValue("RectLabel", m_RectLabel);
            info.AddValue("OptionalName", m_OptionalName);
        }
        #endregion

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Vertex In", "Name of the vertex in", m_Name,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Type", typeof(string), "Vertex In", "Type of the vertex in", m_Type,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Optional Name", typeof(string), "Vertex In", "Optional name for readability purposes", m_OptionalName,
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
            }
        }

        protected override void PropertySetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertySetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": m_Name = (string)e.Value; UpdateRectLabel(); break;
                case "Type": m_Type = (string)e.Value; UpdateRectLabel(); break;
                case "Optional Name": m_OptionalName = (string)e.Value; UpdateRectLabel(); break;
            }
        }

        public string Name
        {
            get { return m_Name; }
            set
            {
                m_Name = (string)value;
                UpdateRectLabel();
            }
        }

        public string Type
        {
            get { return m_Type; }
            set
            {
                m_Type = (string)value;
                UpdateRectLabel();
            }
        }

        public OutputNode Output
        {
            get
            {
                return m_Output;
            }

            //set
            //{
            //    m_Output = (OutputNode)value;
            //}
        }

        private void UpdateRectLabel()
        {
            m_RectLabel.Text = m_OptionalName != null && m_OptionalName.Length > 0 ? m_Type + " " + m_Name + "\n( " + m_OptionalName + " )" :
                m_Type + " " + m_Name;
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


        public string GetOwnerName()
        {
            return Name;
        }

        public string GetOwnerType()
        {
            return Type;
        }

        public object Clone()
        {
            VertexInNode vertIn = new VertexInNode(m_Name);
            vertIn.Type = Type;
            vertIn.OptionalName = OptionalName;

            vertIn.Output.EllipseLabel.X = Output.EllipseLabel.X;
            vertIn.Output.EllipseLabel.Y = Output.EllipseLabel.Y;

            vertIn.RectLabel.X = RectLabel.X;
            vertIn.RectLabel.Y = RectLabel.Y;

            return vertIn;
        }

        #region Material Gen
        private VertexIn m_Impl;
        public void BeginMaterialGen(MaterialGen gen)
        {
            if (Impl == null)
            {
                ParamSig sig;
                sig.hName = Name;
                sig.hType = Type;
                m_Impl = gen.GetResourceMgr().CreateVertexIn(sig);

                Output.BeginMaterialGen(m_Impl.GetOutput());
            }
            //m_Proxy.BeginMaterialGen(gen);
        }

        public void ProcessBinding()
        {
            Output.ProcessBinding();
        }

        public void EndMaterialGen()
        {
            if (Impl != null)
            {
                Output.EndMaterialGen();
                m_Impl.Dispose();
                m_Impl = null;
            }

        }

        public VertexIn Impl
        {
            get
            {
                return m_Impl;
            }
        }
        #endregion
    }
}
