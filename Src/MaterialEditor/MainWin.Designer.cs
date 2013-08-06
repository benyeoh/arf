namespace MaterialEditor
{
    partial class MainWin
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
            if (disposing)
            {
                m_Callback.Dispose();
                m_Callback = null;

                m_Explorer.Dispose();
                m_Explorer = null;
                m_Properties.Dispose();
                m_Properties = null;
                m_Output.Dispose();
                m_Output = null;
                m_Preferences.Dispose();
                m_Preferences = null;
            }

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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainWin));
            this.mainWindowMenuStrip = new System.Windows.Forms.MenuStrip();
            this.fileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.recentProjectsFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.exitFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viewMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viewExplorerMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viewPropertiesMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viewOutputMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.compileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.verifyAllCompileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exportAllCompileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.optionsFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.preferencesOptionMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.mainWindowToolStrip = new System.Windows.Forms.ToolStrip();
            this.toolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.mainWindowDockPanel = new WeifenLuo.WinFormsUI.Docking.DockPanel();
            this.viewPluginsMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.mainWindowMenuStrip.SuspendLayout();
            this.mainWindowToolStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // mainWindowMenuStrip
            // 
            this.mainWindowMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileMenuItem,
            this.viewMenuItem,
            this.compileMenuItem,
            this.optionsFileMenuItem,
            this.helpToolStripMenuItem});
            this.mainWindowMenuStrip.Location = new System.Drawing.Point(0, 0);
            this.mainWindowMenuStrip.Name = "mainWindowMenuStrip";
            this.mainWindowMenuStrip.Size = new System.Drawing.Size(1008, 24);
            this.mainWindowMenuStrip.TabIndex = 2;
            this.mainWindowMenuStrip.Text = "mainWindowMenuStrip";
            // 
            // fileMenuItem
            // 
            this.fileMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openFileMenuItem,
            this.saveFileMenuItem,
            this.saveAsFileMenuItem,
            this.toolStripSeparator1,
            this.recentProjectsFileMenuItem,
            this.toolStripSeparator2,
            this.exitFileMenuItem});
            this.fileMenuItem.Name = "fileMenuItem";
            this.fileMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileMenuItem.Text = "File";
            this.fileMenuItem.DropDownOpening += new System.EventHandler(this.fileMenuItem_DropDownOpening);
            // 
            // openFileMenuItem
            // 
            this.openFileMenuItem.Name = "openFileMenuItem";
            this.openFileMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.openFileMenuItem.Size = new System.Drawing.Size(155, 22);
            this.openFileMenuItem.Text = "Open";
            this.openFileMenuItem.Click += new System.EventHandler(this.loadFileMenuItem_Click);
            // 
            // saveFileMenuItem
            // 
            this.saveFileMenuItem.Name = "saveFileMenuItem";
            this.saveFileMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.saveFileMenuItem.Size = new System.Drawing.Size(155, 22);
            this.saveFileMenuItem.Text = "Save";
            this.saveFileMenuItem.Click += new System.EventHandler(this.saveFileMenuItem_Click);
            // 
            // saveAsFileMenuItem
            // 
            this.saveAsFileMenuItem.Name = "saveAsFileMenuItem";
            this.saveAsFileMenuItem.Size = new System.Drawing.Size(155, 22);
            this.saveAsFileMenuItem.Text = "Save As";
            this.saveAsFileMenuItem.Click += new System.EventHandler(this.saveAsMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(152, 6);
            // 
            // recentProjectsFileMenuItem
            // 
            this.recentProjectsFileMenuItem.Name = "recentProjectsFileMenuItem";
            this.recentProjectsFileMenuItem.Size = new System.Drawing.Size(155, 22);
            this.recentProjectsFileMenuItem.Text = "Recent Projects";
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(152, 6);
            // 
            // exitFileMenuItem
            // 
            this.exitFileMenuItem.Name = "exitFileMenuItem";
            this.exitFileMenuItem.Size = new System.Drawing.Size(155, 22);
            this.exitFileMenuItem.Text = "Exit";
            this.exitFileMenuItem.Click += new System.EventHandler(this.exitFileMenuItem_Click);
            // 
            // viewMenuItem
            // 
            this.viewMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.viewExplorerMenuItem,
            this.viewPropertiesMenuItem,
            this.viewOutputMenuItem,
            this.viewPluginsMenuItem});
            this.viewMenuItem.Name = "viewMenuItem";
            this.viewMenuItem.Size = new System.Drawing.Size(44, 20);
            this.viewMenuItem.Text = "View";
            // 
            // viewExplorerMenuItem
            // 
            this.viewExplorerMenuItem.Name = "viewExplorerMenuItem";
            this.viewExplorerMenuItem.Size = new System.Drawing.Size(152, 22);
            this.viewExplorerMenuItem.Text = "Explorer";
            this.viewExplorerMenuItem.Click += new System.EventHandler(this.viewExplorerMenuItem_Click);
            // 
            // viewPropertiesMenuItem
            // 
            this.viewPropertiesMenuItem.Name = "viewPropertiesMenuItem";
            this.viewPropertiesMenuItem.Size = new System.Drawing.Size(152, 22);
            this.viewPropertiesMenuItem.Text = "Properties";
            this.viewPropertiesMenuItem.Click += new System.EventHandler(this.viewPropertiesMenuItem_Click);
            // 
            // viewOutputMenuItem
            // 
            this.viewOutputMenuItem.Name = "viewOutputMenuItem";
            this.viewOutputMenuItem.Size = new System.Drawing.Size(152, 22);
            this.viewOutputMenuItem.Text = "Output";
            this.viewOutputMenuItem.Click += new System.EventHandler(this.viewOutputMenuItem_Click);
            // 
            // compileMenuItem
            // 
            this.compileMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.verifyAllCompileMenuItem,
            this.exportAllCompileMenuItem});
            this.compileMenuItem.Name = "compileMenuItem";
            this.compileMenuItem.Size = new System.Drawing.Size(64, 20);
            this.compileMenuItem.Text = "Compile";
            // 
            // verifyAllCompileMenuItem
            // 
            this.verifyAllCompileMenuItem.Name = "verifyAllCompileMenuItem";
            this.verifyAllCompileMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F7;
            this.verifyAllCompileMenuItem.Size = new System.Drawing.Size(251, 22);
            this.verifyAllCompileMenuItem.Text = "Verify All";
            this.verifyAllCompileMenuItem.Click += new System.EventHandler(this.verifyAllCompileMenuItem_Click);
            // 
            // exportAllCompileMenuItem
            // 
            this.exportAllCompileMenuItem.Name = "exportAllCompileMenuItem";
            this.exportAllCompileMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Alt)
                        | System.Windows.Forms.Keys.F7)));
            this.exportAllCompileMenuItem.Size = new System.Drawing.Size(251, 22);
            this.exportAllCompileMenuItem.Text = "Verify And Export All";
            this.exportAllCompileMenuItem.Click += new System.EventHandler(this.exportAllCompileMenuItem_Click);
            // 
            // optionsFileMenuItem
            // 
            this.optionsFileMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.preferencesOptionMenuItem});
            this.optionsFileMenuItem.Name = "optionsFileMenuItem";
            this.optionsFileMenuItem.Size = new System.Drawing.Size(61, 20);
            this.optionsFileMenuItem.Text = "Options";
            // 
            // preferencesOptionMenuItem
            // 
            this.preferencesOptionMenuItem.Name = "preferencesOptionMenuItem";
            this.preferencesOptionMenuItem.Size = new System.Drawing.Size(135, 22);
            this.preferencesOptionMenuItem.Text = "Preferences";
            this.preferencesOptionMenuItem.Click += new System.EventHandler(this.preferencesOptionMenuItem_Click);
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.helpToolStripMenuItem.Text = "Help";
            // 
            // mainWindowToolStrip
            // 
            this.mainWindowToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton1});
            this.mainWindowToolStrip.Location = new System.Drawing.Point(0, 24);
            this.mainWindowToolStrip.Name = "mainWindowToolStrip";
            this.mainWindowToolStrip.Size = new System.Drawing.Size(1008, 25);
            this.mainWindowToolStrip.TabIndex = 3;
            this.mainWindowToolStrip.Text = "toolStrip1";
            // 
            // toolStripButton1
            // 
            this.toolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton1.Image")));
            this.toolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton1.Name = "toolStripButton1";
            this.toolStripButton1.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton1.Text = "toolStripButton1";
            // 
            // mainWindowDockPanel
            // 
            this.mainWindowDockPanel.ActiveAutoHideContent = null;
            this.mainWindowDockPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mainWindowDockPanel.DocumentStyle = WeifenLuo.WinFormsUI.Docking.DocumentStyle.DockingWindow;
            this.mainWindowDockPanel.Location = new System.Drawing.Point(0, 49);
            this.mainWindowDockPanel.Name = "mainWindowDockPanel";
            this.mainWindowDockPanel.Size = new System.Drawing.Size(1008, 683);
            this.mainWindowDockPanel.TabIndex = 4;
            // 
            // viewPluginsMenuItem
            // 
            this.viewPluginsMenuItem.Name = "viewPluginsMenuItem";
            this.viewPluginsMenuItem.Size = new System.Drawing.Size(152, 22);
            this.viewPluginsMenuItem.Text = "Plugins";
            this.viewPluginsMenuItem.Click += new System.EventHandler(this.viewPluginsMenuItem_Click);
            // 
            // MainWin
            // 
            this.AllowDrop = true;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1008, 732);
            this.Controls.Add(this.mainWindowDockPanel);
            this.Controls.Add(this.mainWindowToolStrip);
            this.Controls.Add(this.mainWindowMenuStrip);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.mainWindowMenuStrip;
            this.Name = "MainWin";
            this.Text = "Minotaur";
            this.Load += new System.EventHandler(this.MainWindow_Load);
            this.DragDrop += new System.Windows.Forms.DragEventHandler(this.MainWin_DragDrop);
            this.DragEnter += new System.Windows.Forms.DragEventHandler(this.MainWin_DragEnter);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainWindow_FormClosing);
            this.mainWindowMenuStrip.ResumeLayout(false);
            this.mainWindowMenuStrip.PerformLayout();
            this.mainWindowToolStrip.ResumeLayout(false);
            this.mainWindowToolStrip.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip mainWindowMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem fileMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAsFileMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openFileMenuItem;
        private System.Windows.Forms.ToolStrip mainWindowToolStrip;
        private System.Windows.Forms.ToolStripButton toolStripButton1;
        private WeifenLuo.WinFormsUI.Docking.DockPanel mainWindowDockPanel;
        private System.Windows.Forms.ToolStripMenuItem viewMenuItem;
        private System.Windows.Forms.ToolStripMenuItem viewExplorerMenuItem;
        private System.Windows.Forms.ToolStripMenuItem viewPropertiesMenuItem;
        private System.Windows.Forms.ToolStripMenuItem viewOutputMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveFileMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem recentProjectsFileMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem exitFileMenuItem;
        private System.Windows.Forms.ToolStripMenuItem compileMenuItem;
        private System.Windows.Forms.ToolStripMenuItem verifyAllCompileMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exportAllCompileMenuItem;
        private System.Windows.Forms.ToolStripMenuItem optionsFileMenuItem;
        private System.Windows.Forms.ToolStripMenuItem preferencesOptionMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem viewPluginsMenuItem;



    }
}

