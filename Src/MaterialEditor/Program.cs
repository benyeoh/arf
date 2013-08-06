using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using WeifenLuo.WinFormsUI.Docking;
using ARFManaged.MaterialGenManaged;
using ARFManaged.RendererManaged;
using Common.PropertyGrid;

using System.Diagnostics;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Reflection;

namespace MaterialEditor
{
    static class Program
    {
        static Assembly MyResolveEventHandler(object sender, ResolveEventArgs args)
        {
            Console.WriteLine(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Resolve assembly!!!!");

            return null;
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            AppDomain currentDomain = AppDomain.CurrentDomain;
            currentDomain.AssemblyResolve += new ResolveEventHandler(MyResolveEventHandler);

            ARFManaged.CoreMemoryManaged.Main.Initialize("CoreMemory.dll");

            ARFManaged.CoreMemoryManaged.MemAllocator memAlloc = ARFManaged.CoreMemoryManaged.Main.CreateMemAllocator();

            ARFManaged.CoreManaged.Main.Initialize(memAlloc);
            ARFManaged.RendererManaged.Main.Initialize("RendererD3D.dll", memAlloc);
            ARFManaged.MaterialGenManaged.Main.Initialize("MaterialGenD3D.dll", memAlloc);

            //ResolveEventHandler translator = new ResolveEventHandler(CurrentDomain_TypeResolveEventHandler);
            //AppDomain.CurrentDomain.TypeResolve += translator;
  
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainWin());
           // MaterialGen gen = ARFManaged.MaterialGenManaged.Main.CreateMaterialGen();
           //// gen.Dispose();
           // gen = null;
            //AppDomain.CurrentDomain.TypeResolve -= translator;

            
            System.GC.Collect();
            System.GC.WaitForPendingFinalizers();

            memAlloc.DumpLeaks();
           
            ARFManaged.MaterialGenManaged.Main.Shutdown();
            ARFManaged.RendererManaged.Main.Shutdown();
            ARFManaged.CoreManaged.Main.Shutdown();

            memAlloc = null;
        }

        static Assembly CurrentDomain_AssemblyResolveEventHandler(object sender, ResolveEventArgs args)
        {
            Assembly result = null;
            string shortAssemblyName = args.Name.Split(',')[0];
            Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();

            if (shortAssemblyName.Equals("MaterialEditor"))
            {
                foreach (Assembly assembly in assemblies)
                {
                    if (assembly.FullName.Split(',')[0].Equals("Minotaur"))
                    {
                        result = assembly;
                        break;
                    }
                }
            }

            //if (result == null)
            //{
            //    foreach (Assembly assembly in assemblies)
            //    {
            //        if (shortAssemblyName == assembly.FullName.Split(',')[0])
            //        {
            //            result = assembly;
            //            break;
            //        }
            //    }
            //}

            return result;

        }
    }
}