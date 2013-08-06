using System;
using System.Collections.Generic;
using System.Text;

namespace Common.PluginFramework
{
    public delegate void PluginEventHandler(object sender, PluginEventArgs args);

    public class PluginEventArgs : EventArgs
    {
        public PluginEventArgs()
        {
        }
    }

    public class GetObjectEventArgs : PluginEventArgs
    {
        public const string EVENT_TYPE = "GetObject";
        
        private string m_ObjectName = null;
        private object m_Result = null;

        public GetObjectEventArgs(string name)
        {
            m_ObjectName = name;
        }

        public string ObjectName
        {
            get { return m_ObjectName;  }
        }

        public object Result
        {
            set { m_Result = value; }
            get { return m_Result; }
        }
    }
}
