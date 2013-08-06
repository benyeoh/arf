using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.Serialization;
using Common.PropertyGrid;
using System.Diagnostics;
using System.ComponentModel;
using ARFManaged.MaterialGenManaged;

namespace MaterialEditor
{
    [Serializable]
    public class LocalNodeProxy : PropertyNode, ISerializable
    {
        LocalNode m_Proxy;

        public LocalNodeProxy(LocalNode proxy)
            : base()
        {
            m_Proxy = proxy;
        }

        protected LocalNodeProxy(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Proxy = (LocalNode) info.GetValue("Proxy", typeof(LocalNode));
        }

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();
                
            PropertySpec ps = new PropertySpec(
                "Name", typeof(string), "Local", "Name of the local variable", "",
                typeof(DropDownTextAreaEditor), (string)null);
            ps.Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
            m_Properties.Properties.Add(ps);

            ps = new PropertySpec(
                "Type", typeof(string), "Local", "Type of the local variable", "",
                typeof(DropDownTextAreaEditor), (string)null);
            ps.Attributes = new Attribute[] { ReadOnlyAttribute.Yes };           
            m_Properties.Properties.Add(ps);

            ps = new PropertySpec(
                "Default Value", typeof(string), "Local", "Default value of the local variable", "",
                typeof(DropDownTextAreaEditor), (string)null);
            ps.Attributes = new Attribute[] { ReadOnlyAttribute.Yes };           
            m_Properties.Properties.Add(ps);

            ps = new PropertySpec(
               "Preamble", typeof(string), "Local", "Preamble for the local variable", "",
               typeof(DropDownTextAreaEditor), (string)null);
            ps.Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
            m_Properties.Properties.Add(ps);

            ps = new PropertySpec(
               "Postamble", typeof(string), "Local", "Postamble for the local variable", "",
               typeof(DropDownTextAreaEditor), (string)null);
            ps.Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
            m_Properties.Properties.Add(ps);
        }

        protected override void PropertyGetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertyGetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": e.Value = m_Proxy.Name; break;
                case "Type": e.Value = m_Proxy.Type; break;
                case "Default Value": e.Value = m_Proxy.DefaultValue; break;
                case "Preamble": e.Value = m_Proxy.Preamble; break;
                case "Postamble": e.Value = m_Proxy.Postamble; break;                
            }
        }

        protected override void PropertySetValue(object sender, PropertySpecEventArgs e)
        {
            Debug.Assert(false);
        }

        public LocalNode Proxy
        {
            get
            {
                return m_Proxy;
            }
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Proxy", Proxy);
        }

        #region Material Gen
        public void BeginMaterialGen(MaterialGen gen)
        {
            m_Proxy.BeginMaterialGen(gen);
        }

        public void ProcessBinding()
        {
            m_Proxy.ProcessBinding();
        }

        public void EndMaterialGen()
        {
            m_Proxy.EndMaterialGen();
        }
        #endregion
    }
}
