namespace MaterialEditor
{
    partial class ExplorerWin
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.TreeNode treeNode1 = new System.Windows.Forms.TreeNode("Vertex Shaders");
            System.Windows.Forms.TreeNode treeNode2 = new System.Windows.Forms.TreeNode("Pixel Shaders");
            System.Windows.Forms.TreeNode treeNode3 = new System.Windows.Forms.TreeNode("Locals");
            System.Windows.Forms.TreeNode treeNode4 = new System.Windows.Forms.TreeNode("Globals");
            System.Windows.Forms.TreeNode treeNode5 = new System.Windows.Forms.TreeNode("Functions");
            System.Windows.Forms.TreeNode treeNode6 = new System.Windows.Forms.TreeNode("Abstractions");
            System.Windows.Forms.TreeNode treeNode7 = new System.Windows.Forms.TreeNode("Material Templates");
            this.VertexShadersContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addVertexShader_VertexShadersContext = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator15 = new System.Windows.Forms.ToolStripSeparator();
            this.addFolder_VertexShadersContext = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator14 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteFolder_VertexShadersContext = new System.Windows.Forms.ToolStripMenuItem();
            this.PixelShadersContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addPixelShader_PixelShadersContext = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator16 = new System.Windows.Forms.ToolStripSeparator();
            this.addFolder_PixelShadersContext = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator17 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteFolder_PixelShadersContext = new System.Windows.Forms.ToolStripMenuItem();
            this.LocalsContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addLocal_LocalsContext = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator18 = new System.Windows.Forms.ToolStripSeparator();
            this.addFolder_LocalsContext = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator11 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteFolder_LocalsContext = new System.Windows.Forms.ToolStripMenuItem();
            this.GlobalsContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addGlobal_GlobalsContext = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator19 = new System.Windows.Forms.ToolStripSeparator();
            this.addFolder_GlobalsContext = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator12 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteFolder_GlobalsContext = new System.Windows.Forms.ToolStripMenuItem();
            this.FunctionsContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addFunction_FunctionsContext = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator20 = new System.Windows.Forms.ToolStripSeparator();
            this.addFolder_FunctionsContext = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator13 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteFolder_FunctionsContext = new System.Windows.Forms.ToolStripMenuItem();
            this.AbstractionsContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addAbstraction_AbstractionsContext = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator24 = new System.Windows.Forms.ToolStripSeparator();
            this.addFolder_AbstractionsContext = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator23 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteFolder_AbstractionsContext = new System.Windows.Forms.ToolStripMenuItem();
            this.MaterialTemplatesContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addMaterialTemplate_MaterialTemplatesContext = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator21 = new System.Windows.Forms.ToolStripSeparator();
            this.addFolder_MaterialTemplatesContext = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator22 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteFolder_MaterialTemplatesContext = new System.Windows.Forms.ToolStripMenuItem();
            this.explorerTreeView = new System.Windows.Forms.TreeView();
            this.VertexShaderObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addGlobalVertexShaderObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addAbstractionVertexShaderObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addFunctionOpVertexShaderObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addVertexOutVertexShaderObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addVertexInVertexShaderObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.duplicateVertexShaderObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator27 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteVertexShaderObject = new System.Windows.Forms.ToolStripMenuItem();
            this.PixelShaderObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addGlobalPixelShaderObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addAbstractionPixelShaderObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addFunctionOpPixelShaderObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addPixelInPixelShaderObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addPixelOutPixelShaderObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.duplicatePixelShaderObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator28 = new System.Windows.Forms.ToolStripSeparator();
            this.deletePixelShaderObject = new System.Windows.Forms.ToolStripMenuItem();
            this.TechniqueObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addPassTechniqueObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator8 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteTechniqueObject = new System.Windows.Forms.ToolStripMenuItem();
            this.PassObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.setVertexShaderPassObject = new System.Windows.Forms.ToolStripMenuItem();
            this.setPixelShaderPassObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addRenderStatePassObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator9 = new System.Windows.Forms.ToolStripSeparator();
            this.deletePassObject = new System.Windows.Forms.ToolStripMenuItem();
            this.MaterialTemplateObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addGlobalMaterialTemplateObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addTechniqueMaterialTemplateObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator10 = new System.Windows.Forms.ToolStripSeparator();
            this.verifyMaterialTemplateObject = new System.Windows.Forms.ToolStripMenuItem();
            this.exportMaterialTemplateObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator7 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteMaterialTemplateObject = new System.Windows.Forms.ToolStripMenuItem();
            this.GlobalObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addLocalGlobalObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addInputGlobalObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addDescriptionGlobalObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.addGlobalDependentGlobalObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteGlobalObject = new System.Windows.Forms.ToolStripMenuItem();
            this.InputObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteInputObject = new System.Windows.Forms.ToolStripMenuItem();
            this.GlobalDescriptionObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteGlobalDescriptionObject = new System.Windows.Forms.ToolStripMenuItem();
            this.LocalObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteLocalObject = new System.Windows.Forms.ToolStripMenuItem();
            this.LocalProxyObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteLocalProxyObject = new System.Windows.Forms.ToolStripMenuItem();
            this.GlobalProxyObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteGlobalProxyObject = new System.Windows.Forms.ToolStripMenuItem();
            this.FunctionObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addInputFunctionObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addLocalFunctionObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteFunctionObject = new System.Windows.Forms.ToolStripMenuItem();
            this.FunctionOpObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteFunctionOpObject = new System.Windows.Forms.ToolStripMenuItem();
            this.VertexInObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteVertexInObject = new System.Windows.Forms.ToolStripMenuItem();
            this.VertexOutObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteVertexOutObject = new System.Windows.Forms.ToolStripMenuItem();
            this.PixelInObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deletePixelInObject = new System.Windows.Forms.ToolStripMenuItem();
            this.PixelOutObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deletePixelOutObject = new System.Windows.Forms.ToolStripMenuItem();
            this.RenderStateObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addInputRenderStateObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addGlobalDependentRenderStateObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator6 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteRenderStateObject = new System.Windows.Forms.ToolStripMenuItem();
            this.VertexShaderOpObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteVertexShaderOpObject = new System.Windows.Forms.ToolStripMenuItem();
            this.PixelShaderOpObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deletePixelShaderOpObject = new System.Windows.Forms.ToolStripMenuItem();
            this.AbstractionObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addFunctionOpAbstractionObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addAbstractionAbstractionObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addAbstractionInAbstractionObject = new System.Windows.Forms.ToolStripMenuItem();
            this.addAbstractionOutAbstractionObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator25 = new System.Windows.Forms.ToolStripSeparator();
            this.duplicateAbstractionObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator26 = new System.Windows.Forms.ToolStripSeparator();
            this.deleteAbstractionObject = new System.Windows.Forms.ToolStripMenuItem();
            this.AbstractionInObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteAbstractionInObject = new System.Windows.Forms.ToolStripMenuItem();
            this.AbstractionOutObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteAbstractionOutObject = new System.Windows.Forms.ToolStripMenuItem();
            this.AbstractionProxyObjectContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteAbstractionProxyObject = new System.Windows.Forms.ToolStripMenuItem();
            this.duplicateMaterialTemplateObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator29 = new System.Windows.Forms.ToolStripSeparator();
            this.VertexShadersContext.SuspendLayout();
            this.PixelShadersContext.SuspendLayout();
            this.LocalsContext.SuspendLayout();
            this.GlobalsContext.SuspendLayout();
            this.FunctionsContext.SuspendLayout();
            this.AbstractionsContext.SuspendLayout();
            this.MaterialTemplatesContext.SuspendLayout();
            this.VertexShaderObjectContext.SuspendLayout();
            this.PixelShaderObjectContext.SuspendLayout();
            this.TechniqueObjectContext.SuspendLayout();
            this.PassObjectContext.SuspendLayout();
            this.MaterialTemplateObjectContext.SuspendLayout();
            this.GlobalObjectContext.SuspendLayout();
            this.InputObjectContext.SuspendLayout();
            this.GlobalDescriptionObjectContext.SuspendLayout();
            this.LocalObjectContext.SuspendLayout();
            this.LocalProxyObjectContext.SuspendLayout();
            this.GlobalProxyObjectContext.SuspendLayout();
            this.FunctionObjectContext.SuspendLayout();
            this.FunctionOpObjectContext.SuspendLayout();
            this.VertexInObjectContext.SuspendLayout();
            this.VertexOutObjectContext.SuspendLayout();
            this.PixelInObjectContext.SuspendLayout();
            this.PixelOutObjectContext.SuspendLayout();
            this.RenderStateObjectContext.SuspendLayout();
            this.VertexShaderOpObjectContext.SuspendLayout();
            this.PixelShaderOpObjectContext.SuspendLayout();
            this.AbstractionObjectContext.SuspendLayout();
            this.AbstractionInObjectContext.SuspendLayout();
            this.AbstractionOutObjectContext.SuspendLayout();
            this.AbstractionProxyObjectContext.SuspendLayout();
            this.SuspendLayout();
            // 
            // VertexShadersContext
            // 
            this.VertexShadersContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addVertexShader_VertexShadersContext,
            this.toolStripSeparator15,
            this.addFolder_VertexShadersContext,
            this.toolStripSeparator14,
            this.deleteFolder_VertexShadersContext});
            this.VertexShadersContext.Name = "VertexShadersContext";
            this.VertexShadersContext.Size = new System.Drawing.Size(171, 82);
            this.VertexShadersContext.Text = "VertexShadersContext";
            // 
            // addVertexShader_VertexShadersContext
            // 
            this.addVertexShader_VertexShadersContext.Name = "addVertexShader_VertexShadersContext";
            this.addVertexShader_VertexShadersContext.Size = new System.Drawing.Size(170, 22);
            this.addVertexShader_VertexShadersContext.Text = "Add Vertex Shader";
            this.addVertexShader_VertexShadersContext.Click += new System.EventHandler(this.addVertexShader_VertexShadersContext_Click);
            // 
            // toolStripSeparator15
            // 
            this.toolStripSeparator15.Name = "toolStripSeparator15";
            this.toolStripSeparator15.Size = new System.Drawing.Size(167, 6);
            // 
            // addFolder_VertexShadersContext
            // 
            this.addFolder_VertexShadersContext.Name = "addFolder_VertexShadersContext";
            this.addFolder_VertexShadersContext.Size = new System.Drawing.Size(170, 22);
            this.addFolder_VertexShadersContext.Text = "Add Folder";
            this.addFolder_VertexShadersContext.Click += new System.EventHandler(this.addFolder_Click);
            // 
            // toolStripSeparator14
            // 
            this.toolStripSeparator14.Name = "toolStripSeparator14";
            this.toolStripSeparator14.Size = new System.Drawing.Size(167, 6);
            // 
            // deleteFolder_VertexShadersContext
            // 
            this.deleteFolder_VertexShadersContext.Name = "deleteFolder_VertexShadersContext";
            this.deleteFolder_VertexShadersContext.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteFolder_VertexShadersContext.Size = new System.Drawing.Size(170, 22);
            this.deleteFolder_VertexShadersContext.Text = "Delete";
            this.deleteFolder_VertexShadersContext.Click += new System.EventHandler(this.deleteFolder_Click);
            // 
            // PixelShadersContext
            // 
            this.PixelShadersContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addPixelShader_PixelShadersContext,
            this.toolStripSeparator16,
            this.addFolder_PixelShadersContext,
            this.toolStripSeparator17,
            this.deleteFolder_PixelShadersContext});
            this.PixelShadersContext.Name = "PixelShadersContext";
            this.PixelShadersContext.Size = new System.Drawing.Size(163, 82);
            // 
            // addPixelShader_PixelShadersContext
            // 
            this.addPixelShader_PixelShadersContext.Name = "addPixelShader_PixelShadersContext";
            this.addPixelShader_PixelShadersContext.Size = new System.Drawing.Size(162, 22);
            this.addPixelShader_PixelShadersContext.Text = "Add Pixel Shader";
            this.addPixelShader_PixelShadersContext.Click += new System.EventHandler(this.addPixelShader_PixelShadersContext_Click);
            // 
            // toolStripSeparator16
            // 
            this.toolStripSeparator16.Name = "toolStripSeparator16";
            this.toolStripSeparator16.Size = new System.Drawing.Size(159, 6);
            // 
            // addFolder_PixelShadersContext
            // 
            this.addFolder_PixelShadersContext.Name = "addFolder_PixelShadersContext";
            this.addFolder_PixelShadersContext.Size = new System.Drawing.Size(162, 22);
            this.addFolder_PixelShadersContext.Text = "Add Folder";
            this.addFolder_PixelShadersContext.Click += new System.EventHandler(this.addFolder_Click);
            // 
            // toolStripSeparator17
            // 
            this.toolStripSeparator17.Name = "toolStripSeparator17";
            this.toolStripSeparator17.Size = new System.Drawing.Size(159, 6);
            // 
            // deleteFolder_PixelShadersContext
            // 
            this.deleteFolder_PixelShadersContext.Name = "deleteFolder_PixelShadersContext";
            this.deleteFolder_PixelShadersContext.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteFolder_PixelShadersContext.Size = new System.Drawing.Size(162, 22);
            this.deleteFolder_PixelShadersContext.Text = "Delete";
            this.deleteFolder_PixelShadersContext.Click += new System.EventHandler(this.deleteFolder_Click);
            // 
            // LocalsContext
            // 
            this.LocalsContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addLocal_LocalsContext,
            this.toolStripSeparator18,
            this.addFolder_LocalsContext,
            this.toolStripSeparator11,
            this.deleteFolder_LocalsContext});
            this.LocalsContext.Name = "LocalsContext";
            this.LocalsContext.Size = new System.Drawing.Size(133, 82);
            // 
            // addLocal_LocalsContext
            // 
            this.addLocal_LocalsContext.Name = "addLocal_LocalsContext";
            this.addLocal_LocalsContext.Size = new System.Drawing.Size(132, 22);
            this.addLocal_LocalsContext.Text = "Add Local";
            this.addLocal_LocalsContext.Click += new System.EventHandler(this.addLocal_LocalsContext_Click);
            // 
            // toolStripSeparator18
            // 
            this.toolStripSeparator18.Name = "toolStripSeparator18";
            this.toolStripSeparator18.Size = new System.Drawing.Size(129, 6);
            // 
            // addFolder_LocalsContext
            // 
            this.addFolder_LocalsContext.Name = "addFolder_LocalsContext";
            this.addFolder_LocalsContext.Size = new System.Drawing.Size(132, 22);
            this.addFolder_LocalsContext.Text = "Add Folder";
            this.addFolder_LocalsContext.Click += new System.EventHandler(this.addFolder_Click);
            // 
            // toolStripSeparator11
            // 
            this.toolStripSeparator11.Name = "toolStripSeparator11";
            this.toolStripSeparator11.Size = new System.Drawing.Size(129, 6);
            // 
            // deleteFolder_LocalsContext
            // 
            this.deleteFolder_LocalsContext.Name = "deleteFolder_LocalsContext";
            this.deleteFolder_LocalsContext.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteFolder_LocalsContext.Size = new System.Drawing.Size(132, 22);
            this.deleteFolder_LocalsContext.Text = "Delete";
            this.deleteFolder_LocalsContext.Click += new System.EventHandler(this.deleteFolder_Click);
            // 
            // GlobalsContext
            // 
            this.GlobalsContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addGlobal_GlobalsContext,
            this.toolStripSeparator19,
            this.addFolder_GlobalsContext,
            this.toolStripSeparator12,
            this.deleteFolder_GlobalsContext});
            this.GlobalsContext.Name = "GlobalsContext";
            this.GlobalsContext.Size = new System.Drawing.Size(134, 82);
            // 
            // addGlobal_GlobalsContext
            // 
            this.addGlobal_GlobalsContext.Name = "addGlobal_GlobalsContext";
            this.addGlobal_GlobalsContext.Size = new System.Drawing.Size(133, 22);
            this.addGlobal_GlobalsContext.Text = "Add Global";
            this.addGlobal_GlobalsContext.Click += new System.EventHandler(this.addGlobal_GlobalsContext_Click);
            // 
            // toolStripSeparator19
            // 
            this.toolStripSeparator19.Name = "toolStripSeparator19";
            this.toolStripSeparator19.Size = new System.Drawing.Size(130, 6);
            // 
            // addFolder_GlobalsContext
            // 
            this.addFolder_GlobalsContext.Name = "addFolder_GlobalsContext";
            this.addFolder_GlobalsContext.Size = new System.Drawing.Size(133, 22);
            this.addFolder_GlobalsContext.Text = "Add Folder";
            this.addFolder_GlobalsContext.Click += new System.EventHandler(this.addFolder_Click);
            // 
            // toolStripSeparator12
            // 
            this.toolStripSeparator12.Name = "toolStripSeparator12";
            this.toolStripSeparator12.Size = new System.Drawing.Size(130, 6);
            // 
            // deleteFolder_GlobalsContext
            // 
            this.deleteFolder_GlobalsContext.Name = "deleteFolder_GlobalsContext";
            this.deleteFolder_GlobalsContext.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteFolder_GlobalsContext.Size = new System.Drawing.Size(133, 22);
            this.deleteFolder_GlobalsContext.Text = "Delete";
            this.deleteFolder_GlobalsContext.Click += new System.EventHandler(this.deleteFolder_Click);
            // 
            // FunctionsContext
            // 
            this.FunctionsContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addFunction_FunctionsContext,
            this.toolStripSeparator20,
            this.addFolder_FunctionsContext,
            this.toolStripSeparator13,
            this.deleteFolder_FunctionsContext});
            this.FunctionsContext.Name = "FunctionsContext";
            this.FunctionsContext.Size = new System.Drawing.Size(147, 82);
            // 
            // addFunction_FunctionsContext
            // 
            this.addFunction_FunctionsContext.Name = "addFunction_FunctionsContext";
            this.addFunction_FunctionsContext.Size = new System.Drawing.Size(146, 22);
            this.addFunction_FunctionsContext.Text = "Add Function";
            this.addFunction_FunctionsContext.Click += new System.EventHandler(this.addFunction_FunctionsContext_Click);
            // 
            // toolStripSeparator20
            // 
            this.toolStripSeparator20.Name = "toolStripSeparator20";
            this.toolStripSeparator20.Size = new System.Drawing.Size(143, 6);
            // 
            // addFolder_FunctionsContext
            // 
            this.addFolder_FunctionsContext.Name = "addFolder_FunctionsContext";
            this.addFolder_FunctionsContext.Size = new System.Drawing.Size(146, 22);
            this.addFolder_FunctionsContext.Text = "Add Folder";
            this.addFolder_FunctionsContext.Click += new System.EventHandler(this.addFolder_Click);
            // 
            // toolStripSeparator13
            // 
            this.toolStripSeparator13.Name = "toolStripSeparator13";
            this.toolStripSeparator13.Size = new System.Drawing.Size(143, 6);
            // 
            // deleteFolder_FunctionsContext
            // 
            this.deleteFolder_FunctionsContext.Name = "deleteFolder_FunctionsContext";
            this.deleteFolder_FunctionsContext.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteFolder_FunctionsContext.Size = new System.Drawing.Size(146, 22);
            this.deleteFolder_FunctionsContext.Text = "Delete";
            this.deleteFolder_FunctionsContext.Click += new System.EventHandler(this.deleteFolder_Click);
            // 
            // AbstractionsContext
            // 
            this.AbstractionsContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addAbstraction_AbstractionsContext,
            this.toolStripSeparator24,
            this.addFolder_AbstractionsContext,
            this.toolStripSeparator23,
            this.deleteFolder_AbstractionsContext});
            this.AbstractionsContext.Name = "AbstractionsContext";
            this.AbstractionsContext.Size = new System.Drawing.Size(161, 82);
            // 
            // addAbstraction_AbstractionsContext
            // 
            this.addAbstraction_AbstractionsContext.Name = "addAbstraction_AbstractionsContext";
            this.addAbstraction_AbstractionsContext.Size = new System.Drawing.Size(160, 22);
            this.addAbstraction_AbstractionsContext.Text = "Add Abstraction";
            this.addAbstraction_AbstractionsContext.Click += new System.EventHandler(this.addAbstraction_AbstractionsContext_Click);
            // 
            // toolStripSeparator24
            // 
            this.toolStripSeparator24.Name = "toolStripSeparator24";
            this.toolStripSeparator24.Size = new System.Drawing.Size(157, 6);
            // 
            // addFolder_AbstractionsContext
            // 
            this.addFolder_AbstractionsContext.Name = "addFolder_AbstractionsContext";
            this.addFolder_AbstractionsContext.Size = new System.Drawing.Size(160, 22);
            this.addFolder_AbstractionsContext.Text = "Add Folder";
            this.addFolder_AbstractionsContext.Click += new System.EventHandler(this.addFolder_Click);
            // 
            // toolStripSeparator23
            // 
            this.toolStripSeparator23.Name = "toolStripSeparator23";
            this.toolStripSeparator23.Size = new System.Drawing.Size(157, 6);
            // 
            // deleteFolder_AbstractionsContext
            // 
            this.deleteFolder_AbstractionsContext.Name = "deleteFolder_AbstractionsContext";
            this.deleteFolder_AbstractionsContext.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteFolder_AbstractionsContext.Size = new System.Drawing.Size(160, 22);
            this.deleteFolder_AbstractionsContext.Text = "Delete";
            this.deleteFolder_AbstractionsContext.Click += new System.EventHandler(this.deleteFolder_Click);
            // 
            // MaterialTemplatesContext
            // 
            this.MaterialTemplatesContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addMaterialTemplate_MaterialTemplatesContext,
            this.toolStripSeparator21,
            this.addFolder_MaterialTemplatesContext,
            this.toolStripSeparator22,
            this.deleteFolder_MaterialTemplatesContext});
            this.MaterialTemplatesContext.Name = "MaterialTemplatesContext";
            this.MaterialTemplatesContext.Size = new System.Drawing.Size(196, 82);
            // 
            // addMaterialTemplate_MaterialTemplatesContext
            // 
            this.addMaterialTemplate_MaterialTemplatesContext.Name = "addMaterialTemplate_MaterialTemplatesContext";
            this.addMaterialTemplate_MaterialTemplatesContext.Size = new System.Drawing.Size(195, 22);
            this.addMaterialTemplate_MaterialTemplatesContext.Text = "Add Material Template";
            this.addMaterialTemplate_MaterialTemplatesContext.Click += new System.EventHandler(this.addMaterialTemplate_MaterialTemplatesContext_Click);
            // 
            // toolStripSeparator21
            // 
            this.toolStripSeparator21.Name = "toolStripSeparator21";
            this.toolStripSeparator21.Size = new System.Drawing.Size(192, 6);
            // 
            // addFolder_MaterialTemplatesContext
            // 
            this.addFolder_MaterialTemplatesContext.Name = "addFolder_MaterialTemplatesContext";
            this.addFolder_MaterialTemplatesContext.Size = new System.Drawing.Size(195, 22);
            this.addFolder_MaterialTemplatesContext.Text = "Add Folder";
            this.addFolder_MaterialTemplatesContext.Click += new System.EventHandler(this.addFolder_Click);
            // 
            // toolStripSeparator22
            // 
            this.toolStripSeparator22.Name = "toolStripSeparator22";
            this.toolStripSeparator22.Size = new System.Drawing.Size(192, 6);
            // 
            // deleteFolder_MaterialTemplatesContext
            // 
            this.deleteFolder_MaterialTemplatesContext.Name = "deleteFolder_MaterialTemplatesContext";
            this.deleteFolder_MaterialTemplatesContext.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteFolder_MaterialTemplatesContext.Size = new System.Drawing.Size(195, 22);
            this.deleteFolder_MaterialTemplatesContext.Text = "Delete";
            this.deleteFolder_MaterialTemplatesContext.Click += new System.EventHandler(this.deleteFolder_Click);
            // 
            // explorerTreeView
            // 
            this.explorerTreeView.AllowDrop = true;
            this.explorerTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.explorerTreeView.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F);
            this.explorerTreeView.Location = new System.Drawing.Point(0, 0);
            this.explorerTreeView.Name = "explorerTreeView";
            treeNode1.ContextMenuStrip = this.VertexShadersContext;
            treeNode1.Name = "Vertex Shaders";
            treeNode1.Text = "Vertex Shaders";
            treeNode2.ContextMenuStrip = this.PixelShadersContext;
            treeNode2.Name = "Pixel Shaders";
            treeNode2.Text = "Pixel Shaders";
            treeNode3.ContextMenuStrip = this.LocalsContext;
            treeNode3.Name = "Locals";
            treeNode3.Text = "Locals";
            treeNode4.ContextMenuStrip = this.GlobalsContext;
            treeNode4.Name = "Globals";
            treeNode4.Text = "Globals";
            treeNode5.ContextMenuStrip = this.FunctionsContext;
            treeNode5.Name = "Functions";
            treeNode5.Text = "Functions";
            treeNode6.ContextMenuStrip = this.AbstractionsContext;
            treeNode6.Name = "Abstractions";
            treeNode6.Text = "Abstractions";
            treeNode7.ContextMenuStrip = this.MaterialTemplatesContext;
            treeNode7.Name = "Material Templates";
            treeNode7.Text = "Material Templates";
            this.explorerTreeView.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
            treeNode1,
            treeNode2,
            treeNode3,
            treeNode4,
            treeNode5,
            treeNode6,
            treeNode7});
            this.explorerTreeView.Size = new System.Drawing.Size(284, 282);
            this.explorerTreeView.TabIndex = 1;
            this.explorerTreeView.BeforeCollapse += new System.Windows.Forms.TreeViewCancelEventHandler(this.explorerTreeView_BeforeCollapse);
            this.explorerTreeView.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.explorerTreeView_BeforeExpand);
            this.explorerTreeView.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.explorerTreeView_ItemDrag);
            this.explorerTreeView.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.explorerTreeView_NodeMouseClick);
            this.explorerTreeView.DragDrop += new System.Windows.Forms.DragEventHandler(this.explorerTreeView_DragDrop);
            this.explorerTreeView.DragEnter += new System.Windows.Forms.DragEventHandler(this.explorerTreeView_DragEnter);
            this.explorerTreeView.DragOver += new System.Windows.Forms.DragEventHandler(this.explorerTreeView_DragOver);
            this.explorerTreeView.DragLeave += new System.EventHandler(this.explorerTreeView_DragLeave);
            this.explorerTreeView.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.explorerTreeView_KeyPress);
            // 
            // VertexShaderObjectContext
            // 
            this.VertexShaderObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addGlobalVertexShaderObject,
            this.addAbstractionVertexShaderObject,
            this.addFunctionOpVertexShaderObject,
            this.addVertexOutVertexShaderObject,
            this.addVertexInVertexShaderObject,
            this.toolStripSeparator4,
            this.duplicateVertexShaderObject,
            this.toolStripSeparator27,
            this.deleteVertexShaderObject});
            this.VertexShaderObjectContext.Name = "VertexShaderObjectContext";
            this.VertexShaderObjectContext.Size = new System.Drawing.Size(166, 170);
            this.VertexShaderObjectContext.Opening += new System.ComponentModel.CancelEventHandler(this.VertexShaderObjectContext_Opening);
            // 
            // addGlobalVertexShaderObject
            // 
            this.addGlobalVertexShaderObject.Name = "addGlobalVertexShaderObject";
            this.addGlobalVertexShaderObject.Size = new System.Drawing.Size(165, 22);
            this.addGlobalVertexShaderObject.Text = "Add Global";
            // 
            // addAbstractionVertexShaderObject
            // 
            this.addAbstractionVertexShaderObject.Name = "addAbstractionVertexShaderObject";
            this.addAbstractionVertexShaderObject.Size = new System.Drawing.Size(165, 22);
            this.addAbstractionVertexShaderObject.Text = "Add Abstraction";
            // 
            // addFunctionOpVertexShaderObject
            // 
            this.addFunctionOpVertexShaderObject.Name = "addFunctionOpVertexShaderObject";
            this.addFunctionOpVertexShaderObject.Size = new System.Drawing.Size(165, 22);
            this.addFunctionOpVertexShaderObject.Text = "Add Function Op";
            // 
            // addVertexOutVertexShaderObject
            // 
            this.addVertexOutVertexShaderObject.Name = "addVertexOutVertexShaderObject";
            this.addVertexOutVertexShaderObject.Size = new System.Drawing.Size(165, 22);
            this.addVertexOutVertexShaderObject.Text = "Add Vertex Out";
            this.addVertexOutVertexShaderObject.Click += new System.EventHandler(this.addVertexOutVertexShaderObject_Click);
            // 
            // addVertexInVertexShaderObject
            // 
            this.addVertexInVertexShaderObject.Name = "addVertexInVertexShaderObject";
            this.addVertexInVertexShaderObject.Size = new System.Drawing.Size(165, 22);
            this.addVertexInVertexShaderObject.Text = "Add Vertex In";
            this.addVertexInVertexShaderObject.Click += new System.EventHandler(this.addVertexInVertexShaderObject_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(162, 6);
            // 
            // duplicateVertexShaderObject
            // 
            this.duplicateVertexShaderObject.Name = "duplicateVertexShaderObject";
            this.duplicateVertexShaderObject.Size = new System.Drawing.Size(165, 22);
            this.duplicateVertexShaderObject.Text = "Duplicate";
            this.duplicateVertexShaderObject.Click += new System.EventHandler(this.duplicateVertexShaderObject_Click);
            // 
            // toolStripSeparator27
            // 
            this.toolStripSeparator27.Name = "toolStripSeparator27";
            this.toolStripSeparator27.Size = new System.Drawing.Size(162, 6);
            // 
            // deleteVertexShaderObject
            // 
            this.deleteVertexShaderObject.Name = "deleteVertexShaderObject";
            this.deleteVertexShaderObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteVertexShaderObject.Size = new System.Drawing.Size(165, 22);
            this.deleteVertexShaderObject.Text = "Delete";
            this.deleteVertexShaderObject.Click += new System.EventHandler(this.deleteVertexShaderObject_Click);
            // 
            // PixelShaderObjectContext
            // 
            this.PixelShaderObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addGlobalPixelShaderObject,
            this.addAbstractionPixelShaderObject,
            this.addFunctionOpPixelShaderObject,
            this.addPixelInPixelShaderObject,
            this.addPixelOutPixelShaderObject,
            this.toolStripSeparator5,
            this.duplicatePixelShaderObject,
            this.toolStripSeparator28,
            this.deletePixelShaderObject});
            this.PixelShaderObjectContext.Name = "PixelShaderObjectContext";
            this.PixelShaderObjectContext.Size = new System.Drawing.Size(166, 170);
            this.PixelShaderObjectContext.Opening += new System.ComponentModel.CancelEventHandler(this.PixelShaderObjectContext_Opening);
            // 
            // addGlobalPixelShaderObject
            // 
            this.addGlobalPixelShaderObject.Name = "addGlobalPixelShaderObject";
            this.addGlobalPixelShaderObject.Size = new System.Drawing.Size(165, 22);
            this.addGlobalPixelShaderObject.Text = "Add Global";
            // 
            // addAbstractionPixelShaderObject
            // 
            this.addAbstractionPixelShaderObject.Name = "addAbstractionPixelShaderObject";
            this.addAbstractionPixelShaderObject.Size = new System.Drawing.Size(165, 22);
            this.addAbstractionPixelShaderObject.Text = "Add Abstraction";
            // 
            // addFunctionOpPixelShaderObject
            // 
            this.addFunctionOpPixelShaderObject.Name = "addFunctionOpPixelShaderObject";
            this.addFunctionOpPixelShaderObject.Size = new System.Drawing.Size(165, 22);
            this.addFunctionOpPixelShaderObject.Text = "Add Function Op";
            // 
            // addPixelInPixelShaderObject
            // 
            this.addPixelInPixelShaderObject.Name = "addPixelInPixelShaderObject";
            this.addPixelInPixelShaderObject.Size = new System.Drawing.Size(165, 22);
            this.addPixelInPixelShaderObject.Text = "Add Pixel In";
            this.addPixelInPixelShaderObject.Click += new System.EventHandler(this.addPixelInPixelShaderObject_Click);
            // 
            // addPixelOutPixelShaderObject
            // 
            this.addPixelOutPixelShaderObject.Name = "addPixelOutPixelShaderObject";
            this.addPixelOutPixelShaderObject.Size = new System.Drawing.Size(165, 22);
            this.addPixelOutPixelShaderObject.Text = "Add Pixel Out";
            this.addPixelOutPixelShaderObject.Click += new System.EventHandler(this.addPixelOutPixelShaderObject_Click);
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(162, 6);
            // 
            // duplicatePixelShaderObject
            // 
            this.duplicatePixelShaderObject.Name = "duplicatePixelShaderObject";
            this.duplicatePixelShaderObject.Size = new System.Drawing.Size(165, 22);
            this.duplicatePixelShaderObject.Text = "Duplicate";
            this.duplicatePixelShaderObject.Click += new System.EventHandler(this.duplicatePixelShaderObject_Click);
            // 
            // toolStripSeparator28
            // 
            this.toolStripSeparator28.Name = "toolStripSeparator28";
            this.toolStripSeparator28.Size = new System.Drawing.Size(162, 6);
            // 
            // deletePixelShaderObject
            // 
            this.deletePixelShaderObject.Name = "deletePixelShaderObject";
            this.deletePixelShaderObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deletePixelShaderObject.Size = new System.Drawing.Size(165, 22);
            this.deletePixelShaderObject.Text = "Delete";
            this.deletePixelShaderObject.Click += new System.EventHandler(this.deletePixelShaderObject_Click);
            // 
            // TechniqueObjectContext
            // 
            this.TechniqueObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addPassTechniqueObject,
            this.toolStripSeparator8,
            this.deleteTechniqueObject});
            this.TechniqueObjectContext.Name = "TechniqueContext";
            this.TechniqueObjectContext.Size = new System.Drawing.Size(132, 54);
            // 
            // addPassTechniqueObject
            // 
            this.addPassTechniqueObject.Name = "addPassTechniqueObject";
            this.addPassTechniqueObject.Size = new System.Drawing.Size(131, 22);
            this.addPassTechniqueObject.Text = "Add Pass";
            this.addPassTechniqueObject.Click += new System.EventHandler(this.addPassTechniqueObject_Click);
            // 
            // toolStripSeparator8
            // 
            this.toolStripSeparator8.Name = "toolStripSeparator8";
            this.toolStripSeparator8.Size = new System.Drawing.Size(128, 6);
            // 
            // deleteTechniqueObject
            // 
            this.deleteTechniqueObject.Name = "deleteTechniqueObject";
            this.deleteTechniqueObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteTechniqueObject.Size = new System.Drawing.Size(131, 22);
            this.deleteTechniqueObject.Text = "Delete";
            this.deleteTechniqueObject.Click += new System.EventHandler(this.deleteTechniqueObject_Click);
            // 
            // PassObjectContext
            // 
            this.PassObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.setVertexShaderPassObject,
            this.setPixelShaderPassObject,
            this.addRenderStatePassObject,
            this.toolStripSeparator9,
            this.deletePassObject});
            this.PassObjectContext.Name = "PassContext";
            this.PassObjectContext.Size = new System.Drawing.Size(166, 98);
            this.PassObjectContext.Opening += new System.ComponentModel.CancelEventHandler(this.PassObjectContext_Opening);
            // 
            // setVertexShaderPassObject
            // 
            this.setVertexShaderPassObject.Name = "setVertexShaderPassObject";
            this.setVertexShaderPassObject.Size = new System.Drawing.Size(165, 22);
            this.setVertexShaderPassObject.Text = "Set Vertex Shader";
            // 
            // setPixelShaderPassObject
            // 
            this.setPixelShaderPassObject.Name = "setPixelShaderPassObject";
            this.setPixelShaderPassObject.Size = new System.Drawing.Size(165, 22);
            this.setPixelShaderPassObject.Text = "Set Pixel Shader";
            // 
            // addRenderStatePassObject
            // 
            this.addRenderStatePassObject.Name = "addRenderStatePassObject";
            this.addRenderStatePassObject.Size = new System.Drawing.Size(165, 22);
            this.addRenderStatePassObject.Text = "Add Render State";
            this.addRenderStatePassObject.Click += new System.EventHandler(this.addRenderStatePassObject_Click);
            // 
            // toolStripSeparator9
            // 
            this.toolStripSeparator9.Name = "toolStripSeparator9";
            this.toolStripSeparator9.Size = new System.Drawing.Size(162, 6);
            // 
            // deletePassObject
            // 
            this.deletePassObject.Name = "deletePassObject";
            this.deletePassObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deletePassObject.Size = new System.Drawing.Size(165, 22);
            this.deletePassObject.Text = "Delete";
            this.deletePassObject.Click += new System.EventHandler(this.deletePassObject_Click);
            // 
            // MaterialTemplateObjectContext
            // 
            this.MaterialTemplateObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addGlobalMaterialTemplateObject,
            this.addTechniqueMaterialTemplateObject,
            this.toolStripSeparator10,
            this.verifyMaterialTemplateObject,
            this.exportMaterialTemplateObject,
            this.toolStripSeparator7,
            this.duplicateMaterialTemplateObject,
            this.toolStripSeparator29,
            this.deleteMaterialTemplateObject});
            this.MaterialTemplateObjectContext.Name = "MaterialTemplateObjectContext";
            this.MaterialTemplateObjectContext.Size = new System.Drawing.Size(156, 176);
            this.MaterialTemplateObjectContext.Opening += new System.ComponentModel.CancelEventHandler(this.MaterialTemplateObjectContext_Opening);
            // 
            // addGlobalMaterialTemplateObject
            // 
            this.addGlobalMaterialTemplateObject.Name = "addGlobalMaterialTemplateObject";
            this.addGlobalMaterialTemplateObject.Size = new System.Drawing.Size(155, 22);
            this.addGlobalMaterialTemplateObject.Text = "Add Global";
            // 
            // addTechniqueMaterialTemplateObject
            // 
            this.addTechniqueMaterialTemplateObject.Name = "addTechniqueMaterialTemplateObject";
            this.addTechniqueMaterialTemplateObject.Size = new System.Drawing.Size(155, 22);
            this.addTechniqueMaterialTemplateObject.Text = "Add Technique";
            this.addTechniqueMaterialTemplateObject.Click += new System.EventHandler(this.addTechniqueMaterialTemplateObject_Click);
            // 
            // toolStripSeparator10
            // 
            this.toolStripSeparator10.Name = "toolStripSeparator10";
            this.toolStripSeparator10.Size = new System.Drawing.Size(152, 6);
            // 
            // verifyMaterialTemplateObject
            // 
            this.verifyMaterialTemplateObject.Name = "verifyMaterialTemplateObject";
            this.verifyMaterialTemplateObject.Size = new System.Drawing.Size(155, 22);
            this.verifyMaterialTemplateObject.Text = "Verify";
            this.verifyMaterialTemplateObject.Click += new System.EventHandler(this.verifyMaterialTemplateObject_Click);
            // 
            // exportMaterialTemplateObject
            // 
            this.exportMaterialTemplateObject.Name = "exportMaterialTemplateObject";
            this.exportMaterialTemplateObject.Size = new System.Drawing.Size(155, 22);
            this.exportMaterialTemplateObject.Text = "Export";
            this.exportMaterialTemplateObject.Click += new System.EventHandler(this.exportMaterialTemplateObject_Click);
            // 
            // toolStripSeparator7
            // 
            this.toolStripSeparator7.Name = "toolStripSeparator7";
            this.toolStripSeparator7.Size = new System.Drawing.Size(152, 6);
            // 
            // deleteMaterialTemplateObject
            // 
            this.deleteMaterialTemplateObject.Name = "deleteMaterialTemplateObject";
            this.deleteMaterialTemplateObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteMaterialTemplateObject.Size = new System.Drawing.Size(155, 22);
            this.deleteMaterialTemplateObject.Text = "Delete";
            this.deleteMaterialTemplateObject.Click += new System.EventHandler(this.deleteMaterialTemplateObject_Click);
            // 
            // GlobalObjectContext
            // 
            this.GlobalObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addLocalGlobalObject,
            this.addInputGlobalObject,
            this.addDescriptionGlobalObject,
            this.toolStripSeparator1,
            this.addGlobalDependentGlobalObject,
            this.toolStripSeparator2,
            this.deleteGlobalObject});
            this.GlobalObjectContext.Name = "GlobalObjectContext";
            this.GlobalObjectContext.Size = new System.Drawing.Size(195, 126);
            this.GlobalObjectContext.Opening += new System.ComponentModel.CancelEventHandler(this.GlobalObjectContext_Opening);
            // 
            // addLocalGlobalObject
            // 
            this.addLocalGlobalObject.Name = "addLocalGlobalObject";
            this.addLocalGlobalObject.Size = new System.Drawing.Size(194, 22);
            this.addLocalGlobalObject.Text = "Add Local";
            // 
            // addInputGlobalObject
            // 
            this.addInputGlobalObject.Name = "addInputGlobalObject";
            this.addInputGlobalObject.Size = new System.Drawing.Size(194, 22);
            this.addInputGlobalObject.Text = "Add Input";
            this.addInputGlobalObject.Click += new System.EventHandler(this.addInputGlobalObject_Click);
            // 
            // addDescriptionGlobalObject
            // 
            this.addDescriptionGlobalObject.Name = "addDescriptionGlobalObject";
            this.addDescriptionGlobalObject.Size = new System.Drawing.Size(194, 22);
            this.addDescriptionGlobalObject.Text = "Add Description";
            this.addDescriptionGlobalObject.Click += new System.EventHandler(this.addDescriptionGlobalObject_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(191, 6);
            // 
            // addGlobalDependentGlobalObject
            // 
            this.addGlobalDependentGlobalObject.Name = "addGlobalDependentGlobalObject";
            this.addGlobalDependentGlobalObject.Size = new System.Drawing.Size(194, 22);
            this.addGlobalDependentGlobalObject.Text = "Add Global Dependent";
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(191, 6);
            // 
            // deleteGlobalObject
            // 
            this.deleteGlobalObject.Name = "deleteGlobalObject";
            this.deleteGlobalObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteGlobalObject.Size = new System.Drawing.Size(194, 22);
            this.deleteGlobalObject.Text = "Delete";
            this.deleteGlobalObject.Click += new System.EventHandler(this.deleteGlobalObject_Click);
            // 
            // InputObjectContext
            // 
            this.InputObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteInputObject});
            this.InputObjectContext.Name = "InputObjectContext";
            this.InputObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deleteInputObject
            // 
            this.deleteInputObject.Name = "deleteInputObject";
            this.deleteInputObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteInputObject.Size = new System.Drawing.Size(131, 22);
            this.deleteInputObject.Text = "Delete";
            this.deleteInputObject.Click += new System.EventHandler(this.deleteInputObject_Click);
            // 
            // GlobalDescriptionObjectContext
            // 
            this.GlobalDescriptionObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteGlobalDescriptionObject});
            this.GlobalDescriptionObjectContext.Name = "GlobalDescriptionObjectContext";
            this.GlobalDescriptionObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deleteGlobalDescriptionObject
            // 
            this.deleteGlobalDescriptionObject.Name = "deleteGlobalDescriptionObject";
            this.deleteGlobalDescriptionObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteGlobalDescriptionObject.Size = new System.Drawing.Size(131, 22);
            this.deleteGlobalDescriptionObject.Text = "Delete";
            this.deleteGlobalDescriptionObject.Click += new System.EventHandler(this.deleteGlobalDescriptionObject_Click);
            // 
            // LocalObjectContext
            // 
            this.LocalObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteLocalObject});
            this.LocalObjectContext.Name = "LocalObjectContext";
            this.LocalObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deleteLocalObject
            // 
            this.deleteLocalObject.Name = "deleteLocalObject";
            this.deleteLocalObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteLocalObject.Size = new System.Drawing.Size(131, 22);
            this.deleteLocalObject.Text = "Delete";
            this.deleteLocalObject.Click += new System.EventHandler(this.deleteLocalObject_Click);
            // 
            // LocalProxyObjectContext
            // 
            this.LocalProxyObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteLocalProxyObject});
            this.LocalProxyObjectContext.Name = "LocalProxyObjectContext";
            this.LocalProxyObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deleteLocalProxyObject
            // 
            this.deleteLocalProxyObject.Name = "deleteLocalProxyObject";
            this.deleteLocalProxyObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteLocalProxyObject.Size = new System.Drawing.Size(131, 22);
            this.deleteLocalProxyObject.Text = "Delete";
            this.deleteLocalProxyObject.Click += new System.EventHandler(this.deleteLocalProxyObject_Click);
            // 
            // GlobalProxyObjectContext
            // 
            this.GlobalProxyObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteGlobalProxyObject});
            this.GlobalProxyObjectContext.Name = "GlobalProxyObjectContext";
            this.GlobalProxyObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deleteGlobalProxyObject
            // 
            this.deleteGlobalProxyObject.Name = "deleteGlobalProxyObject";
            this.deleteGlobalProxyObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteGlobalProxyObject.Size = new System.Drawing.Size(131, 22);
            this.deleteGlobalProxyObject.Text = "Delete";
            this.deleteGlobalProxyObject.Click += new System.EventHandler(this.deleteGlobalProxyObject_Click);
            // 
            // FunctionObjectContext
            // 
            this.FunctionObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addInputFunctionObject,
            this.addLocalFunctionObject,
            this.toolStripSeparator3,
            this.deleteFunctionObject});
            this.FunctionObjectContext.Name = "FunctionObjectContext";
            this.FunctionObjectContext.Size = new System.Drawing.Size(132, 76);
            this.FunctionObjectContext.Opening += new System.ComponentModel.CancelEventHandler(this.FunctionObjectContext_Opening);
            // 
            // addInputFunctionObject
            // 
            this.addInputFunctionObject.Name = "addInputFunctionObject";
            this.addInputFunctionObject.Size = new System.Drawing.Size(131, 22);
            this.addInputFunctionObject.Text = "Add Input";
            this.addInputFunctionObject.Click += new System.EventHandler(this.addInputFunctionObject_Click);
            // 
            // addLocalFunctionObject
            // 
            this.addLocalFunctionObject.Name = "addLocalFunctionObject";
            this.addLocalFunctionObject.Size = new System.Drawing.Size(131, 22);
            this.addLocalFunctionObject.Text = "Add Local";
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(128, 6);
            // 
            // deleteFunctionObject
            // 
            this.deleteFunctionObject.Name = "deleteFunctionObject";
            this.deleteFunctionObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteFunctionObject.Size = new System.Drawing.Size(131, 22);
            this.deleteFunctionObject.Text = "Delete";
            this.deleteFunctionObject.Click += new System.EventHandler(this.deleteFunctionObject_Click);
            // 
            // FunctionOpObjectContext
            // 
            this.FunctionOpObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteFunctionOpObject});
            this.FunctionOpObjectContext.Name = "FunctionOpObjectContext";
            this.FunctionOpObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deleteFunctionOpObject
            // 
            this.deleteFunctionOpObject.Name = "deleteFunctionOpObject";
            this.deleteFunctionOpObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteFunctionOpObject.Size = new System.Drawing.Size(131, 22);
            this.deleteFunctionOpObject.Text = "Delete";
            this.deleteFunctionOpObject.Click += new System.EventHandler(this.deleteFunctionOpObject_Click);
            // 
            // VertexInObjectContext
            // 
            this.VertexInObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteVertexInObject});
            this.VertexInObjectContext.Name = "VertexInObjectContext";
            this.VertexInObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deleteVertexInObject
            // 
            this.deleteVertexInObject.Name = "deleteVertexInObject";
            this.deleteVertexInObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteVertexInObject.Size = new System.Drawing.Size(131, 22);
            this.deleteVertexInObject.Text = "Delete";
            this.deleteVertexInObject.Click += new System.EventHandler(this.deleteVertexInObject_Click);
            // 
            // VertexOutObjectContext
            // 
            this.VertexOutObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteVertexOutObject});
            this.VertexOutObjectContext.Name = "VertexOutObjectContext";
            this.VertexOutObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deleteVertexOutObject
            // 
            this.deleteVertexOutObject.Name = "deleteVertexOutObject";
            this.deleteVertexOutObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteVertexOutObject.Size = new System.Drawing.Size(131, 22);
            this.deleteVertexOutObject.Text = "Delete";
            this.deleteVertexOutObject.Click += new System.EventHandler(this.deleteVertexOutObject_Click);
            // 
            // PixelInObjectContext
            // 
            this.PixelInObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deletePixelInObject});
            this.PixelInObjectContext.Name = "PixelInObjectContext";
            this.PixelInObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deletePixelInObject
            // 
            this.deletePixelInObject.Name = "deletePixelInObject";
            this.deletePixelInObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deletePixelInObject.Size = new System.Drawing.Size(131, 22);
            this.deletePixelInObject.Text = "Delete";
            this.deletePixelInObject.Click += new System.EventHandler(this.deletePixelInObject_Click);
            // 
            // PixelOutObjectContext
            // 
            this.PixelOutObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deletePixelOutObject});
            this.PixelOutObjectContext.Name = "PixelOutObjectContext";
            this.PixelOutObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deletePixelOutObject
            // 
            this.deletePixelOutObject.Name = "deletePixelOutObject";
            this.deletePixelOutObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deletePixelOutObject.Size = new System.Drawing.Size(131, 22);
            this.deletePixelOutObject.Text = "Delete";
            this.deletePixelOutObject.Click += new System.EventHandler(this.deletePixelOutObject_Click);
            // 
            // RenderStateObjectContext
            // 
            this.RenderStateObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addInputRenderStateObject,
            this.addGlobalDependentRenderStateObject,
            this.toolStripSeparator6,
            this.deleteRenderStateObject});
            this.RenderStateObjectContext.Name = "RenderStateObjectContext";
            this.RenderStateObjectContext.Size = new System.Drawing.Size(195, 76);
            this.RenderStateObjectContext.Opening += new System.ComponentModel.CancelEventHandler(this.RenderStateObjectContext_Opening);
            // 
            // addInputRenderStateObject
            // 
            this.addInputRenderStateObject.Name = "addInputRenderStateObject";
            this.addInputRenderStateObject.Size = new System.Drawing.Size(194, 22);
            this.addInputRenderStateObject.Text = "Add Input";
            this.addInputRenderStateObject.Click += new System.EventHandler(this.addInputRenderStateObject_Click);
            // 
            // addGlobalDependentRenderStateObject
            // 
            this.addGlobalDependentRenderStateObject.Name = "addGlobalDependentRenderStateObject";
            this.addGlobalDependentRenderStateObject.Size = new System.Drawing.Size(194, 22);
            this.addGlobalDependentRenderStateObject.Text = "Add Global Dependent";
            // 
            // toolStripSeparator6
            // 
            this.toolStripSeparator6.Name = "toolStripSeparator6";
            this.toolStripSeparator6.Size = new System.Drawing.Size(191, 6);
            // 
            // deleteRenderStateObject
            // 
            this.deleteRenderStateObject.Name = "deleteRenderStateObject";
            this.deleteRenderStateObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteRenderStateObject.Size = new System.Drawing.Size(194, 22);
            this.deleteRenderStateObject.Text = "Delete";
            this.deleteRenderStateObject.Click += new System.EventHandler(this.deleteRenderStateObject_Click);
            // 
            // VertexShaderOpObjectContext
            // 
            this.VertexShaderOpObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteVertexShaderOpObject});
            this.VertexShaderOpObjectContext.Name = "VertexShaderOpObjectContext";
            this.VertexShaderOpObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deleteVertexShaderOpObject
            // 
            this.deleteVertexShaderOpObject.Name = "deleteVertexShaderOpObject";
            this.deleteVertexShaderOpObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteVertexShaderOpObject.Size = new System.Drawing.Size(131, 22);
            this.deleteVertexShaderOpObject.Text = "Delete";
            this.deleteVertexShaderOpObject.Click += new System.EventHandler(this.deleteVertexShaderOpObject_Click);
            // 
            // PixelShaderOpObjectContext
            // 
            this.PixelShaderOpObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deletePixelShaderOpObject});
            this.PixelShaderOpObjectContext.Name = "PixelShaderOpObjectContext";
            this.PixelShaderOpObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deletePixelShaderOpObject
            // 
            this.deletePixelShaderOpObject.Name = "deletePixelShaderOpObject";
            this.deletePixelShaderOpObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deletePixelShaderOpObject.Size = new System.Drawing.Size(131, 22);
            this.deletePixelShaderOpObject.Text = "Delete";
            this.deletePixelShaderOpObject.Click += new System.EventHandler(this.deletePixelShaderOpObject_Click);
            // 
            // AbstractionObjectContext
            // 
            this.AbstractionObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addFunctionOpAbstractionObject,
            this.addAbstractionAbstractionObject,
            this.addAbstractionInAbstractionObject,
            this.addAbstractionOutAbstractionObject,
            this.toolStripSeparator25,
            this.duplicateAbstractionObject,
            this.toolStripSeparator26,
            this.deleteAbstractionObject});
            this.AbstractionObjectContext.Name = "AbstractionObjectContext";
            this.AbstractionObjectContext.Size = new System.Drawing.Size(184, 148);
            this.AbstractionObjectContext.Opening += new System.ComponentModel.CancelEventHandler(this.AbstractionObjectContext_Opening);
            // 
            // addFunctionOpAbstractionObject
            // 
            this.addFunctionOpAbstractionObject.Name = "addFunctionOpAbstractionObject";
            this.addFunctionOpAbstractionObject.Size = new System.Drawing.Size(183, 22);
            this.addFunctionOpAbstractionObject.Text = "Add Function Op";
            // 
            // addAbstractionAbstractionObject
            // 
            this.addAbstractionAbstractionObject.Name = "addAbstractionAbstractionObject";
            this.addAbstractionAbstractionObject.Size = new System.Drawing.Size(183, 22);
            this.addAbstractionAbstractionObject.Text = "Add Abstraction";
            // 
            // addAbstractionInAbstractionObject
            // 
            this.addAbstractionInAbstractionObject.Name = "addAbstractionInAbstractionObject";
            this.addAbstractionInAbstractionObject.Size = new System.Drawing.Size(183, 22);
            this.addAbstractionInAbstractionObject.Text = "Add Abstraction In";
            this.addAbstractionInAbstractionObject.Click += new System.EventHandler(this.addAbstractionInAbstractionObject_Click);
            // 
            // addAbstractionOutAbstractionObject
            // 
            this.addAbstractionOutAbstractionObject.Name = "addAbstractionOutAbstractionObject";
            this.addAbstractionOutAbstractionObject.Size = new System.Drawing.Size(183, 22);
            this.addAbstractionOutAbstractionObject.Text = "Add Abstraction Out";
            this.addAbstractionOutAbstractionObject.Click += new System.EventHandler(this.addAbstractionOutAbstractionObject_Click);
            // 
            // toolStripSeparator25
            // 
            this.toolStripSeparator25.Name = "toolStripSeparator25";
            this.toolStripSeparator25.Size = new System.Drawing.Size(180, 6);
            // 
            // duplicateAbstractionObject
            // 
            this.duplicateAbstractionObject.Name = "duplicateAbstractionObject";
            this.duplicateAbstractionObject.Size = new System.Drawing.Size(183, 22);
            this.duplicateAbstractionObject.Text = "Duplicate";
            this.duplicateAbstractionObject.Click += new System.EventHandler(this.duplicateAbstractionObject_Click);
            // 
            // toolStripSeparator26
            // 
            this.toolStripSeparator26.Name = "toolStripSeparator26";
            this.toolStripSeparator26.Size = new System.Drawing.Size(180, 6);
            // 
            // deleteAbstractionObject
            // 
            this.deleteAbstractionObject.Name = "deleteAbstractionObject";
            this.deleteAbstractionObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteAbstractionObject.Size = new System.Drawing.Size(183, 22);
            this.deleteAbstractionObject.Text = "Delete";
            this.deleteAbstractionObject.Click += new System.EventHandler(this.deleteAbstractionObject_Click);
            // 
            // AbstractionInObjectContext
            // 
            this.AbstractionInObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteAbstractionInObject});
            this.AbstractionInObjectContext.Name = "AbstractionInObjectContext";
            this.AbstractionInObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deleteAbstractionInObject
            // 
            this.deleteAbstractionInObject.Name = "deleteAbstractionInObject";
            this.deleteAbstractionInObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteAbstractionInObject.Size = new System.Drawing.Size(131, 22);
            this.deleteAbstractionInObject.Text = "Delete";
            this.deleteAbstractionInObject.Click += new System.EventHandler(this.deleteAbstractionInObject_Click);
            // 
            // AbstractionOutObjectContext
            // 
            this.AbstractionOutObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteAbstractionOutObject});
            this.AbstractionOutObjectContext.Name = "AbstractionOutObjectContext";
            this.AbstractionOutObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deleteAbstractionOutObject
            // 
            this.deleteAbstractionOutObject.Name = "deleteAbstractionOutObject";
            this.deleteAbstractionOutObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteAbstractionOutObject.Size = new System.Drawing.Size(131, 22);
            this.deleteAbstractionOutObject.Text = "Delete";
            this.deleteAbstractionOutObject.Click += new System.EventHandler(this.deleteAbstractionOutObject_Click);
            // 
            // AbstractionProxyObjectContext
            // 
            this.AbstractionProxyObjectContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteAbstractionProxyObject});
            this.AbstractionProxyObjectContext.Name = "AbstractionProxyObjectContext";
            this.AbstractionProxyObjectContext.Size = new System.Drawing.Size(132, 26);
            // 
            // deleteAbstractionProxyObject
            // 
            this.deleteAbstractionProxyObject.Name = "deleteAbstractionProxyObject";
            this.deleteAbstractionProxyObject.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.deleteAbstractionProxyObject.Size = new System.Drawing.Size(131, 22);
            this.deleteAbstractionProxyObject.Text = "Delete";
            this.deleteAbstractionProxyObject.Click += new System.EventHandler(this.deleteAbstractionProxyObject_Click);
            // 
            // duplicateMaterialTemplateObject
            // 
            this.duplicateMaterialTemplateObject.Name = "duplicateMaterialTemplateObject";
            this.duplicateMaterialTemplateObject.Size = new System.Drawing.Size(155, 22);
            this.duplicateMaterialTemplateObject.Text = "Duplicate";
            this.duplicateMaterialTemplateObject.Click += new System.EventHandler(this.duplicateMaterialTemplateObject_Click);
            // 
            // toolStripSeparator29
            // 
            this.toolStripSeparator29.Name = "toolStripSeparator29";
            this.toolStripSeparator29.Size = new System.Drawing.Size(152, 6);
            // 
            // ExplorerWin
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 282);
            this.Controls.Add(this.explorerTreeView);
            this.DockAreas = ((WeifenLuo.WinFormsUI.Docking.DockAreas)(((((WeifenLuo.WinFormsUI.Docking.DockAreas.Float | WeifenLuo.WinFormsUI.Docking.DockAreas.DockLeft) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockRight) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockTop) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockBottom)));
            this.HideOnClose = true;
            this.Name = "ExplorerWin";
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockLeft;
            this.TabText = "Explorer";
            this.Text = "Explorer";
            this.VertexShadersContext.ResumeLayout(false);
            this.PixelShadersContext.ResumeLayout(false);
            this.LocalsContext.ResumeLayout(false);
            this.GlobalsContext.ResumeLayout(false);
            this.FunctionsContext.ResumeLayout(false);
            this.AbstractionsContext.ResumeLayout(false);
            this.MaterialTemplatesContext.ResumeLayout(false);
            this.VertexShaderObjectContext.ResumeLayout(false);
            this.PixelShaderObjectContext.ResumeLayout(false);
            this.TechniqueObjectContext.ResumeLayout(false);
            this.PassObjectContext.ResumeLayout(false);
            this.MaterialTemplateObjectContext.ResumeLayout(false);
            this.GlobalObjectContext.ResumeLayout(false);
            this.InputObjectContext.ResumeLayout(false);
            this.GlobalDescriptionObjectContext.ResumeLayout(false);
            this.LocalObjectContext.ResumeLayout(false);
            this.LocalProxyObjectContext.ResumeLayout(false);
            this.GlobalProxyObjectContext.ResumeLayout(false);
            this.FunctionObjectContext.ResumeLayout(false);
            this.FunctionOpObjectContext.ResumeLayout(false);
            this.VertexInObjectContext.ResumeLayout(false);
            this.VertexOutObjectContext.ResumeLayout(false);
            this.PixelInObjectContext.ResumeLayout(false);
            this.PixelOutObjectContext.ResumeLayout(false);
            this.RenderStateObjectContext.ResumeLayout(false);
            this.VertexShaderOpObjectContext.ResumeLayout(false);
            this.PixelShaderOpObjectContext.ResumeLayout(false);
            this.AbstractionObjectContext.ResumeLayout(false);
            this.AbstractionInObjectContext.ResumeLayout(false);
            this.AbstractionOutObjectContext.ResumeLayout(false);
            this.AbstractionProxyObjectContext.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TreeView explorerTreeView;
        private System.Windows.Forms.ContextMenuStrip VertexShadersContext;
        private System.Windows.Forms.ContextMenuStrip PixelShadersContext;
        private System.Windows.Forms.ContextMenuStrip GlobalsContext;
        private System.Windows.Forms.ContextMenuStrip FunctionsContext;
        private System.Windows.Forms.ContextMenuStrip MaterialTemplatesContext;
        private System.Windows.Forms.ContextMenuStrip VertexShaderObjectContext;
        private System.Windows.Forms.ContextMenuStrip PixelShaderObjectContext;
        private System.Windows.Forms.ContextMenuStrip TechniqueObjectContext;
        private System.Windows.Forms.ContextMenuStrip PassObjectContext;
        private System.Windows.Forms.ToolStripMenuItem addVertexShader_VertexShadersContext;
        private System.Windows.Forms.ToolStripMenuItem addPixelShader_PixelShadersContext;
        private System.Windows.Forms.ToolStripMenuItem addGlobal_GlobalsContext;
        private System.Windows.Forms.ToolStripMenuItem addFunction_FunctionsContext;
        private System.Windows.Forms.ToolStripMenuItem addMaterialTemplate_MaterialTemplatesContext;
        private System.Windows.Forms.ContextMenuStrip MaterialTemplateObjectContext;
        private System.Windows.Forms.ToolStripMenuItem setVertexShaderPassObject;
        private System.Windows.Forms.ToolStripMenuItem setPixelShaderPassObject;
        private System.Windows.Forms.ToolStripMenuItem addRenderStatePassObject;
        private System.Windows.Forms.ToolStripMenuItem addGlobalVertexShaderObject;
        private System.Windows.Forms.ToolStripMenuItem addVertexInVertexShaderObject;
        private System.Windows.Forms.ToolStripMenuItem addVertexOutVertexShaderObject;
        private System.Windows.Forms.ToolStripMenuItem addFunctionOpVertexShaderObject;
        private System.Windows.Forms.ToolStripMenuItem addGlobalPixelShaderObject;
        private System.Windows.Forms.ToolStripMenuItem addPixelInPixelShaderObject;
        private System.Windows.Forms.ToolStripMenuItem addPixelOutPixelShaderObject;
        private System.Windows.Forms.ToolStripMenuItem addFunctionOpPixelShaderObject;
        private System.Windows.Forms.ToolStripMenuItem addPassTechniqueObject;
        private System.Windows.Forms.ToolStripMenuItem addGlobalMaterialTemplateObject;
        private System.Windows.Forms.ToolStripMenuItem addTechniqueMaterialTemplateObject;
        private System.Windows.Forms.ContextMenuStrip GlobalObjectContext;
        private System.Windows.Forms.ToolStripMenuItem addInputGlobalObject;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem deleteGlobalObject;
        private System.Windows.Forms.ContextMenuStrip InputObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deleteInputObject;
        private System.Windows.Forms.ContextMenuStrip GlobalDescriptionObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deleteGlobalDescriptionObject;
        private System.Windows.Forms.ToolStripMenuItem addDescriptionGlobalObject;
        private System.Windows.Forms.ToolStripMenuItem addLocalGlobalObject;
        private System.Windows.Forms.ContextMenuStrip LocalObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deleteLocalObject;
        private System.Windows.Forms.ContextMenuStrip LocalsContext;
        private System.Windows.Forms.ToolStripMenuItem addLocal_LocalsContext;
        private System.Windows.Forms.ContextMenuStrip LocalProxyObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deleteLocalProxyObject;
        private System.Windows.Forms.ToolStripMenuItem addGlobalDependentGlobalObject;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ContextMenuStrip GlobalProxyObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deleteGlobalProxyObject;
        private System.Windows.Forms.ContextMenuStrip FunctionObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deleteFunctionObject;
        private System.Windows.Forms.ToolStripMenuItem addInputFunctionObject;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem deleteVertexShaderObject;
        private System.Windows.Forms.ContextMenuStrip FunctionOpObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deleteFunctionOpObject;
        private System.Windows.Forms.ContextMenuStrip VertexInObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deleteVertexInObject;
        private System.Windows.Forms.ContextMenuStrip VertexOutObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deleteVertexOutObject;
        private System.Windows.Forms.ContextMenuStrip PixelInObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deletePixelInObject;
        private System.Windows.Forms.ContextMenuStrip PixelOutObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deletePixelOutObject;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
        private System.Windows.Forms.ToolStripMenuItem deletePixelShaderObject;
        private System.Windows.Forms.ContextMenuStrip RenderStateObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deleteRenderStateObject;
        private System.Windows.Forms.ToolStripMenuItem addInputRenderStateObject;
        private System.Windows.Forms.ToolStripMenuItem addGlobalDependentRenderStateObject;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator6;
        private System.Windows.Forms.ToolStripMenuItem deleteMaterialTemplateObject;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator7;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator8;
        private System.Windows.Forms.ToolStripMenuItem deleteTechniqueObject;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator9;
        private System.Windows.Forms.ToolStripMenuItem deletePassObject;
        private System.Windows.Forms.ContextMenuStrip VertexShaderOpObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deleteVertexShaderOpObject;
        private System.Windows.Forms.ContextMenuStrip PixelShaderOpObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deletePixelShaderOpObject;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator10;
        private System.Windows.Forms.ToolStripMenuItem verifyMaterialTemplateObject;
        private System.Windows.Forms.ToolStripMenuItem addFolder_LocalsContext;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator11;
        private System.Windows.Forms.ToolStripMenuItem deleteFolder_LocalsContext;
        private System.Windows.Forms.ToolStripMenuItem addFolder_GlobalsContext;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator12;
        private System.Windows.Forms.ToolStripMenuItem deleteFolder_GlobalsContext;
        private System.Windows.Forms.ToolStripMenuItem addFolder_FunctionsContext;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator13;
        private System.Windows.Forms.ToolStripMenuItem deleteFolder_FunctionsContext;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator15;
        private System.Windows.Forms.ToolStripMenuItem addFolder_VertexShadersContext;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator14;
        private System.Windows.Forms.ToolStripMenuItem deleteFolder_VertexShadersContext;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator16;
        private System.Windows.Forms.ToolStripMenuItem addFolder_PixelShadersContext;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator17;
        private System.Windows.Forms.ToolStripMenuItem deleteFolder_PixelShadersContext;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator18;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator19;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator20;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator21;
        private System.Windows.Forms.ToolStripMenuItem addFolder_MaterialTemplatesContext;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator22;
        private System.Windows.Forms.ToolStripMenuItem deleteFolder_MaterialTemplatesContext;
        private System.Windows.Forms.ContextMenuStrip AbstractionsContext;
        private System.Windows.Forms.ToolStripMenuItem addAbstraction_AbstractionsContext;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator24;
        private System.Windows.Forms.ToolStripMenuItem addFolder_AbstractionsContext;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator23;
        private System.Windows.Forms.ToolStripMenuItem deleteFolder_AbstractionsContext;
        private System.Windows.Forms.ContextMenuStrip AbstractionObjectContext;
        private System.Windows.Forms.ToolStripMenuItem addFunctionOpAbstractionObject;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator25;
        private System.Windows.Forms.ToolStripMenuItem deleteAbstractionObject;
        private System.Windows.Forms.ToolStripMenuItem addAbstractionInAbstractionObject;
        private System.Windows.Forms.ToolStripMenuItem addAbstractionOutAbstractionObject;
        private System.Windows.Forms.ContextMenuStrip AbstractionInObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deleteAbstractionInObject;
        private System.Windows.Forms.ContextMenuStrip AbstractionOutObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deleteAbstractionOutObject;
        private System.Windows.Forms.ContextMenuStrip AbstractionProxyObjectContext;
        private System.Windows.Forms.ToolStripMenuItem deleteAbstractionProxyObject;
        private System.Windows.Forms.ToolStripMenuItem addAbstractionVertexShaderObject;
        private System.Windows.Forms.ToolStripMenuItem addAbstractionPixelShaderObject;
        private System.Windows.Forms.ToolStripMenuItem addAbstractionAbstractionObject;
        private System.Windows.Forms.ToolStripMenuItem duplicateAbstractionObject;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator26;
        private System.Windows.Forms.ToolStripMenuItem duplicateVertexShaderObject;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator27;
        private System.Windows.Forms.ToolStripMenuItem duplicatePixelShaderObject;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator28;
        private System.Windows.Forms.ToolStripMenuItem addLocalFunctionObject;
        private System.Windows.Forms.ToolStripMenuItem exportMaterialTemplateObject;
        private System.Windows.Forms.ToolStripMenuItem duplicateMaterialTemplateObject;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator29;

    }
}