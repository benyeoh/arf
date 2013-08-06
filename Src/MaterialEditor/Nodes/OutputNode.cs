using System;
using System.Collections.Generic;
using System.Text;
using Common.PropertyGrid;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Drawing;
using ARFManaged.MaterialGenManaged;

namespace MaterialEditor
{
    [Serializable]
    public class OutputNode : ISerializable, IOutput
    {
        private IOutputNodeOwner m_Owner;

        private EllipseLabel m_EllipseLabel;
       
        public OutputNode(IOutputNodeOwner owner)
        {
            m_Owner = owner;

            m_EllipseLabel = new EllipseLabel();
            m_EllipseLabel.BackColor = Color.LimeGreen;
            m_EllipseLabel.Width = 12;
            m_EllipseLabel.Height = 12;
        }

        #region Serialization
        protected OutputNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            Type ownerType = (Type) info.GetValue("Owner Type", typeof(Type));
            m_Owner = (IOutputNodeOwner)info.GetValue("Owner", ownerType);
            m_EllipseLabel = (EllipseLabel)info.GetValue("EllipseLabel", typeof(EllipseLabel));           
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            Type ownerType = m_Owner.GetType();
            info.AddValue("Owner Type", ownerType);
            info.AddValue("Owner", m_Owner);
            info.AddValue("EllipseLabel", m_EllipseLabel);
        }
        #endregion

        public string Name
        {
            get
            {
                return m_Owner.GetOwnerName();
            }
        }

        public string Type
        {
            get
            {
                return m_Owner.GetOwnerType();
            }
        }

        public IOutputNodeOwner Owner
        {
            get
            {
                return m_Owner;
            }
        }

        public EllipseLabel EllipseLabel
        {
            get
            {
                return m_EllipseLabel;
            }
        }

        #region Material Gen
        private OutputResource m_Impl;
        public void BeginMaterialGen(OutputResource output)
        {
            if (Impl == null)
            {
                m_Impl = output;
                //m_Proxy.BeginMaterialGen(gen);
            }
        }

        public void ProcessBinding()
        {

        }

        public void EndMaterialGen()
        {
            if (Impl != null)
            {
                m_Impl.Dispose();
                m_Impl = null;
            }
        }

        public OutputResource Impl
        {
            get
            {
                return m_Impl;
            }
        }
        #endregion
    }
}
