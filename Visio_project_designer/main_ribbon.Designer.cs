namespace visio_project_designer
    {
    partial class main_ribbon : Microsoft.Office.Tools.Ribbon.RibbonBase
        {
        public void show()
            {
            tab1.Visible = true;
            PerformLayout();
            }

        public void hide()
            {
            tab1.Visible = false;
            PerformLayout();
            }

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        public main_ribbon()
            : base( Globals.Factory.GetRibbonFactory() )
            {
            InitializeComponent();
            }

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose( bool disposing )
            {
            if( disposing && ( components != null ) )
                {
                components.Dispose();
                }
            base.Dispose( disposing );
            }

        #region Component Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
            {
            this.tab1 = this.Factory.CreateRibbonTab();
            this.group1 = this.Factory.CreateRibbonGroup();
            this.toggleButton_edit_mode = this.Factory.CreateRibbonToggleButton();
            this.group2 = this.Factory.CreateRibbonGroup();
            this.button2 = this.Factory.CreateRibbonButton();
            this.tab1.SuspendLayout();
            this.group1.SuspendLayout();
            this.group2.SuspendLayout();
            // 
            // tab1
            // 
            this.tab1.Groups.Add( this.group1 );
            this.tab1.Groups.Add( this.group2 );
            this.tab1.Label = "Описание техпроцесса";
            this.tab1.Name = "tab1";
            this.tab1.Visible = false;
            // 
            // group1
            // 
            this.group1.Items.Add( this.toggleButton_edit_mode );
            this.group1.Label = "Редактирование";
            this.group1.Name = "group1";
            // 
            // toggleButton_edit_mode
            // 
            this.toggleButton_edit_mode.Label = "Привязка";
            this.toggleButton_edit_mode.Name = "toggleButton_edit_mode";
            this.toggleButton_edit_mode.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler( this.toggleButton_edit_mode_Click );
            // 
            // group2
            // 
            this.group2.Items.Add( this.button2 );
            this.group2.Label = "Редактор WAGO";
            this.group2.Name = "group2";
            // 
            // button2
            // 
            this.button2.ControlSize = Microsoft.Office.Core.RibbonControlSize.RibbonControlSizeLarge;
            this.button2.Image = global::visio_project_designer.Properties.Resources.wago_linux;
            this.button2.Label = "Сохранить описание";
            this.button2.Name = "button2";
            this.button2.ShowImage = true;
            this.button2.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler( this.button1_Click );
            // 
            // main_ribbon
            // 
            this.Name = "main_ribbon";
            this.RibbonType = "Microsoft.Visio.Drawing";
            this.Tabs.Add( this.tab1 );
            this.Load += new Microsoft.Office.Tools.Ribbon.RibbonUIEventHandler( this.Ribbon1_Load );
            this.tab1.ResumeLayout( false );
            this.tab1.PerformLayout();
            this.group1.ResumeLayout( false );
            this.group1.PerformLayout();
            this.group2.ResumeLayout( false );
            this.group2.PerformLayout();

            }

        #endregion

        internal Microsoft.Office.Tools.Ribbon.RibbonGroup group1;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup group2;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton button2;
        internal Microsoft.Office.Tools.Ribbon.RibbonToggleButton toggleButton_edit_mode;
        internal Microsoft.Office.Tools.Ribbon.RibbonTab tab1;
        }

    partial class ThisRibbonCollection
        {
        internal main_ribbon Ribbon1
            {
            get { return this.GetRibbon<main_ribbon>(); }
            }
        }
    }
