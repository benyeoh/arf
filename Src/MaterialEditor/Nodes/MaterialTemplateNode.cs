using System;
using System.Collections.Generic;
using System.Text;
using Common.PropertyGrid;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Diagnostics;
using System.Drawing;
using ARFManaged.MaterialGenManaged;
using ARFManaged.CoreManaged;

namespace MaterialEditor
{
    [Serializable]
    public class MaterialTemplateNode : PropertyNode, ISerializable
    {
        private string m_Name;

        List<GlobalNodeProxy> m_GlobalProxies;
        List<TechniqueNode> m_Techniques;

        public MaterialTemplateNode(string defaultName)
            : base()
        {
            m_GlobalProxies = new List<GlobalNodeProxy>();
            m_Techniques = new List<TechniqueNode>();

            m_Name = defaultName;
        }

        #region Serialization
        protected MaterialTemplateNode(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Techniques = (List<TechniqueNode>)info.GetValue("Techniques", typeof(List<TechniqueNode>));
            if (m_Techniques == null)
                m_Techniques = new List<TechniqueNode>();
            m_GlobalProxies = (List<GlobalNodeProxy>)info.GetValue("GlobalProxies", typeof(List<GlobalNodeProxy>));
            if (m_GlobalProxies == null)
                m_GlobalProxies = new List<GlobalNodeProxy>();

            m_Name = info.GetString("Name");
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            info.AddValue("Techniques", m_Techniques);
            info.AddValue("GlobalProxies", m_GlobalProxies);
        }
        #endregion

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Material Template", "Name of the material template", m_Name,
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

        public List<TechniqueNode> Techniques
        {
            get
            {
                return m_Techniques;
            }
        }

        public List<GlobalNodeProxy> GlobalProxies
        {
            get
            {
                return m_GlobalProxies;
            }
        }

        public object Clone()
        {
            MaterialTemplateNode materialTemplate = new MaterialTemplateNode(m_Name);

            foreach (GlobalNodeProxy proxy in GlobalProxies)
            {
                GlobalNodeProxy clonedProxy = (GlobalNodeProxy)proxy.Clone();
                materialTemplate.GlobalProxies.Add(clonedProxy);
            }

            foreach (TechniqueNode tech in Techniques)
            {
                TechniqueNode clonedTech = (TechniqueNode)tech.Clone();
                materialTemplate.Techniques.Add(clonedTech);
            }

            return materialTemplate;
        }

        #region Material Gen
        private void BeginMaterialGen(MaterialGen gen, MaterialTemplate matTemplate, List<GlobalNode> globals)
        {
            foreach (GlobalNodeProxy proxy in GlobalProxies)
            {
                proxy.BeginMaterialGen(gen);
            }

            foreach (TechniqueNode tech in Techniques)
            {
                tech.BeginMaterialGen(gen);
            }

            List<LocalNode> locals = new List<LocalNode>();
            List<FunctionNode> funcs = new List<FunctionNode>();

            List<VertexShaderNode> vss = new List<VertexShaderNode>();
            List<PixelShaderNode> pss = new List<PixelShaderNode>();
            foreach (TechniqueNode tech in Techniques)
            {
                foreach (PassNode pass in tech.Passes)
                {
                    VertexShaderNode vs = pass.VS.Owner;
                    if (!vss.Contains(vs))
                        vss.Add(vs);

                    PixelShaderNode ps = pass.PS.Owner;
                    if (!pss.Contains(ps))
                        pss.Add(ps);

                    foreach (RenderStateNode state in pass.States)
                    {
                        foreach (GlobalNodeProxy proxy in state.Dependents)
                        {
                            if (!globals.Contains(proxy.Proxy))
                                globals.Add(proxy.Proxy);

                            proxy.Proxy.GatherAllGlobals(globals);
                        }
                    }
                }
            }

            foreach (GlobalNodeProxy proxy in GlobalProxies)
            {
                proxy.Proxy.GatherAllGlobals(globals);

                if (!globals.Contains(proxy.Proxy))
                    globals.Add(proxy.Proxy);
            }

            foreach (VertexShaderNode vs in vss)
            {
                foreach (GlobalNodeProxy proxy in vs.GlobalProxies)
                {
                    proxy.Proxy.GatherAllGlobals(globals);

                    if (!globals.Contains(proxy.Proxy))
                        globals.Add(proxy.Proxy);
                }

                List<FunctionOpNode> allOps = new List<FunctionOpNode>();
                vs.GatherAllOps(allOps);
                foreach (FunctionOpNode op in allOps)
                {
                    if (!funcs.Contains(op.Owner))
                        funcs.Add(op.Owner);
                }
            }

            foreach (PixelShaderNode ps in pss)
            {
                foreach (GlobalNodeProxy proxy in ps.GlobalProxies)
                {
                    proxy.Proxy.GatherAllGlobals(globals);

                    if (!globals.Contains(proxy.Proxy))
                        globals.Add(proxy.Proxy);
                }

                List<FunctionOpNode> allOps = new List<FunctionOpNode>();
                ps.GatherAllOps(allOps);
                foreach (FunctionOpNode op in allOps)
                {
                    if (!funcs.Contains(op.Owner))
                        funcs.Add(op.Owner);
                }
            }

            foreach (GlobalNode global in globals)
            {
                foreach (LocalNodeProxy proxy in global.Locals)
                {
                    if (!locals.Contains(proxy.Proxy))
                        locals.Add(proxy.Proxy);
                }

                matTemplate.AddGlobal(global.Impl);
            }

            foreach (FunctionNode func in funcs)
            {
                foreach (LocalNodeProxy proxy in func.Locals)
                {
                    if (!locals.Contains(proxy.Proxy))
                        locals.Add(proxy.Proxy);
                }

                matTemplate.AddFunction(func.Impl);
            }

            foreach (LocalNode local in locals)
            {
                matTemplate.AddLocal(local.Impl);
            }

            foreach (TechniqueNode tech in Techniques)
            {
                matTemplate.AddTechnique(tech.Impl);
            }

            foreach (VertexShaderNode vs in vss)
            {
                matTemplate.AddVertexShader(vs.Impl);
            }

            foreach (PixelShaderNode ps in pss)
            {
                matTemplate.AddPixelShader(ps.Impl);
            }

            // Process binding
            foreach (GlobalNodeProxy proxy in GlobalProxies)
            {
                proxy.ProcessBinding();
            }

            foreach (TechniqueNode tech in Techniques)
            {
                tech.ProcessBinding();
            }
        }

        private void EndMaterialGen(MaterialGen gen)
        {
            // End Material Gen
            foreach (GlobalNodeProxy proxy in GlobalProxies)
            {
                proxy.EndMaterialGen();
            }

            foreach (TechniqueNode tech in Techniques)
            {
                tech.EndMaterialGen();
            }
        }

        public ByteBuffer Compile(MaterialGen gen, Dictionary<string, bool> globalAvail)
        {
            MaterialTemplate matTemplate = gen.GetResourceMgr().CreateMaterialTemplate();

            List<GlobalNode> globals = new List<GlobalNode>();
            BeginMaterialGen(gen, matTemplate, globals);

            ByteBuffer codeBuffer = new ByteBuffer();
            bool[] availabilty = new bool[globals.Count];
            for (int i = 0; i < availabilty.Length; i++)
            {
                availabilty[i] = true;
            }
 
            if (globalAvail != null)
            {
                for(int i=0; i < globals.Count; i++)
                {
                    GlobalNode global = globals[i];
                    if (globalAvail.ContainsKey(global.Name))
                        availabilty[i] = globalAvail[global.Name];
                }
            }

            bool result = matTemplate.Compile(codeBuffer, availabilty);
                //byte[] dataString = codeBuffer.GetData();
                //System.Text.Encoding enc = System.Text.Encoding.ASCII;
                //output = enc.GetString(dataString);

            // End Material Gen
            EndMaterialGen(gen);

            matTemplate.Dispose();
            matTemplate = null;

            return result ? codeBuffer : null;
        }

        public ByteBuffer Export(MaterialGen gen)
        {
            MaterialTemplate matTemplate = gen.GetResourceMgr().CreateMaterialTemplate();
            List<GlobalNode> globals = new List<GlobalNode>();
            BeginMaterialGen(gen, matTemplate, globals);

            ByteBuffer exportedBin = new ByteBuffer();
            SaveFns.SaveMaterialTemplate(matTemplate, exportedBin);

            // End Material Gen
            EndMaterialGen(gen);

            matTemplate.Dispose();
            matTemplate = null;

            return exportedBin;
        }

        #endregion
    }
}
