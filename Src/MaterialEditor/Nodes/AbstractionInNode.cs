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
    public class AbstractionInNode : PropertyNode, ISerializable, IOutputNodeOwner, ICloneable
    {
        private string m_Name;
        private string m_Type;

        private OutputNode m_Output;

        private RectLabel m_RectLabel;

        public AbstractionInNode(string defaultName)
            : base()
        {
            m_Name = defaultName;
            m_Type = "";
            m_Output = new OutputNode(this);

            m_RectLabel = new RectLabel();
            m_RectLabel.X = -99999;
            m_RectLabel.Y = -99999;
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.MistyRose;
            m_RectLabel.Text = m_Type + " " + m_Name;
        }

        #region Serialization
        protected AbstractionInNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Name = info.GetString("Name");
            m_Type = info.GetString("Type");
            m_Output = (OutputNode)info.GetValue("Output", typeof(OutputNode));
            m_RectLabel = (RectLabel)info.GetValue("RectLabel", typeof(RectLabel));
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.MistyRose;
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Type", Type);
            info.AddValue("Output", Output);
            info.AddValue("RectLabel", m_RectLabel);
        }
        #endregion

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Abstraction In", "Name of the abstraction in", m_Name,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Type", typeof(string), "Abstraction In", "Type of the abstraction in", m_Type,
                typeof(DropDownTextAreaEditor), (string)null));
        }

        protected override void PropertyGetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertyGetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": e.Value = m_Name; break;
                case "Type": e.Value = m_Type; break;
            }
        }

        protected override void PropertySetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertySetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": m_Name = (string)e.Value; m_RectLabel.Text = m_Type + " " + m_Name; break;
                case "Type": m_Type = (string)e.Value; m_RectLabel.Text = m_Type + " " + m_Name; break;
            }
        }

        public string Name
        {
            get { return m_Name; }
            set
            {
                m_Name = (string)value;
                m_RectLabel.Text = m_Type + " " + m_Name;
            }
        }

        public string Type
        {
            get { return m_Type; }
            set
            {
                m_Type = (string)value;
                m_RectLabel.Text = m_Type + " " + m_Name;
            }
        }

        public OutputNode Output
        {
            get
            {
                return m_Output;
            }

            set
            {
                m_Output = (OutputNode)value;
            }
        }

        public RectLabel RectLabel
        {
            get
            {
                return m_RectLabel;
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
            AbstractionInNode abstractIn = new AbstractionInNode(m_Name);
            abstractIn.Type = Type;

            abstractIn.Output.EllipseLabel.X = Output.EllipseLabel.X;
            abstractIn.Output.EllipseLabel.Y = Output.EllipseLabel.Y;
            
            abstractIn.RectLabel.X = RectLabel.X;
            abstractIn.RectLabel.Y = RectLabel.Y;

            return abstractIn;
        }
    }
}
