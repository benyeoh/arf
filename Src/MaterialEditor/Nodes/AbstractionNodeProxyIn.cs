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
    public class AbstractionNodeProxyIn : ISerializable
    {
        InputNode m_Input;
        AbstractionInNode m_Owner;
       
        public AbstractionNodeProxyIn(AbstractionInNode abstractIn)
            : base()
        {
            m_Owner = abstractIn;

            m_Input = new InputNode(m_Owner.Name);
            m_Input.Type = m_Owner.Type;
            m_Input.IsInputInstanced = true;

            m_Owner.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(m_Owner_OnPropertyChanged);
        }

        protected AbstractionNodeProxyIn(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Owner = (AbstractionInNode)info.GetValue("Owner", typeof(AbstractionInNode));
            m_Input = (InputNode)info.GetValue("Input", typeof(InputNode));

            m_Owner.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(m_Owner_OnPropertyChanged);
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Input", Input);
            info.AddValue("Owner", Owner);
        }

    
        void m_Owner_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            switch (e.Property.Name)
            {
                case "Name": Input.Name = (string)e.Value; break;
                case "Type": Input.Type = (string)e.Value; break;
            }
        }

        public InputNode Input
        {
            get
            {
                return m_Input;
            }
        }

        public AbstractionInNode Owner
        {
            get
            {
                return m_Owner;
            }
        }
    }
}
