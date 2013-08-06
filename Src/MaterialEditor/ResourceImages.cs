using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Reflection;
using System.IO;

namespace MaterialEditor
{
    public class ResourceImages
    {
        public static Bitmap GetImage(string fileName)
        {
            string path = "MaterialEditor.Images." + fileName;
            Assembly assembly = Assembly.GetExecutingAssembly();
            Stream stream = assembly.GetManifestResourceStream(path);
            Bitmap bitmap = new Bitmap(stream, true);
            bitmap.MakeTransparent(Color.Magenta);
            return bitmap;
        }
    }
}
