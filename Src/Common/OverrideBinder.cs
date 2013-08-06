using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.Serialization;
using System.Reflection;

namespace Common
{
    public sealed class OverrideBinder : SerializationBinder
    {
        //Dictionary<string, string> m_AssemblyMap;     

        public string overrideAssemblyName;

        public override Type BindToType(string assemblyName, string typeName)
        {


            
                    // For each assemblyName/typeName that you want to deserialize to
                    // a different type, set typeToDeserialize to the desired type.
            //String oldAssem = "MaterialEditor";

            //if (assemblyName.Split(',')[0] == oldAssem && overrideAssemblyName != null)
            //{
            //    // To use a type from a different assembly version, 
            //    // change the version number.
            //    // To do this, uncomment the following line of code.
            //    // assemblyName = assemblyName.Replace("1.0.0.0", "2.0.0.0");

            //    // To use a different type from the same assembly, 
            //    // change the type name.
            //    // typeName = "Version2Type";

            //    assemblyName = overrideAssemblyName;
            //}

            //int index = typeName.IndexOf("MaterialEditor");
            //if (index >= 0)
            //    typeName = typeName.Replace("MaterialEditor", "Minotaur");
            //index = assemblyName.IndexOf("MaterialEditor");
            //if (index >= 0)
            //    assemblyName = assemblyName.Replace("MaterialEditor", "Minotaur");

            // in my case I only want to override the old assembly/version imprinted in the serialized data with the current assembly/version info so that I can deserialize into the same type, so ...



            Type typeToDeserialize = null;

            // The following line of code returns the type.
            try
            {
                typeToDeserialize = Type.GetType(String.Format("{0}, {1}", typeName, assemblyName));
            }
            catch(Exception)
            {
                typeToDeserialize = null;
            }

            return typeToDeserialize;
        }
    }
}
