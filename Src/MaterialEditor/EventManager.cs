using System;
using System.Collections.Generic;
using System.Text;
using Common.Plugin;
using Common.Event;

namespace MaterialEditor
{
    public class EventManager : EventContainer
    {
        private static EventManager g_GlobalEventManager;
        public EventManager()
        {
        }

        public static EventManager GlobalEventManager
        {
            get { return g_GlobalEventManager; }
            set { g_GlobalEventManager = (EventManager)value; }
        }
    }
}
