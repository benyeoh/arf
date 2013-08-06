namespace MaterialEditor
{
    partial class DrawWin
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

            if(EventManager.GlobalEventManager != null)
                EventManager.GlobalEventManager.RemoveEventHandler(RefreshEventArgs.EVENT_TYPE, m_Main_OnRefresh);

            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.panel = new System.Windows.Forms.Panel();
            this.splitContainer = new System.Windows.Forms.SplitContainer();
            this.splitContainerTop = new System.Windows.Forms.SplitContainer();
            this.zoomControl = new MaterialEditor.ZoomControl();
            this.splitContainer.Panel1.SuspendLayout();
            this.splitContainer.Panel2.SuspendLayout();
            this.splitContainer.SuspendLayout();
            this.splitContainerTop.Panel2.SuspendLayout();
            this.splitContainerTop.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel
            // 
            this.panel.BackColor = System.Drawing.Color.White;
            this.panel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel.Location = new System.Drawing.Point(0, 0);
            this.panel.Name = "panel";
            this.panel.Size = new System.Drawing.Size(1083, 557);
            this.panel.TabIndex = 1;
            // 
            // splitContainer
            // 
            this.splitContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer.IsSplitterFixed = true;
            this.splitContainer.Location = new System.Drawing.Point(0, 0);
            this.splitContainer.Margin = new System.Windows.Forms.Padding(0);
            this.splitContainer.Name = "splitContainer";
            this.splitContainer.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer.Panel1
            // 
            this.splitContainer.Panel1.Controls.Add(this.splitContainerTop);
            // 
            // splitContainer.Panel2
            // 
            this.splitContainer.Panel2.BackColor = System.Drawing.SystemColors.Control;
            this.splitContainer.Panel2.Controls.Add(this.panel);
            this.splitContainer.Size = new System.Drawing.Size(1083, 583);
            this.splitContainer.SplitterDistance = 25;
            this.splitContainer.SplitterWidth = 1;
            this.splitContainer.TabIndex = 2;
            // 
            // splitContainerTop
            // 
            this.splitContainerTop.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainerTop.IsSplitterFixed = true;
            this.splitContainerTop.Location = new System.Drawing.Point(0, 0);
            this.splitContainerTop.Name = "splitContainerTop";
            this.splitContainerTop.Panel1Collapsed = true;
            // 
            // splitContainerTop.Panel2
            // 
            this.splitContainerTop.Panel2.Controls.Add(this.zoomControl);
            this.splitContainerTop.Size = new System.Drawing.Size(1083, 25);
            this.splitContainerTop.SplitterDistance = 173;
            this.splitContainerTop.SplitterWidth = 3;
            this.splitContainerTop.TabIndex = 0;
            // 
            // zoomControl
            // 
            this.zoomControl.BackColor = System.Drawing.SystemColors.Control;
            this.zoomControl.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.zoomControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.zoomControl.Location = new System.Drawing.Point(0, 0);
            this.zoomControl.Name = "zoomControl";
            this.zoomControl.Size = new System.Drawing.Size(1083, 25);
            this.zoomControl.TabIndex = 0;
            this.zoomControl.Zoom = 1F;
            // 
            // DrawWin
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1083, 583);
            this.Controls.Add(this.splitContainer);
            this.DockAreas = ((WeifenLuo.WinFormsUI.Docking.DockAreas)((WeifenLuo.WinFormsUI.Docking.DockAreas.Float | WeifenLuo.WinFormsUI.Docking.DockAreas.Document)));
            this.Name = "DrawWin";
            this.TabText = "DrawWin";
            this.Text = "DrawWin";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.splitContainer.Panel1.ResumeLayout(false);
            this.splitContainer.Panel2.ResumeLayout(false);
            this.splitContainer.ResumeLayout(false);
            this.splitContainerTop.Panel2.ResumeLayout(false);
            this.splitContainerTop.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private ZoomControl zoomControl;
        private System.Windows.Forms.Panel panel;
        private System.Windows.Forms.SplitContainer splitContainer;
        private System.Windows.Forms.SplitContainer splitContainerTop;

    }
}