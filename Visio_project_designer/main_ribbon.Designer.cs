/// @file main_ribbon.Designer.cs
/// @brief Визуальные компоненты ленты меню @ref visio_prj_designer::main_ribbon.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.
///

namespace visio_prj_designer
    {
    partial class main_ribbon : Microsoft.Office.Tools.Ribbon.RibbonBase
        {
        /// <summary> Показать ленту меню. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        public void show()
            {
            process_definition_tab.Visible = true;
            PerformLayout();
            }

        /// <summary> Скрыть ленту меню. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        public void hide()
            {
            process_definition_tab.Visible = false;
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
            this.process_definition_tab = this.Factory.CreateRibbonTab();
            this.group1 = this.Factory.CreateRibbonGroup();
            this.edit_mode_tbutton = this.Factory.CreateRibbonToggleButton();
            this.group2 = this.Factory.CreateRibbonGroup();
            this.save_as_lua_button = this.Factory.CreateRibbonButton();
            this.group3 = this.Factory.CreateRibbonGroup();
            this.Btn_devices = this.Factory.CreateRibbonButton();
            this.button_modes = this.Factory.CreateRibbonButton();
            this.process_definition_tab.SuspendLayout();
            this.group1.SuspendLayout();
            this.group2.SuspendLayout();
            this.group3.SuspendLayout();
            // 
            // process_definition_tab
            // 
            this.process_definition_tab.Groups.Add( this.group1 );
            this.process_definition_tab.Groups.Add( this.group2 );
            this.process_definition_tab.Groups.Add( this.group3 );
            this.process_definition_tab.Label = "Описание техпроцесса";
            this.process_definition_tab.Name = "process_definition_tab";
            this.process_definition_tab.Visible = false;
            // 
            // group1
            // 
            this.group1.Items.Add( this.edit_mode_tbutton );
            this.group1.Label = "Редактирование";
            this.group1.Name = "group1";
            // 
            // edit_mode_tbutton
            // 
            this.edit_mode_tbutton.Label = "Привязка";
            this.edit_mode_tbutton.Name = "edit_mode_tbutton";
            this.edit_mode_tbutton.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler( this.toggleButton_edit_mode_Click );
            // 
            // group2
            // 
            this.group2.Items.Add( this.save_as_lua_button );
            this.group2.Label = "Редактор WAGO";
            this.group2.Name = "group2";
            // 
            // save_as_lua_button
            // 
            this.save_as_lua_button.ControlSize = Microsoft.Office.Core.RibbonControlSize.RibbonControlSizeLarge;
            this.save_as_lua_button.Image = global::Visio_project_designer.Properties.Resources.wago_linux;
            this.save_as_lua_button.Label = "Сохранить описание LUA";
            this.save_as_lua_button.Name = "save_as_lua_button";
            this.save_as_lua_button.ShowImage = true;
            this.save_as_lua_button.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler( this.button1_Click );
            // 
            // group3
            // 
            this.group3.Items.Add( this.Btn_devices );
            this.group3.Items.Add( this.button_modes );
            this.group3.Label = "Объекты";
            this.group3.Name = "group3";
            // 
            // Btn_devices
            // 
            this.Btn_devices.Label = "Устройства";
            this.Btn_devices.Name = "Btn_devices";
            this.Btn_devices.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler( this.Btn_devices_Click );
            // 
            // button_modes
            // 
            this.button_modes.Label = "Режиы объекта";
            this.button_modes.Name = "button_modes";
            this.button_modes.Click += new Microsoft.Office.Tools.Ribbon.RibbonControlEventHandler( this.button_modes_Click );
            // 
            // main_ribbon
            // 
            this.Name = "main_ribbon";
            this.RibbonType = "Microsoft.Visio.Drawing";
            this.Tabs.Add( this.process_definition_tab );
            this.Load += new Microsoft.Office.Tools.Ribbon.RibbonUIEventHandler( this.Ribbon1_Load );
            this.process_definition_tab.ResumeLayout( false );
            this.process_definition_tab.PerformLayout();
            this.group1.ResumeLayout( false );
            this.group1.PerformLayout();
            this.group2.ResumeLayout( false );
            this.group2.PerformLayout();
            this.group3.ResumeLayout( false );
            this.group3.PerformLayout();

            }

        #endregion

        internal Microsoft.Office.Tools.Ribbon.RibbonGroup group1;
        internal Microsoft.Office.Tools.Ribbon.RibbonGroup group2;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton save_as_lua_button;
        internal Microsoft.Office.Tools.Ribbon.RibbonToggleButton edit_mode_tbutton;
        internal Microsoft.Office.Tools.Ribbon.RibbonTab process_definition_tab;
		internal Microsoft.Office.Tools.Ribbon.RibbonGroup group3;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton Btn_devices;
        internal Microsoft.Office.Tools.Ribbon.RibbonButton button_modes;
        }

    partial class ThisRibbonCollection
        {
        internal main_ribbon Ribbon1
            {
            get { return this.GetRibbon<main_ribbon>(); }
            }
        }
    }
