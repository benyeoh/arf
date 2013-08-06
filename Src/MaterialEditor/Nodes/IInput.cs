using System;
using System.Collections.Generic;
using System.Text;
using Common.PropertyGrid;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Drawing;
using ARFManaged.MaterialGenManaged;

namespace MaterialEditor
{
    public interface IInput
    {
        string Name
        {
            get;
        }

        string Type
        {
            get;
        }

        string DefaultValue
        {
            get;
        }

        OutputNode Binding
        {
            get;
            set;
        }

        OutputNode AltBinding
        {
            get;
            set;
        }

        #region Material Gen
        void BeginMaterialGen(InputResource input);
        void ProcessBinding();        
        void EndMaterialGen();
        
        InputResource Impl
        {
            get;
        }
        #endregion
    }
}
