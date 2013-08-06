using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.IO;

using Common.PropertyGrid;
using System.Drawing.Design;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace MaterialEditor
{
    public partial class PreferencesWin : Form
    {
        [Serializable]
        private class PreferencesData : ISerializable
        {
            public string dirPath = "";
            public string pluginsPath = "";

            public PreferencesData()
            {
                dirPath = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "..\\Data\\Materials");
                pluginsPath = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "Minotaur\\Plugins");
            }

            protected PreferencesData(SerializationInfo info, StreamingContext context)
            {
                dirPath = info.GetString("DirPath");
                pluginsPath = info.GetString("PluginsPath");

                if (pluginsPath == null || pluginsPath.Equals(""))
                {
                    pluginsPath = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "Minotaur\\Plugins");
                }
            }

            public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
            {
                info.AddValue("DirPath", dirPath);
                info.AddValue("PluginsPath", pluginsPath);
            }
        }

        protected PropertyBag m_Properties;

        private PreferencesData m_Data;
        private MainWin m_Main;

        public PreferencesWin(MainWin main)
        {
            m_Main = main;

            m_Data = new PreferencesData();
            m_Properties = new PropertyBag();

            m_Properties.GetValue += new PropertySpecEventHandler(this.PropertyGetValue);
            m_Properties.SetValue += new PropertySpecEventHandler(this.PropertySetValue);

            m_Properties.Properties.Add(new PropertySpec(
                "Directory Path", typeof(string), "Export", "The directory where materials are exported", m_Data.dirPath,
                typeof(DirNameEditor), (string)null));

            m_Properties.Properties.Add(new PropertySpec(
                 "Plugins Path", typeof(string), "Plugins", "The directory where plugins are located", m_Data.pluginsPath,
                 typeof(DirNameEditor), (string)null));

            InitializeComponent();

            propertyGrid.SelectedObject = m_Properties;
        }

        protected virtual void PropertyGetValue(object sender, PropertySpecEventArgs e)
        {
            switch(e.Property.Name)
            {
                case "Directory Path": e.Value = m_Data.dirPath; break;
                case "Plugins Path": e.Value = m_Data.pluginsPath; break;
            }
        }

        protected virtual void PropertySetValue(object sender, PropertySpecEventArgs e)
        {
            switch(e.Property.Name)
            {
                case "Directory Path": m_Data.dirPath = (string)e.Value; break;
                case "Plugins Path": m_Data.pluginsPath = (string) e.Value; break;
            }
        }
        
        public string ExportDirectoryPath
        {
            get
            {
                return m_Data.dirPath;
            }
        }

        public string PluginsDirectoryPath
        {
            get { return m_Data.pluginsPath; }
        }

        private void PreferencesWin_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
            {
                Hide();
                e.Cancel = true;
            }
        }

        #region Serialization
        public void Serialize(Stream outStream)
        {
            IFormatter formatter = m_Main.GetBinaryFormatter();
            try
            {
                formatter.Serialize(outStream, m_Data);
            }
            catch (Exception)
            {

            }
        }

        public void Deserialize(Stream inStream)
        {
            IFormatter formatter = m_Main.GetBinaryFormatter();
            try
            {
                m_Data = (PreferencesData)formatter.Deserialize(inStream);
            }
            catch (Exception)
            {
            }
        }
        #endregion

    }

    public class DirNameEditor : UITypeEditor
    {
        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return UITypeEditorEditStyle.Modal;
        }

        public override object EditValue(ITypeDescriptorContext context,
            IServiceProvider provider,
            object value)
        {
            FolderBrowserDialog folder = new FolderBrowserDialog();
            folder.RootFolder = Environment.SpecialFolder.MyComputer;// Path.GetDirectoryName(Application.ExecutablePath);

            if (folder.ShowDialog() == DialogResult.OK)
            {
                return folder.SelectedPath;
            }

            return value;
        }
    }

}