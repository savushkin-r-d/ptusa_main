// AnchorBarsUsage.cs
// <copyright>Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
// <summary>This class demonstrates creating an add-on created anchor bar with a
// list view control and shows how to set its docking, positioning, and merge
// behavior.</summary>

using System;

namespace Visio_project_designer
    {

    /// <summary>This class contains the implementation for the form that will
    /// be set as the content of the anchor bar created in the AnchorBarUsage
    /// class.</summary>
    public class AnchorBarForm
        : System.Windows.Forms.Form
        {

        /// <summary>Required list view variable</summary>
        private System.Windows.Forms.ListView myListView;

        /// <summary>Required by the Windows Form Designer</summary>
        private System.ComponentModel.Container components = null;
        private System.Windows.Forms.ComboBox comboBox_type;
        private System.Windows.Forms.Label label_type;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ColumnHeader channel;
        private System.Windows.Forms.ColumnHeader columnHeader2;

        /// <summary>The Visio Application object</summary>
        private Microsoft.Office.Interop.Visio.Application
            visioApplication;

        public void clear_prop()
            {
            myListView.Items.Clear();            
            }

        public void disable_prop()
            {
            myListView.Visible = false;
            comboBox_type.Visible = false;
            label_type.Visible = false;
            }

        public void enable_prop()
            {
            myListView.Visible = true;
            comboBox_type.Visible = true;
            label_type.Visible = true;
            }

        void add_prop( string name, string value )
            {
            System.Windows.Forms.ListViewItem item;
            item = myListView.Items.Add( name );
            item.SubItems.Add( value );
            }

        public void change_type( string type )
            {
            if( type == ( string ) comboBox_type.SelectedItem )
                {
                comboBox_type_SelectedIndexChanged( null, null );
                }
            else
                {
                comboBox_type.SelectedItem = type;
                }            
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

        /// <summary>This is the constructor for the AnchorBarForm class.
        /// </summary>
        public AnchorBarForm()
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
            this.myListView = new System.Windows.Forms.ListView();
            this.comboBox_type = new System.Windows.Forms.ComboBox();
            this.label_type = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.channel = ( ( System.Windows.Forms.ColumnHeader ) ( new System.Windows.Forms.ColumnHeader() ) );
            this.columnHeader2 = ( ( System.Windows.Forms.ColumnHeader ) ( new System.Windows.Forms.ColumnHeader() ) );
            this.SuspendLayout();
            // 
            // myListView
            // 
            this.myListView.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
                        | System.Windows.Forms.AnchorStyles.Left )
                        | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.myListView.Columns.AddRange( new System.Windows.Forms.ColumnHeader[] {
            this.channel,
            this.columnHeader2} );
            this.myListView.Location = new System.Drawing.Point( 1, 43 );
            this.myListView.MultiSelect = false;
            this.myListView.Name = "myListView";
            this.myListView.Size = new System.Drawing.Size( 232, 108 );
            this.myListView.TabIndex = 0;
            this.myListView.UseCompatibleStateImageBehavior = false;
            this.myListView.View = System.Windows.Forms.View.Details;
            this.myListView.Visible = false;
            this.myListView.SelectedIndexChanged += new System.EventHandler( this.myListView_SelectedIndexChanged );
            this.myListView.DoubleClick += new System.EventHandler( this.myListView_DoubleClick );
            // 
            // comboBox_type
            // 
            this.comboBox_type.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left )
                        | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.comboBox_type.CausesValidation = false;
            this.comboBox_type.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_type.Items.AddRange( new object[] {
            "1 КУ",
            "2 КУ",
            "1 КУ 1 ОС",
            "1 КУ 2 ОС",
            "2 КУ 2 ОС"} );
            this.comboBox_type.Location = new System.Drawing.Point( 60, 12 );
            this.comboBox_type.Name = "comboBox_type";
            this.comboBox_type.Size = new System.Drawing.Size( 173, 21 );
            this.comboBox_type.TabIndex = 1;
            this.comboBox_type.Visible = false;
            this.comboBox_type.SelectedIndexChanged += new System.EventHandler( this.comboBox_type_SelectedIndexChanged );
            // 
            // label_type
            // 
            this.label_type.AutoSize = true;
            this.label_type.Location = new System.Drawing.Point( 12, 15 );
            this.label_type.Name = "label_type";
            this.label_type.Size = new System.Drawing.Size( 26, 13 );
            this.label_type.TabIndex = 2;
            this.label_type.Text = "Тип";
            this.label_type.Visible = false;
            // 
            // label2
            // 
            this.label2.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
                        | System.Windows.Forms.AnchorStyles.Left )
                        | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point( 76, 70 );
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size( 70, 13 );
            this.label2.TabIndex = 3;
            this.label2.Text = "Каналов нет";
            // 
            // channel
            // 
            this.channel.Text = "Канал";
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Привязка";
            this.columnHeader2.Width = 159;
            // 
            // AnchorBarForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size( 5, 13 );
            this.ClientSize = new System.Drawing.Size( 234, 152 );
            this.Controls.Add( this.myListView );
            this.Controls.Add( this.label_type );
            this.Controls.Add( this.comboBox_type );
            this.Controls.Add( this.label2 );
            this.Name = "AnchorBarForm";
            this.Text = "Каналы";
            this.Load += new System.EventHandler( this.AnchorBarForm_Load );
            this.ResumeLayout( false );
            this.PerformLayout();

            }
        #endregion

        /// <summary>This method adds the column headers and an item for each
        /// master to the list view of the form.</summary>
        /// <param name="sender">Parameter not used by this implementation
        /// </param>
        /// <param name="e">Parameter not used by this implementation</param>
        private void AnchorBarForm_Load( object sender,
            EventArgs e )
            {
            }

        private void myListView_DoubleClick( object sender, EventArgs e )
            {
            //myListView.SelectedItems[ 0 ].Font. BackColor = System.Drawing.Color.FromName( "DarkSeaGreen" );
            //myListView.Refresh();
            //myListView.HideSelection = true;
            //myListView.SelectedItems.Clear();

            // Выделяем все задействованные клеммы.
            for( int i = 0; i < myListView.Items.Count; i++ )
                {
                string channel = myListView.Items[ i ].SubItems[ 1 ].Text;
                Globals.ThisAddIn.select_channel( channel, 2 );
                }

            // Выделяем текущую клемму.    
            string tmp = myListView.SelectedItems[ 0 ].SubItems[ 1 ].Text;
            Globals.ThisAddIn.select_channel( tmp, 5 );
            }

        private void comboBox_type_SelectedIndexChanged( object sender, EventArgs e )
            {
            Microsoft.Office.Interop.Visio.Shape shape =
                visioApplication.Windows[ ( short ) ThisAddIn.VISIO_WNDOWS.IO_EDIT ].Selection[ 1 ];

            string tmp = ( string ) comboBox_type.SelectedItem;
            shape.Cells[ "Prop.type" ].Formula = 
                string.Format( "INDEX({0};Prop.type.Format)", comboBox_type.SelectedIndex );

            //Обновляем таблицу свойств.
            clear_prop();

            add_prop( "DO1",
                shape.Cells[ "Prop.DO1" ].Formula );

            switch( ( string ) comboBox_type.SelectedItem )
                {
                case "2 КУ":
                    add_prop( "DO2",
                        shape.Cells[ "Prop.DO2" ].Formula );
                    break;

                case "1 КУ 1 ОС":
                    add_prop( "DI1",
                        shape.Cells[ "Prop.DI1" ].Formula );
                    break;

                case "1 КУ 2 ОС":
                    add_prop( "DI1",
                        shape.Cells[ "Prop.DI1" ].Formula );
                    add_prop( "DI2",
                        shape.Cells[ "Prop.DI2" ].Formula );
                    break;

                case "2 КУ 2 ОС":
                    add_prop( "DO2",
                        shape.Cells[ "Prop.DO2" ].Formula );

                    add_prop( "DI1",
                        shape.Cells[ "Prop.DI1" ].Formula );
                    add_prop( "DI2",
                        shape.Cells[ "Prop.DI2" ].Formula );
                    break;
                }
            }

        private void myListView_SelectedIndexChanged( object sender, EventArgs e )
            {
            // Выделяем все задействованные клеммы.
            for( int i = 0; i < myListView.Items.Count; i++  )
                {
                string channel = myListView.Items[ i ].SubItems[ 1 ].Text;
                Globals.ThisAddIn.select_channel( channel, 2 );
                }

            // Выделяем текущую клемму.
            if( myListView.SelectedItems.Count > 0 )
                {
                string channel = myListView.SelectedItems[ 0 ].SubItems[ 1 ].Text;
                Globals.ThisAddIn.select_channel( channel, 5 );
                }          
            }

        }
    }

namespace Visio_project_designer
    {   

    /// <summary>This class demonstrates creating an add-on created anchor bar
    /// with a list view control and shows how to set its docking, positioning,
    /// and merge behavior.</summary>
    public class AnchorBarsUsage
        {
        public AnchorBarForm listForm;

        /// <summary>GUID that identifies the custom anchor window when it 
        /// is merged.</summary>
        private const string customMergeId =
            "{91439584-A97D-46e8-92E3-AD10BA4C8B6B}";

        /// <summary>This constructor is intentionally left blank.</summary>
        public AnchorBarsUsage()
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
        public bool DemoAnchorBar(
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
                listForm = new AnchorBarForm();
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
