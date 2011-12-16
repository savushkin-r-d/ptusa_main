/// @file edit_valve_type.cs
/// @brief Форма редактирования типа клапана ( 1 КУ, 2 КУ и т.д.).
/// 
/// @author  Иванюк Дмитрий Сергеевич.
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.
///

using System;

using tech_device;

namespace visio_prj_designer
    {

    /// <summary>This class contains the implementation for the form that will
    /// be set as the content of the anchor bar created in the
    /// edit_valve_type_form_usage class.</summary>
    public class edit_valve_type_form
        : System.Windows.Forms.Form
        {

        /// <summary>Required list view variable</summary>
        public System.Windows.Forms.ListView type_lview;

        /// <summary>Required by the Windows Form Designer</summary>
        private System.ComponentModel.Container components = null;
        public  System.Windows.Forms.ComboBox type_cbox;
        private System.Windows.Forms.Label type_label;
        private System.Windows.Forms.Label no_channel_label;
        private System.Windows.Forms.ColumnHeader channel;
        private System.Windows.Forms.ColumnHeader value;

        /// <summary>The Visio Application object</summary>
        private Microsoft.Office.Interop.Visio.Application
            visioApplication;

        /// <summary> Установка окна в неактивное состояние. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        internal void disable_prop()
            {
            type_lview.Visible = false;
            type_cbox.Visible = false;
            type_label.Visible = false;
            }

        /// <summary> Установка окна в активное состояние. </summary>
        ///
        /// <remarks> Id, 17.08.2011. </remarks>
        internal void enable_prop()
            {
            type_lview.Visible = true;
            type_cbox.Visible = true;
            type_label.Visible = true;
            }

        /// <summary>The Visio Application property</summary>
        /// <value>Instance of the Visio Application object</value>
        public Microsoft.Office.Interop.Visio.Application
            ParentVisioApplication
            {

            get
                {
                return visioApplication;
                }
            set
                {
                visioApplication = value;
                }
            }

        /// <summary>This is the constructor for the edit_valve_type_form class.
        /// </summary>
        public edit_valve_type_form()
            {

            // Required for Windows Form Designer support
            InitializeComponent();
            }

        /// <summary>This form overrides Dispose to clean up the component list.
        /// </summary>
        /// <param name="disposing">true to release both managed and unmanaged
        /// resources; false to release only unmanaged resources.</param>
        protected override void Dispose( bool disposing )
            {

            if( disposing )
                {
                if( components != null )
                    {
                    components.Dispose();
                    }
                }

            base.Dispose( disposing );
            }

        #region Windows Form Designer generated code
        /// <summary>Required method for Designer support - do not modify
        /// the contents of this method with the code editor.</summary>
        private void InitializeComponent()
            {
			this.type_lview = new System.Windows.Forms.ListView();
			this.channel = ( ( System.Windows.Forms.ColumnHeader ) ( new System.Windows.Forms.ColumnHeader() ) );
			this.value = ( ( System.Windows.Forms.ColumnHeader ) ( new System.Windows.Forms.ColumnHeader() ) );
			this.type_cbox = new System.Windows.Forms.ComboBox();
			this.type_label = new System.Windows.Forms.Label();
			this.no_channel_label = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// type_lview
			// 
			this.type_lview.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
						| System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.type_lview.Columns.AddRange( new System.Windows.Forms.ColumnHeader[] {
            this.channel,
            this.value} );
			this.type_lview.GridLines = true;
			this.type_lview.HideSelection = false;
			this.type_lview.Location = new System.Drawing.Point( 1, 39 );
			this.type_lview.MultiSelect = false;
			this.type_lview.Name = "type_lview";
			this.type_lview.Size = new System.Drawing.Size( 232, 117 );
			this.type_lview.TabIndex = 0;
			this.type_lview.UseCompatibleStateImageBehavior = false;
			this.type_lview.View = System.Windows.Forms.View.Details;
			this.type_lview.Visible = false;
			this.type_lview.SelectedIndexChanged += new System.EventHandler( this.type_lview__SelectedIndexChanged );
			this.type_lview.DoubleClick += new System.EventHandler( this.type_lview__DoubleClick );
			// 
			// channel
			// 
			this.channel.Text = "Канал";
			// 
			// value
			// 
			this.value.Text = "Привязка";
			this.value.Width = 159;
			// 
			// type_cbox
			// 
			this.type_cbox.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.type_cbox.CausesValidation = false;
			this.type_cbox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.type_cbox.Items.AddRange( new object[] {
            "1 КУ",
            "2 КУ",
            "1 КУ 1 ОС",
            "1 КУ 2 ОС",
            "2 КУ 2 ОС",
            "3 КУ 2 ОС",
            "AS-Interface"} );
			this.type_cbox.Location = new System.Drawing.Point( 60, 12 );
			this.type_cbox.Name = "type_cbox";
			this.type_cbox.Size = new System.Drawing.Size( 173, 21 );
			this.type_cbox.TabIndex = 1;
			this.type_cbox.Visible = false;
			this.type_cbox.SelectedIndexChanged += new System.EventHandler( this.type_cbox__SelectedIndexChanged );
			// 
			// type_label
			// 
			this.type_label.AutoSize = true;
			this.type_label.Location = new System.Drawing.Point( 12, 15 );
			this.type_label.Name = "type_label";
			this.type_label.Size = new System.Drawing.Size( 26, 13 );
			this.type_label.TabIndex = 2;
			this.type_label.Text = "Тип";
			this.type_label.Visible = false;
			// 
			// no_channel_label
			// 
			this.no_channel_label.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
						| System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.no_channel_label.AutoSize = true;
			this.no_channel_label.Location = new System.Drawing.Point( 76, 70 );
			this.no_channel_label.Name = "no_channel_label";
			this.no_channel_label.Size = new System.Drawing.Size( 70, 13 );
			this.no_channel_label.TabIndex = 3;
			this.no_channel_label.Text = "Каналов нет";
			// 
			// edit_valve_type_form
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size( 5, 13 );
			this.ClientSize = new System.Drawing.Size( 234, 161 );
			this.Controls.Add( this.type_lview );
			this.Controls.Add( this.type_label );
			this.Controls.Add( this.type_cbox );
			this.Controls.Add( this.no_channel_label );
			this.Name = "edit_valve_type_form";
			this.Text = "Каналы";
			this.ResumeLayout( false );
			this.PerformLayout();

            }
        #endregion

        private void type_lview__DoubleClick( object sender, EventArgs e )
            {
            // Выделяем все доступные клеммы.
            for ( int i = 0; i < Globals.visio_addin.g_PAC_nodes.Count; i++ )
                {
                Globals.visio_addin.g_PAC_nodes[ i ].mark_suitable(
                    type_lview.SelectedItems[ 0 ].Text.Substring( 0,
                    type_lview.SelectedItems[ 0 ].Text.Length - 1 ) );
                }

            // Выделяем задействованные устройством клеммы.
            Globals.visio_addin.cur_sel_dev.select_channels();


            Globals.visio_addin.is_selecting_clamp = true;


            visioApplication.Windows[ ( short ) visio_addin.VISIO_WNDOWS.MAIN ].MouseMove +=
                new Microsoft.Office.Interop.Visio.EWindow_MouseMoveEventHandler(
                Globals.visio_addin.visio_addin__MouseMove );
            }

        private void type_lview__SelectedIndexChanged( object sender, EventArgs e )
            {
            if( type_lview.SelectedItems.Count > 0 )
                {
                Globals.visio_addin.cur_sel_dev.set_active_channel(
                    type_lview.SelectedItems[ 0 ].Text );                
                }
            else
                {
                Globals.visio_addin.cur_sel_dev.set_active_channel( "" );
                }

            Globals.visio_addin.cur_sel_dev.select_channels();
            }

        private void type_cbox__SelectedIndexChanged( object sender, EventArgs e )
            {
            if( Globals.visio_addin.cur_sel_dev.get_sub_type() != 
                type_cbox.SelectedIndex )
                {
                Globals.visio_addin.cur_sel_dev.change_sub_type(
                                        ( device.SUB_TYPES ) type_cbox.SelectedIndex,
                                        Globals.visio_addin.g_PAC_nodes );

                Globals.visio_addin.cur_sel_dev.refresh_edit_window(
                    type_cbox, type_lview, true );
                Globals.visio_addin.cur_sel_dev.select_channels();
                }

            }

        }
 

    /// <summary>This class demonstrates creating an add-on created anchor bar
    /// with a list view control and shows how to set its docking, positioning,
    /// and merge behavior.</summary>
    public class edit_valve_type_form_usage
        {
        internal edit_valve_type_form listForm;

        /// <summary>GUID that identifies the custom anchor window when it 
        /// is merged.</summary>
        private const string customMergeId =
            "{91439584-A97D-46e8-92E3-AD10BA4C8B6B}";

        /// <summary>This constructor is intentionally left blank.</summary>
        public edit_valve_type_form_usage()
            {
            // No initialization is required.
            }

        /// <summary>This method adds an anchor bar, sets the properties of
        /// the anchor bar, and adds a form as contents of the anchor bar. The
        /// form will contain a list view of masters from stencils in the Basic
        /// Flowchart template. The list view items can be dragged onto the
        /// drawing page.</summary>
        /// <param name="visioApplication">A running Visio application</param>
        /// <param name="runningFromAddIn">whether or not we are running from an add-in</param>
        /// <returns>true if successful, otherwise false</returns>
        public bool create_tool_form(
            Microsoft.Office.Interop.Visio.Application visioApplication,
            bool runningFromAddIn )
            {

            if( visioApplication == null )
                {
                return false;
                }

            const string anchorBarTitle = "Каналы";
            const string anchorBarMergeTitle = "Каналы";

            bool returnValue = false;

            Microsoft.Office.Interop.Visio.Window anchorWindow;
                        
            object windowStates;
            object windowTypes;

            try
                {

                // Add a document using the Basic Flowchart template.  This will
                // open several stencils.
                //visioApplication.Documents.Add( templateName );

                // The anchor bar will be docked to the bottom-left corner of the
                // drawing window and set the anchor bar to auto-hide when not in
                // use.
                windowStates = Microsoft.Office.Interop.Visio.
                        VisWindowStates.visWSAnchorTop |
                    Microsoft.Office.Interop.Visio.
                        VisWindowStates.visWSAnchorRight |
                    //Microsoft.Office.Interop.Visio.
                    //    VisWindowStates.visWSAnchorAutoHide |
                    Microsoft.Office.Interop.Visio.
                        VisWindowStates.visWSVisible;

                // The anchor bar is a window created by an add-on
                windowTypes = Microsoft.Office.Interop.Visio.
                    VisWinTypes.visAnchorBarAddon;

                // Add a custom anchor bar window.
                anchorWindow = addAnchorWindow( visioApplication,
                    anchorBarTitle,
                    windowStates,
                    windowTypes );

                // Set the form as contents of the anchor bar.
                listForm = new edit_valve_type_form();
                listForm.ParentVisioApplication = visioApplication;
                addFormToAnchorWindow( anchorWindow,
                    listForm );

                // The MergeID allows the anchor bar window to be identified 
                // when it is merged with another window.
                anchorWindow.MergeID = customMergeId;

                // Allow the anchor window to be merged with other windows that
                // have a zero-length MergeClass property value.
                anchorWindow.MergeClass = "";

                // Set the MergeCaption property with string that is shorter 
                // than the window caption. The MergeCaption property value 
                // appears on the tab of the merged window.
                anchorWindow.MergeCaption = anchorBarMergeTitle;

                // If this function is not running from an add-in, use
                // Application.Run() to run a message loop on the current thread
                // with the form that was just placed into the anchor bar window.
                // When the anchor bar window is closed, or Visio is closed, this
                // function will return, allowing execution to continue.
                if( !runningFromAddIn )
                    {
                    System.Windows.Forms.Application.Run( listForm );
                    }

                returnValue = true;
                }
            catch( Exception err )
                {
                System.Diagnostics.Debug.WriteLine( err.Message );
                throw;
                }

            return returnValue;
            }

        /// <summary>This method adds an anchor bar with the specified
        /// caption, window properties and anchor bar types.</summary>
        /// <param name="visioApplication">Reference to the Visio Application
        /// object</param>
        /// <param name="caption">Anchor bar caption</param>
        /// <param name="windowStates">Properties of the anchor bar</param>
        /// <param name="windowTypes">Built-in or add-on anchor bar</param>
        /// <returns>Created window, otherwise null</returns>
        private Microsoft.Office.Interop.Visio.Window addAnchorWindow(
            Microsoft.Office.Interop.Visio.Application visioApplication,
            string caption,
            object windowStates,
            object windowTypes )
            {

            Microsoft.Office.Interop.Visio.Window anchorWindow = null;

            try
                {

                // Add a new anchor bar with the required information.
                anchorWindow = visioApplication.ActiveWindow.Windows.Add(
                    caption,
                    windowStates,
                    windowTypes,
                    8, 10, 250, 150, 0, 0, 0 );
                }
            catch( Exception err )
                {
                System.Diagnostics.Debug.WriteLine( err.Message );
                throw;
                }

            return anchorWindow;
            }

        /// <summary>This method adds a form as the contents of the anchor bar.
        /// </summary>
        /// <param name="anchorBar">Reference to the anchor bar window</param>
        /// <param name="displayForm">Content of the anchor bar</param>
        private void addFormToAnchorWindow(
            Microsoft.Office.Interop.Visio.Window anchorBar,
            System.Windows.Forms.Form displayForm )
            {

            int left;
            int top;
            int width;
            int height;
            int windowHandle;

            try
                {

                // Show the form as a modeless dialog.
                displayForm.Show();

                // Get the window handle of the form.
                windowHandle = displayForm.Handle.ToInt32();

                // Set the form as a visible child window.
                if( NativeMethods.SetWindowLongW( windowHandle,
                    NativeMethods.GWL_STYLE,
                    NativeMethods.WS_CHILD | NativeMethods.WS_VISIBLE ) == 0 &&
                    System.Runtime.InteropServices.Marshal.GetLastWin32Error() != 0 )
                    {
                    throw new Exception( "Can not set windows long" );
                    }

                // Set the anchor bar window as the parent of the form.
                if( NativeMethods.SetParent( windowHandle,
                    anchorBar.WindowHandle32 ) == 0 )
                    throw new Exception( "Can not set parent" );

                // Set the dock property of the form to fill, so that the form
                // automatically resizes to the size of the anchor bar.
                displayForm.Dock = System.Windows.Forms.DockStyle.Fill;

                // Resize the anchor bar so it will refresh.
                anchorBar.GetWindowRect( out left,
                    out top,
                    out width,
                    out height );
                anchorBar.SetWindowRect( left,
                    top,
                    width,
                    height + 1 );
                }
            catch( Exception err )
                {
                System.Diagnostics.Debug.WriteLine( err.Message );
                throw;
                }
            }

        /// <summary>Explicitly declare calls to unmanaged code inside a
        /// 'NativeMethods' class.  This class does not suppress stack walks for
        /// unmanaged code permission.</summary>
        private class NativeMethods
            {

            /// <summary>Windows constant - Sets a new window style.</summary>
            internal const short GWL_STYLE = ( -16 );

            /// <summary>Windows constant - Creates a child window..</summary>
            internal const int WS_CHILD = 0x40000000;

            /// <summary>Windows constant - Creates a window that is initially
            /// visible.</summary>
            internal const int WS_VISIBLE = 0x10000000;

            /// <summary>Declare a private constructor to prevent new instances
            /// of the NativeMethods class from being created. This constructor
            /// is intentionally left blank.</summary>
            private NativeMethods()
                {

                // No initialization is required.
                }

            /// <summary>Prototype of SetParent() for PInvoke</summary>
            [System.Runtime.InteropServices.DllImport( "user32.dll" )]
            internal static extern int SetParent( int hWndChild,
                int hWndNewParent );

            /// <summary>Prototype of SetWindowLong() for PInvoke</summary>
            [System.Runtime.InteropServices.DllImport( "user32.dll", SetLastError = true )]
            internal static extern int SetWindowLongW( int hwnd,
                int nIndex,
                int dwNewLong );
            }
        }
    }
