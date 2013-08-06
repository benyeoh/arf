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
    public class VertexShaderNode : PropertyNode, ISerializable, ICloneable
    {
        private string m_Name;
        private string m_VerMajor;
        private string m_VerMinor;

        List<GlobalNodeProxy> m_GlobalProxies;
        List<FunctionOpNode> m_FunctionOps;
        List<VertexInNode> m_VertexIns;
        List<VertexOutNode> m_VertexOuts;
        List<AbstractionNodeProxy> m_AbstractionProxies;

        public VertexShaderNode(string defaultName)
            : base()
        {
            m_GlobalProxies = new List<GlobalNodeProxy>();
            m_FunctionOps = new List<FunctionOpNode>();
            m_VertexIns = new List<VertexInNode>();
            m_VertexOuts = new List<VertexOutNode>();
            m_AbstractionProxies = new List<AbstractionNodeProxy>();
            m_VerMajor = "3";
            m_VerMinor = "0";

            m_Name = defaultName;
        }

        #region Serialization
        protected VertexShaderNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_FunctionOps = (List<FunctionOpNode>)info.GetValue("FunctionOps", typeof(List<FunctionOpNode>));
            if (m_FunctionOps == null)
                m_FunctionOps = new List<FunctionOpNode>();
            m_GlobalProxies = (List<GlobalNodeProxy>)info.GetValue("GlobalProxies", typeof(List<GlobalNodeProxy>));
            if (m_GlobalProxies == null)
                m_GlobalProxies = new List<GlobalNodeProxy>();
            m_AbstractionProxies = (List<AbstractionNodeProxy>)info.GetValue("AbstractionProxies", typeof(List<AbstractionNodeProxy>));
            if (m_AbstractionProxies == null)
                m_AbstractionProxies = new List<AbstractionNodeProxy>();
            m_VertexIns = (List<VertexInNode>)info.GetValue("VertexIns", typeof(List<VertexInNode>));
            if (m_VertexIns == null)
                m_VertexIns = new List<VertexInNode>();
            m_VertexOuts = (List<VertexOutNode>)info.GetValue("VertexOuts", typeof(List<VertexOutNode>));
            if (m_VertexOuts == null)
                m_VertexOuts = new List<VertexOutNode>();

            m_VerMajor = info.GetString("VerMajor");
            m_VerMinor = info.GetString("VerMinor");
            m_Name = info.GetString("Name");
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("FunctionOps", m_FunctionOps);
            info.AddValue("GlobalProxies", m_GlobalProxies);
            info.AddValue("VertexIns", m_VertexIns);
            info.AddValue("VertexOuts", m_VertexOuts);
            info.AddValue("VerMajor", VerMajor);
            info.AddValue("VerMinor", VerMinor);
            info.AddValue("AbstractionProxies", m_AbstractionProxies);
        }
        #endregion

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Vertex Shader", "Name of the vertex shader", m_Name,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Version Major", typeof(string), "Vertex Shader", "Compile version (major) of the vertex shader", VerMajor,
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties.Add(new PropertySpec(
                "Version Minor", typeof(string), "Vertex Shader", "Compile version (minor) of the vertex shader", VerMinor,
                typeof(DropDownTextAreaEditor), (string)null));
        }

        protected override void PropertyGetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertyGetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": e.Value = m_Name; break;
                case "Version Major": e.Value = m_VerMajor; break;
                case "Version Minor": e.Value = m_VerMinor; break;
            }
        }

        protected override void PropertySetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertySetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": m_Name = (string)e.Value; break;
                case "Version Major": m_VerMajor = (string) e.Value; break;
                case "Version Minor": m_VerMinor = (string) e.Value; break;
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

        public string VerMajor
        {
            get
            {
                return m_VerMajor;
            }
        }

        public string VerMinor
        {
            get
            {
                return m_VerMinor;
            }
        }

        public List<FunctionOpNode> FunctionOps
        {
            get
            {
                return m_FunctionOps;
            }
        }

        public List<GlobalNodeProxy> GlobalProxies
        {
            get
            {
                return m_GlobalProxies;
            }
        }
        
        public List<AbstractionNodeProxy> AbstractionProxies
        {
            get
            {
                return m_AbstractionProxies;
            }
        }

        public List<VertexInNode> VertexIns
        {
            get
            {
                return m_VertexIns;
            }
        }

        public List<VertexOutNode> VertexOuts
        {
            get
            {
                return m_VertexOuts;
            }
        }

        public object Clone()
        {
            VertexShaderNode vs = new VertexShaderNode(m_Name);

            Dictionary<OutputNode, OutputNode> outputMapping = new Dictionary<OutputNode, OutputNode>();

            // Add to output map
            foreach (VertexInNode vertIn in VertexIns)
            {
                VertexInNode clonedVertexIn = (VertexInNode)vertIn.Clone();
                outputMapping.Add(vertIn.Output, clonedVertexIn.Output);
                vs.VertexIns.Add(clonedVertexIn);
            }

            foreach (GlobalNodeProxy proxy in GlobalProxies)
            {
                GlobalNodeProxy clonedProxy = (GlobalNodeProxy)proxy.Clone();
                outputMapping.Add(proxy.Output, clonedProxy.Output);
                vs.GlobalProxies.Add(clonedProxy);
            }

            foreach (VertexOutNode vertexOut in VertexOuts)
            {
                vs.VertexOuts.Add((VertexOutNode)vertexOut.Clone());
            }

            foreach (FunctionOpNode op in FunctionOps)
            {
                FunctionOpNode clonedOp = (FunctionOpNode)op.Clone();
                outputMapping.Add(op.Output, clonedOp.Output);
                vs.FunctionOps.Add(clonedOp);
            }

            foreach (AbstractionNodeProxy absProxy in AbstractionProxies)
            {
                AbstractionNodeProxy clonedProxy = (AbstractionNodeProxy)absProxy.Clone();

                for (int i = 0; i < absProxy.Outputs.Count; i++)
                {
                    outputMapping.Add(absProxy.Outputs[i].Output, clonedProxy.Outputs[i].Output);
                }

                vs.AbstractionProxies.Add(clonedProxy);
            }


            // Link outputs
            foreach (FunctionOpNode op in vs.FunctionOps)
            {
                foreach (InputNode input in op.Inputs)
                {
                    OutputNode binding = null;
                    if (input.Binding != null && outputMapping.TryGetValue(input.Binding, out binding))
                        input.Binding = binding;

                    if (input.AltBinding != null)
                    {
                        if (outputMapping.TryGetValue(input.AltBinding, out binding))
                            input.AltBinding = binding;
                    }
                }
            }

            foreach (VertexOutNode vertexOut in vs.VertexOuts)
            {
                InputNode input = vertexOut.Input;
                OutputNode binding = null;
                if (input.Binding != null)
                    if (outputMapping.TryGetValue(input.Binding, out binding))
                        input.Binding = binding;

                if (input.AltBinding != null)
                {
                    if (outputMapping.TryGetValue(input.AltBinding, out binding))
                        input.AltBinding = binding;
                }
            }

            foreach (AbstractionNodeProxy absProxy in vs.AbstractionProxies)
            {
                foreach (AbstractionNodeProxyIn absIn in absProxy.Inputs)
                {
                    InputNode input = absIn.Input;
                    OutputNode binding = null;
                    if (input.Binding != null && outputMapping.TryGetValue(input.Binding, out binding))
                        input.Binding = binding;

                    if (input.AltBinding != null)
                    {
                        if (outputMapping.TryGetValue(input.AltBinding, out binding))
                            input.AltBinding = binding;
                    }

                }
            }

            return vs;
        }


        #region Material Gen
        private Dictionary<InputNode, OutputNode> m_OldBindings = new Dictionary<InputNode,OutputNode>();
        private Dictionary<InputNode, OutputNode> m_OldAltBindings = new Dictionary<InputNode, OutputNode>();
        private void SaveAllBindings()
        {
            foreach(FunctionOpNode op in FunctionOps)
            {
                foreach(InputNode input in op.Inputs)
                {
                    m_OldBindings.Add(input, input.Binding);
                    m_OldAltBindings.Add(input, input.AltBinding);
                }
            }

            foreach(VertexOutNode vOut in VertexOuts)
            {
                m_OldBindings.Add(vOut.Input, vOut.Input.Binding);
                m_OldAltBindings.Add(vOut.Input, vOut.Input.AltBinding);
            }

            foreach(AbstractionNodeProxy abstractProxy in AbstractionProxies)
            {
                foreach(AbstractionNodeProxyIn proxyIn in abstractProxy.Inputs)
                {
                    m_OldBindings.Add(proxyIn.Input, proxyIn.Input.Binding);
                    m_OldAltBindings.Add(proxyIn.Input, proxyIn.Input.AltBinding);
                }
            }
        }

        private void RestoreAllBindings()
        {
            foreach(KeyValuePair<InputNode, OutputNode> entry in m_OldBindings)
            {
                entry.Key.Binding = entry.Value;
            }

            foreach (KeyValuePair<InputNode, OutputNode> entry in m_OldAltBindings)
            {
                entry.Key.AltBinding = entry.Value;
            }

            m_OldBindings.Clear();
            m_OldAltBindings.Clear(); 
        }

        private void ProcessAbstraction(AbstractionNodeProxy abstractionProxy, MaterialGen gen)
        {
            AbstractionNode abstraction = (AbstractionNode)abstractionProxy.Owner.Clone();
            m_ClonedAbstractions.Add(abstraction);

            // Expand the abstraction recursively first
            abstraction.BeginMaterialGen(gen);

            List<InputNode> allInputs = new List<InputNode>();
            abstraction.GatherAllInputs(allInputs);

            // Find input mapping
            Dictionary<InputNode, AbstractionInNode> inputMapping = new Dictionary<InputNode, AbstractionInNode>();
            Dictionary<InputNode, AbstractionInNode> inputAltMapping = new Dictionary<InputNode, AbstractionInNode>();
            foreach (AbstractionInNode abstractIn in abstraction.Ins)
            {
                foreach (InputNode input in allInputs)
                {
                    if (abstractIn.Output == input.Binding)
                        inputMapping.Add(input, abstractIn);

                    if (abstractIn.Output == input.AltBinding)
                        inputAltMapping.Add(input, abstractIn);
                }
            }

            // Bind inputs
            int i = 0;
            foreach(AbstractionNodeProxyIn proxyIn in abstractionProxy.Inputs)
            {
                OutputNode toBind = proxyIn.Input.Binding;
              
                // Rebind
                AbstractionInNode abstractInToBind = abstraction.Ins[i];
                foreach(KeyValuePair<InputNode, AbstractionInNode> entry in inputMapping)
                {
                    // If the in node matches the relevant proxy
                    if(entry.Value == abstractInToBind)
                    {
                        entry.Key.Binding = toBind;
                    }
                }

                foreach (KeyValuePair<InputNode, AbstractionInNode> entry in inputAltMapping)
                {
                    // If the in node matches the relevant proxy
                    if (entry.Value == abstractInToBind)
                    {
                        entry.Key.AltBinding = toBind;
                    }
                }

                i++;
            }

            // Bind outputs 
            // Find all input in shader that binds to abstract proxy outputs
            i = 0;
            foreach(AbstractionNodeProxyOut proxyOut in abstractionProxy.Outputs)
            {
                AbstractionOutNode abstractOutToBind = abstraction.Outs[i];
                
                foreach (FunctionOpNode op in FunctionOps)
                {
                    foreach(InputNode input in op.Inputs)
                    {
                        if(proxyOut.Output == input.Binding)
                        {
                            // Rebind to the relevant out node
                            input.Binding = abstractOutToBind.Input.Binding;
                        }

                        if (proxyOut.Output == input.AltBinding)
                        {
                            // Rebind to the relevant out node alt
                            input.AltBinding = abstractOutToBind.Input.Binding;
                        }
                    }
                }

                foreach(VertexOutNode vOut in VertexOuts)
                {
                    if(proxyOut.Output == vOut.Input.Binding)
                    {
                        // Rebind to the relevant out node
                        vOut.Input.Binding = abstractOutToBind.Input.Binding;
                    }

                    if (proxyOut.Output == vOut.Input.AltBinding)
                    {
                        // Rebind to the relevant out node alt
                        vOut.Input.AltBinding = abstractOutToBind.Input.Binding;
                    }
                }

                foreach(AbstractionNodeProxy otherAbstractProxy in AbstractionProxies)
                {
                    if(otherAbstractProxy != abstractionProxy)
                    {
                        foreach(AbstractionNodeProxyIn proxyIn in otherAbstractProxy.Inputs)
                        {
                            if(proxyOut.Output == proxyIn.Input.Binding)
                            {
                                proxyIn.Input.Binding = abstractOutToBind.Input.Binding;
                            }

                            if (proxyOut.Output == proxyIn.Input.AltBinding)
                            {
                                proxyIn.Input.AltBinding = abstractOutToBind.Input.Binding;
                            }
                        }
                    }
                }

                foreach (AbstractionNode cloned in m_ClonedAbstractions)
                {
                    List<FunctionOpNode> ops = new List<FunctionOpNode>();
                    cloned.GatherAllOps(ops);

                    // Find the input mapping to the current output proxy in the cloned abstraction
                    foreach (FunctionOpNode op in ops)
                    {
                        foreach(InputNode input in op.Inputs)
                        {
                            if (proxyOut.Output == input.Binding)
                                input.Binding = abstractOutToBind.Input.Binding;
                            if (proxyOut.Output == input.AltBinding)
                                input.AltBinding = abstractOutToBind.Input.Binding;
                        }
                    }
                }

                i++;
            }

            AddToShader(abstraction);
        }

        private void AddToShader(AbstractionNode abstraction)
        {
            if(m_Impl != null)
            {
                // Recursively gather all ops
                List<FunctionOpNode> allOps = new List<FunctionOpNode>();
                abstraction.GatherAllOps(allOps);

                foreach (FunctionOpNode op in allOps)
                {
                    Debug.Assert(op.Impl != null);
                    m_Impl.AddFunctionOp(op.Impl);
                }
            }
        }

        public void GatherAllOps(List<FunctionOpNode> allOps)
        {
            foreach (FunctionOpNode op in FunctionOps)
            {
                allOps.Add(op);
            }

            // Recursively call for cloned abstractions
            foreach (AbstractionNode absNode in m_ClonedAbstractions)
            {
                absNode.GatherAllOps(allOps);
            }
        }

        private VertexShader m_Impl;
        private List<AbstractionNode> m_ClonedAbstractions = new List<AbstractionNode>();
        public void BeginMaterialGen(MaterialGen gen)
        {
            if (Impl == null)
            {
                SaveAllBindings();

                m_Impl = gen.GetResourceMgr().CreateVertexShader(Name);

                // Rebind and clone proxies
                foreach (AbstractionNodeProxy abstractProxy in AbstractionProxies)
                {
                    // Expand all abstractions recursively and rebind them
                    ProcessAbstraction(abstractProxy, gen);
                }

                foreach(GlobalNodeProxy proxy in GlobalProxies)
                {
                    proxy.BeginMaterialGen(gen);
                }

                foreach(FunctionOpNode op in FunctionOps)
                {
                    op.BeginMaterialGen(gen);
                    m_Impl.AddFunctionOp(op.Impl);
                }

                foreach(VertexInNode vertIn in VertexIns)
                {
                    vertIn.BeginMaterialGen(gen);
                    m_Impl.AddVertexIn(vertIn.Impl);
                }

                foreach(VertexOutNode vertOut in VertexOuts)
                {
                    vertOut.BeginMaterialGen(gen);
                    m_Impl.AddVertexOut(vertOut.Impl);
                }
        
                m_Impl.SetVersion(m_VerMajor, m_VerMinor);
            }
        }

        public void ProcessBinding()
        {
            foreach (AbstractionNode abstraction in m_ClonedAbstractions)
            {
                abstraction.ProcessBinding();
            } 

            foreach (GlobalNodeProxy proxy in GlobalProxies)
            {
                proxy.ProcessBinding();
            }

            foreach (FunctionOpNode op in FunctionOps)
            {
                op.ProcessBinding();
            }

            foreach (VertexInNode vertIn in VertexIns)
            {
                vertIn.ProcessBinding();
            }

            foreach (VertexOutNode vertOut in VertexOuts)
            {
                vertOut.ProcessBinding();
            }
        }

        public void EndMaterialGen()
        {
            if (Impl != null)
            {
                m_Impl.Dispose();
                m_Impl = null;

                foreach (AbstractionNode abstraction in m_ClonedAbstractions)
                {
                    abstraction.EndMaterialGen();
                }

                foreach (GlobalNodeProxy proxy in GlobalProxies)
                {
                    proxy.EndMaterialGen();
                }

                foreach (FunctionOpNode op in FunctionOps)
                {
                    op.EndMaterialGen();
                }

                foreach (VertexInNode vertIn in VertexIns)
                {
                    vertIn.EndMaterialGen();
                }

                foreach (VertexOutNode vertOut in VertexOuts)
                {
                    vertOut.EndMaterialGen();
                }

                RestoreAllBindings();
                m_ClonedAbstractions.Clear();
            }
        }

        public VertexShader Impl
        {
            get
            {
                return m_Impl;
            }
        }
        #endregion
    }
}
