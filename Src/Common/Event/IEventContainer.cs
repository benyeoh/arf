using System;
using System.Collections.Generic;
using System.Text;

namespace Common.Event
{
    public interface IEventContainer
    {
        void AddEventHandler(string eventType, EventHandler handler);
        void RemoveEventHandler(string eventType, EventHandler handler);
        void RaiseEvent(string eventType, object sender, EventArgs args);
    }
}
