using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Common.PluginFramework
{
    public abstract class PluginOwner : IPluginOwner
    {
        protected Dictionary<string, PluginEventHandler> m_PluginEventTable;
        protected Dictionary<string, IPlugin> m_PluginTable;

        public PluginOwner()
        {
            m_PluginEventTable = new Dictionary<string, PluginEventHandler>();
            m_PluginTable = new Dictionary<string, IPlugin>();
        }

        public void AddPluginEventHandler(string eventType, PluginEventHandler handler)
        {
            if (!m_PluginEventTable.ContainsKey(eventType))
                m_PluginEventTable.Add(eventType, handler);

            m_PluginEventTable[eventType] = ((PluginEventHandler)m_PluginEventTable[eventType]) + handler;
        }

        public void RemovePluginEventHandler(string eventType, PluginEventHandler handler)
        {
            if (!m_PluginEventTable.ContainsKey(eventType))
                return;

            m_PluginEventTable[eventType] = ((PluginEventHandler)m_PluginEventTable[eventType]) - handler;
        }

        public void RaisePluginEvent(string eventType, object sender, PluginEventArgs args)
        {
            PluginEventHandler handler = (PluginEventHandler) m_PluginEventTable[eventType];
            if (handler != null)
            {
                handler(sender, args);
            }
        }

        public IPlugin GetPlugin(string pluginName)
        {
            return m_PluginTable[pluginName];
        }
        
        public void LoadPlugins(string folderName)
        {
            List<FileInfo> fileInfos = Utility.GetPluginFileList(folderName);
            foreach (FileInfo fileInfo in fileInfos)
            {
                List<string> pluginNames = Utility.GetPlugins(fileInfo.FullName);
                foreach(string pluginName in pluginNames)
                {
                    IPlugin plugin = Utility.GetPlugin(fileInfo.FullName, pluginName);
                    plugin.InitializePlugin(this);

                    m_PluginTable[pluginName] = plugin;
                }
            }      
        }
    }
}
