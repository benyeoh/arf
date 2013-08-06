namespace MaterialEditor
{
    partial class PluginManagerWin
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
            this.pluginDataGridView = new System.Windows.Forms.DataGridView();
            this.PluginName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.PluginDescription = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.PluginFile = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.pluginDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // pluginDataGridView
            // 
            this.pluginDataGridView.AllowUserToAddRows = false;
            this.pluginDataGridView.AllowUserToDeleteRows = false;
            this.pluginDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.AllCellsExceptHeader;
            this.pluginDataGridView.BackgroundColor = System.Drawing.SystemColors.Window;
            this.pluginDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.pluginDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.PluginName,
            this.PluginDescription,
            this.PluginFile});
            this.pluginDataGridView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pluginDataGridView.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.pluginDataGridView.GridColor = System.Drawing.SystemColors.Window;
            this.pluginDataGridView.Location = new System.Drawing.Point(0, 0);
            this.pluginDataGridView.Name = "pluginDataGridView";
            this.pluginDataGridView.ReadOnly = true;
            this.pluginDataGridView.RowHeadersVisible = false;
            this.pluginDataGridView.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.pluginDataGridView.ShowEditingIcon = false;
            this.pluginDataGridView.ShowRowErrors = false;
            this.pluginDataGridView.Size = new System.Drawing.Size(603, 350);
            this.pluginDataGridView.TabIndex = 0;
            // 
            // PluginName
            // 
            this.PluginName.HeaderText = "Name";
            this.PluginName.Name = "PluginName";
            this.PluginName.ReadOnly = true;
            this.PluginName.Width = 5;
            // 
            // PluginDescription
            // 
            this.PluginDescription.HeaderText = "Description";
            this.PluginDescription.Name = "PluginDescription";
            this.PluginDescription.ReadOnly = true;
            this.PluginDescription.Width = 5;
            // 
            // PluginFile
            // 
            this.PluginFile.HeaderText = "File";
            this.PluginFile.Name = "PluginFile";
            this.PluginFile.ReadOnly = true;
            this.PluginFile.Width = 5;
            // 
            // PluginManagerWin
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.ClientSize = new System.Drawing.Size(603, 350);
            this.Controls.Add(this.pluginDataGridView);
            this.HideOnClose = true;
            this.Name = "PluginManagerWin";
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.Float;
            this.TabText = "PluginManagerWin";
            this.Text = "PluginManagerWin";
            ((System.ComponentModel.ISupportInitialize)(this.pluginDataGridView)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataGridView pluginDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn PluginName;
        private System.Windows.Forms.DataGridViewTextBoxColumn PluginDescription;
        private System.Windows.Forms.DataGridViewTextBoxColumn PluginFile;
    }
}