using System;
using System.Collections.Generic;
using System.Text;
using Common.PropertyGrid;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Diagnostics;
using System.Drawing;
using ARFManaged.MaterialGenManaged;

namespace MaterialEditor
{
    [Serializable]
    public class GlobalNode : PropertyNode, ISerializable
    {
        private string m_Name;
        private string m_Type;
        private string m_DefaultValue;

        private string m_Semantic;
        private string m_Usage;
        private bool m_IsAlwaysUsed;
        private bool m_IsVolatile;

        List<LocalNodeProxy> m_Locals;
        List<InputNode> m_Inputs;
        List<GlobalDescriptionNode> m_Descriptions;
        List<GlobalNodeProxy> m_Dependents;

        RectLabel m_RectLabel;

        public GlobalNode(string defaultName)
            : base()
        {
            m_Inputs = new List<InputNode>();
            m_Descriptions = new List<GlobalDescriptionNode>();
            m_Locals = new List<LocalNodeProxy>();
            m_Dependents = new List<GlobalNodeProxy>();

            m_Name = defaultName;
            m_Type = "";
            m_DefaultValue = "";
            m_Semantic = "Invalid";
            m_Usage = Enum.GetNames(typeof(eMGMUsage))[0];
            m_IsAlwaysUsed = false;
            m_IsVolatile = false;

            m_RectLabel = new RectLabel();
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.PaleTurquoise;
            m_RectLabel.Text = m_Type + " " + m_Name;
            m_RectLabel.X = -9999;
            m_RectLabel.Y = -9999;
        }

        #region Serialization
        protected GlobalNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Inputs = (List<InputNode>) info.GetValue("Inputs", typeof(List<InputNode>));
            if (m_Inputs == null)
                m_Inputs = new List<InputNode>();
            m_Descriptions = (List<GlobalDescriptionNode>)info.GetValue("Descriptions", typeof(List<GlobalDescriptionNode>));
            if (m_Descriptions == null)
                m_Descriptions = new List<GlobalDescriptionNode>();
            m_Locals = (List<LocalNodeProxy>)info.GetValue("Locals", typeof(List<LocalNodeProxy>));
            if (m_Locals == null)
                m_Locals = new List<LocalNodeProxy>();
            m_Dependents = (List<GlobalNodeProxy>)info.GetValue("Dependents", typeof(List<GlobalNodeProxy>));
            if (m_Dependents == null)
                m_Dependents = new List<GlobalNodeProxy>();
         
            m_Name = info.GetString("Name");
            m_Type = info.GetString("Type");
            m_DefaultValue = info.GetString("Default Value");
            m_Semantic = info.GetString("Semantic");
            m_Usage = info.GetString("Usage");            
            m_IsAlwaysUsed = info.GetBoolean("Is Always Used");
            m_IsVolatile = info.GetBoolean("Is Volatile");

            m_RectLabel = (RectLabel) info.GetValue("RectLabel", typeof(RectLabel));
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.PaleTurquoise;            
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Type", Type);
            info.AddValue("Default Value", DefaultValue);
            info.AddValue("Locals", Locals);
            info.AddValue("Inputs", Inputs);
            info.AddValue("Descriptions", Descriptions);
            info.AddValue("Semantic", m_Semantic);
            info.AddValue("Usage", m_Usage);
            info.AddValue("Is Always Used", m_IsAlwaysUsed);
            info.AddValue("Is Volatile", m_IsVolatile);
            info.AddValue("RectLabel", m_RectLabel);
            info.AddValue("Dependents", m_Dependents);
        }
        #endregion

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Global", "Name of the global", m_Name,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Type", typeof(string), "Global", "Type of the global", m_Type,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Default Value", typeof(string), "Global", "Default value of the global", m_DefaultValue, 
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Semantic", typeof(string), "Global", "Semantic value of the global", m_Semantic,
                typeof(ListBoxSemanticTypeEditor), (string) null));
            m_Properties.Properties.Add(new PropertySpec(
                "Usage", typeof(string), "Global", "The global can be a Constant/Dynamic/Instanced", m_Usage,
                typeof(ListBoxUsageTypeEditor), (string) null));
            m_Properties.Properties.Add(new PropertySpec(
                "Is Always Used", typeof(bool), "Global", "Is the global always considered during generation?", m_IsAlwaysUsed));
            m_Properties.Properties.Add(new PropertySpec(
                "Is Volatile", typeof(bool), "Global", "Does the global need to be resolved every time?", m_IsVolatile));
        }

        protected override void PropertyGetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertyGetValue(sender, e);
            
            switch (e.Property.Name)
            {
                case "Name": e.Value = m_Name; break;
                case "Type": e.Value = m_Type; break;
                case "Default Value": e.Value = m_DefaultValue; break;
                case "Semantic": e.Value = m_Semantic; break;
                case "Usage": e.Value = m_Usage; break;
                case "Is Always Used": e.Value = m_IsAlwaysUsed; break;
                case "Is Volatile": e.Value = m_IsVolatile; break;
            }
        }

        protected override void PropertySetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertySetValue(sender, e);
            
            switch (e.Property.Name)
            {
                case "Name": m_Name = (string)e.Value; m_RectLabel.Text = m_Type + " " + m_Name; break;
                case "Type": m_Type = (string)e.Value; m_RectLabel.Text = m_Type + " " + m_Name; break;
                case "Default Value": m_DefaultValue = (string) e.Value; break;
                case "Semantic": m_Semantic = (string) e.Value; break;
                case "Usage": m_Usage = (string) e.Value; break;
                case "Is Always Used": m_IsAlwaysUsed = (bool)e.Value; break;
                case "Is Volatile": m_IsVolatile = (bool)e.Value; break;
            }
        }

        public string Name
        {
            get{ return m_Name; }
            set
            {
                //OnChangedDelegate(this,
                //    new PropertySpecEventArgs(
                //        new PropertySpec("Name", typeof(string).AssemblyQualifiedName), (string)value)); 
                m_Name = (string)value;
                m_RectLabel.Text = m_Type + " " + m_Name; 
            }
        }

        public string Type
        {
            get{ return m_Type; }
            set
            {
                //OnChangedDelegate(this,
                //    new PropertySpecEventArgs(
                //        new PropertySpec("Type", typeof(string).AssemblyQualifiedName), (string)value)); 
                m_Type = (string)value;
                m_RectLabel.Text = m_Type + " " + m_Name; 
            }
        }

        public string DefaultValue
        {
            get{ return m_DefaultValue; }
            set
            {
                //OnChangedDelegate(this,
                //    new PropertySpecEventArgs(
                //        new PropertySpec("Default Value", typeof(string).AssemblyQualifiedName), (string)value)); 
                m_DefaultValue = (string)value;
            }
        }


        public string Semantic
        {
            get
            {
                return m_Semantic;
            }
        }

        public string Usage
        {
            get
            {
                return m_Usage;
            }
        }

        public bool IsAlwaysUsed
        {
            get
            {
                return m_IsAlwaysUsed;
            }
        }

        public bool IsVolatile
        {
            get
            {
                return m_IsVolatile;
            }
        }

        public List<InputNode> Inputs
        {
            get
            {
                return m_Inputs;
            }
        }

        public List<GlobalDescriptionNode> Descriptions
        {
            get
            {
                return m_Descriptions;
            }
        }

        public List<LocalNodeProxy> Locals
        {
            get
            {
                return m_Locals;
            }
        }

        public List<GlobalNodeProxy> Dependents
        {
            get
            {
                return m_Dependents;
            }
        }

        public RectLabel RectLabel
        {
            get
            {
                return m_RectLabel;
            }
        }      

        public void GatherAllGlobals(List<GlobalNode> allGlobals)
        {
            foreach(GlobalNodeProxy proxy in Dependents)
            {
                if(!allGlobals.Contains(proxy.Proxy))
                    allGlobals.Add(proxy.Proxy);
                proxy.Proxy.GatherAllGlobals(allGlobals);
            }
        }

        #region Material Gen
        private uint ResolveUsage(string usageVal)
        {
            int usageNum = (int) eMGMUsage.MGMU_CONSTANT;
        
            try
            {
                eMGMUsage resVal = (eMGMUsage)Enum.Parse(typeof(eMGMUsage), usageVal);
                usageNum = (int)resVal;
            }
            catch (ArgumentException)
            {
                // Semantic is not within param range
                // Do nothing
            }

            return (uint)usageNum;
        }

        private uint ResolveSemantic(string semanticVal)
        {
            int semanticNum = -1;

            ResolveSemanticEventArgs args = new ResolveSemanticEventArgs(semanticVal);
            EventManager.GlobalEventManager.RaiseEvent(ResolveSemanticEventArgs.EVENT_TYPE, this, args);
            if (args.Result != ResolveSemanticEventArgs.UNRESOLVED_SEMANTIC)
            {
                semanticNum = args.Result;
            }
            else
            {
                try
                {
                    semanticNum = Int32.Parse(semanticVal);
                }
                catch (FormatException)
                {
                }
            }

            return (uint) semanticNum;
        }

        private Global m_Impl;
        public void BeginMaterialGen(MaterialGen gen)
        {
            if (Impl == null)
            {
                ParamSig paramSig;
                paramSig.hName = Name;
                paramSig.hType = Type;

                InputSig[] inputSigs = new InputSig[Inputs.Count];
                uint i = 0;
                foreach (InputNode input in Inputs)
                {
                    inputSigs[i].hDefaultVal = input.DefaultValue;
                    inputSigs[i].hName = input.Name;
                    inputSigs[i].hType = input.Type;
                    i++;
                }

                m_Impl = gen.GetResourceMgr().CreateGlobal(paramSig, inputSigs);

                i = 0;
                foreach (InputNode input in Inputs)
                {
                    input.BeginMaterialGen(m_Impl.GetInput(i));
                    i++;
                }

                // Descriptions
                foreach (GlobalDescriptionNode desc in Descriptions)
                {
                    InputSig sig;
                    sig.hType = desc.Type;
                    sig.hName = desc.Name;
                    sig.hDefaultVal = desc.DefaultValue;
                    m_Impl.AddDescription(sig);
                }

                m_Impl.SetIsAlwaysUsed(m_IsAlwaysUsed);
                m_Impl.SetUsage(ResolveUsage(m_Usage));
                m_Impl.SetIsVolatile(m_IsVolatile);
                m_Impl.SetSemantic(ResolveSemantic(m_Semantic));
                m_Impl.SetValue(m_DefaultValue);

                // Locals
                foreach (LocalNodeProxy proxy in Locals)
                {
                    proxy.BeginMaterialGen(gen);
                }   
   
                // Dependents
                foreach(GlobalNodeProxy globalProxy in Dependents)
                {
                    globalProxy.BeginMaterialGen(gen);
                }
            }
        }

        public void ProcessBinding()
        {
            foreach(InputNode input in Inputs)
            {
                input.ProcessBinding();
            }

            foreach(LocalNodeProxy local in Locals)
            {
                local.ProcessBinding();
            }

            // Dependents
            foreach (GlobalNodeProxy globalProxy in Dependents)
            {
                globalProxy.ProcessBinding();
            }
        }

        public void EndMaterialGen()
        {
            if (Impl != null)
            {
                m_Impl.Dispose();
                m_Impl = null;

                foreach (InputNode input in Inputs)
                {
                    input.EndMaterialGen();
                }

                foreach (LocalNodeProxy local in Locals)
                {
                    local.EndMaterialGen();
                }

                // Dependents
                foreach (GlobalNodeProxy globalProxy in Dependents)
                {
                    globalProxy.EndMaterialGen();
                }
            }
        }

        public Global Impl
        {
            get
            {
                return m_Impl;
            }
        }
        #endregion
    }
}
