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
    public class PixelShaderOpNode : PropertyNode, ISerializable
    {
        PixelShaderNode m_Owner;

        public PixelShaderOpNode(PixelShaderNode ps)
            : base()
        {
            m_Owner = ps;
        }

        protected PixelShaderOpNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Owner = (PixelShaderNode)info.GetValue("Owner", typeof(PixelShaderNode));
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Owner", m_Owner);
        }

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            // Add input info

            PropertySpec ps = new PropertySpec(
                "Name", typeof(string), "Pixel Shader", "Name of the pixel shader", "",
                typeof(DropDownTextAreaEditor), (string)null);
            ps.Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
            m_Properties.Properties.Add(ps);

            ps = new PropertySpec(
                "Version Major", typeof(string), "Pixel Shader", "Compiler version (major) of the pixel shader", "",
                typeof(DropDownTextAreaEditor), (string)null);
            ps.Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
            m_Properties.Properties.Add(ps);

            ps = new PropertySpec(
                "Version Minor", typeof(string), "Pixel Shader", "Compiler version (minor) of the pixel shader", "",
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
                case "Version Major": e.Value = m_Owner.VerMajor; break;
                case "Version Minor": e.Value = m_Owner.VerMinor; break;
            }
        }

        protected override void PropertySetValue(object sender, PropertySpecEventArgs e)
        {
            Debug.Assert(false);
        }

        public PixelShaderNode Owner
        {
            get
            {
                return m_Owner;
            }
        }

        public object Clone()
        {
            return new PixelShaderOpNode(m_Owner);
        }

        #region Material Gen
        private PixelShaderOp m_Impl = null;
        public void BeginMaterialGen(MaterialGen gen)
        {
            if (Impl == null)
            {
                m_Owner.BeginMaterialGen(gen);
                m_Impl = m_Owner.Impl.CreateShaderOp();
            }
        }

        public void ProcessBinding()
        {
            m_Owner.ProcessBinding();
        }

        public void EndMaterialGen()
        {
            if (Impl != null)
            {
                m_Impl.Dispose();
                m_Impl = null;

                m_Owner.EndMaterialGen();
            }
        }

        public PixelShaderOp Impl
        {
            get
            {
                return m_Impl;
            }
        }
        #endregion

    }
}
