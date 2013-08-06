using System;
using System.Collections.Generic;
using System.Text;
using Common.PropertyGrid;

namespace MaterialEditor
{
    public class ListBoxSemanticTypeEditor : ListBoxTypeEditor
    {
        public override object[] GetListBoxValues()
        {
            GetAllSemanticNamesEventArgs resArgs = new GetAllSemanticNamesEventArgs();
            EventManager.GlobalEventManager.RaiseEvent(
                GetAllSemanticNamesEventArgs.EVENT_TYPE, this, resArgs);

            return resArgs.Result.ToArray();
        }
    }
}
