using System;
using System.Collections.Generic;
using System.Text;
using Common.PropertyGrid;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Diagnostics;
using System.Drawing;
using System.ComponentModel;

namespace MaterialEditor
{
    public abstract class PropertyNode
    {
        #region Property Changed Event Declaration
        public delegate void HandlePropertyChanged(PropertyNode node, PropertySpecEventArgs e);

        [NonSerialized]
        private HandlePropertyChanged OnChangedDelegate;
        public event HandlePropertyChanged OnPropertyChanged
        {
            add
            {
                OnChangedDelegate = (HandlePropertyChanged)Delegate.Combine(OnChangedDelegate, value);
            }

            remove
            {
                OnChangedDelegate = (HandlePropertyChanged)Delegate.Remove(OnChangedDelegate, value);
            }
        }
        #endregion

        protected PropertyBag m_Properties;
        protected bool m_ReadOnly;

        public PropertyNode()
        {
            m_ReadOnly = false;
            InitPropertyBag();
        }

        public PropertyBag Properties
        {
            get { return m_Properties; }
        }

        protected virtual void InitPropertyBag()
        {
            m_Properties = new PropertyBag();

            m_Properties.GetValue += new PropertySpecEventHandler(this.PropertyGetValue);
            m_Properties.SetValue += new PropertySpecEventHandler(this.PropertySetValue);
        }      

        protected virtual void PropertyGetValue(object sender, PropertySpecEventArgs e)
        {
        }

        protected virtual void PropertySetValue(object sender, PropertySpecEventArgs e)
        {
            if(OnChangedDelegate != null)
                OnChangedDelegate(this, e);
        }

        protected void MakeReadOnly(string propName, ReadOnlyAttribute readOnly)
        {
            foreach (PropertySpec spec in m_Properties.Properties)
            {
                if(spec.Name.Equals(propName))
                {
                    // TODO: overwrite old values is better
                    spec.Attributes = new Attribute[] { readOnly };
                    break;
                }
            }
        }

        //public bool ReadOnly
        //{
        //    get
        //    {
        //        return m_ReadOnly;
        //    }

        //    set
        //    {
        //        m_ReadOnly = (bool)value;
        //        ReadOnlyAttribute readOnly = m_ReadOnly ? ReadOnlyAttribute.Yes : ReadOnlyAttribute.No;
                
        //        foreach(PropertySpec spec in m_Properties.Properties)
        //        {
        //            // TODO: overwrite old values is better
        //            spec.Attributes = new Attribute[] { readOnly };
        //        }
        //    }
        //}
    }
}
