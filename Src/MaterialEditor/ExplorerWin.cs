using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using WeifenLuo.WinFormsUI.Docking;
using Common.PropertyGrid;
using ARFManaged.CoreManaged;
using ARFManaged.RendererManaged;
using ARFManaged.MaterialGenManaged;

using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Collections;

namespace MaterialEditor
{
    public partial class ExplorerWin : DockContent
    {
        #region Serialization Helper Class
        [Serializable]
        private class ExplorerSerializer : ISerializable
        {
            public ExplorerWin m_Explorer = null;
            private List<ISerializable> m_Locals = new List<ISerializable>();
            private List<ISerializable> m_Globals = new List<ISerializable>();
            private List<ISerializable> m_Functions = new List<ISerializable>();
            private List<ISerializable> m_VSs = new List<ISerializable>();
            private List<ISerializable> m_PSs = new List<ISerializable>();
            private List<ISerializable> m_Templates = new List<ISerializable>();
            private List<ISerializable> m_Abstractions = new List<ISerializable>();

            public ExplorerWin Explorer
            {
                set
                {
                    m_Explorer = (ExplorerWin)value;
                }
            }

            public ExplorerSerializer()
            {
            }

            protected ExplorerSerializer(SerializationInfo info, StreamingContext context)
            {
                // Deserialize local
                int numLocals = info.GetInt32("Num Of Locals");
                for (int i = 0; i < numLocals; i++)
                {
                    ISerializable local = DeserializeNode("Local" + i, info, context);
                    m_Locals.Add(local);
                }

                // Deserialize globals
                int numGlobals = info.GetInt32("Num Of Globals");
                for (int i = 0; i < numGlobals; i++)
                {
                    ISerializable global = DeserializeNode("Global" + i, info, context);
                    m_Globals.Add(global);
                }

                // Deserialize Functions
                int numFunctions = info.GetInt32("Num Of Functions");
                for (int i = 0; i < numFunctions; i++)
                {
                    ISerializable function = DeserializeNode("Function" + i, info, context);
                    m_Functions.Add(function);
                }

                // Deserialize VS
                int numVS = info.GetInt32("Num Of VS");
                for (int i = 0; i < numVS; i++)
                {
                    ISerializable vs = DeserializeNode("VS" + i, info, context);
                    m_VSs.Add(vs);
                }

                // Deserialize PS
                int numPS = info.GetInt32("Num Of PS");
                for (int i = 0; i < numPS; i++)
                {
                    ISerializable ps = DeserializeNode("PS" + i, info, context);
                    m_PSs.Add(ps);
                }

                // Deserialize Material Templates
                int numTemplates = info.GetInt32("Num Of Templates");
                for (int i = 0; i < numTemplates; i++)
                {
                    ISerializable template = DeserializeNode("Template" + i, info, context);
                    m_Templates.Add(template);
                }

                // Deserialize Abstractions
                int numAbs = info.GetInt32("Num Of Abstractions");
                for (int i = 0; i < numAbs; i++)
                {
                    ISerializable abs = DeserializeNode("Abstraction" + i, info, context);
                    m_Abstractions.Add(abs);
                }
            }

            private void SerializeNode(ISerializable node, string label, SerializationInfo info, StreamingContext context)
            {
                Type nodeType = node.GetType();
                info.AddValue(label + "Type", nodeType);
                info.AddValue(label, node);
            }

            private ISerializable DeserializeNode(string label, SerializationInfo info, StreamingContext context)
            {
                Type nodeType = (Type)info.GetValue(label + "Type", typeof(Type));
                ISerializable obj = (ISerializable)info.GetValue(label, nodeType);
                
                return obj;
            }

            private void AddToExplorer(ISerializable obj, TreeNode root, ContextMenuStrip strip)
            {
                if (obj.GetType().Equals(typeof(GlobalNode)))
                {
                    m_Explorer.AddGlobal((GlobalNode)obj, root);
                }
                else if (obj.GetType().Equals(typeof(LocalNode)))
                {
                    m_Explorer.AddLocal((LocalNode)obj, root);
                }
                else if (obj.GetType().Equals(typeof(FunctionNode)))
                {
                    m_Explorer.AddFunction((FunctionNode)obj, root);
                }
                else if (obj.GetType().Equals(typeof(VertexShaderNode)))
                {
                    m_Explorer.AddVertexShader((VertexShaderNode)obj, root);
                }
                else if (obj.GetType().Equals(typeof(PixelShaderNode)))
                {
                    m_Explorer.AddPixelShader((PixelShaderNode)obj, root);
                }
                else if (obj.GetType().Equals(typeof(MaterialTemplateNode)))
                {
                    m_Explorer.AddMaterialTemplate((MaterialTemplateNode)obj, root);
                }
                else if(obj.GetType().Equals(typeof(AbstractionNode)))
                {
                    m_Explorer.AddAbstraction((AbstractionNode)obj, root);
                }
                else if (obj.GetType().Equals(typeof(FolderNode)))
                {
                    m_Explorer.AddFolder((FolderNode)obj, root, strip);
                }
            }

            public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
            {
                // Serialize locals
                TreeNode localRoot = m_Explorer.explorerTreeView.Nodes["Locals"];

                int numLocals = localRoot.Nodes.Count;
                info.AddValue("Num Of Locals", numLocals);
                int count = 0;
                foreach (TreeNode child in localRoot.Nodes)
                {
                    SerializeNode((ISerializable)child.Tag, "Local" + count, info, context);
                    count++;
                }

                // Serialize globals
                TreeNode globalRoot = m_Explorer.explorerTreeView.Nodes["Globals"];

                int numGlobals = globalRoot.Nodes.Count;
                info.AddValue("Num Of Globals", numGlobals);
                count = 0;
                foreach (TreeNode child in globalRoot.Nodes)
                {
                    SerializeNode((ISerializable)child.Tag, "Global" + count, info, context);
                    count++;
                }

                // Serialize Functions
                TreeNode functionRoot = m_Explorer.explorerTreeView.Nodes["Functions"];

                int numFunctions = functionRoot.Nodes.Count;
                info.AddValue("Num Of Functions", numFunctions);
                count = 0;
                foreach (TreeNode child in functionRoot.Nodes)
                {
                    SerializeNode((ISerializable)child.Tag, "Function" + count, info, context);
                    count++;
                }

                // Serialize Vertex Shader
                TreeNode vsRoot = m_Explorer.explorerTreeView.Nodes["Vertex Shaders"];

                int numVS = vsRoot.Nodes.Count;
                info.AddValue("Num Of VS", numVS);
                count = 0;
                foreach (TreeNode child in vsRoot.Nodes)
                {
                    SerializeNode((ISerializable)child.Tag, "VS" + count, info, context);
                    count++;
                }

                // Serialize Pixel Shader
                TreeNode psRoot = m_Explorer.explorerTreeView.Nodes["Pixel Shaders"];

                int numPS = psRoot.Nodes.Count;
                info.AddValue("Num Of PS", numPS);
                count = 0;
                foreach (TreeNode child in psRoot.Nodes)
                {
                    SerializeNode((ISerializable)child.Tag, "PS" + count, info, context);
                    count++;
                }

                // Serialize Material Templates
                TreeNode templateRoot = m_Explorer.explorerTreeView.Nodes["Material Templates"];

                int numTemplates = templateRoot.Nodes.Count;
                info.AddValue("Num Of Templates", numTemplates);
                count = 0;
                foreach (TreeNode child in templateRoot.Nodes)
                {
                    SerializeNode((ISerializable)child.Tag, "Template" + count, info, context);
                    count++;
                }

                // Serialize Abstractions
                TreeNode absRoot = m_Explorer.explorerTreeView.Nodes["Abstractions"];

                int numAbs = absRoot.Nodes.Count;
                info.AddValue("Num Of Abstractions", numAbs);
                count = 0;
                foreach (TreeNode child in absRoot.Nodes)
                {
                    SerializeNode((ISerializable)child.Tag, "Abstraction" + count, info, context);
                    count++;
                }
            }

            public void LoadAll()
            {
                List<DockContent> contents = m_Explorer.m_Main.GetAllDocumentsAndFloatingDocks();
                foreach(DockContent content in contents)
                {
                    if (content.GetType().Equals(typeof(DrawWin)) ||
                        content.GetType().Equals(typeof(CodeOutputWin)))
                    {
                        content.Close();
                    }
                }

                m_Explorer.m_Main.Properties.PropertyGrid.SelectedObject = null;

                // Add local
                TreeNode localRoot = m_Explorer.explorerTreeView.Nodes["Locals"];
                localRoot.Nodes.Clear();
                foreach (ISerializable local in m_Locals)
                {
                    AddToExplorer(local, localRoot, m_Explorer.LocalsContext);
                }
                m_Locals.Clear();

                // Add globals
                TreeNode globalRoot = m_Explorer.explorerTreeView.Nodes["Globals"];
                globalRoot.Nodes.Clear();
                foreach (ISerializable global in m_Globals)
                {
                    AddToExplorer(global, globalRoot, m_Explorer.GlobalsContext);
                }
                m_Globals.Clear();

                // Add Functions
                TreeNode functionRoot = m_Explorer.explorerTreeView.Nodes["Functions"];
                functionRoot.Nodes.Clear();
                foreach (ISerializable function in m_Functions)
                {
                    AddToExplorer(function, functionRoot, m_Explorer.FunctionsContext);
                }
                m_Functions.Clear();

                // Add VS
                TreeNode vsRoot = m_Explorer.explorerTreeView.Nodes["Vertex Shaders"];
                vsRoot.Nodes.Clear();
                foreach (ISerializable vs in m_VSs)
                {
                    AddToExplorer(vs, vsRoot, m_Explorer.VertexShadersContext);
                }
                m_VSs.Clear();

                // Add PS
                TreeNode psRoot = m_Explorer.explorerTreeView.Nodes["Pixel Shaders"];
                psRoot.Nodes.Clear();
                foreach (ISerializable ps in m_PSs)
                {
                    AddToExplorer(ps, psRoot, m_Explorer.PixelShadersContext);
                }
                m_PSs.Clear();

                // Add Material Templates
                TreeNode templateRoot = m_Explorer.explorerTreeView.Nodes["Material Templates"];
                templateRoot.Nodes.Clear();
                foreach (ISerializable template in m_Templates)
                {
                    AddToExplorer(template, templateRoot, m_Explorer.MaterialTemplatesContext);
                }
                m_Templates.Clear();

                // Add Material Templates
                TreeNode absRoot = m_Explorer.explorerTreeView.Nodes["Abstractions"];
                absRoot.Nodes.Clear();
                foreach (ISerializable abstraction in m_Abstractions)
                {
                    AddToExplorer(abstraction, absRoot, m_Explorer.AbstractionsContext);
                }
                m_Abstractions.Clear();

                // m_Explorer.explorerTreeView.ExpandAll();
                //  m_Explorer.explorerTreeView.CollapseAll();
                //m_Explorer.explorerTreeView.
                // m_Explorer.UnboldAllNodes(m_Explorer.explorerTreeView.Nodes);
            }
        }
        #endregion

        private MainWin m_Main;

        public ExplorerWin(MainWin main)
        {
            m_Main = main;

            InitializeComponent();
            InitializeTreeViewImages();
        }

        #region Images
        public void InitializeTreeViewImages()
        {
            ImageList images = new ImageList();
            images.ColorDepth = ColorDepth.Depth32Bit;
            images.Images.Add("Folder Open", ResourceImages.GetImage("folder_open.bmp"));
            images.Images.Add("Folder Closed", ResourceImages.GetImage("folder_closed.bmp"));
            images.Images.Add("Function", ResourceImages.GetImage("function.bmp"));
            images.Images.Add("Function Op", ResourceImages.GetImage("function_op.bmp"));
            images.Images.Add("Global", ResourceImages.GetImage("global.bmp"));
            images.Images.Add("Global Proxy", ResourceImages.GetImage("global_proxy.bmp"));
            images.Images.Add("Pixel In", ResourceImages.GetImage("pixel_in.bmp"));
            images.Images.Add("Pixel Out", ResourceImages.GetImage("pixel_out.bmp"));
            images.Images.Add("Vertex In", ResourceImages.GetImage("vertex_in.bmp"));
            images.Images.Add("Vertex Out", ResourceImages.GetImage("vertex_out.bmp"));
            images.Images.Add("Local", ResourceImages.GetImage("local.bmp"));
            images.Images.Add("Local Proxy", ResourceImages.GetImage("local_proxy.bmp"));
            images.Images.Add("Material Template", ResourceImages.GetImage("material_template.bmp"));
            images.Images.Add("PS", ResourceImages.GetImage("ps.bmp"));
            images.Images.Add("PS Op", ResourceImages.GetImage("ps_op.bmp"));
            images.Images.Add("VS", ResourceImages.GetImage("vs.bmp"));
            images.Images.Add("VS Op", ResourceImages.GetImage("vs_op.bmp"));
            images.Images.Add("Render State", ResourceImages.GetImage("render_state.bmp"));
            images.Images.Add("Technique", ResourceImages.GetImage("technique.bmp"));
            images.Images.Add("Pass", ResourceImages.GetImage("pass.bmp"));
            images.Images.Add("Description", ResourceImages.GetImage("description.bmp"));
            images.Images.Add("Input", ResourceImages.GetImage("input.bmp"));
            images.Images.Add("Abstraction", ResourceImages.GetImage("abstraction.bmp"));
            images.Images.Add("Abstraction Proxy", ResourceImages.GetImage("abstraction_proxy.bmp"));
            images.Images.Add("Abstraction In", ResourceImages.GetImage("abstract_in.bmp"));
            images.Images.Add("Abstraction Out", ResourceImages.GetImage("abstract_out.bmp"));

            explorerTreeView.ImageList = images;

            explorerTreeView.Nodes["Locals"].ImageKey = "Folder Closed";
            explorerTreeView.Nodes["Locals"].SelectedImageKey = explorerTreeView.Nodes["Locals"].ImageKey;
            explorerTreeView.Nodes["Globals"].ImageKey = "Folder Closed";
            explorerTreeView.Nodes["Globals"].SelectedImageKey = explorerTreeView.Nodes["Globals"].ImageKey;
            explorerTreeView.Nodes["Functions"].ImageKey = "Folder Closed";
            explorerTreeView.Nodes["Functions"].SelectedImageKey = explorerTreeView.Nodes["Functions"].ImageKey;
            explorerTreeView.Nodes["Pixel Shaders"].ImageKey = "Folder Closed";
            explorerTreeView.Nodes["Pixel Shaders"].SelectedImageKey = explorerTreeView.Nodes["Pixel Shaders"].ImageKey;
            explorerTreeView.Nodes["Vertex Shaders"].ImageKey = "Folder Closed";
            explorerTreeView.Nodes["Vertex Shaders"].SelectedImageKey = explorerTreeView.Nodes["Vertex Shaders"].ImageKey;
            explorerTreeView.Nodes["Abstractions"].ImageKey = "Folder Closed";
            explorerTreeView.Nodes["Abstractions"].SelectedImageKey = explorerTreeView.Nodes["Abstractions"].ImageKey;
            explorerTreeView.Nodes["Material Templates"].ImageKey = "Folder Closed";
            explorerTreeView.Nodes["Material Templates"].SelectedImageKey = explorerTreeView.Nodes["Material Templates"].ImageKey;
        }
        #endregion

        #region Serialization Accessors
        public void SerializeNodes(Stream outStream)
        {
            IFormatter formatter = m_Main.GetBinaryFormatter();
            
            m_Main.Output.WriteLine("Saving...");

            ExplorerSerializer serializer = new ExplorerSerializer();
            serializer.Explorer = this;

            try
            {
                formatter.Serialize(outStream, serializer);
                m_Main.Output.WriteLine("Done!");
            }
            catch (Exception e)
            {
                m_Main.Output.WriteLine("Failed to serialize: " + e.ToString());
            }

            //            ExplorerSerializer.m_Explorer = null;
        }

        public void DeserializeNodes(Stream inStream)
        {
            IFormatter formatter = m_Main.GetBinaryFormatter();

            m_Main.Output.Clear();
            m_Main.Output.WriteLine("Loading...");

            SuspendLayout();
            try
            {
                ExplorerSerializer loader = (ExplorerSerializer)formatter.Deserialize(inStream);
                loader.Explorer = this;
                loader.LoadAll();
                m_Main.Output.WriteLine("Done!");
            }
            catch (Exception e)
            {
                m_Main.Output.WriteLine("Failed to de-serialize: " + e.ToString());
            }
            m_Main.Output.WriteLine("");

            ResumeLayout();
        }
        #endregion

        #region Material Template Compilation
        private bool VerifyMaterialTemplate(MaterialTemplateNode template, bool isCodeGenerated, Dictionary<string, bool> globalAvail)
        {
            CodeOutputWin codeOutput = null;
                
            if (isCodeGenerated)
            {
                List<CodeOutputWin> allWin = new List<CodeOutputWin>();
                IEnumerable<IDockContent> contents = m_Main.DockPanel.Documents;
                foreach (IDockContent content in contents)
                {
                    if (content.GetType().Equals(typeof(CodeOutputWin)))
                    {
                        // Check to see if the draw win contains a particular panel
                        CodeOutputWin curWin = (CodeOutputWin)content;
                        allWin.Add(curWin);
                    }
                }

                foreach (FloatWindow content in m_Main.DockPanel.FloatWindows)
                {
                    foreach (DockPane pane in content.NestedPanes)
                    {
                        foreach (DockContent dock in pane.Contents)
                        {
                            if (dock.GetType().Equals(typeof(CodeOutputWin)))
                            {
                                // Check to see if the draw win contains a particular panel
                                CodeOutputWin curWin = (CodeOutputWin)dock;
                                allWin.Add(curWin);
                            }
                        }
                    }
                }

                foreach (CodeOutputWin curWin in allWin)
                {
                    if (curWin.TabText.Equals(template.Name + " [ generated code ]"))
                    {
                        codeOutput = curWin;
                        codeOutput.Activate();
                        break;
                    }
                }

                if (codeOutput == null)
                {
                    codeOutput = new CodeOutputWin(m_Main);
                    codeOutput.DockAreas = DockAreas.Float | DockAreas.Document;
                    codeOutput.TabText = template.Name + " [ generated code ]";
                    codeOutput.Show(m_Main.DockPanel, DockState.Document);
                }
            }

            m_Main.Output.WriteLine("------------------ Compiling: " + template.Name + " --------------------");
            bool success = true;
            ByteBuffer codeBuffer = template.Compile(m_Main.MaterialGen, globalAvail);
            if (codeBuffer == null)
            {
                m_Main.Output.WriteLine("Error: Failed to generate code");
                success = false;
            }
            else
            {

                if (isCodeGenerated)
                {
                    byte[] dataString = codeBuffer.GetData();
                    System.Text.Encoding enc = System.Text.Encoding.ASCII;
                    string code = enc.GetString(dataString);
                    codeOutput.SetText(code);
                }

                Effect effect = m_Main.Renderer.GetResourceMgr().LoadEffect(null, codeBuffer, null, null);
                if (effect == null)
                {
                    m_Main.Output.WriteLine("Error: Failed to compile effect");
                    success = false;
                }
                else
                    effect.Dispose();
            }


            m_Main.Output.WriteLine("");
            m_Main.Output.WriteLine("Compile complete -- " + (success ? "success" : "failed"));
            m_Main.Output.WriteLine("");

            return success;
        }

        private void ExportMaterialTemplate(MaterialTemplateNode template)
        {
            // Export material template to the same folder structure as the explorer tree
            TreeNode thisNode = FindNodeByTag(explorerTreeView.Nodes, template);
            List<FolderNode> parentFolders = new List<FolderNode>();
            TreeNode parent = thisNode.Parent;
            while (parent != null)
            {
                if (parent.Tag != null && parent.Tag.GetType() == typeof(FolderNode))
                {
                    parentFolders.Add((FolderNode) parent.Tag);
                }

                parent = parent.Parent;
            }

            parentFolders.Reverse();
            string exportPath = m_Main.Preferences.ExportDirectoryPath;
            foreach (FolderNode folder in parentFolders)
            {
                exportPath += "\\" + folder.Name;
            }

            if (!Directory.Exists(exportPath))
            {
                try
                {
                    Directory.CreateDirectory(exportPath);
                }
                catch (Exception e)
                {
                    m_Main.Output.WriteLine(e.ToString());
                    return;
                }
            }

            // Verify and export
            if (VerifyMaterialTemplate(template, false, null))
            {
                m_Main.Output.WriteLine("Exporting " + template.Name);
                ByteBuffer exportedBin = template.Export(m_Main.MaterialGen);
                m_Main.Output.WriteLine("Num Of Bytes: " + exportedBin.GetDataLength().ToString());

                string outPath = exportPath + "\\" + template.Name + ".bam";
                FileStream outStream = new FileStream(outPath, FileMode.Create, FileAccess.Write);
                    
                m_Main.Output.WriteLine("Writing file: " + outPath);
                outStream.Write(exportedBin.GetData(), 0, (int) exportedBin.GetDataLength());
                outStream.Close();
                m_Main.Output.WriteLine("Done!");                    
            }          
        }

        public void VerifyAll()
        {
            List<TreeNode> allNodes = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, allNodes, typeof(MaterialTemplateNode));
            foreach(TreeNode node in allNodes)
            {
                VerifyMaterialTemplate((MaterialTemplateNode) node.Tag, false, null);
            }
        }

        public void ExportAll()
        {
            List<TreeNode> allNodes = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, allNodes, typeof(MaterialTemplateNode));
            foreach(TreeNode node in allNodes)
            {
                ExportMaterialTemplate((MaterialTemplateNode) node.Tag);
            }
        }

        #endregion

        #region Add Nodes
        private TreeNode AddGlobal(GlobalNode global, TreeNode root)
        {
            global.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(global_OnPropertyChanged);
            TreeNode newNode = new TreeNode(global.Name);
            newNode.Tag = global;
            newNode.ContextMenuStrip = GlobalObjectContext;
            newNode.ImageKey = "Global";
            newNode.SelectedImageKey = "Global";

            foreach (LocalNodeProxy proxy in global.Locals)
            {
                AddLocalProxy(proxy, newNode);
            }

            foreach (InputNode input in global.Inputs)
            {
                AddInput(input, newNode);
            }

            foreach (GlobalDescriptionNode desc in global.Descriptions)
            {
                AddDescription(desc, newNode);
            }

            foreach (GlobalNodeProxy proxy in global.Dependents)
            {
                AddGlobalProxy(proxy, newNode);
            }

            if (root.Tag != null && root.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)root.Tag;
                if (!parentFolder.Nodes.Contains(global))
                    parentFolder.Nodes.Add(global);
            }

            root.Nodes.Add(newNode);
            //   newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddLocalProxy(LocalNodeProxy proxy, TreeNode root)
        {
            //proxy.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(input_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = LocalProxyObjectContext;
            newNode.Text = proxy.Proxy.Name;
            newNode.Tag = proxy;
            newNode.ImageKey = "Local Proxy";
            newNode.SelectedImageKey = "Local Proxy";

            // Add only to the end of the list before the descriptions
            int index = 0;
            foreach (TreeNode curNode in root.Nodes)
            {
                if (curNode.Tag.GetType().Equals(typeof(InputNode)) ||
                    curNode.Tag.GetType().Equals(typeof(GlobalDescriptionNode)))
                    break;
                index++;
            }

            root.Nodes.Insert(index, newNode);
            //    newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold | FontStyle.Italic);

            return newNode;
        }

        private TreeNode AddGlobalProxy(GlobalNodeProxy proxy, TreeNode root)
        {
            //proxy.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(input_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = GlobalProxyObjectContext;
            newNode.Text = proxy.Proxy.Name;
            newNode.Tag = proxy;
            newNode.ImageKey = "Global Proxy";
            newNode.SelectedImageKey = "Global Proxy";

            int index = 0;
            foreach (TreeNode curNode in root.Nodes)
            {
                if (!curNode.Tag.GetType().Equals(typeof(GlobalNodeProxy)))
                    break;
                index++;
            }

            root.Nodes.Insert(index, newNode);
            //    newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold | FontStyle.Italic);

            return newNode;
        }

        private TreeNode AddInput(InputNode input, TreeNode root)
        {
            input.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(input_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = InputObjectContext;
            newNode.Text = input.Name;
            newNode.Tag = input;
            newNode.ImageKey = "Input";
            newNode.SelectedImageKey = "Input";

            // Add only to the end of the list before the descriptions
            int index = 0;
            if (root.Tag.GetType().Equals(typeof(GlobalNode)))
            {
                foreach (TreeNode curNode in root.Nodes)
                {
                    if (curNode.Tag.GetType().Equals(typeof(GlobalDescriptionNode)))
                        break;
                    index++;
                }
            }
            else if (root.Tag.GetType().Equals(typeof(FunctionNode)))
            {
                index = root.Nodes.Count;
            }

            root.Nodes.Insert(index, newNode);
            //   newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddLocal(LocalNode local, TreeNode root)
        {
            local.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(local_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = LocalObjectContext;
            newNode.Text = local.Name;
            newNode.Tag = local;
            newNode.ImageKey = "Local";
            newNode.SelectedImageKey = "Local";

            if (root.Tag != null && root.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)root.Tag;
                if (!parentFolder.Nodes.Contains(local)) 
                    parentFolder.Nodes.Add(local);
            }

            root.Nodes.Add(newNode);
            //   newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddDescription(GlobalDescriptionNode desc, TreeNode root)
        {
            desc.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(globalDescription_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = GlobalDescriptionObjectContext;
            newNode.Text = desc.Name;
            newNode.Tag = desc;
            newNode.ImageKey = "Description";
            newNode.SelectedImageKey = "Description";

            root.Nodes.Add(newNode);
            //   newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddFunction(FunctionNode func, TreeNode root)
        {
            func.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(func_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = FunctionObjectContext;
            newNode.Text = func.Name;
            newNode.Tag = func;
            newNode.ImageKey = "Function";
            newNode.SelectedImageKey = "Function";

            foreach (InputNode input in func.Inputs)
            {
                AddInput(input, newNode);
            }

            foreach (LocalNodeProxy proxy in func.Locals)
            {
                AddLocalProxy(proxy, newNode);
            }

            if (root.Tag != null && root.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)root.Tag;
                if (!parentFolder.Nodes.Contains(func)) 
                    parentFolder.Nodes.Add(func);
            }

            root.Nodes.Add(newNode);
            //   newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddVertexShader(VertexShaderNode vs, TreeNode root)
        {
            vs.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(vs_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = VertexShaderObjectContext;
            newNode.Text = vs.Name;
            newNode.Tag = vs;
            newNode.ImageKey = "VS";
            newNode.SelectedImageKey = "VS";

            foreach (GlobalNodeProxy proxy in vs.GlobalProxies)
            {
                AddGlobalProxy(proxy, newNode);
            }

            foreach (FunctionOpNode op in vs.FunctionOps)
            {
                AddFunctionOp(op, newNode);
            }

            foreach (VertexInNode vertIn in vs.VertexIns)
            {
                AddVertexIn(vertIn, newNode);
            }

            foreach (VertexOutNode vertOut in vs.VertexOuts)
            {
                AddVertexOut(vertOut, newNode);
            }

            foreach (AbstractionNodeProxy absProxy in vs.AbstractionProxies)
            {
                AddAbstractionProxy(absProxy, newNode);
            }

            if (root.Tag != null && root.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)root.Tag;
                if (!parentFolder.Nodes.Contains(vs)) 
                    parentFolder.Nodes.Add(vs);
            }

            root.Nodes.Add(newNode);
            //   newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddPixelShader(PixelShaderNode ps, TreeNode root)
        {
            ps.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(ps_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = PixelShaderObjectContext;
            newNode.Text = ps.Name;
            newNode.Tag = ps;
            newNode.ImageKey = "PS";
            newNode.SelectedImageKey = "PS";

            foreach (GlobalNodeProxy proxy in ps.GlobalProxies)
            {
                AddGlobalProxy(proxy, newNode);
            }

            foreach (FunctionOpNode op in ps.FunctionOps)
            {
                AddFunctionOp(op, newNode);
            }

            foreach (PixelInNode pixIn in ps.PixelIns)
            {
                AddPixelIn(pixIn, newNode);
            }

            foreach (PixelOutNode pixOut in ps.PixelOuts)
            {
                AddPixelOut(pixOut, newNode);
            }

            foreach (AbstractionNodeProxy absProxy in ps.AbstractionProxies)
            {
                AddAbstractionProxy(absProxy, newNode);
            }

            if (root.Tag != null && root.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)root.Tag;
                if (!parentFolder.Nodes.Contains(ps)) 
                    parentFolder.Nodes.Add(ps);
            }

            root.Nodes.Add(newNode);
            //  newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddFunctionOp(FunctionOpNode op, TreeNode root)
        {
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = FunctionOpObjectContext;
            newNode.Text = op.Name;
            newNode.Tag = op;
            newNode.ImageKey = "Function Op";
            newNode.SelectedImageKey = "Function Op";

            root.Nodes.Add(newNode);
            //   newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold | FontStyle.Italic);

            return newNode;

        }

        private TreeNode AddVertexIn(VertexInNode vertIn, TreeNode root)
        {
            vertIn.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(vertIn_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = VertexInObjectContext;
            newNode.Text = vertIn.Name;
            newNode.Tag = vertIn;
            newNode.ImageKey = "Vertex In";
            newNode.SelectedImageKey = "Vertex In";

            int index = 0;
            foreach (TreeNode curNode in root.Nodes)
            {
                if (curNode.Tag.GetType().Equals(typeof(FunctionOpNode)) ||
                    curNode.Tag.GetType().Equals(typeof(VertexOutNode)) ||
                    curNode.Tag.GetType().Equals(typeof(AbstractionNodeProxy)))
                    break;
                index++;
            }

            root.Nodes.Insert(index, newNode);
            //   newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddVertexOut(VertexOutNode vertOut, TreeNode root)
        {
            vertOut.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(vertOut_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = VertexOutObjectContext;
            newNode.Text = vertOut.Name;
            newNode.Tag = vertOut;
            newNode.ImageKey = "Vertex Out";
            newNode.SelectedImageKey = "Vertex Out";

            int index = 0;
            foreach (TreeNode curNode in root.Nodes)
            {
                if (curNode.Tag.GetType().Equals(typeof(FunctionOpNode)) ||
                     curNode.Tag.GetType().Equals(typeof(AbstractionNodeProxy)))
                    break;
                index++;
            }

            root.Nodes.Insert(index, newNode);
            //  newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddPixelIn(PixelInNode pixIn, TreeNode root)
        {
            pixIn.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(pixIn_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = PixelInObjectContext;
            newNode.Text = pixIn.Name;
            newNode.Tag = pixIn;
            newNode.ImageKey = "Pixel In";
            newNode.SelectedImageKey = "Pixel In";

            int index = 0;
            foreach (TreeNode curNode in root.Nodes)
            {
                if (curNode.Tag.GetType().Equals(typeof(FunctionOpNode)) ||
                    curNode.Tag.GetType().Equals(typeof(PixelOutNode)) ||
                    curNode.Tag.GetType().Equals(typeof(AbstractionNodeProxy)))
                    break;
                index++;
            }

            root.Nodes.Insert(index, newNode);
            // newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddPixelOut(PixelOutNode pixOut, TreeNode root)
        {
            pixOut.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(pixOut_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = PixelOutObjectContext;
            newNode.Text = pixOut.Name;
            newNode.Tag = pixOut;
            newNode.ImageKey = "Pixel Out";
            newNode.SelectedImageKey = "Pixel Out";

            int index = 0;
            foreach (TreeNode curNode in root.Nodes)
            {
                if (curNode.Tag.GetType().Equals(typeof(FunctionOpNode)) ||
                    curNode.Tag.GetType().Equals(typeof(AbstractionNodeProxy)))
                    break;
                index++;
            }

            root.Nodes.Insert(index, newNode);
            //  newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddRenderState(RenderStateNode state, TreeNode root)
        {
            state.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(state_OnPropertyChanged);
            TreeNode newNode = new TreeNode(state.Name);
            newNode.Tag = state;
            newNode.ContextMenuStrip = RenderStateObjectContext;
            newNode.ImageKey = "Render State";
            newNode.SelectedImageKey = "Render State";

            foreach (InputNode input in state.Inputs)
            {
                AddInput(input, newNode);
            }

            foreach (GlobalNodeProxy proxy in state.Dependents)
            {
                AddGlobalProxy(proxy, newNode);
            }

            root.Nodes.Add(newNode);
            //  newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddMaterialTemplate(MaterialTemplateNode template, TreeNode root)
        {
            template.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(template_OnPropertyChanged);
            TreeNode newNode = new TreeNode(template.Name);
            newNode.Tag = template;
            newNode.ContextMenuStrip = MaterialTemplateObjectContext;
            newNode.ImageKey = "Material Template";
            newNode.SelectedImageKey = "Material Template";

            foreach (GlobalNodeProxy proxy in template.GlobalProxies)
            {
                AddGlobalProxy(proxy, newNode);
            }

            foreach (TechniqueNode tech in template.Techniques)
            {
                AddTechnique(tech, newNode);
            }

            if (root.Tag != null && root.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)root.Tag;
                if (!parentFolder.Nodes.Contains(template)) 
                    parentFolder.Nodes.Add(template);
            }

            root.Nodes.Add(newNode);
            //  newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddTechnique(TechniqueNode tech, TreeNode root)
        {
            tech.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(tech_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = TechniqueObjectContext;
            newNode.Text = tech.Name;
            newNode.Tag = tech;
            newNode.ImageKey = "Technique";
            newNode.SelectedImageKey = "Technique";

            foreach (PassNode pass in tech.Passes)
            {
                AddPass(pass, newNode);
            }

            //// Add only to the end of the list before the descriptions
            //int index = 0;
            //if (root.Tag.GetType().Equals(typeof(GlobalNode)))
            //{
            //    foreach (TreeNode curNode in root.Nodes)
            //    {
            //        if (curNode.Tag.GetType().Equals(typeof(GlobalDescriptionNode)))
            //            break;
            //        index++;
            //    }
            //}
            //else if (root.Tag.GetType().Equals(typeof(FunctionNode)))
            //{
            //    index = root.Nodes.Count;
            //}

            root.Nodes.Add(newNode);
            // newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddPass(PassNode pass, TreeNode root)
        {
            pass.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(pass_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = PassObjectContext;
            newNode.Text = pass.Name;
            newNode.Tag = pass;
            newNode.ImageKey = "Pass";
            newNode.SelectedImageKey = "Pass";

            foreach (RenderStateNode state in pass.States)
            {
                AddRenderState(state, newNode);
            }

            if (pass.VS != null)
                SetVertexShaderOp(pass.VS, newNode);

            if (pass.PS != null)
                SetPixelShaderOp(pass.PS, newNode);

            root.Nodes.Add(newNode);
            // newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode SetPixelShaderOp(PixelShaderOpNode op, TreeNode root)
        {
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = PixelShaderOpObjectContext;
            newNode.Text = op.Owner.Name;
            newNode.Tag = op;
            newNode.ImageKey = "PS Op";
            newNode.SelectedImageKey = "PS Op";

            int index = 0;
            foreach (TreeNode curNode in root.Nodes)
            {
                if (curNode.Tag.GetType().Equals(typeof(PixelShaderOpNode)))
                {
                    // Remove the previous
                    RemovePixelShaderOp((PixelShaderOpNode)curNode.Tag, root);
                    break;
                }
                else if (curNode.Tag.GetType().Equals(typeof(RenderStateNode)))
                {
                    break;
                }

                index++;
            }

            root.Nodes.Insert(index, newNode);
            // newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold | FontStyle.Italic);

            return newNode;
        }

        private TreeNode SetVertexShaderOp(VertexShaderOpNode op, TreeNode root)
        {
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = VertexShaderOpObjectContext;
            newNode.Text = op.Owner.Name;
            newNode.Tag = op;
            newNode.ImageKey = "VS Op";
            newNode.SelectedImageKey = "VS Op";

            int index = 0;
            foreach (TreeNode curNode in root.Nodes)
            {
                if (curNode.Tag.GetType().Equals(typeof(VertexShaderOpNode)))
                {
                    // Remove the previous
                    RemoveVertexShaderOp((VertexShaderOpNode)curNode.Tag, root);
                    break;
                }
                else if (curNode.Tag.GetType().Equals(typeof(PixelShaderOpNode)))
                {
                    break;
                }
                else if (curNode.Tag.GetType().Equals(typeof(RenderStateNode)))
                {
                    break;
                }

                index++;
            }

            root.Nodes.Insert(index, newNode);
            //newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold | FontStyle.Italic);

            return newNode;
        }

        private TreeNode AddAbstractionIn(AbstractionInNode abstractIn, TreeNode root)
        {
            abstractIn.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(abstractIn_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = AbstractionInObjectContext;
            newNode.Text = abstractIn.Name;
            newNode.Tag = abstractIn;
            newNode.ImageKey = "Abstraction In";
            newNode.SelectedImageKey = "Abstraction In";

            int index = 0;
            foreach (TreeNode curNode in root.Nodes)
            {
                if (curNode.Tag.GetType().Equals(typeof(FunctionOpNode)) ||
                    curNode.Tag.GetType().Equals(typeof(AbstractionOutNode)) ||
                    curNode.Tag.GetType().Equals(typeof(AbstractionNodeProxy)))
                    break;
                index++;
            }

            root.Nodes.Insert(index, newNode);

            return newNode;
        }

        private TreeNode AddAbstractionOut(AbstractionOutNode abstractOut, TreeNode root)
        {
            abstractOut.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(abstractOut_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = AbstractionOutObjectContext;
            newNode.Text = abstractOut.Name;
            newNode.Tag = abstractOut;
            newNode.ImageKey = "Abstraction Out";
            newNode.SelectedImageKey = "Abstraction Out";

            int index = 0;
            foreach (TreeNode curNode in root.Nodes)
            {
                if (curNode.Tag.GetType().Equals(typeof(FunctionOpNode)) ||
                    curNode.Tag.GetType().Equals(typeof(AbstractionNodeProxy)))
                    break;
                index++;
            }

            root.Nodes.Insert(index, newNode);

            return newNode;
        }

        private TreeNode AddAbstraction(AbstractionNode abstraction, TreeNode root)
        {
            abstraction.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(abstraction_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = AbstractionObjectContext;
            newNode.Text = abstraction.Name;
            newNode.Tag = abstraction;
            newNode.ImageKey = "Abstraction";
            newNode.SelectedImageKey = "Abstraction";

            foreach (FunctionOpNode op in abstraction.Ops)
            {
                AddFunctionOp(op, newNode);
            }

            foreach (AbstractionNodeProxy absProxy in abstraction.Proxies)
            {
                AddAbstractionProxy(absProxy, newNode);
            }

            foreach (AbstractionInNode abstractIn in abstraction.Ins)
            {
                AddAbstractionIn(abstractIn, newNode);
            }

            foreach (AbstractionOutNode abstractOut in abstraction.Outs)
            {
                AddAbstractionOut(abstractOut, newNode);
            }

            if (root.Tag != null && root.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)root.Tag;
                if (!parentFolder.Nodes.Contains(abstraction))
                    parentFolder.Nodes.Add(abstraction);
            }

            root.Nodes.Add(newNode);
            //   newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddAbstractionProxy(AbstractionNodeProxy abstractionProxy, TreeNode root)
        {
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = AbstractionProxyObjectContext;
            newNode.Text = abstractionProxy.Name;
            newNode.Tag = abstractionProxy;
            newNode.ImageKey = "Abstraction Proxy";
            newNode.SelectedImageKey = "Abstraction Proxy";

            int index = 0;
            foreach (TreeNode curNode in root.Nodes)
            {
                if (curNode.Tag.GetType().Equals(typeof(FunctionOpNode)))
                    break;
                index++;
            }

            root.Nodes.Insert(index, newNode);
            //   newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        private TreeNode AddFolder(FolderNode folder, TreeNode root, ContextMenuStrip strip)
        {
            folder.OnPropertyChanged += new PropertyNode.HandlePropertyChanged(folder_OnPropertyChanged);
            TreeNode newNode = new TreeNode();
            newNode.ContextMenuStrip = strip;
            newNode.Text = folder.Name;
            newNode.Tag = folder;
            newNode.ImageKey = "Folder Closed";
            newNode.SelectedImageKey = "Folder Closed";

            foreach (ISerializable obj in folder.Nodes)
            {
                if (obj.GetType().Equals(typeof(GlobalNode)))
                {
                    AddGlobal((GlobalNode)obj, newNode);
                }
                else if (obj.GetType().Equals(typeof(LocalNode)))
                {
                    AddLocal((LocalNode)obj, newNode);
                }
                else if (obj.GetType().Equals(typeof(FunctionNode)))
                {
                    AddFunction((FunctionNode)obj, newNode);
                }
                else if (obj.GetType().Equals(typeof(VertexShaderNode)))
                {
                    AddVertexShader((VertexShaderNode)obj, newNode);
                }
                else if (obj.GetType().Equals(typeof(PixelShaderNode)))
                {
                    AddPixelShader((PixelShaderNode)obj, newNode);
                }
                else if (obj.GetType().Equals(typeof(MaterialTemplateNode)))
                {
                    AddMaterialTemplate((MaterialTemplateNode)obj, newNode);
                }
                else if (obj.GetType().Equals(typeof(AbstractionNode)))
                {
                    AddAbstraction((AbstractionNode)obj, newNode);
                }
                else if (obj.GetType().Equals(typeof(FolderNode)))
                {
                    AddFolder((FolderNode)obj, newNode, strip);
                }
            }

            int index = 0;
            foreach (TreeNode curNode in root.Nodes)
            {
                if (curNode.Tag.GetType().Equals(typeof(FolderNode)))
                {
                    if (newNode.Text.CompareTo(curNode.Text) < 0)
                        break;
                }
                else
                    break;

                index++;
            }

            if (root.Tag != null && root.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)root.Tag;
                if (!parentFolder.Nodes.Contains(folder)) 
                    parentFolder.Nodes.Insert(index, folder);
            }

            root.Nodes.Insert(index, newNode);
            //newNode.NodeFont = new Font(explorerTreeView.Font, FontStyle.Bold);

            return newNode;
        }

        #endregion

        #region Remove Nodes
        private void RemoveInput(InputNode input, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == input);

            // Remove all references to the object
            TreeNode parentNode = toDelete.Parent;
            Type objectType = parentNode.Tag.GetType();
            if (objectType.Equals(typeof(GlobalNode)))
            {
                GlobalNode global = (GlobalNode)parentNode.Tag;
                global.Inputs.Remove(input);
            }
            else if (objectType.Equals(typeof(FunctionNode)))
            {
                FunctionNode function = (FunctionNode)parentNode.Tag;
                int index = function.Inputs.IndexOf(input);
                function.Inputs.Remove(input);

                // Remove the input from all function ops
                List<TreeNode> opTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, opTreeNodes, typeof(FunctionOpNode));
                foreach (TreeNode treeNode in opTreeNodes)
                {
                    FunctionOpNode op = (FunctionOpNode)treeNode.Tag;
                    if (op.Owner == function)
                    {
                        op.Inputs.RemoveAt(index);
                    }
                }
            }
            else if (objectType.Equals(typeof(RenderStateNode)))
            {
                RenderStateNode state = (RenderStateNode)parentNode.Tag;
                state.Inputs.Remove(input);
            }

            // We remove it from the property window
            if (input.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveLocal(LocalNode local, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == local);

            // Remove object only if there are no proxies
            List<TreeNode> proxies = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, proxies, typeof(LocalNodeProxy));
            foreach (TreeNode treeNode in proxies)
            {
                LocalNodeProxy proxy = (LocalNodeProxy)treeNode.Tag;
                if (proxy.Proxy == local)
                {
                    RemoveLocalProxy(proxy, treeNode);
                }
            }

            // We remove it from the property window
            if (local.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            if (node.Parent.Tag != null && node.Parent.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)node.Parent.Tag;
                parentFolder.Nodes.Remove(local);
            }

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveLocalProxy(LocalNodeProxy localProxy, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == localProxy);

            // Remove all references to the object
            TreeNode parentNode = toDelete.Parent;
            Type objectType = parentNode.Tag.GetType();
            if (objectType.Equals(typeof(GlobalNode)))
            {
                GlobalNode global = (GlobalNode)parentNode.Tag;
                global.Locals.Remove(localProxy);
            }
            else if (objectType.Equals(typeof(FunctionNode)))
            {
                FunctionNode func = (FunctionNode)parentNode.Tag;
                func.Locals.Remove(localProxy);
            }

            // We remove it from the property window
            if (localProxy.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveGlobalProxy(GlobalNodeProxy globalProxy, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == globalProxy);

            // Remove all references to the object
            TreeNode parentNode = toDelete.Parent;
            Type objectType = parentNode.Tag.GetType();
            if (objectType.Equals(typeof(GlobalNode)))
            {
                GlobalNode global = (GlobalNode)parentNode.Tag;
                global.Dependents.Remove(globalProxy);
            }
            else if (objectType.Equals(typeof(RenderStateNode)))
            {
                RenderStateNode state = (RenderStateNode)parentNode.Tag;
                state.Dependents.Remove(globalProxy);
            }
            else if (objectType.Equals(typeof(VertexShaderNode)))
            {
                VertexShaderNode vs = (VertexShaderNode)parentNode.Tag;
                vs.GlobalProxies.Remove(globalProxy);
            }
            else if (objectType.Equals(typeof(PixelShaderNode)))
            {
                PixelShaderNode ps = (PixelShaderNode)parentNode.Tag;
                ps.GlobalProxies.Remove(globalProxy);
            }
            else if (objectType.Equals(typeof(MaterialTemplateNode)))
            {
                MaterialTemplateNode template = (MaterialTemplateNode)parentNode.Tag;
                template.GlobalProxies.Remove(globalProxy);
            }

            ResetInputBindings(globalProxy.Output);

            // We remove it from the property window
            if (globalProxy.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveDescription(GlobalDescriptionNode desc, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == desc);

            // Remove all references to the object
            TreeNode parentNode = toDelete.Parent;
            Type objectType = parentNode.Tag.GetType();
            if (objectType.Equals(typeof(GlobalNode)))
            {
                GlobalNode global = (GlobalNode)parentNode.Tag;
                global.Descriptions.Remove(desc);
            }

            // We remove it from the property window
            if (desc.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveGlobal(GlobalNode global, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == global);

            // Remove object only if there are no proxies
            List<TreeNode> proxies = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, proxies, typeof(GlobalNodeProxy));
            foreach (TreeNode treeNode in proxies)
            {
                GlobalNodeProxy proxy = (GlobalNodeProxy)treeNode.Tag;
                if (proxy.Proxy == global)
                {
                    RemoveGlobalProxy(proxy, treeNode);
                }
            }

            // Remove all references to the object

            // We remove it from the property window
            if (global.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove all inputs
            while (node.Nodes.Count > 0)
            {
                TreeNode curNode = node.Nodes[0];
                if (curNode.Tag.GetType().Equals(typeof(InputNode)))
                {
                    InputNode curInput = global.Inputs[0];
                    RemoveInput(curInput, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(GlobalDescriptionNode)))
                {
                    GlobalDescriptionNode curDesc = global.Descriptions[0];
                    RemoveDescription(curDesc, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(LocalNodeProxy)))
                {
                    LocalNodeProxy curProxy = global.Locals[0];
                    RemoveLocalProxy(curProxy, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(GlobalNodeProxy)))
                {
                    GlobalNodeProxy curProxy = global.Dependents[0];
                    RemoveGlobalProxy(curProxy, curNode);
                }
            }

            // Remove from the draw window
            List<DrawWin> allWin = new List<DrawWin>();
            FindPanelByType(allWin, typeof(GlobalNodePanel));
            foreach (DrawWin win in allWin)
            {
                GlobalNodePanel panel = (GlobalNodePanel)win.Panel;
                if (panel.Global == global)
                {
                    win.Close();
                }
            }

            if (node.Parent.Tag != null && node.Parent.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)node.Parent.Tag;
                parentFolder.Nodes.Remove(global);
            }

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveFunction(FunctionNode function, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == function);

            // Remove all function ops
            List<TreeNode> opTreeNodes = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, opTreeNodes, typeof(FunctionOpNode));
            foreach (TreeNode treeNode in opTreeNodes)
            {
                FunctionOpNode op = (FunctionOpNode)treeNode.Tag;
                if (op.Owner == function)
                {
                    // Remove the function op
                    RemoveFunctionOp(op, treeNode);
                }
            }

            //if (sameProxies.Count > 0)
            //{
            //    // For now
            //    return;
            //}

            // Remove all references to the object

            // We remove it from the property window
            if (function.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove all inputs
            while (node.Nodes.Count > 0)
            {
                TreeNode curNode = node.Nodes[0];
                if (curNode.Tag.GetType().Equals(typeof(InputNode)))
                {
                    InputNode curInput = function.Inputs[0];
                    RemoveInput(curInput, curNode);
                }
                else if(curNode.Tag.GetType().Equals(typeof(LocalNodeProxy)))
                {
                    LocalNodeProxy proxy = function.Locals[0];
                    RemoveLocalProxy(proxy, curNode);
                }
            }

            if (node.Parent.Tag != null && node.Parent.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)node.Parent.Tag;
                parentFolder.Nodes.Remove(function);
            }

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveVertexShader(VertexShaderNode vs, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == vs);

            // Remove all vs ops
            List<TreeNode> opTreeNodes = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, opTreeNodes, typeof(VertexShaderOpNode));
            foreach (TreeNode treeNode in opTreeNodes)
            {
                VertexShaderOpNode op = (VertexShaderOpNode)treeNode.Tag;
                if (op.Owner == vs)
                {
                    RemoveVertexShaderOp(op, treeNode);
                }
            }

            // We remove it from the property window
            if (vs.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove all references to the object
            while (node.Nodes.Count > 0)
            {
                TreeNode curNode = node.Nodes[0];
                if (curNode.Tag.GetType().Equals(typeof(GlobalNodeProxy)))
                {
                    //if (vs.GlobalProxies.Count<= 0)
                    //{
                    //    node.Nodes.Clear();
                    //    continue;
                    //}

                    GlobalNodeProxy globalProxy = vs.GlobalProxies[0];
                    RemoveGlobalProxy(globalProxy, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(FunctionOpNode)))
                {
                    FunctionOpNode op = vs.FunctionOps[0];
                    RemoveFunctionOp(op, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(VertexInNode)))
                {
                    VertexInNode vertIn = vs.VertexIns[0];
                    RemoveVertexIn(vertIn, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(VertexOutNode)))
                {
                    VertexOutNode vertOut = vs.VertexOuts[0];
                    RemoveVertexOut(vertOut, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(AbstractionNodeProxy)))
                {
                    AbstractionNodeProxy absProxy = vs.AbstractionProxies[0];
                    RemoveAbstractionProxy(absProxy, curNode);
                }
            }

            // Remove from the draw window
            List<DrawWin> allWin = new List<DrawWin>();
            FindPanelByType(allWin, typeof(VertexShaderNodePanel));
            foreach (DrawWin win in allWin)
            {
                VertexShaderNodePanel panel = (VertexShaderNodePanel)win.Panel;
                if (panel.VS == vs)
                {
                    win.Close();
                }
            }

            if (node.Parent.Tag != null && node.Parent.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)node.Parent.Tag;
                parentFolder.Nodes.Remove(vs);
            }

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemovePixelShader(PixelShaderNode ps, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == ps);

            // Remove all ps ops
            List<TreeNode> opTreeNodes = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, opTreeNodes, typeof(PixelShaderOpNode));
            foreach (TreeNode treeNode in opTreeNodes)
            {
                PixelShaderOpNode op = (PixelShaderOpNode)treeNode.Tag;
                if (op.Owner == ps)
                {
                    RemovePixelShaderOp(op, treeNode);
                }
            }

            // We remove it from the property window
            if (ps.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove all references to the object
            while (node.Nodes.Count > 0)
            {
                TreeNode curNode = node.Nodes[0];
                if (curNode.Tag.GetType().Equals(typeof(GlobalNodeProxy)))
                {
                    if (ps.GlobalProxies.Count > 0)
                    {
                        GlobalNodeProxy globalProxy = ps.GlobalProxies[0];
                        RemoveGlobalProxy(globalProxy, curNode);
                    }
                    else
                    {
                        curNode.Remove();
                    }
                }
                else if (curNode.Tag.GetType().Equals(typeof(FunctionOpNode)))
                {
                    if (ps.FunctionOps.Count > 0)
                    {
                        FunctionOpNode op = ps.FunctionOps[0];
                        RemoveFunctionOp(op, curNode);
                    }
                    else
                    {
                        curNode.Remove();
                    }
                }
                else if (curNode.Tag.GetType().Equals(typeof(PixelInNode)))
                {
                    if (ps.PixelIns.Count > 0)
                    {
                        PixelInNode pixIn = ps.PixelIns[0];
                        RemovePixelIn(pixIn, curNode);
                    }
                    else
                    {
                        curNode.Remove();
                    }
                }
                else if (curNode.Tag.GetType().Equals(typeof(PixelOutNode)))
                {
                    if (ps.PixelOuts.Count > 0)
                    {
                        PixelOutNode pixOut = ps.PixelOuts[0];
                        RemovePixelOut(pixOut, curNode);
                    }
                    else
                    {
                        curNode.Remove();
                    }
                }
                else if( curNode.Tag.GetType().Equals(typeof(AbstractionNodeProxy)))
                {
                    if (ps.AbstractionProxies.Count > 0)
                    {
                        AbstractionNodeProxy absProxy = ps.AbstractionProxies[0];
                        RemoveAbstractionProxy(absProxy, curNode);
                    }
                    else
                    {
                        curNode.Remove();
                    }
                }
            }

            // Remove from the draw window
            List<DrawWin> allWin = new List<DrawWin>();
            FindPanelByType(allWin, typeof(PixelShaderNodePanel));
            foreach (DrawWin win in allWin)
            {
                PixelShaderNodePanel panel = (PixelShaderNodePanel)win.Panel;
                if (panel.PS == ps)
                {
                    win.Close();
                }
            }

            if (node.Parent.Tag != null && node.Parent.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)node.Parent.Tag;
                parentFolder.Nodes.Remove(ps);
            }

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveFunctionOp(FunctionOpNode op, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == op);

            // Remove all references to the object
            TreeNode parentNode = toDelete.Parent;
            Type objectType = parentNode.Tag.GetType();
            if (objectType.Equals(typeof(VertexShaderNode)))
            {
                VertexShaderNode vs = (VertexShaderNode)parentNode.Tag;
                vs.FunctionOps.Remove(op);
            }
            else if (objectType.Equals(typeof(PixelShaderNode)))
            {
                PixelShaderNode ps = (PixelShaderNode)parentNode.Tag;
                ps.FunctionOps.Remove(op);
            }
            else if (objectType.Equals(typeof(AbstractionNode)))
            {
                AbstractionNode abstraction = (AbstractionNode)parentNode.Tag;
                abstraction.Ops.Remove(op);
            }

            ResetInputBindings(op.Output);

            // We remove it from the property window
            if (op.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveVertexIn(VertexInNode vertIn, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == vertIn);

            // Remove all references to the object
            TreeNode parentNode = toDelete.Parent;
            Type objectType = parentNode.Tag.GetType();
            if (objectType.Equals(typeof(VertexShaderNode)))
            {
                VertexShaderNode vs = (VertexShaderNode)parentNode.Tag;
                vs.VertexIns.Remove(vertIn);
            }

            ResetInputBindings(vertIn.Output);

            // We remove it from the property window
            if (vertIn.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveVertexOut(VertexOutNode vertOut, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == vertOut);

            // Remove all references to the object
            TreeNode parentNode = toDelete.Parent;
            Type objectType = parentNode.Tag.GetType();
            if (objectType.Equals(typeof(VertexShaderNode)))
            {
                VertexShaderNode vs = (VertexShaderNode)parentNode.Tag;
                vs.VertexOuts.Remove(vertOut);
            }

            // We remove it from the property window
            if (vertOut.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemovePixelIn(PixelInNode pixIn, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == pixIn);

            // Remove all references to the object
            TreeNode parentNode = toDelete.Parent;
            Type objectType = parentNode.Tag.GetType();
            if (objectType.Equals(typeof(PixelShaderNode)))
            {
                PixelShaderNode ps = (PixelShaderNode)parentNode.Tag;
                ps.PixelIns.Remove(pixIn);
            }

            ResetInputBindings(pixIn.Output);

            // We remove it from the property window
            if (pixIn.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemovePixelOut(PixelOutNode pixOut, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == pixOut);

            // Remove all references to the object
            TreeNode parentNode = toDelete.Parent;
            Type objectType = parentNode.Tag.GetType();
            if (objectType.Equals(typeof(PixelShaderNode)))
            {
                PixelShaderNode ps = (PixelShaderNode)parentNode.Tag;
                ps.PixelOuts.Remove(pixOut);
            }

            // We remove it from the property window
            if (pixOut.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveRenderState(RenderStateNode state, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == state);

            // Remove all references to the object
            TreeNode parentNode = toDelete.Parent;
            Type objectType = parentNode.Tag.GetType();
            if (objectType.Equals(typeof(PassNode)))
            {
                PassNode pass = (PassNode)parentNode.Tag;
                pass.States.Remove(state);
            }

            // We remove it from the property window
            if (state.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove all inputs
            while (node.Nodes.Count > 0)
            {
                TreeNode curNode = node.Nodes[0];
                if (curNode.Tag.GetType().Equals(typeof(InputNode)))
                {
                    InputNode curInput = state.Inputs[0];
                    RemoveInput(curInput, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(GlobalNodeProxy)))
                {
                    GlobalNodeProxy curProxy = state.Dependents[0];
                    RemoveGlobalProxy(curProxy, curNode);
                }
            }

            // Remove from the draw window
            List<DrawWin> allWin = new List<DrawWin>();
            FindPanelByType(allWin, typeof(RenderStateNodePanel));
            foreach (DrawWin win in allWin)
            {
                RenderStateNodePanel panel = (RenderStateNodePanel)win.Panel;
                if (panel.State == state)
                {
                    win.Close();
                }
            }

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveMaterialTemplate(MaterialTemplateNode template, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == template);

            // Remove all references to the object

            // We remove it from the property window
            if (template.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            while (node.Nodes.Count > 0)
            {
                TreeNode curNode = node.Nodes[0];
                if (curNode.Tag.GetType().Equals(typeof(TechniqueNode)))
                {
                    TechniqueNode curTech = template.Techniques[0];
                    RemoveTechnique(curTech, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(GlobalNodeProxy)))
                {
                    GlobalNodeProxy curProxy = template.GlobalProxies[0];
                    RemoveGlobalProxy(curProxy, curNode);
                }
            }

            if (node.Parent.Tag != null && node.Parent.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)node.Parent.Tag;
                parentFolder.Nodes.Remove(template);
            }

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveTechnique(TechniqueNode tech, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == tech);

            // Remove all references to the object
            TreeNode parentNode = toDelete.Parent;
            Type objectType = parentNode.Tag.GetType();
            if (objectType.Equals(typeof(MaterialTemplateNode)))
            {
                MaterialTemplateNode template = (MaterialTemplateNode)parentNode.Tag;
                template.Techniques.Remove(tech);
            }

            // We remove it from the property window
            if (tech.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            while (node.Nodes.Count > 0)
            {
                TreeNode curNode = node.Nodes[0];
                if (curNode.Tag.GetType().Equals(typeof(PassNode)))
                {
                    PassNode pass = tech.Passes[0];
                    RemovePass(pass, curNode);
                }
            }

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemovePass(PassNode pass, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == pass);

            // Remove all references to the object
            TreeNode parentNode = toDelete.Parent;
            Type objectType = parentNode.Tag.GetType();
            if (objectType.Equals(typeof(TechniqueNode)))
            {
                TechniqueNode tech = (TechniqueNode)parentNode.Tag;
                tech.Passes.Remove(pass);
            }

            // We remove it from the property window
            if (pass.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            while (node.Nodes.Count > 0)
            {
                TreeNode curNode = node.Nodes[0];
                if (curNode.Tag.GetType().Equals(typeof(VertexShaderOpNode)))
                {
                    VertexShaderOpNode op = pass.VS;
                    RemoveVertexShaderOp(op, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(PixelShaderOpNode)))
                {
                    PixelShaderOpNode op = pass.PS;
                    RemovePixelShaderOp(op, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(RenderStateNode)))
                {
                    RenderStateNode state = pass.States[0];
                    RemoveRenderState(state, curNode);
                }
            }

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemovePixelShaderOp(PixelShaderOpNode op, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == op);

            // Remove all references to the object
            TreeNode parentNode = toDelete.Parent;
            Type objectType = parentNode.Tag.GetType();
            if (objectType.Equals(typeof(PassNode)))
            {
                PassNode pass = (PassNode)parentNode.Tag;
                pass.PS = null;
            }

            // We remove it from the property window
            if (op.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveVertexShaderOp(VertexShaderOpNode op, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == op);

            // Remove all references to the object
            TreeNode parentNode = toDelete.Parent;
            Type objectType = parentNode.Tag.GetType();
            if (objectType.Equals(typeof(PassNode)))
            {
                PassNode pass = (PassNode)parentNode.Tag;
                pass.VS = null;
            }

            // We remove it from the property window
            if (op.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveAbstractionProxy(AbstractionNodeProxy abstractionProxy, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == abstractionProxy);

            // We remove it from the property window
            if (abstractionProxy.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove all abstraction output bindings
            foreach(AbstractionNodeProxyOut output in abstractionProxy.Outputs)
            {
                ResetInputBindings(output.Output);
            }

            if (node.Parent.Tag.GetType().Equals(typeof(VertexShaderNode)))
            {
                VertexShaderNode vs = (VertexShaderNode)node.Parent.Tag;
                vs.AbstractionProxies.Remove(abstractionProxy);
            }
            else if(node.Parent.Tag.GetType().Equals(typeof(PixelShaderNode)))
            {
                PixelShaderNode ps = (PixelShaderNode)node.Parent.Tag;
                ps.AbstractionProxies.Remove(abstractionProxy);
            }
            else if (node.Parent.Tag.GetType().Equals(typeof(AbstractionNode)))
            {
                AbstractionNode abs = (AbstractionNode)node.Parent.Tag;
                abs.Proxies.Remove(abstractionProxy);
            }

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveAbstractionIn(AbstractionInNode abstractIn, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == abstractIn);

            ResetInputBindings(abstractIn.Output);

            // We remove it from the property window
            if (abstractIn.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            AbstractionNode abstraction = (AbstractionNode)node.Parent.Tag;
            abstraction.Ins.Remove(abstractIn);

            // Find all abstraction proxies
            List<TreeNode> allProxies = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, allProxies, typeof(AbstractionNodeProxy));
            foreach(TreeNode proxyNode in allProxies)
            {
                AbstractionNodeProxy abstractProxy = (AbstractionNodeProxy)proxyNode.Tag;
                if(abstractProxy.Owner == abstraction)
                {
                    // Remove the corresponding proxy in
                    foreach(AbstractionNodeProxyIn proxyIn in abstractProxy.Inputs)
                    {
                        if(proxyIn.Owner == abstractIn)
                        {
                            abstractProxy.Inputs.Remove(proxyIn);
                            break;
                        }
                    }
                }
            }             

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveAbstractionOut(AbstractionOutNode abstractOut, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == abstractOut);

            // We remove it from the property window
            if (abstractOut.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            AbstractionNode abstraction = (AbstractionNode)node.Parent.Tag;
           abstraction.Outs.Remove(abstractOut);

            // Find all abstraction proxies
            List<TreeNode> allProxies = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, allProxies, typeof(AbstractionNodeProxy));
            foreach (TreeNode proxyNode in allProxies)
            {
                AbstractionNodeProxy abstractProxy = (AbstractionNodeProxy)proxyNode.Tag;
                if (abstractProxy.Owner == abstraction)
                {
                    // Remove the corresponding proxy out
                    foreach (AbstractionNodeProxyOut proxyOut in abstractProxy.Outputs)
                    {
                        if (proxyOut.Owner == abstractOut)
                        {
                            ResetInputBindings(proxyOut.Output);
                            abstractProxy.Outputs.Remove(proxyOut);
                            break;
                        }
                    }
                }
            }

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveAbstraction(AbstractionNode abstraction, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == abstraction);

            // We remove it from the property window
            if (abstraction.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove all proxies
            List<TreeNode> proxies = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, proxies, typeof(AbstractionNodeProxy));
            foreach(TreeNode proxyNode in proxies)
            {
                AbstractionNodeProxy abstractionProxy = (AbstractionNodeProxy)proxyNode.Tag;
                if(abstractionProxy.Owner == abstraction)
                {
                    RemoveAbstractionProxy(abstractionProxy, proxyNode);
                }
            }

            // Remove all references to the object
            while (node.Nodes.Count > 0)
            {
                TreeNode curNode = node.Nodes[0];
                if (curNode.Tag.GetType().Equals(typeof(FunctionOpNode)))
                {
                    FunctionOpNode op = abstraction.Ops[0];
                    RemoveFunctionOp(op, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(AbstractionInNode)))
                {
                    AbstractionInNode abstractIn = abstraction.Ins[0];
                    RemoveAbstractionIn(abstractIn, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(AbstractionOutNode)))
                {
                    AbstractionOutNode abstractOut = abstraction.Outs[0];
                    RemoveAbstractionOut(abstractOut, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(AbstractionNodeProxy)))
                {
                    AbstractionNodeProxy absProxy = abstraction.Proxies[0];
                    RemoveAbstractionProxy(absProxy, curNode);
                }
                         
            }

            // Remove from the draw window
            List<DrawWin> allWin = new List<DrawWin>();
            FindPanelByType(allWin, typeof(AbstractionNodePanel));
            foreach (DrawWin win in allWin)
            {
                AbstractionNodePanel panel = (AbstractionNodePanel)win.Panel;
                if (panel.Abstraction == abstraction)
                {
                    win.Close();
                }
            }

            if (node.Parent.Tag != null && node.Parent.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)node.Parent.Tag;
                parentFolder.Nodes.Remove(abstraction);
            }

            // Remove the node from the tree
            toDelete.Remove();
        }

        private void RemoveFolder(FolderNode folder, TreeNode node)
        {
            TreeNode toDelete = node;
            Debug.Assert(toDelete.Tag == folder);

            // We remove it from the property window
            if (folder.Properties == m_Main.Properties.PropertyGrid.SelectedObject)
                m_Main.Properties.PropertyGrid.SelectedObject = null;

            // Remove all inputs
            while (node.Nodes.Count > 0)
            {
                TreeNode curNode = node.Nodes[0];
                if (curNode.Tag.GetType().Equals(typeof(LocalNode)))
                {
                    LocalNode local = (LocalNode)folder.Nodes[0];
                    RemoveLocal(local, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(GlobalNode)))
                {
                    GlobalNode global = (GlobalNode)folder.Nodes[0];
                    RemoveGlobal(global, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(FunctionNode)))
                {
                    FunctionNode fn = (FunctionNode)folder.Nodes[0];
                    RemoveFunction(fn, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(VertexShaderNode)))
                {
                    VertexShaderNode vs = (VertexShaderNode)folder.Nodes[0];
                    RemoveVertexShader(vs, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(PixelShaderNode)))
                {
                    PixelShaderNode ps = (PixelShaderNode)folder.Nodes[0];
                    RemovePixelShader(ps, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(MaterialTemplateNode)))
                {
                    MaterialTemplateNode template = (MaterialTemplateNode)folder.Nodes[0];
                    RemoveMaterialTemplate(template, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(AbstractionNode)))
                {
                    AbstractionNode abstraction = (AbstractionNode)folder.Nodes[0];
                    RemoveAbstraction(abstraction, curNode);
                }
                else if (curNode.Tag.GetType().Equals(typeof(FolderNode)))
                {
                    FolderNode folderRem = (FolderNode)folder.Nodes[0];
                    RemoveFolder(folderRem, curNode);
                }
            }

            if (node.Parent.Tag != null && node.Parent.Tag.GetType().Equals(typeof(FolderNode)))
            {
                FolderNode parentFolder = (FolderNode)node.Parent.Tag;
                parentFolder.Nodes.Remove(folder);
            }

            // Remove the node from the tree
            toDelete.Remove();
        }

        #endregion

        #region Utility Functions
        private void ResetInputBindings(OutputNode output)
        {
            List<TreeNode> allInputs = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, allInputs, typeof(InputNode));
            foreach (TreeNode treeNode in allInputs)
            {
                InputNode input = (InputNode)treeNode.Tag;
                if (input.Binding == output)
                    input.Binding = null;
                if (input.AltBinding == output)
                    input.AltBinding = null;
                
            }

            List<TreeNode> allFunctionOps = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, allFunctionOps, typeof(FunctionOpNode));
            foreach (TreeNode treeNode in allFunctionOps)
            {
                FunctionOpNode functionOp = (FunctionOpNode)treeNode.Tag;
                foreach (InputNode input in functionOp.Inputs)
                {
                    if (input.Binding == output)
                        input.Binding = null;
                    if (input.AltBinding == output)
                        input.AltBinding = null;
                }
            }

            List<TreeNode> allVertexOuts = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, allVertexOuts, typeof(VertexOutNode));
            foreach (TreeNode treeNode in allVertexOuts)
            {
                VertexOutNode vOut = (VertexOutNode)treeNode.Tag;
                if (vOut.Input.Binding == output)
                    vOut.Input.Binding = null;
                if (vOut.Input.AltBinding == output)
                    vOut.Input.AltBinding = null;
            }

            List<TreeNode> allPixelOuts = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, allPixelOuts, typeof(PixelOutNode));
            foreach (TreeNode treeNode in allPixelOuts)
            {
                PixelOutNode pOut = (PixelOutNode)treeNode.Tag;
                if (pOut.Input.Binding == output)
                    pOut.Input.Binding = null;
                if (pOut.Input.AltBinding == output)
                    pOut.Input.AltBinding = null;
            }

            List<TreeNode> allAbstractOuts = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, allAbstractOuts, typeof(AbstractionOutNode));
            foreach (TreeNode treeNode in allAbstractOuts)
            {
                AbstractionOutNode abstractOut = (AbstractionOutNode)treeNode.Tag;
                if (abstractOut.Input.Binding == output)
                    abstractOut.Input.Binding = null;
                if (abstractOut.Input.AltBinding == output)
                    abstractOut.Input.AltBinding = null;
            }

            List<TreeNode> allAbstractProxies = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, allAbstractProxies, typeof(AbstractionNodeProxy));
            foreach (TreeNode treeNode in allAbstractProxies)
            {
                AbstractionNodeProxy abstractionProxy = (AbstractionNodeProxy)treeNode.Tag;
                foreach(AbstractionNodeProxyIn proxyIn in abstractionProxy.Inputs)
                {
                    if (proxyIn.Input.Binding == output)
                        proxyIn.Input.Binding = null;
                    if (proxyIn.Input.AltBinding == output)
                        proxyIn.Input.AltBinding = null;
                }
            }

            EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs()); 
        }

        private void FindPanelByType(List<DrawWin> toStore, Type panelType)
        {
            List<DockContent> contents = m_Main.GetAllDocumentsAndFloatingDocks();
            foreach(DockContent content in contents)
            {
                if (content.GetType().Equals(typeof(DrawWin)))
                {
                    // Check to see if the draw win contains a particular panel
                    DrawWin curWin = (DrawWin)content;
                    if (curWin.Panel.GetType().Equals(panelType))
                    {
                        toStore.Add(curWin);
                    }
                }
            }   
        }

        #region Fucking broken TreeView needs this to bold text
        private void UnboldAllNodes(TreeNodeCollection nodes)
        {
            foreach (TreeNode node in nodes)
            {
                node.NodeFont = new Font(node.NodeFont, node.NodeFont.Italic ? FontStyle.Italic : FontStyle.Regular);
                UnboldAllNodes(node.Nodes);
            }
        }
        #endregion

        private TreeNode FindNodeByTag(TreeNodeCollection allNodes, object tag)
        {
            foreach (TreeNode child in allNodes)
            {
                if (tag == child.Tag)
                    return child;

                TreeNode node = FindNodeByTag(child.Nodes, tag);
                if (node != null)
                    return node;
            }

            return null;
        }

        public void SelectNode(object tag)
        {
            TreeNode node = FindNodeByTag(explorerTreeView.Nodes, tag);
            if (node != null)
            {
                ActivateNode(node);
            }
            else
            {
                TreeNode lastNode = explorerTreeView.SelectedNode;
                if (lastNode != null)
                    UnHighlightNode(lastNode);

                explorerTreeView.SelectedNode = null;

                PropertyNode propertyNode = (PropertyNode)tag;
                m_Main.Properties.PropertyGrid.SelectedObject = propertyNode.Properties;
            }
        }

        private void UnHighlightNode(TreeNode node)
        {
            node.BackColor = explorerTreeView.BackColor;
            node.ForeColor = explorerTreeView.ForeColor;
            //if (lastNode.NodeFont.Italic)
            //    lastNode.NodeFont = new Font(lastNode.NodeFont, FontStyle.Italic);
            //else
            //    lastNode.NodeFont = new Font(lastNode.NodeFont, FontStyle.Regular);
        }

        private void HighlightNode(TreeNode node)
        {
            node.BackColor = Color.FromKnownColor(KnownColor.MenuHighlight);
            node.ForeColor = Color.White;
            //if (node.NodeFont.Italic)
            //    node.NodeFont = new Font(lastNode.NodeFont, FontStyle.Italic | FontStyle.Bold);
            //else
            //    node.NodeFont = new Font(lastNode.NodeFont, FontStyle.Bold);

        }

        private void NodePanel_OnEnter(object sender, EventArgs e)
        {
            // Activated
            if (sender.GetType().Equals(typeof(GlobalNodePanel)))
            {
                SelectNode(((GlobalNodePanel)sender).Global);
            }
            else if (sender.GetType().Equals(typeof(RenderStateNodePanel)))
            {
                SelectNode(((RenderStateNodePanel)sender).State);
            }
            else if (sender.GetType().Equals(typeof(VertexShaderNodePanel)))
            {
                SelectNode(((VertexShaderNodePanel)sender).VS);
            }
            else if (sender.GetType().Equals(typeof(PixelShaderNodePanel)))
            {
                SelectNode(((PixelShaderNodePanel)sender).PS);
            }
            else if (sender.GetType().Equals(typeof(AbstractionNodePanel)))
            {
                SelectNode(((AbstractionNodePanel)sender).Abstraction);
            }

        }

        private void ActivateNode(TreeNode node)
        {
            TreeNode lastNode = explorerTreeView.SelectedNode;
            if (lastNode != null)
            {
                UnHighlightNode(lastNode);
            }

            explorerTreeView.SelectedNode = node;

            // Context sensitive
            Object tag = node.Tag;
            if (tag != null)
            {
                HighlightNode(node);

                PropertyNode propertyNode = (PropertyNode)node.Tag;
                m_Main.Properties.PropertyGrid.SelectedObject = propertyNode.Properties;

                if (tag.GetType().Equals(typeof(GlobalNode)))
                {
                    List<DrawWin> allWin = new List<DrawWin>();
                    FindPanelByType(allWin, typeof(GlobalNodePanel));
                    foreach (DrawWin win in allWin)
                    {
                        GlobalNodePanel panel = (GlobalNodePanel)win.Panel;
                        if (panel.Global == propertyNode)
                        {
                            win.TabText = node.Text;
                            win.Activate();
                            return;
                        }

                    }

                    GlobalNodePanel newPanel = new GlobalNodePanel(m_Main);
                    m_Main.AddDrawWin(node.Text, newPanel);
                    newPanel.Global = (GlobalNode)propertyNode;
                    newPanel.Enter += new EventHandler(NodePanel_OnEnter);
                }
                else if (tag.GetType().Equals(typeof(RenderStateNode)))
                {
                    List<DrawWin> allWin = new List<DrawWin>();
                    FindPanelByType(allWin, typeof(RenderStateNodePanel));
                    foreach (DrawWin win in allWin)
                    {
                        RenderStateNodePanel panel = (RenderStateNodePanel)win.Panel;
                        if (panel.State == propertyNode)
                        {
                            win.TabText = node.Text;
                            win.Activate();
                            return;
                        }

                    }

                    RenderStateNodePanel newPanel = new RenderStateNodePanel(m_Main);
                    m_Main.AddDrawWin(node.Text, newPanel);
                    newPanel.State = (RenderStateNode)propertyNode;
                    newPanel.Enter += new EventHandler(NodePanel_OnEnter);
                }
                else if (tag.GetType().Equals(typeof(VertexShaderNode)))
                {
                    List<DrawWin> allWin = new List<DrawWin>();
                    FindPanelByType(allWin, typeof(VertexShaderNodePanel));
                    foreach (DrawWin win in allWin)
                    {
                        VertexShaderNodePanel panel = (VertexShaderNodePanel)win.Panel;
                        if (panel.VS == propertyNode)
                        {
                            win.TabText = node.Text;
                            win.Activate();
                            return;
                        }
                    }

                    VertexShaderNodePanel newPanel = new VertexShaderNodePanel(m_Main);
                    m_Main.AddDrawWin(node.Text, newPanel);
                    newPanel.VS = (VertexShaderNode)propertyNode;
                    newPanel.Enter += new EventHandler(NodePanel_OnEnter);
                }
                else if (tag.GetType().Equals(typeof(PixelShaderNode)))
                {
                    List<DrawWin> allWin = new List<DrawWin>();
                    FindPanelByType(allWin, typeof(PixelShaderNodePanel));
                    foreach (DrawWin win in allWin)
                    {
                        PixelShaderNodePanel panel = (PixelShaderNodePanel)win.Panel;
                        if (panel.PS == propertyNode)
                        {
                            win.TabText = node.Text;
                            win.Activate();
                            return;
                        }
                    }

                    PixelShaderNodePanel newPanel = new PixelShaderNodePanel(m_Main);
                    m_Main.AddDrawWin(node.Text, newPanel);
                    newPanel.PS = (PixelShaderNode)propertyNode;
                    newPanel.Enter += new EventHandler(NodePanel_OnEnter);
                }
                else if(tag.GetType().Equals(typeof(AbstractionNode)))
                {
                    List<DrawWin> allWin = new List<DrawWin>();
                    FindPanelByType(allWin, typeof(AbstractionNodePanel));
                    foreach (DrawWin win in allWin)
                    {
                        AbstractionNodePanel panel = (AbstractionNodePanel)win.Panel;
                        if (panel.Abstraction == propertyNode)
                        {
                            win.TabText = node.Text;
                            win.Activate();
                            return;
                        }
                    }

                    AbstractionNodePanel newPanel = new AbstractionNodePanel(m_Main);
                    m_Main.AddDrawWin(node.Text, newPanel);
                    newPanel.Abstraction = (AbstractionNode)propertyNode;
                    newPanel.Enter += new EventHandler(NodePanel_OnEnter);
                }
            }

            EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void FindNodesByName(TreeNodeCollection allNodes, List<TreeNode> toStore, string name)
        {
            foreach (TreeNode child in allNodes)
            {
                if (name.Equals(child.Text))
                    toStore.Add(child);

                FindNodesByName(child.Nodes, toStore, name);
            }
        }

        private void FindNodesByTagType(TreeNodeCollection allNodes, List<TreeNode> toStore, Type tagType)
        {
            foreach (TreeNode child in allNodes)
            {
                if (child.Tag != null && tagType.Equals(child.Tag.GetType()))
                    toStore.Add(child);

                FindNodesByTagType(child.Nodes, toStore, tagType);
            }
        }
        #endregion

       

        #region Property Changed Handlers
        void global_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(GlobalNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                        treeNode.Text = (string)e.Value;
                }

                allTreeNodes.Clear();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(GlobalNodeProxy));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (((GlobalNodeProxy)treeNode.Tag).Proxy == node)
                        treeNode.Text = (string)e.Value;
                }

                List<DrawWin> allWin = new List<DrawWin>();
                FindPanelByType(allWin, typeof(GlobalNodePanel));
                foreach (DrawWin win in allWin)
                {
                    GlobalNodePanel panel = (GlobalNodePanel)win.Panel;
                    if (panel.Global == node)
                    {
                        win.TabText = (string)e.Value;
                        win.Text = win.TabText;
                        return;
                    }
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        void state_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(RenderStateNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                        treeNode.Text = (string)e.Value;
                }

                List<DrawWin> allWin = new List<DrawWin>();
                FindPanelByType(allWin, typeof(RenderStateNodePanel));
                foreach (DrawWin win in allWin)
                {
                    RenderStateNodePanel panel = (RenderStateNodePanel)win.Panel;
                    if (panel.State == node)
                    {
                        win.TabText = (string)e.Value;
                        win.Text = win.TabText;
                        return;
                    }
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        void template_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(MaterialTemplateNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                        treeNode.Text = (string)e.Value;
                }

                List<DockContent> contents = m_Main.GetAllDocumentsAndFloatingDocks();
                foreach(DockContent content in contents)
                {
                    if (content.GetType().Equals(typeof(CodeOutputWin)))
                    {
                        MaterialTemplateNode template = (MaterialTemplateNode)node;
                        if (content.TabText.Equals(template.Name + " [ generated code ]"))
                        {
                            content.TabText = (string)e.Value + " [ generated code ]";
                            content.Text = content.TabText;
                            break;
                        }
                    }
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        void tech_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(TechniqueNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                        treeNode.Text = (string)e.Value;
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        void pass_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(PassNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                        treeNode.Text = (string)e.Value;
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }


        private void input_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            List<TreeNode> allTreeNodes = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(InputNode));
            foreach (TreeNode treeNode in allTreeNodes)
            {
                if (treeNode.Tag == node)
                {
                    TreeNode parent = treeNode.Parent;
                    if (parent.Tag.GetType().Equals(typeof(FunctionNode)))
                    {
                        FunctionNode parentFunction = (FunctionNode)parent.Tag;
                        int index = parentFunction.Inputs.IndexOf((InputNode)node);

                        // Change for all function ops
                        List<TreeNode> opTreeNodes = new List<TreeNode>();
                        FindNodesByTagType(explorerTreeView.Nodes, opTreeNodes, typeof(FunctionOpNode));
                        foreach (TreeNode opTreeNode in opTreeNodes)
                        {
                            FunctionOpNode op = (FunctionOpNode)opTreeNode.Tag;
                            if (op.Owner == parentFunction)
                            {
                                if (e.Property.Name.Equals("Name"))
                                    op.Inputs[index].Name = (string)e.Value;
                                else if (e.Property.Name.Equals("Type"))
                                    op.Inputs[index].Type = (string)e.Value;
                                else if (e.Property.Name.Equals("Default Value"))
                                    op.Inputs[index].DefaultValue = (string)e.Value;
                            }
                        }
                    }

                    if (e.Property.Name.Equals("Name"))
                        treeNode.Text = (string)e.Value;

                    break;
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void func_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(FunctionNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                    {
                        treeNode.Text = (string)e.Value;
                        break;
                    }
                }

                allTreeNodes.Clear();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(FunctionOpNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (((FunctionOpNode)treeNode.Tag).Owner == node)
                        treeNode.Text = (string)e.Value;
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void vs_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(VertexShaderNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                    {
                        treeNode.Text = (string)e.Value;
                        break;
                    }
                }

                allTreeNodes.Clear();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(VertexShaderOpNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (((VertexShaderOpNode)treeNode.Tag).Owner == node)
                        treeNode.Text = (string)e.Value;
                }

                List<DrawWin> allWin = new List<DrawWin>();
                FindPanelByType(allWin, typeof(VertexShaderNodePanel));
                foreach (DrawWin win in allWin)
                {
                    VertexShaderNodePanel panel = (VertexShaderNodePanel)win.Panel;
                    if (panel.VS == node)
                    {
                        win.TabText = (string)e.Value;
                        win.Text = win.TabText;                           
                        return;
                    }
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void ps_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(PixelShaderNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                    {
                        treeNode.Text = (string)e.Value;
                        break;
                    }
                }

                allTreeNodes.Clear();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(PixelShaderOpNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (((PixelShaderOpNode)treeNode.Tag).Owner == node)
                        treeNode.Text = (string)e.Value;
                }

                List<DrawWin> allWin = new List<DrawWin>();
                FindPanelByType(allWin, typeof(PixelShaderNodePanel));
                foreach (DrawWin win in allWin)
                {
                    PixelShaderNodePanel panel = (PixelShaderNodePanel)win.Panel;
                    if (panel.PS == node)
                    {
                        win.TabText = (string)e.Value;
                        win.Text = win.TabText;                        
                        return;
                    }
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void local_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(LocalNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                    {
                        treeNode.Text = (string)e.Value;
                        break;
                    }
                }

                allTreeNodes.Clear();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(LocalNodeProxy));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (((LocalNodeProxy)treeNode.Tag).Proxy == node)
                        treeNode.Text = (string)e.Value;
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void globalDescription_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(GlobalDescriptionNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                        treeNode.Text = (string)e.Value;
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void vertOut_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(VertexOutNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                        treeNode.Text = (string)e.Value;
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void vertIn_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(VertexInNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                        treeNode.Text = (string)e.Value;
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void pixOut_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(PixelOutNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                        treeNode.Text = (string)e.Value;
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void pixIn_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(PixelInNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                        treeNode.Text = (string)e.Value;
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void folder_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(FolderNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                    {
                        treeNode.Text = (string)e.Value;
                        TreeNode parent = treeNode.Parent;
                        treeNode.Remove();

                        int index = 0;
                        foreach (TreeNode child in parent.Nodes)
                        {
                            if (child.Tag.GetType().Equals(typeof(FolderNode)))
                            {
                                if (treeNode.Text.CompareTo(child.Text) < 0)
                                    break;
                            }
                            else
                                break;

                            index++;
                        }

                        parent.Nodes.Insert(index, treeNode);

                        if (parent.Tag != null && parent.Tag.GetType().Equals(typeof(FolderNode)))
                        {
                            FolderNode parentFolder = (FolderNode)parent.Tag;
                            parentFolder.Nodes.Remove((FolderNode)treeNode.Tag);
                            parentFolder.Nodes.Insert(index, (FolderNode)treeNode.Tag);
                        }

                        ActivateNode(treeNode);
                    }
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void abstractIn_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(AbstractionInNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                    {
                        treeNode.Text = (string)e.Value;
                        break;
                    }
                }

                allTreeNodes.Clear();
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void abstractOut_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(AbstractionOutNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                    {
                        treeNode.Text = (string)e.Value;
                        break;
                    }
                }

                allTreeNodes.Clear();
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void abstraction_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            if (e.Property.Name.Equals("Name"))
            {
                List<TreeNode> allTreeNodes = new List<TreeNode>();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(AbstractionNode));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (treeNode.Tag == node)
                    {
                        treeNode.Text = (string)e.Value;
                        break;
                    }
                }

                allTreeNodes.Clear();
                FindNodesByTagType(explorerTreeView.Nodes, allTreeNodes, typeof(AbstractionNodeProxy));
                foreach (TreeNode treeNode in allTreeNodes)
                {
                    if (((AbstractionNodeProxy)treeNode.Tag).Owner == node)
                        treeNode.Text = (string)e.Value;
                }

                List<DrawWin> allWin = new List<DrawWin>();
                FindPanelByType(allWin, typeof(AbstractionNodePanel));
                foreach (DrawWin win in allWin)
                {
                    AbstractionNodePanel panel = (AbstractionNodePanel)win.Panel;
                    if (panel.Abstraction == node)
                    {
                        win.TabText = (string)e.Value;
                        win.Text = win.TabText;                        
                        return;
                    }
                }
            }

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }
        #endregion

        #region Processing Handlers
        private void ExpandToolStrip(ToolStripMenuItem strip, TreeNode root, Type type, EventHandler handler, IList toExclude)
        {
            foreach (TreeNode node in root.Nodes)
            {
                if (node.Tag != null)
                {
                    if (node.Tag.GetType().Equals(typeof(FolderNode)))
                    {
                        ToolStripMenuItem newItem =
                            new ToolStripMenuItem(node.Text);
                        ExpandToolStrip(newItem, node, type, handler, toExclude);
                        strip.DropDownItems.Add(newItem);
                    }
                    else if (node.Tag.GetType().Equals(type))
                    {
                        if (!toExclude.Contains(node.Tag))
                        {
                            ToolStripMenuItem newItem =
                                new ToolStripMenuItem(node.Text, null, handler);
                            newItem.Tag = node;
                            strip.DropDownItems.Add(newItem);
                        }
                    }
                }
            }
        }

        private void GlobalObjectContext_Opening(object sender, CancelEventArgs e)
        {
            GlobalNode curGlobal = (GlobalNode)explorerTreeView.SelectedNode.Tag;

            ContextMenuStrip globalObjectContext = (ContextMenuStrip)sender;
            ToolStripMenuItem localItem = (ToolStripMenuItem)globalObjectContext.Items["addLocalGlobalObject"];
            localItem.DropDownItems.Clear();
            ToolStripMenuItem globalDependentItem = (ToolStripMenuItem)globalObjectContext.Items["addGlobalDependentGlobalObject"];
            globalDependentItem.DropDownItems.Clear();

            // Populate with Locals label
            TreeNode allLocals = explorerTreeView.Nodes["Locals"];
            List<LocalNode> toExcludeLocal = new List<LocalNode>();
            foreach (LocalNodeProxy proxy in curGlobal.Locals)
            {
                toExcludeLocal.Add(proxy.Proxy);
            }
            ExpandToolStrip(localItem, allLocals, typeof(LocalNode),
                new EventHandler(addLocalGlobalObject_SubMenuClick), toExcludeLocal);

            // Populate with Global dependents
            TreeNode allGlobals = explorerTreeView.Nodes["Globals"];
            List<GlobalNode> toExcludeGlobal = new List<GlobalNode>();
            foreach (GlobalNodeProxy proxy in curGlobal.Dependents)
            {
                toExcludeGlobal.Add(proxy.Proxy);
            }
            toExcludeGlobal.Add(curGlobal);
            ExpandToolStrip(globalDependentItem, allGlobals, typeof(GlobalNode),
                new EventHandler(addGlobalDependentGlobalObject_SubMenuClick), toExcludeGlobal);
        }

        private void FunctionObjectContext_Opening(object sender, CancelEventArgs e)
        {
            FunctionNode curFunc = (FunctionNode)explorerTreeView.SelectedNode.Tag;

            ContextMenuStrip globalObjectContext = (ContextMenuStrip)sender;
            ToolStripMenuItem localItem = (ToolStripMenuItem)globalObjectContext.Items["addLocalFunctionObject"];
            localItem.DropDownItems.Clear();

            // Populate with Locals label
            TreeNode allLocals = explorerTreeView.Nodes["Locals"];
            List<LocalNode> toExcludeLocal = new List<LocalNode>();
            foreach (LocalNodeProxy proxy in curFunc.Locals)
            {
                toExcludeLocal.Add(proxy.Proxy);
            }
            ExpandToolStrip(localItem, allLocals, typeof(LocalNode),
                new EventHandler(addLocalFunctionObject_SubMenuClick), toExcludeLocal);
        }


        private void VertexShaderObjectContext_Opening(object sender, CancelEventArgs e)
        {
            VertexShaderNode curVS = (VertexShaderNode)explorerTreeView.SelectedNode.Tag;

            ContextMenuStrip vsObjectContext = (ContextMenuStrip)sender;
            ToolStripMenuItem addGlobalItem = (ToolStripMenuItem)vsObjectContext.Items["addGlobalVertexShaderObject"];
            addGlobalItem.DropDownItems.Clear();
            ToolStripMenuItem addFunctionOpItem = (ToolStripMenuItem)vsObjectContext.Items["addFunctionOpVertexShaderObject"];
            addFunctionOpItem.DropDownItems.Clear();
            ToolStripMenuItem addAbstractionItem = (ToolStripMenuItem)vsObjectContext.Items["addAbstractionVertexShaderObject"];
            addAbstractionItem.DropDownItems.Clear();

            // Populate with Global proxies
            TreeNode allGlobals = explorerTreeView.Nodes["Globals"];
            List<GlobalNode> toExcludeGlobal = new List<GlobalNode>();
            foreach (GlobalNodeProxy proxy in curVS.GlobalProxies)
            {
                toExcludeGlobal.Add(proxy.Proxy);
            }

            ExpandToolStrip(addGlobalItem, allGlobals, typeof(GlobalNode),
                new EventHandler(addGlobalVertexShaderObject_SubMenuClick), toExcludeGlobal);

            // Populate with function op label
            TreeNode allFuncs = explorerTreeView.Nodes["Functions"];
            List<FunctionNode> toExcludeFn = new List<FunctionNode>();
            //foreach (FunctionOpNode op in curVS.FunctionOps)
            //{
            //    toExcludeFn.Add(op.Owner);
            //}
            ExpandToolStrip(addFunctionOpItem, allFuncs, typeof(FunctionNode),
                new EventHandler(addFunctionOpVertexShaderObject_SubMenuClick), toExcludeFn);

            // Populate with abstraction proxies
            TreeNode allAbs = explorerTreeView.Nodes["Abstractions"];
            List<AbstractionNode> toExcludeAbs = new List<AbstractionNode>();
            //foreach (FunctionOpNode op in curVS.FunctionOps)
            //{
            //    toExcludeFn.Add(op.Owner);
            //}
            ExpandToolStrip(addAbstractionItem, allAbs, typeof(AbstractionNode),
                new EventHandler(addAbstractionVertexShaderObject_SubMenuClick), toExcludeAbs);

        }


        private void PixelShaderObjectContext_Opening(object sender, CancelEventArgs e)
        {
            PixelShaderNode curPS = (PixelShaderNode)explorerTreeView.SelectedNode.Tag;

            ContextMenuStrip psObjectContext = (ContextMenuStrip)sender;
            ToolStripMenuItem addGlobalItem = (ToolStripMenuItem)psObjectContext.Items["addGlobalPixelShaderObject"];
            addGlobalItem.DropDownItems.Clear();
            ToolStripMenuItem addFunctionOpItem = (ToolStripMenuItem)psObjectContext.Items["addFunctionOpPixelShaderObject"];
            addFunctionOpItem.DropDownItems.Clear();
            ToolStripMenuItem addAbstractionItem = (ToolStripMenuItem)psObjectContext.Items["addAbstractionPixelShaderObject"];
            addAbstractionItem.DropDownItems.Clear();

            // Populate with Global proxies
            TreeNode allGlobals = explorerTreeView.Nodes["Globals"];
            List<GlobalNode> toExcludeGlobal = new List<GlobalNode>();
            foreach (GlobalNodeProxy proxy in curPS.GlobalProxies)
            {
                toExcludeGlobal.Add(proxy.Proxy);
            }

            ExpandToolStrip(addGlobalItem, allGlobals, typeof(GlobalNode),
                new EventHandler(addGlobalPixelShaderObject_SubMenuClick), toExcludeGlobal);

            // Populate with function op label
            TreeNode allFuncs = explorerTreeView.Nodes["Functions"];
            List<FunctionNode> toExcludeFn = new List<FunctionNode>();
            //foreach (FunctionOpNode op in curPS.FunctionOps)
            //{
            //    toExcludeFn.Add(op.Owner);
            //}
            ExpandToolStrip(addFunctionOpItem, allFuncs, typeof(FunctionNode),
                new EventHandler(addFunctionOpPixelShaderObject_SubMenuClick), toExcludeFn);

            // Populate with abstraction proxies
            TreeNode allAbs = explorerTreeView.Nodes["Abstractions"];
            List<AbstractionNode> toExcludeAbs = new List<AbstractionNode>();
            ExpandToolStrip(addAbstractionItem, allAbs, typeof(AbstractionNode),
                new EventHandler(addAbstractionPixelShaderObject_SubMenuClick), toExcludeAbs);

        }

        private void RenderStateObjectContext_Opening(object sender, CancelEventArgs e)
        {
            RenderStateNode curState = (RenderStateNode)explorerTreeView.SelectedNode.Tag;

            ContextMenuStrip objectContext = (ContextMenuStrip)sender;
            ToolStripMenuItem globalDependentItem = (ToolStripMenuItem)objectContext.Items["addGlobalDependentRenderStateObject"];
            globalDependentItem.DropDownItems.Clear();

            // Populate with Global dependents
            TreeNode allGlobals = explorerTreeView.Nodes["Globals"];
            List<GlobalNode> toExcludeGlobal = new List<GlobalNode>();
            foreach (GlobalNodeProxy proxy in curState.Dependents)
            {
                toExcludeGlobal.Add(proxy.Proxy);
            }

            ExpandToolStrip(globalDependentItem, allGlobals, typeof(GlobalNode),
                new EventHandler(addGlobalDependentRenderStateObject_SubMenuClick), toExcludeGlobal);

        }

        private void AbstractionObjectContext_Opening(object sender, CancelEventArgs e)
        {
            AbstractionNode curAbs = (AbstractionNode)explorerTreeView.SelectedNode.Tag;

            ContextMenuStrip absObjectContext = (ContextMenuStrip)sender;
            ToolStripMenuItem addAbstractionItem = (ToolStripMenuItem)absObjectContext.Items["addAbstractionAbstractionObject"];
            addAbstractionItem.DropDownItems.Clear();
            ToolStripMenuItem addFunctionOpItem = (ToolStripMenuItem)absObjectContext.Items["addFunctionOpAbstractionObject"];
            addFunctionOpItem.DropDownItems.Clear();

            // Populate with Abstraction proxies
            TreeNode allAbs = explorerTreeView.Nodes["Abstractions"];
            List<AbstractionNode> toExcludeAbs = new List<AbstractionNode>();
            toExcludeAbs.Add(curAbs);
            //foreach (AbstractionNodeProxy proxy in curAbs.Proxies)
            //{
            //    toExcludeAbs.Add(proxy.Owner);
            //}

            ExpandToolStrip(addAbstractionItem, allAbs, typeof(AbstractionNode),
                new EventHandler(addAbstractionAbstractionObject_SubMenuClick), toExcludeAbs);

            // Populate with function op label
            TreeNode allFuncs = explorerTreeView.Nodes["Functions"];
            List<FunctionNode> toExcludeFn = new List<FunctionNode>();
            //foreach (FunctionOpNode op in curPS.FunctionOps)
            //{
            //    toExcludeFn.Add(op.Owner);
            //}
            ExpandToolStrip(addFunctionOpItem, allFuncs, typeof(FunctionNode),
                new EventHandler(addFunctionOpAbstractionObject_SubMenuClick), toExcludeFn);
        }

        private void MaterialTemplateObjectContext_Opening(object sender, CancelEventArgs e)
        {
            MaterialTemplateNode curTemplate = (MaterialTemplateNode)explorerTreeView.SelectedNode.Tag;

            ContextMenuStrip objectContext = (ContextMenuStrip)sender;
            ToolStripMenuItem globalDependentItem = (ToolStripMenuItem)objectContext.Items["addGlobalMaterialTemplateObject"];
            globalDependentItem.DropDownItems.Clear();

            // Populate with Global dependents
            TreeNode allGlobals = explorerTreeView.Nodes["Globals"];
            List<GlobalNode> toExcludeGlobal = new List<GlobalNode>();
            foreach (GlobalNodeProxy proxy in curTemplate.GlobalProxies)
            {
                toExcludeGlobal.Add(proxy.Proxy);
            }

            ExpandToolStrip(globalDependentItem, allGlobals, typeof(GlobalNode),
                new EventHandler(addGlobalMaterialTemplateObject_SubMenuClick), toExcludeGlobal);
        }


        private void PassObjectContext_Opening(object sender, CancelEventArgs e)
        {
            PassNode curPass = (PassNode)explorerTreeView.SelectedNode.Tag;

            ContextMenuStrip objectContext = (ContextMenuStrip)sender;
            ToolStripMenuItem pixelShaderItem = (ToolStripMenuItem)objectContext.Items["setPixelShaderPassObject"];
            pixelShaderItem.DropDownItems.Clear();

            ToolStripMenuItem vertexShaderItem = (ToolStripMenuItem)objectContext.Items["setVertexShaderPassObject"];
            vertexShaderItem.DropDownItems.Clear();

            // Populate with Pixel Shaders
            TreeNode allPS = explorerTreeView.Nodes["Pixel Shaders"];
            List<PixelShaderNode> toExcludePS = new List<PixelShaderNode>();
            if (curPass.PS != null)
                toExcludePS.Add(curPass.PS.Owner);

            ExpandToolStrip(pixelShaderItem, allPS, typeof(PixelShaderNode),
                new EventHandler(setPixelShaderPassObject_SubMenuClick), toExcludePS);

            // Populate with Vertex Shaders
            TreeNode allVS = explorerTreeView.Nodes["Vertex Shaders"];
            List<VertexShaderNode> toExcludeVS = new List<VertexShaderNode>();
            if (curPass.VS != null)
                toExcludeVS.Add(curPass.VS.Owner);

            ExpandToolStrip(vertexShaderItem, allVS, typeof(VertexShaderNode),
                new EventHandler(setVertexShaderPassObject_SubMenuClick), toExcludeVS);
        }

        private void verifyMaterialTemplateObject_Click(object sender, EventArgs e)
        {
            //new OutputWin(m_Main);
            //codeOutput.DockAreas = DockAreas.Document | DockAreas.Float;

            MaterialTemplateNode template = (MaterialTemplateNode)explorerTreeView.SelectedNode.Tag;

            List<GlobalNode> allGlobals = new List<GlobalNode>();

            List<VertexShaderNode> vss = new List<VertexShaderNode>();
            List<PixelShaderNode> pss = new List<PixelShaderNode>();
            foreach (TechniqueNode tech in template.Techniques)
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
                            if (!allGlobals.Contains(proxy.Proxy))
                                allGlobals.Add(proxy.Proxy);

                            proxy.Proxy.GatherAllGlobals(allGlobals);
                        }
                    }
                }
            }

            foreach (GlobalNodeProxy proxy in template.GlobalProxies)
            {
                if (!allGlobals.Contains(proxy.Proxy))
                    allGlobals.Add(proxy.Proxy);

                proxy.Proxy.GatherAllGlobals(allGlobals);
            }

            foreach (VertexShaderNode vs in vss)
            {
                foreach (GlobalNodeProxy proxy in vs.GlobalProxies)
                {
                    proxy.Proxy.GatherAllGlobals(allGlobals);

                    if (!allGlobals.Contains(proxy.Proxy))
                        allGlobals.Add(proxy.Proxy);
                }
            }

            foreach (PixelShaderNode ps in pss)
            {
                foreach (GlobalNodeProxy proxy in ps.GlobalProxies)
                {
                    proxy.Proxy.GatherAllGlobals(allGlobals);

                    if (!allGlobals.Contains(proxy.Proxy))
                        allGlobals.Add(proxy.Proxy);
                }
            }

            GlobalAvailableWin globalWin = new GlobalAvailableWin();
            Dictionary<string, bool> globalAvail = new Dictionary<string, bool>();
            foreach (GlobalNode global in allGlobals)
            {
                if (global.IsVolatile)
                {
                    object[] newRow = { global.Name, true };
                    globalWin.DataGrid.Rows.Add(newRow);
                    //row["globalsCol"] = global.Name;
                    //row["availableCol"] = true;
                    //dataTable.Rows.Add(row);
                }
            }

            if (globalWin.ShowDialog(m_Main) == DialogResult.OK)
            {
                foreach (DataGridViewRow row in globalWin.DataGrid.Rows)
                {
                    globalAvail.Add((string)row.Cells[0].Value, (bool)row.Cells[1].Value);
                }

                VerifyMaterialTemplate(template, true, globalAvail);
            }

            globalWin.Dispose();
        }

        private void exportMaterialTemplateObject_Click(object sender, EventArgs e)
        {
            MaterialTemplateNode template = (MaterialTemplateNode)explorerTreeView.SelectedNode.Tag;
            ExportMaterialTemplate(template);
        }

        private void explorerTreeView_KeyPress(object sender, KeyPressEventArgs e)
        {
            //if(e.KeyChar == 0x7F)
            //{
            //    // If delete key pressed
            //    if(explorerTreeView.SelectedNode != null)
            //    {
            //    }
            //}
        }
        private void explorerTreeView_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            // Explicitly select the node
            ActivateNode(e.Node);
        }

        #endregion

        #region Add Handlers
        private void addGlobal_GlobalsContext_Click(object sender, EventArgs e)
        {
            GlobalNode global = new GlobalNode("defaultGlobal");
            TreeNode newNode = AddGlobal(global, explorerTreeView.SelectedNode);

            ActivateNode(newNode);

        }

        private void addInputGlobalObject_Click(object sender, EventArgs e)
        {
            InputNode input = new InputNode("defaultInput");
            TreeNode newNode = AddInput(input, explorerTreeView.SelectedNode);

            GlobalNode global = (GlobalNode)explorerTreeView.SelectedNode.Tag;
            global.Inputs.Add(input);

            ActivateNode(newNode);
        }


        private void addDescriptionGlobalObject_Click(object sender, EventArgs e)
        {
            GlobalDescriptionNode desc = new GlobalDescriptionNode("defaultDesc");
            TreeNode newNode = AddDescription(desc, explorerTreeView.SelectedNode);

            GlobalNode global = (GlobalNode)explorerTreeView.SelectedNode.Tag;
            global.Descriptions.Add(desc);

            ActivateNode(newNode);
        }

        private void addLocal_LocalsContext_Click(object sender, EventArgs e)
        {
            LocalNode local = new LocalNode("defaultLocal");
            TreeNode newNode = AddLocal(local, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addLocalFunctionObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem localItem = (ToolStripMenuItem)sender;
            TreeNode localTreeNode = (TreeNode)localItem.Tag;

            LocalNode local = (LocalNode)localTreeNode.Tag;
            LocalNodeProxy proxy = new LocalNodeProxy(local);

            FunctionNode func = (FunctionNode)explorerTreeView.SelectedNode.Tag;
            func.Locals.Add(proxy);

            TreeNode newNode = AddLocalProxy(proxy, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addLocalGlobalObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem localItem = (ToolStripMenuItem)sender;
            TreeNode localTreeNode = (TreeNode)localItem.Tag;

            LocalNode local = (LocalNode)localTreeNode.Tag;
            LocalNodeProxy proxy = new LocalNodeProxy(local);

            GlobalNode global = (GlobalNode)explorerTreeView.SelectedNode.Tag;
            global.Locals.Add(proxy);

            TreeNode newNode = AddLocalProxy(proxy, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addGlobalDependentGlobalObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem globalDependentItem = (ToolStripMenuItem)sender;
            TreeNode dependentTreeNode = (TreeNode)globalDependentItem.Tag;

            GlobalNode global = (GlobalNode)dependentTreeNode.Tag;
            GlobalNodeProxy proxy = new GlobalNodeProxy(global);

            GlobalNode selGlobal = (GlobalNode)explorerTreeView.SelectedNode.Tag;
            selGlobal.Dependents.Add(proxy);

            TreeNode newNode = AddGlobalProxy(proxy, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addGlobalDependentRenderStateObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem globalDependentItem = (ToolStripMenuItem)sender;
            TreeNode dependentTreeNode = (TreeNode)globalDependentItem.Tag;

            GlobalNode global = (GlobalNode)dependentTreeNode.Tag;
            GlobalNodeProxy proxy = new GlobalNodeProxy(global);

            RenderStateNode selState = (RenderStateNode)explorerTreeView.SelectedNode.Tag;
            selState.Dependents.Add(proxy);

            TreeNode newNode = AddGlobalProxy(proxy, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addGlobalMaterialTemplateObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem globalDependentItem = (ToolStripMenuItem)sender;
            TreeNode dependentTreeNode = (TreeNode)globalDependentItem.Tag;

            GlobalNode global = (GlobalNode)dependentTreeNode.Tag;
            GlobalNodeProxy proxy = new GlobalNodeProxy(global);

            MaterialTemplateNode template = (MaterialTemplateNode)explorerTreeView.SelectedNode.Tag;
            template.GlobalProxies.Add(proxy);

            TreeNode newNode = AddGlobalProxy(proxy, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addGlobalVertexShaderObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem globalItem = (ToolStripMenuItem)sender;
            TreeNode treeNode = (TreeNode)globalItem.Tag;

            GlobalNode global = (GlobalNode)treeNode.Tag;
            GlobalNodeProxy proxy = new GlobalNodeProxy(global);

            VertexShaderNode selVS = (VertexShaderNode)explorerTreeView.SelectedNode.Tag;
            selVS.GlobalProxies.Add(proxy);

            TreeNode newNode = AddGlobalProxy(proxy, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addFunctionOpVertexShaderObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem functionItem = (ToolStripMenuItem)sender;
            TreeNode treeNode = (TreeNode)functionItem.Tag;

            FunctionNode func = (FunctionNode)treeNode.Tag;
            FunctionOpNode op = new FunctionOpNode(func);

            VertexShaderNode selVS = (VertexShaderNode)explorerTreeView.SelectedNode.Tag;
            selVS.FunctionOps.Add(op);

            TreeNode newNode = AddFunctionOp(op, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addAbstractionVertexShaderObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem absItem = (ToolStripMenuItem)sender;
            TreeNode treeNode = (TreeNode)absItem.Tag;

            AbstractionNode abstraction = (AbstractionNode)treeNode.Tag;
            AbstractionNodeProxy absProxy = new AbstractionNodeProxy(abstraction);

            VertexShaderNode selVS = (VertexShaderNode)explorerTreeView.SelectedNode.Tag;
            selVS.AbstractionProxies.Add(absProxy);

            TreeNode newNode = AddAbstractionProxy(absProxy, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addGlobalPixelShaderObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem globalItem = (ToolStripMenuItem)sender;
            TreeNode treeNode = (TreeNode)globalItem.Tag;

            GlobalNode global = (GlobalNode)treeNode.Tag;
            GlobalNodeProxy proxy = new GlobalNodeProxy(global);

            PixelShaderNode selPS = (PixelShaderNode)explorerTreeView.SelectedNode.Tag;
            selPS.GlobalProxies.Add(proxy);

            TreeNode newNode = AddGlobalProxy(proxy, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addFunctionOpPixelShaderObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem functionItem = (ToolStripMenuItem)sender;
            TreeNode treeNode = (TreeNode)functionItem.Tag;

            FunctionNode func = (FunctionNode)treeNode.Tag;
            FunctionOpNode op = new FunctionOpNode(func);

            PixelShaderNode selPS = (PixelShaderNode)explorerTreeView.SelectedNode.Tag;
            selPS.FunctionOps.Add(op);

            TreeNode newNode = AddFunctionOp(op, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addAbstractionPixelShaderObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem absItem = (ToolStripMenuItem)sender;
            TreeNode treeNode = (TreeNode)absItem.Tag;

            AbstractionNode abstraction = (AbstractionNode)treeNode.Tag;
            AbstractionNodeProxy absProxy = new AbstractionNodeProxy(abstraction);

            PixelShaderNode selPS = (PixelShaderNode)explorerTreeView.SelectedNode.Tag;
            selPS.AbstractionProxies.Add(absProxy);

            TreeNode newNode = AddAbstractionProxy(absProxy, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }
        
        private void setPixelShaderPassObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem psItem = (ToolStripMenuItem)sender;
            TreeNode treeNode = (TreeNode)psItem.Tag;

            PixelShaderNode ps = (PixelShaderNode)treeNode.Tag;
            PixelShaderOpNode op = new PixelShaderOpNode(ps);

            PassNode selPass = (PassNode)explorerTreeView.SelectedNode.Tag;
            selPass.PS = op;

            TreeNode newNode = SetPixelShaderOp(op, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void setVertexShaderPassObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem vsItem = (ToolStripMenuItem)sender;
            TreeNode treeNode = (TreeNode)vsItem.Tag;

            VertexShaderNode vs = (VertexShaderNode)treeNode.Tag;
            VertexShaderOpNode op = new VertexShaderOpNode(vs);

            PassNode selPass = (PassNode)explorerTreeView.SelectedNode.Tag;
            selPass.VS = op;

            TreeNode newNode = SetVertexShaderOp(op, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addFunction_FunctionsContext_Click(object sender, EventArgs e)
        {
            FunctionNode func = new FunctionNode("defaultFunction");
            TreeNode newNode = AddFunction(func, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addInputFunctionObject_Click(object sender, EventArgs e)
        {
            InputNode input = new InputNode("defaultInput");
            TreeNode newNode = AddInput(input, explorerTreeView.SelectedNode);

            FunctionNode function = (FunctionNode)explorerTreeView.SelectedNode.Tag;
            function.Inputs.Add(input);

            // Add the input to all function ops also
            // Change for all function ops
            List<TreeNode> opTreeNodes = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, opTreeNodes, typeof(FunctionOpNode));
            foreach (TreeNode opTreeNode in opTreeNodes)
            {
                FunctionOpNode op = (FunctionOpNode)opTreeNode.Tag;
                if (op.Owner == function)
                {
                    InputNode duplicate = new InputNode(input.Name);
                    duplicate.IsInputInstanced = true;
                    duplicate.Type = input.Type;
                    duplicate.DefaultValue = input.DefaultValue;
                    op.Inputs.Add(duplicate);
                }
            }

            ActivateNode(newNode);
        }

        private void addVertexShader_VertexShadersContext_Click(object sender, EventArgs e)
        {
            VertexShaderNode vs = new VertexShaderNode("defaultVS");
            TreeNode newNode = AddVertexShader(vs, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addVertexInVertexShaderObject_Click(object sender, EventArgs e)
        {
            VertexInNode vertIn = new VertexInNode("defaultVertIn");
            TreeNode newNode = AddVertexIn(vertIn, explorerTreeView.SelectedNode);

            VertexShaderNode selVS = (VertexShaderNode)explorerTreeView.SelectedNode.Tag;
            selVS.VertexIns.Add(vertIn);

            ActivateNode(newNode);
        }

        private void addVertexOutVertexShaderObject_Click(object sender, EventArgs e)
        {
            VertexOutNode vertOut = new VertexOutNode("defaultVertOut");
            TreeNode newNode = AddVertexOut(vertOut, explorerTreeView.SelectedNode);

            VertexShaderNode selVS = (VertexShaderNode)explorerTreeView.SelectedNode.Tag;
            selVS.VertexOuts.Add(vertOut);

            ActivateNode(newNode);
        }

        private void addPixelInPixelShaderObject_Click(object sender, EventArgs e)
        {
            PixelInNode pixIn = new PixelInNode("defaultPixIn");
            TreeNode newNode = AddPixelIn(pixIn, explorerTreeView.SelectedNode);

            PixelShaderNode selPS = (PixelShaderNode)explorerTreeView.SelectedNode.Tag;
            selPS.PixelIns.Add(pixIn);

            ActivateNode(newNode);
        }

        private void addPixelOutPixelShaderObject_Click(object sender, EventArgs e)
        {
            PixelOutNode pixOut = new PixelOutNode("defaultPixOut");
            TreeNode newNode = AddPixelOut(pixOut, explorerTreeView.SelectedNode);

            PixelShaderNode selPS = (PixelShaderNode)explorerTreeView.SelectedNode.Tag;
            selPS.PixelOuts.Add(pixOut);

            ActivateNode(newNode);
        }

        private void addPixelShader_PixelShadersContext_Click(object sender, EventArgs e)
        {
            PixelShaderNode ps = new PixelShaderNode("defaultPS");
            TreeNode newNode = AddPixelShader(ps, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }


        private void addInputRenderStateObject_Click(object sender, EventArgs e)
        {
            InputNode input = new InputNode("defaultInput");
            TreeNode newNode = AddInput(input, explorerTreeView.SelectedNode);

            RenderStateNode state = (RenderStateNode)explorerTreeView.SelectedNode.Tag;
            state.Inputs.Add(input);

            ActivateNode(newNode);
        }

        private void addFunctionOpAbstractionObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem functionItem = (ToolStripMenuItem)sender;
            TreeNode treeNode = (TreeNode)functionItem.Tag;

            FunctionNode func = (FunctionNode)treeNode.Tag;
            FunctionOpNode op = new FunctionOpNode(func);

            AbstractionNode selAbs = (AbstractionNode)explorerTreeView.SelectedNode.Tag;
            selAbs.Ops.Add(op);

            TreeNode newNode = AddFunctionOp(op, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }


        private void addAbstractionAbstractionObject_SubMenuClick(object sender, EventArgs e)
        {
            ToolStripMenuItem absItem = (ToolStripMenuItem)sender;
            TreeNode treeNode = (TreeNode)absItem.Tag;

            AbstractionNode abs = (AbstractionNode)treeNode.Tag;
            AbstractionNodeProxy proxy = new AbstractionNodeProxy(abs);

            AbstractionNode selAbs = (AbstractionNode)explorerTreeView.SelectedNode.Tag;
            selAbs.Proxies.Add(proxy);

            TreeNode newNode = AddAbstractionProxy(proxy, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }
        
        private void addAbstractionInAbstractionObject_Click(object sender, EventArgs e)
        {
            AbstractionInNode abstractIn = new AbstractionInNode("defaultAbstractIn");
            TreeNode newNode = AddAbstractionIn(abstractIn, explorerTreeView.SelectedNode);

            AbstractionNode selAbs = (AbstractionNode)explorerTreeView.SelectedNode.Tag;
            selAbs.Ins.Add(abstractIn);

            // Add the input to all abstraction proxies also
            List<TreeNode> proxyTreeNodes = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, proxyTreeNodes, typeof(AbstractionNodeProxy));
            foreach (TreeNode proxyTreeNode in proxyTreeNodes)
            {
                AbstractionNodeProxy proxy = (AbstractionNodeProxy)proxyTreeNode.Tag;
                if (proxy.Owner == selAbs)
                {
                    AbstractionNodeProxyIn proxyIn = new AbstractionNodeProxyIn(abstractIn);
                    proxy.Inputs.Add(proxyIn);
                }
            }

            ActivateNode(newNode);
        }

        private void addAbstractionOutAbstractionObject_Click(object sender, EventArgs e)
        {
            AbstractionOutNode abstractOut = new AbstractionOutNode("defaultAbstractOut");
            TreeNode newNode = AddAbstractionOut(abstractOut, explorerTreeView.SelectedNode);

            AbstractionNode selAbs = (AbstractionNode)explorerTreeView.SelectedNode.Tag;
            selAbs.Outs.Add(abstractOut);

            // Add the output to all abstraction proxies also
            List<TreeNode> proxyTreeNodes = new List<TreeNode>();
            FindNodesByTagType(explorerTreeView.Nodes, proxyTreeNodes, typeof(AbstractionNodeProxy));
            foreach (TreeNode proxyTreeNode in proxyTreeNodes)
            {
                AbstractionNodeProxy proxy = (AbstractionNodeProxy)proxyTreeNode.Tag;
                if (proxy.Owner == selAbs)
                {
                    AbstractionNodeProxyOut proxyOut = new AbstractionNodeProxyOut(abstractOut);
                    proxy.Outputs.Add(proxyOut);
                }
            }

            ActivateNode(newNode);
        }

        private void addAbstraction_AbstractionsContext_Click(object sender, EventArgs e)
        {
            AbstractionNode abstraction = new AbstractionNode("defaultAbstraction");
            TreeNode newNode = AddAbstraction(abstraction, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }


        private void addMaterialTemplate_MaterialTemplatesContext_Click(object sender, EventArgs e)
        {
            MaterialTemplateNode template = new MaterialTemplateNode("defaultTemplate");
            TreeNode newNode = AddMaterialTemplate(template, explorerTreeView.SelectedNode);

            ActivateNode(newNode);
        }

        private void addTechniqueMaterialTemplateObject_Click(object sender, EventArgs e)
        {
            TechniqueNode tech = new TechniqueNode("defaultTech");
            TreeNode newNode = AddTechnique(tech, explorerTreeView.SelectedNode);

            MaterialTemplateNode template = (MaterialTemplateNode)explorerTreeView.SelectedNode.Tag;
            template.Techniques.Add(tech);

            ActivateNode(newNode);
        }

        private void addPassTechniqueObject_Click(object sender, EventArgs e)
        {
            PassNode pass = new PassNode("defaultPass");
            TreeNode newNode = AddPass(pass, explorerTreeView.SelectedNode);

            TechniqueNode tech = (TechniqueNode)explorerTreeView.SelectedNode.Tag;
            tech.Passes.Add(pass);

            ActivateNode(newNode);
        }

        private void addRenderStatePassObject_Click(object sender, EventArgs e)
        {
            RenderStateNode state = new RenderStateNode("defaultState");
            TreeNode newNode = AddRenderState(state, explorerTreeView.SelectedNode);

            PassNode pass = (PassNode)explorerTreeView.SelectedNode.Tag;
            pass.States.Add(state);

            ActivateNode(newNode);
        }

        private void addFolder_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (ToolStripMenuItem)sender;
            ContextMenuStrip strip = (ContextMenuStrip)item.Owner;

            FolderNode folder = new FolderNode("defaultFolder");

            TreeNode newNode = AddFolder(folder, explorerTreeView.SelectedNode, strip);
            ActivateNode(newNode);
        }

        #endregion

        #region Delete Handlers
        private void deleteFolder_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            if (toDelete.Tag != null && toDelete.Tag.GetType().Equals(typeof(FolderNode)))
                RemoveFolder((FolderNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deletePixelShaderOpObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemovePixelShaderOp((PixelShaderOpNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteVertexShaderOpObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveVertexShaderOp((VertexShaderOpNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteTechniqueObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveTechnique((TechniqueNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteMaterialTemplateObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveMaterialTemplate((MaterialTemplateNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteAbstractionProxyObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveAbstractionProxy((AbstractionNodeProxy)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteAbstractionInObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveAbstractionIn((AbstractionInNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteAbstractionObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveAbstraction((AbstractionNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }


        private void deleteAbstractionOutObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveAbstractionOut((AbstractionOutNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deletePixelShaderObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemovePixelShader((PixelShaderNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteRenderStateObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveRenderState((RenderStateNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deletePixelInObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemovePixelIn((PixelInNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deletePixelOutObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemovePixelOut((PixelOutNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteVertexInObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveVertexIn((VertexInNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteVertexOutObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveVertexOut((VertexOutNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteVertexShaderObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveVertexShader((VertexShaderNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteFunctionObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveFunction((FunctionNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteFunctionOpObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveFunctionOp((FunctionOpNode)toDelete.Tag, toDelete);
        }

        private void deleteGlobalProxyObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveGlobalProxy((GlobalNodeProxy)toDelete.Tag, toDelete);
        }

        private void deleteLocalProxyObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveLocalProxy((LocalNodeProxy)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteLocalObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveLocal((LocalNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteInputObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveInput((InputNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteGlobalDescriptionObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveDescription((GlobalDescriptionNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deleteGlobalObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemoveGlobal((GlobalNode)toDelete.Tag, toDelete);

                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }

        private void deletePassObject_Click(object sender, EventArgs e)
        {
            TreeNode toDelete = explorerTreeView.SelectedNode;
            RemovePass((PassNode)toDelete.Tag, toDelete);


                        EventManager.GlobalEventManager.RaiseEvent(RefreshEventArgs.EVENT_TYPE, this, new RefreshEventArgs());
        }
        #endregion

        #region Duplicate Handlers
        private void duplicateVertexShaderObject_Click(object sender, EventArgs e)
        {
            TreeNode toDuplicate = explorerTreeView.SelectedNode;
            TreeNode parentNode = toDuplicate.Parent;
            VertexShaderNode vsClone = (VertexShaderNode)((VertexShaderNode)toDuplicate.Tag).Clone();
            TreeNode newNode = AddVertexShader(vsClone, toDuplicate.Parent);

            ActivateNode(newNode);
        }

        private void duplicatePixelShaderObject_Click(object sender, EventArgs e)
        {
            TreeNode toDuplicate = explorerTreeView.SelectedNode;
            TreeNode parentNode = toDuplicate.Parent;
            PixelShaderNode psClone = (PixelShaderNode)((PixelShaderNode)toDuplicate.Tag).Clone();
            TreeNode newNode = AddPixelShader(psClone, toDuplicate.Parent);

            ActivateNode(newNode);
        }

        private void duplicateAbstractionObject_Click(object sender, EventArgs e)
        {
            TreeNode toDuplicate = explorerTreeView.SelectedNode;
            TreeNode parentNode = toDuplicate.Parent;
            AbstractionNode abstractClone = (AbstractionNode)((AbstractionNode)toDuplicate.Tag).Clone();
            TreeNode newNode = AddAbstraction(abstractClone, toDuplicate.Parent);

            ActivateNode(newNode);
        }

        private void duplicateMaterialTemplateObject_Click(object sender, EventArgs e)
        {
            TreeNode toDuplicate = explorerTreeView.SelectedNode;
            TreeNode parentNode = toDuplicate.Parent;
            MaterialTemplateNode materialTemplateClone = (MaterialTemplateNode)((MaterialTemplateNode)toDuplicate.Tag).Clone();
            TreeNode newNode = AddMaterialTemplate(materialTemplateClone, toDuplicate.Parent);

            ActivateNode(newNode);
        }
        #endregion

        #region Change Folder Graphic Handlers
        private void explorerTreeView_BeforeCollapse(object sender, TreeViewCancelEventArgs e)
        {
            if (e.Node.ImageKey.Equals("Folder Open"))
            {
                e.Node.ImageKey = "Folder Closed";
                e.Node.SelectedImageKey = e.Node.ImageKey;
            }
        }

        private void explorerTreeView_BeforeExpand(object sender, TreeViewCancelEventArgs e)
        {
            if (e.Node.ImageKey.Equals("Folder Closed"))
            {
                e.Node.ImageKey = "Folder Open";
                e.Node.SelectedImageKey = e.Node.ImageKey;
            }
        }
        #endregion

        #region Dragging Handlers
        private TreeNode m_MovingNode;
        private TreeNode m_LastNode;
        private Color m_LastColor;

        private void explorerTreeView_ItemDrag(object sender, ItemDragEventArgs e)
        {
            m_MovingNode = (TreeNode)e.Item;
            ActivateNode(m_MovingNode);
            //UnHighlightNode(m_MovingNode);

            if (m_MovingNode.Tag != null)
            {
                if (m_MovingNode.Tag.GetType().Equals(typeof(GlobalNode)) ||
                    m_MovingNode.Tag.GetType().Equals(typeof(FunctionNode)) ||
                    m_MovingNode.Tag.GetType().Equals(typeof(LocalNode)) ||
                    m_MovingNode.Tag.GetType().Equals(typeof(MaterialTemplateNode)) ||
                    m_MovingNode.Tag.GetType().Equals(typeof(VertexShaderNode)) ||
                    m_MovingNode.Tag.GetType().Equals(typeof(PixelShaderNode)) ||
                    m_MovingNode.Tag.GetType().Equals(typeof(AbstractionNode)))
                {
                    m_LastNode = null;
                    DoDragDrop(e.Item, DragDropEffects.Move);
                    m_LastNode = null;
                }
            }
        }

        private void explorerTreeView_DragEnter(object sender, DragEventArgs e)
        {
            //e.Effect = DragDropEffects.Move;            
        }

        private void explorerTreeView_DragDrop(object sender, DragEventArgs e)
        {
            if (m_LastNode != null)
                m_LastNode.BackColor = m_LastColor;

            TreeNode node = explorerTreeView.GetNodeAt(explorerTreeView.PointToClient(Cursor.Position));
            if (node != null)
            {
                // Get top parent
                TreeNode rootParentNode = node;
                while (rootParentNode.Parent != null)
                {
                    rootParentNode = rootParentNode.Parent;
                }

                TreeNode movingRootParentNode = m_MovingNode.Parent;
                while (movingRootParentNode.Parent != null)
                {
                    movingRootParentNode = movingRootParentNode.Parent;
                }

                if (rootParentNode == movingRootParentNode)
                {
                    if (node.Parent == null)
                    {
                        // The root
                        TreeNode movingParent = m_MovingNode.Parent;
                        m_MovingNode.Remove();
                        node.Nodes.Add(m_MovingNode);

                        if (movingParent.Tag != null && movingParent.Tag.GetType().Equals(typeof(FolderNode)))
                        {
                            FolderNode parentFolder = (FolderNode)movingParent.Tag;
                            parentFolder.Nodes.Remove((ISerializable)m_MovingNode.Tag);
                        }

                        ActivateNode(m_MovingNode);
                    }
                    else if (node.Tag.GetType().Equals(typeof(GlobalNode)) ||
                            node.Tag.GetType().Equals(typeof(FunctionNode)) ||
                            node.Tag.GetType().Equals(typeof(LocalNode)) ||
                            node.Tag.GetType().Equals(typeof(MaterialTemplateNode)) ||
                            node.Tag.GetType().Equals(typeof(VertexShaderNode)) ||
                            node.Tag.GetType().Equals(typeof(PixelShaderNode)) ||
                            node.Tag.GetType().Equals(typeof(AbstractionNode)))
                    {
                        TreeNode parentNode = node.Parent;
                        TreeNode movingParent = m_MovingNode.Parent;
                        m_MovingNode.Remove();
                        parentNode.Nodes.Insert(node.Index, m_MovingNode);

                        if (movingParent.Tag != null && movingParent.Tag.GetType().Equals(typeof(FolderNode)))
                        {
                            FolderNode parentFolder = (FolderNode)parentNode.Tag;
                            parentFolder.Nodes.Remove((ISerializable)m_MovingNode.Tag);
                        }

                        if (parentNode.Tag != null && parentNode.Tag.GetType().Equals(typeof(FolderNode)))
                        {
                            FolderNode parentFolder = (FolderNode)parentNode.Tag;
                            parentFolder.Nodes.Insert(node.Index, (ISerializable)m_MovingNode.Tag);
                        }

                        ActivateNode(m_MovingNode);
                    }
                    else if (node.Tag.GetType().Equals(typeof(FolderNode)))
                    {
                        // Put it last in the folder node
                        FolderNode toAdd = (FolderNode)node.Tag;
                        TreeNode movingParent = m_MovingNode.Parent;
                        m_MovingNode.Remove();
                        node.Nodes.Add(m_MovingNode);

                        if (movingParent.Tag != null && movingParent.Tag.GetType().Equals(typeof(FolderNode)))
                        {
                            FolderNode parentFolder = (FolderNode)movingParent.Tag;
                            parentFolder.Nodes.Remove((ISerializable)m_MovingNode.Tag);
                        }

                        toAdd.Nodes.Add((ISerializable)m_MovingNode.Tag);

                        ActivateNode(m_MovingNode);
                    }
                }
            }

            m_MovingNode = null;
        }

        private void explorerTreeView_DragLeave(object sender, EventArgs e)
        {

        }

        private void explorerTreeView_DragOver(object sender, DragEventArgs e)
        {
            TreeNode node = explorerTreeView.GetNodeAt(explorerTreeView.PointToClient(Cursor.Position));
            e.Effect = DragDropEffects.None;
            if (node != null)
            {
                // Get top parent
                TreeNode rootParentNode = node;
                while (rootParentNode.Parent != null)
                {
                    rootParentNode = rootParentNode.Parent;
                }

                TreeNode movingRootParentNode = m_MovingNode.Parent;
                while (movingRootParentNode.Parent != null)
                {
                    movingRootParentNode = movingRootParentNode.Parent;
                }

                if (rootParentNode == movingRootParentNode)
                {
                    if (node.Parent == null ||
                        node.Tag.GetType().Equals(typeof(GlobalNode)) ||
                        node.Tag.GetType().Equals(typeof(FunctionNode)) ||
                        node.Tag.GetType().Equals(typeof(LocalNode)) ||
                        node.Tag.GetType().Equals(typeof(MaterialTemplateNode)) ||
                        node.Tag.GetType().Equals(typeof(VertexShaderNode)) ||
                        node.Tag.GetType().Equals(typeof(PixelShaderNode)) ||
                        node.Tag.GetType().Equals(typeof(FolderNode)) ||
                        node.Tag.GetType().Equals(typeof(AbstractionNode)))
                    {
                        if (m_LastNode != node)
                        {
                            if (m_LastNode != null)
                                m_LastNode.BackColor = m_LastColor;

                            m_LastNode = node;
                            m_LastColor = node.BackColor;

                            m_LastNode.BackColor = Color.FromKnownColor(KnownColor.InactiveCaption);
                        }

                        e.Effect = DragDropEffects.Move;
                    }
                }
            }
        }
        #endregion




    }
}