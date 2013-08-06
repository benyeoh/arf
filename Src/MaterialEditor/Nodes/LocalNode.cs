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
    public class LocalNode : PropertyNode, ISerializable
    {
        private string m_Preamble;
        private string m_Name;
        private string m_Type;
        private string m_Postamble;
        private string m_DefaultValue;

        public LocalNode(string defaultName)
            : base()
        {
            m_Preamble = "";
            m_Postamble = "";
            m_Name = defaultName;
            m_Type = "";
            m_DefaultValue = "";
        }

        protected LocalNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Name = info.GetString("Name");
            m_Type = info.GetString("Type");
            m_DefaultValue = info.GetString("Default Value");
            m_Preamble = info.GetString("Preamble");
            m_Postamble = info.GetString("Postamble");           
        }

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Local", "Name of the local variable", m_Name,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Type", typeof(string), "Local", "Type of the local variable", m_Type,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Default Value", typeof(string), "Local", "Default value of the local variable", m_DefaultValue,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
               "Preamble", typeof(string), "Local", "Preamble for the local variable", m_Preamble,
               typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
               "Postamble", typeof(string), "Local", "Postamble for the local variable", m_Postamble,
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
                case "Preamble": e.Value = m_Preamble; break;
                case "Postamble": e.Value = m_Postamble; break;
                
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
                case "Preamble": m_Preamble = (string)e.Value; break;
                case "Postamble": m_Postamble = (string)e.Value; break;
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

        public string Preamble
        {
            get { return m_Preamble; }
            set
            {
                m_Preamble = (string)value;
            }
        }

        public string Postamble
        {
            get { return m_Postamble; }
            set
            {
                m_Postamble = (string)value;
            }
        }


        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Type", Type);
            info.AddValue("Default Value", DefaultValue);
            info.AddValue("Preamble", Preamble);
            info.AddValue("Postamble", Postamble);
        }

        #region Material Gen
        private Local m_Impl;
        public void BeginMaterialGen(MaterialGen gen)
        {
            if (Impl == null)
            {
                LocalSig sig;
                sig.hName = Name;
                sig.hPostamble = Postamble;
                sig.hPreamble = Preamble;
                sig.hType = Type;

                m_Impl = gen.GetResourceMgr().CreateLocal(sig);
                if(DefaultValue != null && DefaultValue.Length > 0)
                    m_Impl.SetValue(DefaultValue);
            }
            //m_Proxy.BeginMaterialGen(gen);
        }

        public void ProcessBinding()
        {

        }

        public void EndMaterialGen()
        {
            if (Impl != null)
            {
                m_Impl.Dispose();
                m_Impl = null;
            }
        }

        public Local Impl
        {
            get
            {
                return m_Impl;
            }
        }
        #endregion
    }
}
