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
using System.Runtime.Serialization.Formatters;

namespace MaterialEditor
{
    public partial class MainWin : Form
    {
        public const string DEFAULT_DATA_PATH = "..\\Data\\Minotaur";
        public const string DOCKING_LAYOUT_REL_PATH = DEFAULT_DATA_PATH + "\\layout.xml";
        public const string RECENT_PROJECT_FILE_PATH = DEFAULT_DATA_PATH + "\\projects.bin";
        public const string FORM_PROPERTIES_PATH = DEFAULT_DATA_PATH + "\\main.bin";
        public const string PREFERENCES_DATA_PATH = DEFAULT_DATA_PATH + "\\preferences.bin";

        private EventManager m_EventManager;
        private PluginManagerWin m_PluginManager;
        private ExplorerWin m_Explorer;
        private PropertyWin m_Properties;
        private OutputWin m_Output;
        private PreferencesWin m_Preferences;

        // private DrawWin m_DrawWin;
        private MaterialGen m_Gen;
        private Renderer m_Renderer;

        private MaterialEditorAppCallback m_Callback;
        private string m_CurrentProjectName;
        private string m_DefaultTitle;

        private List<string> m_RecentProjectNames;

        public MainWin()
        {
            InitializeComponent();

            m_EventManager = new EventManager();
            EventManager.GlobalEventManager = m_EventManager;

            m_Callback = new MaterialEditorAppCallback(this);
            m_Gen = ARFManaged.MaterialGenManaged.Main.CreateMaterialGen();
            m_Gen.Initialize(m_Callback);
            
            m_Renderer = ARFManaged.RendererManaged.Main.CreateRenderer();
            InitInfo init;
            init.presentInfo.backBufferHeight = 480;
            init.presentInfo.backBufferWidth = 640;
            init.presentInfo.fullScreen = false;
            init.presentInfo.hWnd = this.Handle; // For now
            init.hApplication = m_Callback;
            m_Renderer.Initialize(init);
            
            m_Explorer = new ExplorerWin(this);
            m_Properties = new PropertyWin(this);
            m_Output = new OutputWin(this);
            m_Preferences = new PreferencesWin(this);
            m_PluginManager = new PluginManagerWin(this);

            m_DefaultTitle = Text;
            m_CurrentProjectName = null;
            m_RecentProjectNames = new List<string>();

           // m_DrawWin = new DrawWin(this);
        }

        #region Layout Save/Load
        private DeserializeDockContent m_DeserializeDockContent;

        public void LoadDefaultLayout()
        {
            mainWindowDockPanel.SuspendLayout(true);
            m_Explorer.Show(mainWindowDockPanel, DockState.DockLeft);
            m_Output.Show(mainWindowDockPanel, DockState.DockBottom);
            m_Properties.Show(mainWindowDockPanel, DockState.DockRight);
            //m_PluginManager.Show(mainWindowDockPanel, DockState.Float);
            mainWindowDockPanel.ResumeLayout(true, true);
        }

        public void SaveLayout(string filePath)
        {
            // Save Layout
            mainWindowDockPanel.SaveAsXml(filePath);
        }

        public bool LoadLayout(string filePath)
        {
            if(m_DeserializeDockContent == null)
                m_DeserializeDockContent = new DeserializeDockContent(GetContentFromPersistString);

            mainWindowDockPanel.SuspendLayout(true);
            bool result = false;

            if (File.Exists(filePath))
            {
                mainWindowDockPanel.LoadFromXml(filePath, m_DeserializeDockContent);
                result = true;
            }

            mainWindowDockPanel.ResumeLayout(true, true);
            return result;
        }

        private IDockContent GetContentFromPersistString(string persistString)
        {
            if (persistString == typeof(ExplorerWin).ToString())
                return m_Explorer;
            else if (persistString == typeof(PropertyWin).ToString())
                return m_Properties;
            else if (persistString == typeof(OutputWin).ToString())
                return m_Output;
            else if (persistString == typeof(PluginManagerWin).ToString())
                return m_PluginManager;

            //else if (persistString == typeof(DrawWin).ToString())
            //    return new DrawWin(this);

            return null;

            //else if (persistString == typeof(DummyPropertyWindow).ToString())
            //    return m_propertyWindow;
            //else if (persistString == typeof(DummyToolbox).ToString())
            //    return m_toolbox;
            //else if (persistString == typeof(DummyOutputWindow).ToString())
            //    return m_outputWindow;
            //else if (persistString == typeof(DummyTaskList).ToString())
            //    return m_taskList;
            //else
            //{
            //    string[] parsedStrings = persistString.Split(new char[] { ',' });
            //    if (parsedStrings.Length != 3)
            //        return null;

            //    if (parsedStrings[0] != typeof(DummyDoc).ToString())
            //        return null;

            //    DummyDoc dummyDoc = new DummyDoc();
            //    if (parsedStrings[1] != string.Empty)
            //        dummyDoc.FileName = parsedStrings[1];
            //    if (parsedStrings[2] != string.Empty)
            //        dummyDoc.Text = parsedStrings[2];

            //    return dummyDoc;
            //}
        }
        #endregion

        #region Accessors
        public PreferencesWin Preferences
        {
            get
            {
                return m_Preferences;
            }
        }

        public ExplorerWin Explorer
        {
            get 
            { 
                return m_Explorer; 
            }
        } 

        public PropertyWin Properties
        {
            get
            {
                return m_Properties;
            }
        }

        public DockPanel DockPanel
        {
            get
            {
                return mainWindowDockPanel;
            }
        }

        public OutputWin Output
        {
            get
            {
                return m_Output;
            }
        }

        public MaterialGen MaterialGen
        {
            get
            {
                return m_Gen;
            }
        }

        public Renderer Renderer
        {
            get
            {
                return m_Renderer;
            }
        }

        public string ProjectName
        {
            get
            {
                return m_CurrentProjectName;
            }

            set
            {
                m_CurrentProjectName = (string)value;

                if (m_CurrentProjectName != null && m_CurrentProjectName.Length > 0)
                {
                    int index = -1;
                    for(int i=0; i < m_RecentProjectNames.Count; ++i)
                    {
                        if (m_RecentProjectNames[i].Equals(m_CurrentProjectName))
                        {
                            index = i;
                            break;
                        }
                    }

                    if(index >= 0)
                    {
                        m_RecentProjectNames.RemoveAt(index);
                    }

                    Text = m_DefaultTitle + " - " + m_CurrentProjectName;
                    m_RecentProjectNames.Insert(0, m_CurrentProjectName);
                    if (m_RecentProjectNames.Count > 8)
                        m_RecentProjectNames.RemoveAt(m_RecentProjectNames.Count - 1);
                }
                else
                    Text = m_DefaultTitle;
            }
        }

        #endregion

        public List<DockContent> GetAllDocumentsAndFloatingDocks()
        {
            List<DockContent> allContents = new List<DockContent>();
            IDockContent[] contents = this.DockPanel.DocumentsToArray();
            foreach (IDockContent content in contents)
            {
                if (content.GetType().Equals(typeof(DrawWin)) ||
                    content.GetType().Equals(typeof(CodeOutputWin)))
                {
                    allContents.Add(((DockContent)content));
                }
            }

            foreach (FloatWindow content in this.DockPanel.FloatWindows)
            {
                foreach (DockPane pane in content.NestedPanes)
                {
                    foreach (DockContent dock in pane.Contents)
                    {
                        allContents.Add(dock);
                    }
                }
            }

            return allContents;
        }

        public DrawWin AddDrawWin(string text, NodePanel panel)
        {
            DrawWin draw = new DrawWin(this);

            panel.Dock = System.Windows.Forms.DockStyle.Fill;  
            draw.Panel = panel;

            draw.TabText = text;
            draw.Show(DockPanel, DockState.Document);
            return draw;
        }

        private void MainWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            // Fire app shutdown event
            EventManager.GlobalEventManager.RaiseEvent(AppShutdownEventArgs.EVENT_TYPE, this, new AppShutdownEventArgs());

            // Then dispose the global event manager
            EventManager.GlobalEventManager = null;

            // Save layout and configs
            string configFile = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), DOCKING_LAYOUT_REL_PATH);
            SaveLayout(configFile);

            string projectsFile = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), RECENT_PROJECT_FILE_PATH);
            IFormatter formatter = GetBinaryFormatter();
            Stream outStream = new FileStream(projectsFile, FileMode.Create, FileAccess.Write);
            try
            {
                formatter.Serialize(outStream, m_RecentProjectNames);
            }
            catch (Exception)
            {
                
            }
            outStream.Close();

            string formFile = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), FORM_PROPERTIES_PATH);
            Stream formStream = new FileStream(formFile, FileMode.Create, FileAccess.Write);
            try
            {
                FormProps formProps = new FormProps(this);
                formatter.Serialize(formStream, formProps);
            }
            catch (Exception)
            {

            }
            formStream.Close();

            string prefFile = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), PREFERENCES_DATA_PATH);
            Stream fileStream = new FileStream(prefFile, FileMode.Create, FileAccess.Write);
            m_Preferences.Serialize(fileStream);
            fileStream.Close();

            // Destroy renderer explicitly
            m_Renderer.Dispose();
        }

        private void MainWindow_Load(object sender, EventArgs e)
        {
            string configFile = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), DOCKING_LAYOUT_REL_PATH);
            if (!LoadLayout(configFile))
                LoadDefaultLayout();

            string projectsFile = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), RECENT_PROJECT_FILE_PATH);
            if(File.Exists(projectsFile))
            {
                IFormatter formatter = GetBinaryFormatter();
                Stream inStream = new FileStream(projectsFile, FileMode.Open, FileAccess.Read);
                try
                {
                    m_RecentProjectNames = (List<string>) formatter.Deserialize(inStream);
                }
                catch (Exception)
                {
                    m_RecentProjectNames.Clear();
                }

                inStream.Close();

            }

            string formFile = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), FORM_PROPERTIES_PATH);
            if (File.Exists(formFile))
            {
                IFormatter formatter = GetBinaryFormatter();
                Stream inStream = new FileStream(formFile, FileMode.Open, FileAccess.Read);
                try
                {
                    FormProps formProps = (FormProps)formatter.Deserialize(inStream);
                    this.Location = formProps.formPos;
                    this.Size = formProps.formSize;
                    this.WindowState = formProps.winState;                   
                }
                catch (Exception)
                {
                }
                inStream.Close();
            }

            string prefFile = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), PREFERENCES_DATA_PATH);
            if(File.Exists(prefFile))
            {
                Stream fileStream = new FileStream(prefFile, FileMode.Open, FileAccess.Read);
                m_Preferences.Deserialize(fileStream);
                fileStream.Close();
            }

            string[] args = Environment.GetCommandLineArgs();
            if(args.Length > 1)
            {
                string fileName = args[1];
                if (File.Exists(fileName))
                {
                    FileStream stream = new FileStream(fileName, FileMode.Open, FileAccess.Read);
                    Explorer.DeserializeNodes(stream);
                    stream.Close();

                    ProjectName = fileName;
                }               
            }

            m_PluginManager.LoadAllPlugins(Preferences.PluginsDirectoryPath);
        }

        private void viewExplorerMenuItem_Click(object sender, EventArgs e)
        {
            m_Explorer.Select();
            if (m_Explorer.IsHidden)
            {
                m_Explorer.Show(mainWindowDockPanel, DockState.Float);
            }
        }

        private void viewPropertiesMenuItem_Click(object sender, EventArgs e)
        {
            m_Properties.Select();
            if (m_Properties.IsHidden)
            {
                m_Properties.Show(mainWindowDockPanel, DockState.Float);
            }
        }

        private void saveAsMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFile = new SaveFileDialog();
            saveFile.RestoreDirectory = false;
            saveFile.Filter = "Minotaur Files (*.minotaur)|*.minotaur| All files (*.*)|*.*";
            saveFile.DefaultExt = "minotaur";

            if (saveFile.ShowDialog(this) == DialogResult.OK)
            {
                string fileName = saveFile.FileName;
                FileStream stream = new FileStream(fileName, FileMode.Create, FileAccess.Write);
                Explorer.SerializeNodes(stream);
                stream.Close();

                ProjectName = fileName;
            }
        }


        private void saveFileMenuItem_Click(object sender, EventArgs e)
        {
            if(ProjectName != null)
            {
                string fileName = ProjectName;
                FileStream stream = new FileStream(fileName, FileMode.Create, FileAccess.Write);
                Explorer.SerializeNodes(stream);
                stream.Close();
            }
            else
            {
                saveAsMenuItem_Click(sender, e);
            }
        }

        private void loadFileMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFile = new OpenFileDialog();
            openFile.CheckFileExists = true;
            openFile.CheckPathExists = true;
            openFile.RestoreDirectory = false;
            openFile.Filter = "Minotaur Files (*.minotaur)|*.minotaur| All files (*.*)|*.*";
            openFile.DefaultExt = "minotaur";

            if (openFile.ShowDialog(this) == DialogResult.OK)
            {
                string fileName = openFile.FileName;
                if (File.Exists(fileName))
                {
                    FileStream stream = new FileStream(fileName, FileMode.Open, FileAccess.Read);
                    Explorer.DeserializeNodes(stream);
                    stream.Close();

                    ProjectName = fileName;
                }
            }
        }

        private void viewOutputMenuItem_Click(object sender, EventArgs e)
        {
            m_Output.Select();
            if (m_Output.IsHidden)
            {
                m_Output.Show(mainWindowDockPanel, DockState.Float);
            }
        }

        private void viewPluginsMenuItem_Click(object sender, EventArgs e)
        {
            m_PluginManager.Select();
            if (m_PluginManager.IsHidden)
            {
                m_PluginManager.Show(mainWindowDockPanel, DockState.Float);
            }
        }

        private void exitFileMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void RecentProjectsDropDown_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem fileNameItem = (ToolStripMenuItem)sender;
            string fileName = fileNameItem.Text;
            if (File.Exists(fileName))
            {
                FileStream stream = new FileStream(fileName, FileMode.Open, FileAccess.Read);
                Explorer.DeserializeNodes(stream);
                stream.Close();

                ProjectName = fileName;
            } 
        }

        private void fileMenuItem_DropDownOpening(object sender, EventArgs e)
        {
            ToolStripMenuItem context = (ToolStripMenuItem)sender;
            ToolStripMenuItem recentProjectsMenuItem = (ToolStripMenuItem)context.DropDownItems["recentProjectsFileMenuItem"];
            recentProjectsMenuItem.DropDownItems.Clear();

            foreach(string str in m_RecentProjectNames)
            {
                ToolStripMenuItem newItem =
                                new ToolStripMenuItem(str, null, new EventHandler(RecentProjectsDropDown_Click));
                recentProjectsMenuItem.DropDownItems.Add(newItem);
            }
        }

        private void preferencesOptionMenuItem_Click(object sender, EventArgs e)
        {
            m_Preferences.Show();
            m_Preferences.BringToFront();
        }

        private void verifyAllCompileMenuItem_Click(object sender, EventArgs e)
        {
            m_Explorer.VerifyAll();
        }

        private void exportAllCompileMenuItem_Click(object sender, EventArgs e)
        {
            m_Explorer.ExportAll();
        }

        private void MainWin_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.None;
            if (e.Data.GetDataPresent(DataFormats.FileDrop, false))
            {
                if (((string[])e.Data.GetData(DataFormats.FileDrop)).Length == 1)
                {
                    string fileName = ((string[])e.Data.GetData(DataFormats.FileDrop))[0];
                    if (fileName.Contains(".minotaur"))
                        e.Effect = DragDropEffects.All;
                }
            }
                
        }

        private void MainWin_DragDrop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop, false))
            {
                if (((string[])e.Data.GetData(DataFormats.FileDrop)).Length == 1)
                {
                    string fileName = ((string[])e.Data.GetData(DataFormats.FileDrop))[0];
                    if (fileName.Contains(".minotaur"))
                    {
                        if (File.Exists(fileName))
                        {
                            FileStream stream = new FileStream(fileName, FileMode.Open, FileAccess.Read);
                            Explorer.DeserializeNodes(stream);
                            stream.Close();

                            ProjectName = fileName;
                        }
                    }
                }
            }
        }

        //public IFormatter GetXmlFormatter()
        //{
        //    return new SoapFormatter();
        //    //return new XmlFormatter();
        //}

        public IFormatter GetBinaryFormatter()
        {
           // SoapFormatter formatter = new SoapFormatter();

            BinaryFormatter formatter = new BinaryFormatter();

           // OverrideBinder binder = new OverrideBinder();
            //formatter.FilterLevel = TypeFilterLevel.Full;

            //binder.overrideAssemblyName = Assembly.GetExecutingAssembly().FullName;
            //formatter.AssemblyFormat = System.Runtime.Serialization.Formatters.FormatterAssemblyStyle.Simple;

            //formatter.Binder = binder;

            return formatter;
        }

        #region Serialization Helper
        [Serializable]
        private class FormProps : ISerializable
        {
            public Point formPos;
            public Size formSize;
            public FormWindowState winState;

            public FormProps(Form form)
            {
                formPos.X = form.Location.X;
                formPos.Y = form.Location.Y;
                formSize.Width = form.Size.Width;
                formSize.Height = form.Size.Height;
                winState = form.WindowState;
            }

            protected FormProps(SerializationInfo info, StreamingContext context)
            {
                int x = info.GetInt32("x");
                int y = info.GetInt32("y");

                int width = info.GetInt32("width");
                int height = info.GetInt32("height");

                winState = (FormWindowState)info.GetValue("winState", typeof(FormWindowState));
                formPos.X = x;
                formPos.Y = y;
                formSize.Width = width;
                formSize.Height = height;
            }

            public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
            {
                info.AddValue("x", formPos.X);
                info.AddValue("y", formPos.Y);
                info.AddValue("width", formSize.Width);
                info.AddValue("height", formSize.Height);
                info.AddValue("winState", winState);
            }
        }

        #endregion


    }
}