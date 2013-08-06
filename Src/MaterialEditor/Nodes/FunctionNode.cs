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
    public class FunctionNode : PropertyNode, ISerializable
    {
        private string m_Name;
        private string m_Type;
        private string m_DefaultValue;

        List<InputNode> m_InputNodes;
        List<LocalNodeProxy> m_Locals;

        public FunctionNode(string defaultName)
            : base()
        {
            m_InputNodes = new List<InputNode>();
            m_Locals = new List<LocalNodeProxy>();

            m_Name = defaultName;
            m_Type = "";
            m_DefaultValue = "";
        }

        protected FunctionNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_InputNodes = (List<InputNode>)info.GetValue("Inputs", typeof(List<InputNode>));
            if (m_InputNodes == null)
                m_InputNodes = new List<InputNode>();

            m_Locals = (List<LocalNodeProxy>)info.GetValue("Locals", typeof(List<LocalNodeProxy>));
            if (m_Locals == null)
                m_Locals  = new List<LocalNodeProxy>();
         
            m_Name = info.GetString("Name");
            m_Type = info.GetString("Type");
            m_DefaultValue = info.GetString("Default Value");
        }

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Function", "Name of the function", m_Name,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Type", typeof(string), "Function", "Type of the function", m_Type,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Default Value", typeof(string), "Function", "Default value of the function", m_DefaultValue,
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
                //OnChangedDelegate(this,
                //    new PropertySpecEventArgs(
                //        new PropertySpec("Name", typeof(string).AssemblyQualifiedName), (string)value)); 
                m_Name = (string)value;
            }
        }

        public string Type
        {
            get { return m_Type; }
            set
            {
                //OnChangedDelegate(this,
                //    new PropertySpecEventArgs(
                //        new PropertySpec("Type", typeof(string).AssemblyQualifiedName), (string)value)); 
                m_Type = (string)value;
            }
        }

        public string DefaultValue
        {
            get { return m_DefaultValue; }
            set
            {
                //OnChangedDelegate(this,
                //    new PropertySpecEventArgs(
                //        new PropertySpec("Default Value", typeof(string).AssemblyQualifiedName), (string)value)); 
                m_DefaultValue = (string)value;
            }
        }

        public List<InputNode> Inputs
        {
            get
            {
                return m_InputNodes;
            }
        }

        public List<LocalNodeProxy> Locals
        {
            get
            {
                return m_Locals;
            }
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Type", Type);
            info.AddValue("Default Value", DefaultValue);
            info.AddValue("Inputs", Inputs);
            info.AddValue("Locals", Locals);
        }

        #region Material Gen
        private Function m_Impl;
        public void BeginMaterialGen(MaterialGen gen)
        {
            if (Impl == null)
            {
                ParamSig sig;
                sig.hName = Name;
                sig.hType = Type;

                InputSig[] inputSigs = new InputSig[m_InputNodes.Count];
                int i = 0;
                foreach (InputNode input in m_InputNodes)
                {
                    inputSigs[i].hDefaultVal = input.DefaultValue;
                    inputSigs[i].hName = input.Name;
                    inputSigs[i].hType = input.Type;
                    ++i;
                }

                m_Impl = gen.GetResourceMgr().CreateFunction(sig, inputSigs);
                m_Impl.SetValue(m_DefaultValue);

                // Locals
                foreach (LocalNodeProxy proxy in Locals)
                {
                    proxy.BeginMaterialGen(gen);
                } 
            }
        }

        public void ProcessBinding()
        {
            foreach (LocalNodeProxy local in Locals)
            {
                local.ProcessBinding();
            }
        }

        public void EndMaterialGen()
        {
            if (Impl != null)
            {
                m_Impl.Dispose();
                m_Impl = null;

                foreach (LocalNodeProxy local in Locals)
                {
                    local.EndMaterialGen();
                }
            }
        }

        public Function Impl
        {
            get
            {
                return m_Impl;
            }
        }
        #endregion
    }
}
