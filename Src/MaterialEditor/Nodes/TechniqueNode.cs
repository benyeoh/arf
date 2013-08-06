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
    public class TechniqueNode : PropertyNode, ISerializable
    {
        private string m_Name;
       
        List<PassNode> m_Passes;

        public TechniqueNode(string defaultName)
            : base()
        {
            m_Passes = new List<PassNode>();

            m_Name = defaultName;
        }

        protected TechniqueNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Passes = (List<PassNode>)info.GetValue("Passes", typeof(List<PassNode>));
            if (m_Passes == null)
                m_Passes = new List<PassNode>();

            m_Name = info.GetString("Name");
        }

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Technique", "Name of the technique", m_Name,
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

        public List<PassNode> Passes
        {
            get
            {
                return m_Passes;
            }
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Passes", Passes);
        }

        public object Clone()
        {
            TechniqueNode tech = new TechniqueNode(m_Name);

            foreach (PassNode pass in Passes)
            {
                PassNode clonedPass = (PassNode)pass.Clone();
                tech.Passes.Add(clonedPass);
            }

            return tech;
        }

        #region Material Gen
        private Technique m_Impl;
        public void BeginMaterialGen(MaterialGen gen)
        {
            if (Impl == null)
            {
                m_Impl = gen.GetResourceMgr().CreateTechnique(Name);
            }

            foreach(PassNode pass in Passes)
            {
                pass.BeginMaterialGen(gen);
                m_Impl.AddPass(pass.Impl);
            }     
            
            //m_Proxy.BeginMaterialGen(gen);
        }

        public void ProcessBinding()
        {
            foreach (PassNode pass in Passes)
            {
                pass.ProcessBinding();
            }
        }

        public void EndMaterialGen()
        {
            if (Impl != null)
            {
                m_Impl.Dispose();
                m_Impl = null;

                foreach (PassNode pass in Passes)
                {
                    pass.EndMaterialGen();
                }
            }
        }

        public Technique Impl
        {
            get
            {
                return m_Impl;
            }
        }
        #endregion
    }
}
