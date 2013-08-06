using System;
using System.Collections.Generic;
using System.Text;

namespace Common.Event
{
    public class EventContainer : IEventContainer
    {
        protected Dictionary<string, EventHandler> m_EventTable;

        public EventContainer()
        {
            m_EventTable = new Dictionary<string, EventHandler>();
        }

        public void AddEventHandler(string eventType, EventHandler handler)
        {
            if (!m_EventTable.ContainsKey(eventType))
                m_EventTable.Add(eventType, handler);
            else
                m_EventTable[eventType] += handler;
        }

        public void RemoveEventHandler(string eventType, EventHandler handler)
        {
            if (!m_EventTable.ContainsKey(eventType))
                return;

            m_EventTable[eventType] -= handler;
            if (m_EventTable[eventType] == null)
                m_EventTable.Remove(eventType);
        }

        public void RaiseEvent(string eventType, object sender, EventArgs args)
        {
            if (!m_EventTable.ContainsKey(eventType))
                return;

            EventHandler handler = (EventHandler) m_EventTable[eventType];
            if (handler != null)
            {
                handler(sender, args);
            }
        }
    }
}
