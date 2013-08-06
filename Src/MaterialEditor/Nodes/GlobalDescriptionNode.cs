using System;
using System.Collections.Generic;
using System.Text;
using Common.PropertyGrid;
using System.Windows.Forms;
using System.Runtime.Serialization;

namespace MaterialEditor
{
    [Serializable]
    public class GlobalDescriptionNode : PropertyNode, ISerializable
    {
        private string m_Name;
        private string m_Type;
        private string m_DefaultValue;

        public GlobalDescriptionNode(string defaultName)
            : base()
        {
            m_Name = defaultName;
            m_Type = "";
            m_DefaultValue = "";
        }

        protected GlobalDescriptionNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Name = info.GetString("Name");
            m_Type = info.GetString("Type");
            m_DefaultValue = info.GetString("Default Value");
        }

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Description", "Name of the description", m_Name,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Type", typeof(string), "Description", "Type of the description", m_Type,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Default Value", typeof(string), "Description", "Default value of the description", m_DefaultValue,
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
                case "Name": m_Name = (string)e.Value; break;
                case "Type": m_Type = (string)e.Value; break;
                case "Default Value": m_DefaultValue = (string)e.Value; break;
            }
        }

        public string Name
        {
            get { return m_Name; }
            set
            {
                m_Name = (string)value;
            }
        }

        public string Type
        {
            get { return m_Type; }
            set
            {
                m_Type = (string)value;
            }
        }

        public string DefaultValue
        {
            get { return m_DefaultValue; }
            set
            {
                m_DefaultValue = (string)value;
            }
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Type", Type);
            info.AddValue("Default Value", DefaultValue);
        }
    }
}
