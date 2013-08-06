using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Common.Plugin;
using System.Reflection;
using WeifenLuo.WinFormsUI.Docking;
using System.IO;

namespace MaterialEditor
{
    public partial class PluginManagerWin : DockContent
    {
        private MainWin m_MainWin;

        private Dictionary<string, IPlugin> m_PluginTable;

        public PluginManagerWin(MainWin mainWin)
        {
            InitializeComponent();

            m_MainWin = mainWin;
            m_PluginTable = new Dictionary<string, IPlugin>();
        }

        public void LoadAllPlugins(string folderName)
        {
            List<FileInfo> fileInfos = Utility.GetPluginFileList(folderName);
            foreach (FileInfo fileInfo in fileInfos)
            {
                List<string> pluginNames = Utility.GetPlugins(fileInfo.FullName);
                foreach (string pluginName in pluginNames)
                {
                    IPlugin plugin = Utility.GetPlugin(fileInfo.FullName, pluginName);
                    plugin.InitializePlugin(EventManager.GlobalEventManager);

                    m_PluginTable[pluginName] = plugin;
                }
            }

            foreach (KeyValuePair<string, IPlugin> entry in m_PluginTable)
            {
                object[] rowData = new object[pluginDataGridView.ColumnCount];
                rowData[0] = entry.Value.Name;
                rowData[1] = entry.Value.Description;
                rowData[2] = entry.Value.GetType().Module.Name;

                pluginDataGridView.Rows.Add(rowData);                
            }
        }
    }
}