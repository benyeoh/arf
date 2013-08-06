using System;
using System.Collections.Generic;
using System.Text;
using ARFManaged;
using ARFManaged.CoreManaged;

namespace MaterialEditor
{
    public class MaterialEditorAppCallback : AppCallback
    {
        private MainWin m_Main;

        public MaterialEditorAppCallback(MainWin main)
        {
            m_Main = main;
        }

        public override ByteBuffer GetFileData(string hFileName)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public override void Log(string msg)
        {
            m_Main.Output.WriteLine(msg);
        }
    }
}
