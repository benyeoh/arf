using System;
using System.Collections.Generic;
using System.Text;
using Common.Event;

namespace MaterialEditor
{
    public class ResolveSemanticEventArgs : EventArgs
    {
        public const string EVENT_TYPE = "ResolveSemantic";

        public const int UNRESOLVED_SEMANTIC = -1;

        private int m_Result = UNRESOLVED_SEMANTIC;
        private string m_SemanticName;

        public ResolveSemanticEventArgs(string semanticName)
        {
            m_SemanticName = semanticName;
        }

        public string SemanticName
        {
            get { return m_SemanticName; }
        }

        public int Result
        {
            get { return m_Result; }
            set { m_Result = value; }
        }
    }

    public class GetAllSemanticNamesEventArgs : EventArgs
    {
        public const string EVENT_TYPE = "GetAllSemanticNames";

        private List<string> m_Result;

        public GetAllSemanticNamesEventArgs()
        {
            m_Result = new List<string>();
        }

        public List<string> Result
        {
            get { return m_Result; }
        }

    }

    public class AppShutdownEventArgs : EventArgs
    {
        public const string EVENT_TYPE = "AppShutdown";
    }

    public class RefreshEventArgs : EventArgs
    {
        public const string EVENT_TYPE = "Refresh";
    }

}
