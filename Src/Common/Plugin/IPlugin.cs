using System;
using System.Collections.Generic;
using System.Text;
using Common.Event;

namespace Common.Plugin
{
    public interface IPlugin
    {
        string Name { get; }
        string Description { get;}

        void InitializePlugin(IEventContainer eventDispatcher);
    }
}
