using System;
using System.Collections.Generic;
using System.Text;
using Common.PropertyGrid;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Drawing;
using ARFManaged.MaterialGenManaged;
using System.ComponentModel;
using System.Diagnostics;

namespace MaterialEditor
{
    [Serializable]
    public class AbstractionNodeProxyOut : PropertyNode, ISerializable, IOutputNodeOwner
    {
        private OutputNode m_Output;
        private AbstractionOutNode m_Owner;

        private RectLabel m_RectLabel;

        public AbstractionNodeProxyOut(AbstractionOutNode abstractOut)
            : base()
        {
            m_Output = new OutputNode(this);
            m_Owner = abstractOut;
            m_Owner.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(m_Owner_OnPropertyChanged);

            m_RectLabel = new RectLabel();
            m_RectLabel.X = -9999;
            m_RectLabel.Y = -9999;
            m_RectLabel.Font = new Font("Verdana", 8);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.Lavender;
            m_RectLabel.Text = Type + "\n" + Name;
        }

        void m_Owner_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            switch (e.Property.Name)
            {
                case "Name": m_RectLabel.Text = m_Owner.Type + "\n" + (string)e.Value; break;
                case "Type": m_RectLabel.Text = (string)e.Value + "\n" + m_Owner.Name; break;
            }
        }

        #region Serialization
        protected AbstractionNodeProxyOut(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Output = (OutputNode)info.GetValue("Output", typeof(OutputNode));
            m_Owner = (AbstractionOutNode)info.GetValue("Owner", typeof(AbstractionOutNode));
            
            m_RectLabel = (RectLabel)info.GetValue("RectLabel", typeof(RectLabel));
            m_RectLabel.Font = new Font("Verdana", 8);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.Lavender;
            
            m_Owner.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(m_Owner_OnPropertyChanged);         
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Output", Output);
            info.AddValue("Owner", m_Owner);
            info.AddValue("RectLabel", m_RectLabel);
        }
        #endregion

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            PropertySpec ps = new PropertySpec(
               "Name", typeof(string), "Abstraction Proxy Out", "Name of the output", "",
               typeof(DropDownTextAreaEditor), (string)null);
            ps.Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
            m_Properties.Properties.Add(ps);

            ps = new PropertySpec(
                "Type", typeof(string), "Abstraction Proxy Out", "Type of the output", "",
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
                case "Type": e.Value = Type; break;
            }
        }

        protected override void PropertySetValue(object sender, PropertySpecEventArgs e)
        {
            Debug.Assert(false);
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

        public AbstractionOutNode Owner
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

        public string GetOwnerName()
        {
            return Name;
        }

        public string GetOwnerType()
        {
            return Type;
        }
    }
}
