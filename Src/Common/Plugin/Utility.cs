using System;
using System.Collections.Generic;
using System.Reflection;
using System.IO;

namespace Common.Plugin
{
    public static class Utility
    {
        public static List<FileInfo> GetPluginFileList(string folder)
        {
            List<FileInfo> plugins = new List<FileInfo>();

            // Check to make sure the folder exists
            if (Directory.Exists(folder) == false)
            {
                return plugins;
            }

            foreach (string filename in Directory.GetFiles(folder))
            {
                if (FileContainsPlugins(filename))
                {
                    plugins.Add(new FileInfo(filename));
                }
            }
           
            return plugins;
        }

        public static bool FileContainsPlugins(string filename)
        {           
            List<string> plugins = new List<string>();

            // Check to see if the file exist
            if (!File.Exists(filename))
            {
                return false;
            }
         
            // Get a list of plugins from the file
            plugins = GetPlugins(filename);

            return (plugins.Count != 0);
        }

        public static List<string> GetPlugins(string filename)
        {    
            Assembly asm;
            List<string> pluginList = new List<string>();
        
            // Check to see if the file exist
            if (!File.Exists(filename))
            {
                return pluginList;
            }
   
            try
            {
                asm = Assembly.LoadFile(filename);

                // Loop through all of the type
                foreach (Type type in asm.GetTypes())
                {
                    Type[] interfaces = type.GetInterfaces();
                    foreach (Type iface in interfaces)
                    {
                        if (iface.Name == typeof(IPlugin).Name)
                        {
                            // This type/class is of type TaskPluginInterface.ITaskPlugin
                            // so we shall return it
                            pluginList.Add(type.FullName);
                        }
                    }
                }
            }
            catch (Exception)
            {
            }

            // Cleanup
            asm = null;

            return pluginList;
        }

        public static IPlugin GetPlugin(string filename, string className)
        {
            // Check to see if the file exist
            if (!File.Exists(filename))
            {
                return null;
            }

            try
            {
                System.Runtime.Remoting.ObjectHandle obj = Activator.CreateInstanceFrom(filename, className);
                return (IPlugin)obj.Unwrap();
            }
            catch (BadImageFormatException)
            {
            }
            catch
            {
                throw;
            }

            return null;

        }

    }
}
