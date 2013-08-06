using System;
using System.Collections.Generic;
using System.Text;

namespace MaterialEditor
{
    public interface IOutputNodeOwner
    {
        string GetOwnerName();
        string GetOwnerType();
    }
}
