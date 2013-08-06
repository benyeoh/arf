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
    public class PassNode : PropertyNode, ISerializable
    {
        private string m_Name;
        
        VertexShaderOpNode m_VS;
        PixelShaderOpNode m_PS;
        List<RenderStateNode> m_States;

        public PassNode(string defaultName)
            : base()
        {
            m_States = new List<RenderStateNode>();
  
            m_VS = null;
            m_PS = null;
            m_Name = defaultName;
        }

        #region Serialization
        protected PassNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_States = (List<RenderStateNode>)info.GetValue("States", typeof(List<RenderStateNode>));
            if (m_States == null)
                m_States = new List<RenderStateNode>();

            m_Name = info.GetString("Name");
            m_VS = (VertexShaderOpNode)info.GetValue("VS", typeof(VertexShaderOpNode));
            m_PS = (PixelShaderOpNode)info.GetValue("PS", typeof(PixelShaderOpNode));

        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("States", m_States);
            info.AddValue("VS", m_VS);
            info.AddValue("PS", m_PS);
        }
        #endregion

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Pass", "Name of the pass", m_Name,
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
                m_Name = (string)value;
            }
        }

        public PixelShaderOpNode PS
        {
            get { return m_PS; }
            set
            {
                m_PS = (PixelShaderOpNode)value;
            }
        }

        public VertexShaderOpNode VS
        {
            get { return m_VS; }
            set
            {
                m_VS = (VertexShaderOpNode)value;
            }
        }

        public List<RenderStateNode> States
        {
            get
            {
                return m_States;
            }
        }

        public object Clone()
        {
            PassNode pass = new PassNode(m_Name);

            pass.PS = PS;
            pass.VS = VS;

            foreach (RenderStateNode rs in States)
            {
                RenderStateNode clonedRS = (RenderStateNode) rs.Clone();
                pass.States.Add(clonedRS);
            }

            return pass;
        }

        #region Material Gen
        private Pass m_Impl;
        public void BeginMaterialGen(MaterialGen gen)
        {
            if (Impl == null)
            {
                m_Impl = gen.GetResourceMgr().CreatePass(Name);
            }

            foreach(RenderStateNode state in States)
            {
                state.BeginMaterialGen(gen);
                m_Impl.AddRenderState(state.Impl);
            }

            m_VS.BeginMaterialGen(gen);
            m_Impl.SetVertexShaderOp(m_VS.Impl);

            m_PS.BeginMaterialGen(gen);
            m_Impl.SetPixelShaderOp(m_PS.Impl);

            //m_Proxy.BeginMaterialGen(gen);
        }

        public void ProcessBinding()
        {
            foreach (RenderStateNode state in States)
            {
                state.ProcessBinding();
            }

            m_VS.ProcessBinding();
            m_PS.ProcessBinding();
        }

        public void EndMaterialGen()
        {
            if (Impl != null)
            {
                m_Impl.Dispose();
                m_Impl = null;

                foreach (RenderStateNode state in States)
                {
                    state.EndMaterialGen();
                }

                m_VS.EndMaterialGen();
                m_PS.EndMaterialGen();
            }
        }

        public Pass Impl
        {
            get
            {
                return m_Impl;
            }
        }
        #endregion
    }
}
