using System;
using System.Collections.Generic;
using System.Text;
using Common.PropertyGrid;
using System.Windows.Forms;
using System.Runtime.Serialization;
using ARFManaged.MaterialGenManaged;

namespace MaterialEditor
{
    [Serializable]
    public class FolderNode : PropertyNode, ISerializable
    {
        private string m_Name;

        List<ISerializable> m_Nodes;

        public FolderNode(string defaultName)
            : base()
        {
            m_Nodes = new List<ISerializable>();

            m_Name = defaultName;
        }

        protected FolderNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Nodes = (List<ISerializable>)info.GetValue("Nodes", typeof(List<ISerializable>));
            if (m_Nodes == null)
                m_Nodes = new List<ISerializable>();

            m_Name = info.GetString("Name");
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Nodes", Nodes);
        }

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Folder", "Name of the folder", m_Name,
                typeof(DropDownTextAreaEditor), (string)null));
        }

        protected override void PropertyGetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertyGetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": e.Value = m_Name; break;
            }
        }

        protected override void PropertySetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertySetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": m_Name = (string)e.Value; break;
            }
        }

        public string Name
        {
            get { return m_Name; }
            set
            {
                //OnChangedDelegate(this,
                //    new PropertySpecEventArgs(
                //        new PropertySpec("Name", typeof(string).AssemblyQualifiedName), (string)value)); 
                m_Name = (string)value;
            }
        }

        public List<ISerializable> Nodes
        {
            get
            {
                return m_Nodes;
            }
        }

    }
}
