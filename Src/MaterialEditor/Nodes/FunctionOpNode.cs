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
    public class FunctionOpNode : PropertyNode, ISerializable, IOutputNodeOwner, ICloneable
    {
        List<InputNode> m_Inputs;
        FunctionNode m_Owner;
        RectLabel m_RectLabel;

        OutputNode m_Output;

        public FunctionOpNode(FunctionNode func)
            : base()
        {
            m_Owner = func;
            m_Output = new OutputNode(this);

            m_Inputs = new List<InputNode>();
            UpdateInputs();

            m_Owner.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(m_Owner_OnPropertyChanged);
            m_RectLabel = new RectLabel();
            m_RectLabel.X = -9999;
            m_RectLabel.Y = -9999;
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.Gold;
            m_RectLabel.Text = Type + " " + Name;
        }

        protected FunctionOpNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Inputs = (List<InputNode>)info.GetValue("Inputs", typeof(List<InputNode>));
            if (m_Inputs == null)
                m_Inputs = new List<InputNode>();

            //foreach(InputNode input in m_Inputs)
            //{
            //    input.ReadOnly = true;
            //}

            m_Owner = (FunctionNode)info.GetValue("Owner", typeof(FunctionNode));
            m_Output = (OutputNode)info.GetValue("Output", typeof(OutputNode));
            m_RectLabel = (RectLabel)info.GetValue("RectLabel", typeof(RectLabel));
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.Gold;

            m_Owner.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(m_Owner_OnPropertyChanged);
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Inputs", Inputs);
            info.AddValue("Output", m_Output);
            info.AddValue("Owner", m_Owner);
            info.AddValue("RectLabel", m_RectLabel);
        }

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            // Add input info

            PropertySpec ps = new PropertySpec(
                "Name", typeof(string), "Function", "Name of the function", "",
                typeof(DropDownTextAreaEditor), (string)null);
            ps.Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
            m_Properties.Properties.Add(ps);

            ps = new PropertySpec(
                "Type", typeof(string), "Function", "Type of the function", "",
                typeof(DropDownTextAreaEditor), (string)null);
            ps.Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
            m_Properties.Properties.Add(ps);

            ps = new PropertySpec(
                "Value", typeof(string), "Function", "Value of the function", "",
                typeof(DropDownTextAreaEditor), (string)null);
            ps.Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
            m_Properties.Properties.Add(ps);
        }

        protected override void PropertyGetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertyGetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": e.Value = m_Owner.Name; break;
                case "Type": e.Value = m_Owner.Type; break;
                case "Value": e.Value = m_Owner.DefaultValue; break;
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
                case "Name": m_RectLabel.Text = m_Owner.Type + " " + (string)e.Value; break;
                case "Type": m_RectLabel.Text = (string)e.Value + " " + m_Owner.Name; break;
            }
        }


        private void UpdateInputs()
        {
            m_Inputs.Clear();

            foreach (InputNode input in m_Owner.Inputs)
            {
                InputNode newInput = new InputNode(input.Name);
                newInput.Type = input.Type;
                newInput.DefaultValue = input.DefaultValue;
                newInput.IsInputInstanced = true;
                m_Inputs.Add(newInput);
            }
        }

        public string Name
        {
            get { return m_Owner.Name; }
        }

        public string Type
        {
            get { return m_Owner.Type; }
        }

        public OutputNode Output
        {
            get
            {
                return m_Output;
            }
        }

        public List<InputNode> Inputs
        {           
            get
            {
                return m_Inputs;
            }
        }

        public string GetOwnerName()
        {
            return Name;
        }

        public string GetOwnerType()
        {
            return Type;
        } 

        public FunctionNode Owner
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
            FunctionOpNode op = new FunctionOpNode(m_Owner);

            for (int i = 0; i < op.Inputs.Count; ++i)
            {
                InputNode clonedInput = op.Inputs[i];
                InputNode origInput = Inputs[i];
                clonedInput.IsInputInstanced = origInput.IsInputInstanced;
                clonedInput.DefaultValue = origInput.DefaultValue;
                clonedInput.Type = origInput.Type;
                clonedInput.Binding = origInput.Binding;
                clonedInput.UseAltBinding = origInput.UseAltBinding;
                clonedInput.AltBinding = origInput.AltBinding;
                clonedInput.EllipseLabel.X = origInput.EllipseLabel.X;
                clonedInput.EllipseLabel.Y = origInput.EllipseLabel.Y;
                clonedInput.RectLabel.X = origInput.RectLabel.X;
                clonedInput.RectLabel.Y = origInput.RectLabel.Y;
            }
                //foreach (InputNode input in Inputs)
                //{
                //    op.Inputs.Add((InputNode)input.Clone());
                //}

            op.RectLabel.X = RectLabel.X;
            op.RectLabel.Y = RectLabel.Y;

            return op;
        }

        #region Material Gen
        private FunctionOp m_Impl;
        public void BeginMaterialGen(MaterialGen gen)
        {
            if (Impl == null)
            {
                m_Owner.BeginMaterialGen(gen);

                m_Impl = m_Owner.Impl.CreateFunctionOp();
                uint i = 0;
                foreach (InputNode input in Inputs)
                {
                    input.BeginMaterialGen(m_Impl.GetInput(i));
                    i++;
                }

                m_Output.BeginMaterialGen(m_Impl.GetOutput());
            }
            //m_Proxy.BeginMaterialGen(gen);
        }

        public void ProcessBinding()
        {
            foreach (InputNode input in Inputs)
            {
                input.ProcessBinding();
            }

            m_Output.ProcessBinding();
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

                m_Owner.EndMaterialGen();

                m_Output.EndMaterialGen();
            }
        }

        public FunctionOp Impl
        {
            get
            {
                return m_Impl;
            }
        }
        #endregion
    }
}
