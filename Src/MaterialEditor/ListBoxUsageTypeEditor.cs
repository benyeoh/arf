using System;
using System.Collections.Generic;
using System.Text;
using Common.PropertyGrid;

namespace MaterialEditor
{
    public class ListBoxUsageTypeEditor : ListBoxTypeEditor
    {
        public override object[] GetListBoxValues()
        {
            return Enum.GetNames(typeof(ARFManaged.MaterialGenManaged.eMGMUsage)); ;
        }
    }
}
