using System;
using System.Collections.Generic;
using System.Text;

namespace Common.Event
{
    public class GetObjectEventArgs : EventArgs
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
            get { return m_ObjectName; }
        }

        public object Result
        {
            set { m_Result = value; }
            get { return m_Result; }
        }
    }
}
