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
    public class AbstractionNode : PropertyNode, ISerializable, ICloneable
    {
        private string m_Name;

        List<AbstractionInNode> m_Ins;
        List<FunctionOpNode> m_Ops;
        List<AbstractionOutNode> m_Outs;
        List<AbstractionNodeProxy> m_Proxies;

        public AbstractionNode(string defaultName)
            : base()
        {
            m_Ops = new List<FunctionOpNode>();
            m_Ins = new List<AbstractionInNode>();
            m_Outs = new List<AbstractionOutNode>();
            m_Proxies = new List<AbstractionNodeProxy>();

            m_Name = defaultName;
        }

        protected AbstractionNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Ops = (List<FunctionOpNode>)info.GetValue("Ops", typeof(List<FunctionOpNode>));
            if (m_Ops == null)
                m_Ops = new List<FunctionOpNode>();
            m_Ins = (List<AbstractionInNode>)info.GetValue("Ins", typeof(List<AbstractionInNode>));
            if (m_Ins == null)
                m_Ins = new List<AbstractionInNode>();
            m_Outs = (List<AbstractionOutNode>)info.GetValue("Outs", typeof(List<AbstractionOutNode>));
            if (m_Outs == null)
                m_Outs = new List<AbstractionOutNode>();
            m_Proxies = (List<AbstractionNodeProxy>)info.GetValue("Proxies", typeof(List<AbstractionNodeProxy>));
            if (m_Proxies == null)
                m_Proxies = new List<AbstractionNodeProxy>();

            m_Name = info.GetString("Name");
        }

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Abstraction", "Name of the abstraction", m_Name,
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

        public List<FunctionOpNode> Ops
        {
            get
            {
                return m_Ops;
            }
        }

        public List<AbstractionInNode> Ins
        {
            get
            {
                return m_Ins;
            }
        }

        public List<AbstractionOutNode> Outs
        {
            get
            {
                return m_Outs;
            }
        }

        public List<AbstractionNodeProxy> Proxies
        {
            get
            {
                return m_Proxies;
            }
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Ops", Ops);
            info.AddValue("Outs", Outs);
            info.AddValue("Ins", Ins);
            info.AddValue("Proxies", Proxies);
        }

        public object Clone()
        {
            AbstractionNode abstraction = new AbstractionNode(m_Name);
           
            Dictionary<OutputNode, OutputNode> outputMapping = new Dictionary<OutputNode, OutputNode>();
            
            // Add to output map
            foreach(AbstractionInNode abstractIn in Ins)
            {
                AbstractionInNode clonedAbstractIn = (AbstractionInNode) abstractIn.Clone();
                outputMapping.Add(abstractIn.Output, clonedAbstractIn.Output);
                abstraction.Ins.Add(clonedAbstractIn);
            }


            foreach (AbstractionOutNode abstractOut in Outs)
            {
                abstraction.Outs.Add((AbstractionOutNode)abstractOut.Clone());
            }

            foreach(FunctionOpNode op in Ops)
            {
                FunctionOpNode clonedOp = (FunctionOpNode)op.Clone();
                outputMapping.Add(op.Output, clonedOp.Output);
                abstraction.Ops.Add(clonedOp);                
            }

            foreach (AbstractionNodeProxy absProxy in Proxies)
            {
                AbstractionNodeProxy clonedProxy = (AbstractionNodeProxy)absProxy.Clone();

                for (int i=0; i < absProxy.Outputs.Count; i++)
                {
                    outputMapping.Add(absProxy.Outputs[i].Output, clonedProxy.Outputs[i].Output);
                }

                abstraction.Proxies.Add(clonedProxy);
            }


            // Link outputs
            foreach(FunctionOpNode op in abstraction.Ops)
            {
                foreach(InputNode input in op.Inputs)
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
            }

            foreach(AbstractionOutNode abstractOut in abstraction.Outs)
            {
                InputNode input = abstractOut.Input;
                OutputNode binding = null;
                if (outputMapping.TryGetValue(input.Binding, out binding))
                    input.Binding = binding;

                if (input.AltBinding != null)
                {
                    if (outputMapping.TryGetValue(input.AltBinding, out binding))
                        input.AltBinding = binding;
                }
            }

            foreach (AbstractionNodeProxy absProxy in abstraction.Proxies)
            {
                foreach(AbstractionNodeProxyIn absIn in absProxy.Inputs)
                {
                    InputNode input = absIn.Input;
                    OutputNode binding = null;
                    if (outputMapping.TryGetValue(input.Binding, out binding))
                        input.Binding = binding;

                    if (input.AltBinding != null)
                    {
                        if (outputMapping.TryGetValue(input.AltBinding, out binding))
                            input.AltBinding = binding;
                    }

                }
            }

            return abstraction;
        }

        #region Material Gen
        public void GatherAllInputs(List<InputNode> allInputs)
        {           
            foreach(FunctionOpNode op in Ops)
            {
                foreach (InputNode input in op.Inputs)
                    allInputs.Add(input);
            }

            foreach(AbstractionOutNode abstractOut in Outs)
            {
                allInputs.Add(abstractOut.Input);
            }

            // Recursively call for cloned abstractions
            foreach(AbstractionNode absNode in m_ClonedAbstractions)
            {
                absNode.GatherAllInputs(allInputs);
            }
        }

        public void GatherAllOps(List<FunctionOpNode> allOps)
        {      
            foreach (FunctionOpNode op in Ops)
            {
                allOps.Add(op);
            }

            // Recursively call for cloned abstractions
            foreach (AbstractionNode absNode in m_ClonedAbstractions)
            {
                absNode.GatherAllOps(allOps);
            }
        }

        private Dictionary<InputNode, OutputNode> m_OldBindings = new Dictionary<InputNode, OutputNode>();
        private Dictionary<InputNode, OutputNode> m_OldAltBindings = new Dictionary<InputNode, OutputNode>();
        private void SaveAllBindings()
        {
            //foreach (FunctionOpNode op in Ops)
            //{
            //    foreach (InputNode input in op.Inputs)
            //    {
            //        m_OldBindings.Add(input, input.Binding);
            //        m_OldAltBindings.Add(input, input.AltBinding);
            //    }
            //}

            //foreach (AbstractionOutNode absOut in Outs)
            //{
            //    m_OldBindings.Add(absOut.Input, absOut.Input.Binding);
            //    m_OldAltBindings.Add(absOut.Input, absOut.Input.AltBinding);
            //}

            //foreach (AbstractionNodeProxy abstractProxy in Proxies)
            //{
            //    foreach (AbstractionNodeProxyIn proxyIn in abstractProxy.Inputs)
            //    {
            //        m_OldBindings.Add(proxyIn.Input, proxyIn.Input.Binding);
            //        m_OldAltBindings.Add(proxyIn.Input, proxyIn.Input.AltBinding);
            //    }
            //}
        }

        private void RestoreAllBindings()
        {
            //foreach (KeyValuePair<InputNode, OutputNode> entry in m_OldBindings)
            //{
            //    entry.Key.Binding = entry.Value;
            //}

            //foreach (KeyValuePair<InputNode, OutputNode> entry in m_OldAltBindings)
            //{
            //    entry.Key.AltBinding = entry.Value;
            //}

            //m_OldBindings.Clear();
            //m_OldAltBindings.Clear();
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
            foreach (AbstractionNodeProxyIn proxyIn in abstractionProxy.Inputs)
            {
                OutputNode toBind = proxyIn.Input.Binding;
                if(toBind == null && proxyIn.Input.UseAltBinding)
                    toBind = proxyIn.Input.AltBinding;

                // Rebind
                AbstractionInNode abstractInToBind = abstraction.Ins[i];
                foreach (KeyValuePair<InputNode, AbstractionInNode> entry in inputMapping)
                {
                    // If the in node matches the relevant proxy
                    if (entry.Value == abstractInToBind)
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
            foreach (AbstractionNodeProxyOut proxyOut in abstractionProxy.Outputs)
            {
                AbstractionOutNode abstractOutToBind = abstraction.Outs[i];

                foreach (FunctionOpNode op in Ops)
                {
                    foreach (InputNode input in op.Inputs)
                    {
                        if (proxyOut.Output == input.Binding)
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

                foreach (AbstractionOutNode absOut in Outs)
                {
                    if (proxyOut.Output == absOut.Input.Binding)
                    {
                        // Rebind to the relevant out node
                        absOut.Input.Binding = abstractOutToBind.Input.Binding;
                    }

                    if (proxyOut.Output == absOut.Input.AltBinding)
                    {
                        // Rebind to the relevant out node alt
                        absOut.Input.AltBinding = abstractOutToBind.Input.Binding;
                    }
                }

                foreach (AbstractionNodeProxy otherAbstractProxy in Proxies)
                {
                    if (otherAbstractProxy != abstractionProxy)
                    {
                        foreach (AbstractionNodeProxyIn proxyIn in otherAbstractProxy.Inputs)
                        {
                            if (proxyOut.Output == proxyIn.Input.Binding)
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
                        foreach (InputNode input in op.Inputs)
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
        }

        private List<AbstractionNode> m_ClonedAbstractions = new List<AbstractionNode>();        
        public void BeginMaterialGen(MaterialGen gen)
        {
            SaveAllBindings();

            // Expand recursively for other abstractions
            foreach (AbstractionNodeProxy abstractProxy in Proxies)
            {
                // Expand all abstractions recursively and rebind them
                ProcessAbstraction(abstractProxy, gen);
            }

            foreach(FunctionOpNode op in Ops)
            {
                op.BeginMaterialGen(gen);
            }
        }

        public void ProcessBinding()
        {
            foreach (AbstractionNode abstraction in m_ClonedAbstractions)
            {
                abstraction.ProcessBinding();
            } 

            foreach (FunctionOpNode op in Ops)
            {
                op.ProcessBinding();
            }
        }

        public void EndMaterialGen()
        {
            foreach (AbstractionNode abstraction in m_ClonedAbstractions)
            {
                abstraction.EndMaterialGen();
            } 

            foreach (FunctionOpNode op in Ops)
            {
                op.EndMaterialGen();
            }

            RestoreAllBindings();
            m_ClonedAbstractions.Clear();
        }


        #endregion
    }
}
