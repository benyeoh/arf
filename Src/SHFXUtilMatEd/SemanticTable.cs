using System;
using System.Collections.Generic;
using System.Text;

using Common.Plugin;
using Common.Event;
using MaterialEditor;
using ARFManaged.ShadowFXManaged;

namespace MaterialEditorPlugins.SHFXUtilMatEd
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
                return "ShadowFX Semantic Table";
            }
        }

        public string Description
        {
            get
            {
                return "Expose parameter semantics for ShadowFX module";
            }
        }

        public void HandleResolveSemantic(object sender, EventArgs args)
        {
            ResolveSemanticEventArgs resArgs = (ResolveSemanticEventArgs)args;

            try
            {
                ParamSemantic resVal = (ParamSemantic)Enum.Parse(typeof(ParamSemantic), resArgs.SemanticName);
                resArgs.Result = (int)resVal;
            }
            catch (ArgumentException)
            {
                // Semantic is not within param range
                // Do nothing
            }
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
