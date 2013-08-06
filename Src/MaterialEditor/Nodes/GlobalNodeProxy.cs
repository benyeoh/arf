using System;
using System.Collections.Generic;
using System.Text;
using Common.PropertyGrid;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Diagnostics;
using System.Drawing;
using System.ComponentModel;
using ARFManaged.MaterialGenManaged;

namespace MaterialEditor
{
    [Serializable]
    public class GlobalNodeProxy : PropertyNode, ISerializable, IOutputNodeOwner, ICloneable
    {
        GlobalNode m_Proxy;

        RectLabel m_RectLabel;
        OutputNode m_Output;

        public GlobalNodeProxy(GlobalNode proxy)
            : base()
        {
            m_Proxy = proxy;
            m_Output = new OutputNode(this);

            m_Proxy.OnPropertyChanged += new HandlePropertyChanged(m_Proxy_OnPropertyChanged);
            m_RectLabel = new RectLabel();
            m_RectLabel.X = -99999;
            m_RectLabel.Y = -99999;
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.LightSkyBlue;
            m_RectLabel.Text = m_Proxy.Type + " " + m_Proxy.Name;
        }

        void m_Proxy_OnPropertyChanged(PropertyNode node, PropertySpecEventArgs e)
        {
            switch (e.Property.Name)
            {
                case "Name": m_RectLabel.Text = m_Proxy.Type + " " + (string) e.Value; break;
                case "Type": m_RectLabel.Text = (string)e.Value + " " + m_Proxy.Name; break;
            }
        }

        protected GlobalNodeProxy(SerializationInfo info, StreamingContext context)
            : base()
        {
            m_Proxy = (GlobalNode)info.GetValue("Proxy", typeof(GlobalNode));
            m_Output = (OutputNode)info.GetValue("Output", typeof(OutputNode));
            m_RectLabel = (RectLabel)info.GetValue("RectLabel", typeof(RectLabel));
            m_RectLabel.Font = new Font("Verdana", 12);
            m_RectLabel.FontColor = Color.Black;
            m_RectLabel.BackColor = Color.LightSkyBlue;
            
            m_Proxy.OnPropertyChanged += new HandlePropertyChanged(m_Proxy_OnPropertyChanged);
            
            Debug.Assert(m_Output.Owner == this);
        }

        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Proxy", Proxy);
            info.AddValue("Output", Output);
            info.AddValue("RectLabel", m_RectLabel);
        }

        protected override void InitPropertyBag()
        {
            base.InitPropertyBag();

            m_Properties.Properties.Add(new PropertySpec(
                "Name", typeof(string), "Global", "Name of the global", "",
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties[m_Properties.Properties.Count - 1].Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
            
            m_Properties.Properties.Add(new PropertySpec(
                "Type", typeof(string), "Global", "Type of the global", "",
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties[m_Properties.Properties.Count - 1].Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
           
            m_Properties.Properties.Add(new PropertySpec(
                "Default Value", typeof(string), "Global", "Default value of the global", "",
                typeof(DropDownTextAreaEditor), (string)null));
            m_Properties.Properties[m_Properties.Properties.Count - 1].Attributes = new Attribute[] { ReadOnlyAttribute.Yes };

            m_Properties.Properties.Add(new PropertySpec(
                "Semantic", typeof(string), "Global", "Semantic value of the global", "Invalid"));
            m_Properties.Properties[m_Properties.Properties.Count - 1].Attributes = new Attribute[] { ReadOnlyAttribute.Yes };

            m_Properties.Properties.Add(new PropertySpec(
                "Usage", typeof(int), "Global", "The global usage can be dynamic/constant/instanced", false));
            m_Properties.Properties[m_Properties.Properties.Count - 1].Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
            
            m_Properties.Properties.Add(new PropertySpec(
                "Is Always Used", typeof(bool), "Global", "Is the global always considered during generation?", false));
            m_Properties.Properties[m_Properties.Properties.Count - 1].Attributes = new Attribute[] { ReadOnlyAttribute.Yes };
            
            m_Properties.Properties.Add(new PropertySpec(
                "Is Volatile", typeof(bool), "Global", "Does the global need to be resolved every time?", false));
            m_Properties.Properties[m_Properties.Properties.Count - 1].Attributes = new Attribute[] { ReadOnlyAttribute.Yes };

        }

        protected override void PropertyGetValue(object sender, PropertySpecEventArgs e)
        {
            base.PropertyGetValue(sender, e);

            switch (e.Property.Name)
            {
                case "Name": e.Value = m_Proxy.Name; break;
                case "Type": e.Value = m_Proxy.Type; break;
                case "Default Value": e.Value = m_Proxy.DefaultValue; break;
                case "Semantic": e.Value = m_Proxy.Semantic; break;
                case "Usage": e.Value = m_Proxy.Usage; break;
                case "Is Always Used": e.Value = m_Proxy.IsAlwaysUsed; break;
                case "Is Volatile": e.Value = m_Proxy.IsVolatile; break;
            }
        }

        protected override void PropertySetValue(object sender, PropertySpecEventArgs e)
        {
            Debug.Assert(false);
        }

        public GlobalNode Proxy
        {
            get
            {
                return m_Proxy;
            }
        }

        public OutputNode Output
        {
            get
            {
                return m_Output;
            }
        }

        public RectLabel RectLabel
        {
            get
            {
                return m_RectLabel;
            }
        }

        public string GetOwnerName()
        {
            return m_Proxy.Name;
        }

        public string GetOwnerType()
        {
            return m_Proxy.Type;
        }

        public object Clone()
        {
            GlobalNodeProxy proxy = new GlobalNodeProxy(Proxy);

            proxy.Output.EllipseLabel.X = Output.EllipseLabel.X;
            proxy.Output.EllipseLabel.Y = Output.EllipseLabel.Y;

            proxy.RectLabel.X = RectLabel.X;
            proxy.RectLabel.Y = RectLabel.Y;

            return proxy;
        }

        #region Material Gen
        public void BeginMaterialGen(MaterialGen gen)
        {
            m_Proxy.BeginMaterialGen(gen);
            Output.BeginMaterialGen(m_Proxy.Impl.GetOutput());

            //m_Proxy.BeginMaterialGen(gen);
        }

        public void ProcessBinding()
        {
            m_Proxy.ProcessBinding();
            Output.ProcessBinding();
        }

        public void EndMaterialGen()
        {
            m_Proxy.EndMaterialGen();
            Output.EndMaterialGen();
        }
        #endregion

    }
}
