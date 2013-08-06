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
    public class AbstractionOutNode : PropertyNode, ISerializable, ICloneable
    {
        private string m_Name;
        private string m_Type;

        private InputNode m_Input;

        private RectLabel m_RectLabel;

        public AbstractionOutNode(string defaultName)
            : base()
        {
            m_Name = defaultName;
            m_Type = "";

            m_RectLabel = new RectLabel();
            m_RectLabel.X = -99999;
            m_RectLabel.Y = -99999;
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.Pink;

            m_RectLabel.Text = m_Type + " " + m_Name;

            m_Input = new InputNode(m_Name);
            m_Input.Type = m_Type;
            m_Input.IsInputInstanced = true;
        }

        #region Serialization
        protected AbstractionOutNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Name = info.GetString("Name");
            m_Type = info.GetString("Type");
            m_Input = (InputNode)info.GetValue("Input", typeof(InputNode));
            m_RectLabel = (RectLabel)info.GetValue("RectLabel", typeof(RectLabel));
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.Pink;

        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Type", Type);
            info.AddValue("Input", Input);
            info.AddValue("RectLabel", m_RectLabel);
        }
        #endregion

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Abstraction Out", "Name of the abstraction out", m_Name,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Type", typeof(string), "Abstraction Out", "Type of the abstraction out", m_Type,
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
                case "Name": m_Name = (string)e.Value; m_Input.Name = m_Name; m_RectLabel.Text = m_Type + " " + m_Name; break;
                case "Type": m_Type = (string)e.Value; m_Input.Type = m_Type; m_RectLabel.Text = m_Type + " " + m_Name; break;
            }
        }

        public string Name
        {
            get { return m_Name; }
            set
            {
                m_Name = (string)value;
                m_RectLabel.Text = m_Type + " " + m_Name;
                m_Input.Name = m_Name;
            }
        }

        public string Type
        {
            get { return m_Type; }
            set
            {
                m_Type = (string)value;
                m_RectLabel.Text = m_Type + " " + m_Name;
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
            //    InputNode input = (InputNode)value;
            //    m_Input.Name = input.Name;
            //    m_Input.Type = 
            //}
        }

        public RectLabel RectLabel
        {
            get
            {
                return m_RectLabel;
            }
        }

        public object Clone()
        {
            AbstractionOutNode abstractOut = new AbstractionOutNode(Name);

            abstractOut.Type = Type;
            abstractOut.Input.Binding = Input.Binding;
            abstractOut.Input.AltBinding = Input.AltBinding;
            abstractOut.Input.EllipseLabel.X = Input.EllipseLabel.X;
            abstractOut.Input.EllipseLabel.Y = Input.EllipseLabel.Y;
            abstractOut.Input.RectLabel.X = Input.RectLabel.X;
            abstractOut.Input.RectLabel.Y = Input.RectLabel.Y;

            abstractOut.RectLabel.X = RectLabel.X;
            abstractOut.RectLabel.Y = RectLabel.Y;

            return abstractOut;
        }
    }
}
