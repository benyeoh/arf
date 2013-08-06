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
    public interface IOutput
    {
        string Name
        {
            get;
        }

        string Type
        {
            get;
        }

        IOutputNodeOwner Owner
        {
            get;
        }


        #region Material Gen
        void BeginMaterialGen(OutputResource output);
        void ProcessBinding();
        void EndMaterialGen();

        OutputResource Impl
        {
            get;
        }
        #endregion
    }
}
