using System;
using System.Collections.Generic;
using System.Text;
using Common.PropertyGrid;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Diagnostics;
using System.Drawing;
using System.ComponentModel;
using ARFManaged.MaterialGenManaged;

namespace MaterialEditor
{
    [Serializable]
    public class AbstractionNodeProxy : PropertyNode, ISerializable, ICloneable
    {
        List<AbstractionNodeProxyIn> m_Inputs;
        List<AbstractionNodeProxyOut> m_Outputs;
        
        AbstractionNode m_Owner;
        RectLabel m_RectLabel;
       
        public AbstractionNodeProxy(AbstractionNode abstraction)
            : base()
        {
            m_Owner = abstraction;
            m_Owner.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(m_Owner_OnPropertyChanged);
            
            m_Inputs = new List<AbstractionNodeProxyIn>();
            ParseInputs();

            m_Outputs = new List<AbstractionNodeProxyOut>();
            ParseOutputs();

            m_RectLabel = new RectLabel();
            m_RectLabel.X = -9999;
            m_RectLabel.Y = -9999;
            m_RectLabel.Font = new Font("Verdana", 13);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.Khaki;
            m_RectLabel.Text = Name;
        }

        protected AbstractionNodeProxy(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Owner = (AbstractionNode)info.GetValue("Owner", typeof(AbstractionNode));
            
            m_Inputs = (List<AbstractionNodeProxyIn>)info.GetValue("Inputs", typeof(List<AbstractionNodeProxyIn>));
            if (m_Inputs == null)
                m_Inputs = new List<AbstractionNodeProxyIn>();

            m_Outputs = (List<AbstractionNodeProxyOut>)info.GetValue("Outputs", typeof(List<AbstractionNodeProxyOut>));
            if (m_Outputs == null)
                m_Outputs = new List<AbstractionNodeProxyOut>();

            m_RectLabel = (RectLabel)info.GetValue("RectLabel", typeof(RectLabel));
            m_RectLabel.Font = new Font("Verdana", 13);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.Khaki;

            m_Owner.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(m_Owner_OnPropertyChanged);
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Inputs", Inputs);
            info.AddValue("Outputs", Outputs);
            info.AddValue("Owner", Owner);
            info.AddValue("RectLabel", RectLabel);
        }

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            // Add input info

            PropertySpec ps = new PropertySpec(
                "Name", typeof(string), "Abstraction Proxy", "Name of the abstraction", "",
                typeof(DropDownTextAreaEditor), (string)null);
            ps.Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
            m_Properties.Properties.Add(ps);
        }

        protected override void PropertyGetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertyGetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": e.Value = Name; break;
            }
        }

        protected override void PropertySetValue(object sender, PropertySpecEventArgs e)
        {
            Debug.Assert(false);
        }

        void m_Owner_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            switch (e.Property.Name)
            {
                case "Name": m_RectLabel.Text = (string)e.Value; break;
            }
        }

        private void ParseInputs()
        {
            m_Inputs.Clear();

            foreach (AbstractionInNode abstractIn in m_Owner.Ins)
            {
                AbstractionNodeProxyIn input = new AbstractionNodeProxyIn(abstractIn);
                m_Inputs.Add(input);
            }
        }

        private void ParseOutputs()
        {
            m_Outputs.Clear();

            foreach(AbstractionOutNode abstractOut in Owner.Outs)
            {
                AbstractionNodeProxyOut output = new AbstractionNodeProxyOut(abstractOut);
                m_Outputs.Add(output);
            }
        }

        public string Name
        {
            get { return m_Owner.Name; }
        }

        public List<AbstractionNodeProxyOut> Outputs
        {
            get
            {
                return m_Outputs;
            }
        }

        public List<AbstractionNodeProxyIn> Inputs
        {
            get
            {
                return m_Inputs;
            }
        }

        public AbstractionNode Owner
        {
            get
            {
                return m_Owner;
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
            AbstractionNodeProxy clonedProxy = new AbstractionNodeProxy(Owner);
            clonedProxy.RectLabel.X = RectLabel.X;
            clonedProxy.RectLabel.Y = RectLabel.Y;

            int index = 0;
            foreach(AbstractionNodeProxyIn proxyIn in Inputs)
            {
                AbstractionNodeProxyIn clonedIn = clonedProxy.Inputs[index];
                clonedIn.Input.Binding = proxyIn.Input.Binding;
                clonedIn.Input.AltBinding = proxyIn.Input.AltBinding;
                clonedIn.Input.RectLabel.X = proxyIn.Input.RectLabel.X;
                clonedIn.Input.RectLabel.Y = proxyIn.Input.RectLabel.Y;
                clonedIn.Input.EllipseLabel.X = proxyIn.Input.EllipseLabel.X;
                clonedIn.Input.EllipseLabel.Y = proxyIn.Input.EllipseLabel.Y;

                index++;
            }

            index = 0;
            foreach (AbstractionNodeProxyOut proxyOut in Outputs)
            {
                AbstractionNodeProxyOut clonedOut = clonedProxy.Outputs[index];
                clonedOut.RectLabel.X = proxyOut.RectLabel.X;
                clonedOut.RectLabel.Y = proxyOut.RectLabel.Y;
                clonedOut.Output.EllipseLabel.X = proxyOut.Output.EllipseLabel.X;
                clonedOut.Output.EllipseLabel.Y = proxyOut.Output.EllipseLabel.Y;

                index++;
            }

            return clonedProxy;
        }
    }
}
