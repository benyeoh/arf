using System;
using System.Collections.Generic;
using System.Text;

using Common.Plugin;
using Common.Event;
using MaterialEditor;
using ARFManaged.DeferredFXManaged;

namespace MaterialEditorPlugins.DEFXUtilMatEd
{
    public class SemanticTable : IPlugin
    {
        public SemanticTable()
        {
        }

        public string Name
        {
            get
            {
                return "DeferredFX Semantic Table";
            }
        }

        public string Description
        {
            get
            {
                return "Expose parameter semantics for DeferredFX module";
            }
        }

        public void HandleResolveSemantic(object sender, EventArgs args)
        {
            ResolveSemanticEventArgs resArgs = (ResolveSemanticEventArgs)args;

            ParamSemantic resVal;
            if (Enum.TryParse(resArgs.SemanticName, out resVal))
                resArgs.Result = (int)resVal;
        }

        public void HandleGetAllSemanticNames(object sender, EventArgs args)
        {
            GetAllSemanticNamesEventArgs resArgs = (GetAllSemanticNamesEventArgs)args;

            foreach (string name in Enum.GetNames(typeof(ParamSemantic)))
            {
                resArgs.Result.Add(name);
            }
        }

        public void InitializePlugin(IEventContainer eventDispatcher)
        {
            eventDispatcher.AddEventHandler(ResolveSemanticEventArgs.EVENT_TYPE, HandleResolveSemantic);
            eventDispatcher.AddEventHandler(GetAllSemanticNamesEventArgs.EVENT_TYPE, HandleGetAllSemanticNames);
        }
    }
}
