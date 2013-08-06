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
    public class RenderStateNode : PropertyNode, ISerializable
    {
        private string m_Name;
        private string m_Type;
        private string m_DefaultValue;

        List<InputNode> m_Inputs;
        List<GlobalNodeProxy> m_Dependents;

        RectLabel m_RectLabel;

        public RenderStateNode(string defaultName)
            : base()
        {
            m_Inputs = new List<InputNode>();
            m_Dependents = new List<GlobalNodeProxy>();

            m_Name = defaultName;
            m_Type = "";
            m_DefaultValue = "";
           
            m_RectLabel = new RectLabel();
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.Wheat;
            m_RectLabel.Text = m_Type + " " + m_Name;
        }

        #region Serialization
        protected RenderStateNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Inputs = (List<InputNode>)info.GetValue("Inputs", typeof(List<InputNode>));
            if (m_Inputs == null)
                m_Inputs = new List<InputNode>();
            m_Dependents = (List<GlobalNodeProxy>)info.GetValue("Dependents", typeof(List<GlobalNodeProxy>));
            if (m_Dependents == null)
                m_Dependents = new List<GlobalNodeProxy>();

            m_Name = info.GetString("Name");
            m_Type = info.GetString("Type");
            m_DefaultValue = info.GetString("Default Value");

            m_RectLabel = (RectLabel)info.GetValue("RectLabel", typeof(RectLabel));
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.Wheat;
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Type", Type);
            info.AddValue("Default Value", DefaultValue);
            info.AddValue("Inputs", Inputs);
            info.AddValue("RectLabel", m_RectLabel);
            info.AddValue("Dependents", m_Dependents);
        }
        #endregion

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Render State", "Name of the render state", m_Name,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Type", typeof(string), "Render State", "Type of the render state", m_Type,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Default Value", typeof(string), "Render State", "Default value of the render state", m_DefaultValue,
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
                case "Name": m_Name = (string)e.Value; m_RectLabel.Text = m_Type + " " + m_Name; break;
                case "Type": m_Type = (string)e.Value; m_RectLabel.Text = m_Type + " " + m_Name; break;
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
                m_RectLabel.Text = m_Type + " " + m_Name;
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
                m_RectLabel.Text = m_Type + " " + m_Name;
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
                return m_Inputs;
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

        public object Clone()
        {
            RenderStateNode rs = new RenderStateNode(m_Name);
            rs.Type = Type;
            rs.DefaultValue = DefaultValue;

            Dictionary<OutputNode, OutputNode> outputMapping = new Dictionary<OutputNode, OutputNode>();

            // Add to output map
            foreach (GlobalNodeProxy proxy in Dependents)
            {
                GlobalNodeProxy clonedProxy = (GlobalNodeProxy)proxy.Clone();
                outputMapping.Add(proxy.Output, clonedProxy.Output);
                rs.Dependents.Add(clonedProxy);
            }

            // Add inputs
            foreach (InputNode input in Inputs)
            {
                InputNode clonedInput = (InputNode)input.Clone();
                rs.Inputs.Add(clonedInput);
            }

            // Link outputs
            foreach (InputNode input in rs.Inputs)
            {
                OutputNode binding = null;
                if (outputMapping.TryGetValue(input.Binding, out binding))
                    input.Binding = binding;

                if (input.AltBinding != null)
                {
                    if (outputMapping.TryGetValue(input.AltBinding, out binding))
                        input.AltBinding = binding;
                }
            }
            
            return rs;
        }

        #region Material Gen
        private RenderState m_Impl;
        public void BeginMaterialGen(MaterialGen gen)
        {
            if (Impl == null)
            {
                ParamSig sig;
                sig.hName = Name;
                sig.hType = Type;
                
                InputSig[] inputSigs = new InputSig[Inputs.Count];
                uint i = 0;
                foreach(InputNode input in Inputs)
                {
                    inputSigs[i].hName = input.Name;
                    inputSigs[i].hType = input.Type;
                    inputSigs[i].hDefaultVal = input.DefaultValue;
                    ++i;
                }

                m_Impl = gen.GetResourceMgr().CreateRenderState(sig, inputSigs);
 
                i = 0;
                foreach(InputNode input in Inputs)
                {
                    input.BeginMaterialGen(m_Impl.GetInput(i));
                    ++i;
                }

                foreach (GlobalNodeProxy proxy in Dependents)
                {
                    proxy.BeginMaterialGen(gen);
                }

                m_Impl.SetValue(m_DefaultValue);
            }
        }

        public void ProcessBinding()
        {
            foreach (InputNode input in Inputs)
            {
                input.ProcessBinding();
            }

            foreach (GlobalNodeProxy proxy in Dependents)
            {
                proxy.ProcessBinding();
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

                foreach (GlobalNodeProxy proxy in Dependents)
                {
                    proxy.EndMaterialGen();
                }
            }
        }

        public RenderState Impl
        {
            get
            {
                return m_Impl;
            }
        }
        #endregion
    }
}
