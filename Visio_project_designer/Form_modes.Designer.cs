namespace Visio_project_designer
    {
    partial class Form_modes
        {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose( bool disposing )
            {
            if ( disposing && ( components != null ) )
                {
                components.Dispose();
                }
            base.Dispose( disposing );
            }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
            {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.TreeNode treeNode1 = new System.Windows.Forms.TreeNode( "Подача в танк" );
            System.Windows.Forms.TreeNode treeNode2 = new System.Windows.Forms.TreeNode( "Подача в байпас" );
            System.Windows.Forms.TreeNode treeNode3 = new System.Windows.Forms.TreeNode( "Дренаж" );
            System.Windows.Forms.TreeNode treeNode4 = new System.Windows.Forms.TreeNode( "Мойка", new System.Windows.Forms.TreeNode[] {
            treeNode1,
            treeNode2,
            treeNode3} );
            System.Windows.Forms.TreeNode treeNode5 = new System.Windows.Forms.TreeNode( "Подача в дренаж" );
            System.Windows.Forms.TreeNode treeNode6 = new System.Windows.Forms.TreeNode( "Подача в Д+Т" );
            System.Windows.Forms.TreeNode treeNode7 = new System.Windows.Forms.TreeNode( "Подача в танк" );
            System.Windows.Forms.TreeNode treeNode8 = new System.Windows.Forms.TreeNode( "Наполнение", new System.Windows.Forms.TreeNode[] {
            treeNode5,
            treeNode6,
            treeNode7} );
            System.Windows.Forms.TreeNode treeNode9 = new System.Windows.Forms.TreeNode( "Выдача" );
            System.Windows.Forms.TreeNode treeNode10 = new System.Windows.Forms.TreeNode( "Выдача", new System.Windows.Forms.TreeNode[] {
            treeNode9} );
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.TreeNode treeNode11 = new System.Windows.Forms.TreeNode( "Время_работы_режима" );
            System.Windows.Forms.TreeNode treeNode12 = new System.Windows.Forms.TreeNode( "Номер_следующего_режима" );
            System.Windows.Forms.TreeNode treeNode13 = new System.Windows.Forms.TreeNode( "Параметры", new System.Windows.Forms.TreeNode[] {
            treeNode11,
            treeNode12} );
            System.Windows.Forms.TreeNode treeNode14 = new System.Windows.Forms.TreeNode( "Включать" );
            System.Windows.Forms.TreeNode treeNode15 = new System.Windows.Forms.TreeNode( "Выключать" );
            System.Windows.Forms.TreeNode treeNode16 = new System.Windows.Forms.TreeNode( "Верхний_флип" );
            System.Windows.Forms.TreeNode treeNode17 = new System.Windows.Forms.TreeNode( "Нижний_флип" );
            System.Windows.Forms.TreeNode treeNode18 = new System.Windows.Forms.TreeNode( "Включать_по_завершению" );
            System.Windows.Forms.TreeNode treeNode19 = new System.Windows.Forms.TreeNode( "Устройства", new System.Windows.Forms.TreeNode[] {
            treeNode14,
            treeNode15,
            treeNode16,
            treeNode17,
            treeNode18} );
            System.Windows.Forms.TreeNode treeNode20 = new System.Windows.Forms.TreeNode( "Включающие_режим" );
            System.Windows.Forms.TreeNode treeNode21 = new System.Windows.Forms.TreeNode( "Выключающие_режим" );
            System.Windows.Forms.TreeNode treeNode22 = new System.Windows.Forms.TreeNode( "Необходимые_для_включения" );
            System.Windows.Forms.TreeNode treeNode23 = new System.Windows.Forms.TreeNode( "Отправляемые_режимом" );
            System.Windows.Forms.TreeNode treeNode24 = new System.Windows.Forms.TreeNode( "Управляющие_устройствами" );
            System.Windows.Forms.TreeNode treeNode25 = new System.Windows.Forms.TreeNode( "Импульсные" );
            System.Windows.Forms.TreeNode treeNode26 = new System.Windows.Forms.TreeNode( "Сигналы", new System.Windows.Forms.TreeNode[] {
            treeNode20,
            treeNode21,
            treeNode22,
            treeNode23,
            treeNode24,
            treeNode25} );
            System.Windows.Forms.TreeNode treeNode27 = new System.Windows.Forms.TreeNode( "Блокирующие_режимы_гребенки" );
            System.Windows.Forms.TreeNode treeNode28 = new System.Windows.Forms.TreeNode( "Блокирующие_режимы_танков" );
            System.Windows.Forms.TreeNode treeNode29 = new System.Windows.Forms.TreeNode( "Блокирующие_устройства" );
            System.Windows.Forms.TreeNode treeNode30 = new System.Windows.Forms.TreeNode( "Ограничения", new System.Windows.Forms.TreeNode[] {
            treeNode27,
            treeNode28,
            treeNode29} );
            System.Windows.Forms.TreeNode treeNode31 = new System.Windows.Forms.TreeNode( "Proporties", new System.Windows.Forms.TreeNode[] {
            treeNode13,
            treeNode19,
            treeNode26,
            treeNode30} );
            System.Windows.Forms.TreeNode treeNode32 = new System.Windows.Forms.TreeNode( "TreeView" );
            this.contextMenuStrip2 = new System.Windows.Forms.ContextMenuStrip( this.components );
            this.add_prop = new System.Windows.Forms.ToolStripMenuItem();
            this.Devices = new System.Windows.Forms.ToolStripMenuItem();
            this.Signals = new System.Windows.Forms.ToolStripMenuItem();
            this.Conditions = new System.Windows.Forms.ToolStripMenuItem();
            this.Parameters = new System.Windows.Forms.ToolStripMenuItem();
            this.set_def_prop = new System.Windows.Forms.ToolStripMenuItem();
            this.treeView_modes = new System.Windows.Forms.TreeView();
            this.contextMenuStrip3 = new System.Windows.Forms.ContextMenuStrip( this.components );
            this.ContextMenuStrip3_insert_mode = new System.Windows.Forms.ToolStripMenuItem();
            this.ContextMenuStrip3_insert_step = new System.Windows.Forms.ToolStripMenuItem();
            this.delete_mode = new System.Windows.Forms.ToolStripMenuItem();
            this.ContextMenuStrip3_update = new System.Windows.Forms.ToolStripMenuItem();
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.number = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.par_name = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.val = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.numericUpDown1 = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.label2 = new System.Windows.Forms.Label();
            this.numericUpDown2 = new System.Windows.Forms.NumericUpDown();
            this.dataGridView2 = new System.Windows.Forms.DataGridView();
            this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip( this.components );
            this.выбратьИзСпискаToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.выбратьНаСхемеToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.treeView_prop = new System.Windows.Forms.TreeView();
            this.dev_list = new System.Windows.Forms.ListBox();
            this.treeView2 = new System.Windows.Forms.TreeView();
            this.label16 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.contextMenuStrip4 = new System.Windows.Forms.ContextMenuStrip( this.components );
            this.ContextMenuStrip4_select_on_map = new System.Windows.Forms.ToolStripMenuItem();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.contextMenuStrip2.SuspendLayout();
            this.contextMenuStrip3.SuspendLayout();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.dataGridView1 ) ).BeginInit();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.numericUpDown1 ) ).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.numericUpDown2 ) ).BeginInit();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.dataGridView2 ) ).BeginInit();
            this.contextMenuStrip1.SuspendLayout();
            this.contextMenuStrip4.SuspendLayout();
            this.SuspendLayout();
            // 
            // contextMenuStrip2
            // 
            this.contextMenuStrip2.Items.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.add_prop,
            this.set_def_prop} );
            this.contextMenuStrip2.Name = "contextMenuStrip2";
            this.contextMenuStrip2.Size = new System.Drawing.Size( 249, 48 );
            // 
            // add_prop
            // 
            this.add_prop.DropDownItems.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.Devices,
            this.Signals,
            this.Conditions,
            this.Parameters} );
            this.add_prop.Name = "add_prop";
            this.add_prop.Size = new System.Drawing.Size( 248, 22 );
            this.add_prop.Text = "Добавить характеристику";
            // 
            // Devices
            // 
            this.Devices.Checked = true;
            this.Devices.CheckOnClick = true;
            this.Devices.CheckState = System.Windows.Forms.CheckState.Checked;
            this.Devices.Name = "Devices";
            this.Devices.Size = new System.Drawing.Size( 148, 22 );
            this.Devices.Text = "Устройства";
            this.Devices.CheckStateChanged += new System.EventHandler( this.ToolStripMenuItem_CheckStateChanged );
            // 
            // Signals
            // 
            this.Signals.Checked = true;
            this.Signals.CheckOnClick = true;
            this.Signals.CheckState = System.Windows.Forms.CheckState.Checked;
            this.Signals.Name = "Signals";
            this.Signals.Size = new System.Drawing.Size( 148, 22 );
            this.Signals.Text = "Сигналы";
            this.Signals.CheckStateChanged += new System.EventHandler( this.ToolStripMenuItem_CheckStateChanged );
            // 
            // Conditions
            // 
            this.Conditions.Checked = true;
            this.Conditions.CheckOnClick = true;
            this.Conditions.CheckState = System.Windows.Forms.CheckState.Checked;
            this.Conditions.Name = "Conditions";
            this.Conditions.Size = new System.Drawing.Size( 148, 22 );
            this.Conditions.Text = "Ограничения";
            this.Conditions.CheckStateChanged += new System.EventHandler( this.ToolStripMenuItem_CheckStateChanged );
            // 
            // Parameters
            // 
            this.Parameters.Checked = true;
            this.Parameters.CheckOnClick = true;
            this.Parameters.CheckState = System.Windows.Forms.CheckState.Checked;
            this.Parameters.Name = "Parameters";
            this.Parameters.Size = new System.Drawing.Size( 148, 22 );
            this.Parameters.Text = "Параметры";
            this.Parameters.CheckStateChanged += new System.EventHandler( this.ToolStripMenuItem_CheckStateChanged );
            // 
            // set_def_prop
            // 
            this.set_def_prop.Name = "set_def_prop";
            this.set_def_prop.Size = new System.Drawing.Size( 248, 22 );
            this.set_def_prop.Text = "Характеристики по умолчанию";
            this.set_def_prop.Click += new System.EventHandler( this.ContextMenuStrip2_set_def_prop_Click );
            // 
            // treeView_modes
            // 
            this.treeView_modes.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
                        | System.Windows.Forms.AnchorStyles.Left )
                        | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.treeView_modes.ContextMenuStrip = this.contextMenuStrip3;
            this.treeView_modes.HideSelection = false;
            this.treeView_modes.LabelEdit = true;
            this.treeView_modes.Location = new System.Drawing.Point( 2, 21 );
            this.treeView_modes.Name = "treeView_modes";
            treeNode1.Name = "Node1";
            treeNode1.Text = "Подача в танк";
            treeNode2.Name = "Node2";
            treeNode2.Text = "Подача в байпас";
            treeNode3.Name = "Node3";
            treeNode3.Text = "Дренаж";
            treeNode4.Name = "Node0";
            treeNode4.Text = "Мойка";
            treeNode5.Name = "Node5";
            treeNode5.Text = "Подача в дренаж";
            treeNode6.Name = "Node0";
            treeNode6.Text = "Подача в Д+Т";
            treeNode7.Name = "Node2";
            treeNode7.Text = "Подача в танк";
            treeNode8.Name = "Node4";
            treeNode8.Text = "Наполнение";
            treeNode9.Name = "Node3";
            treeNode9.Text = "Выдача";
            treeNode10.Name = "Node1";
            treeNode10.Text = "Выдача";
            this.treeView_modes.Nodes.AddRange( new System.Windows.Forms.TreeNode[] {
            treeNode4,
            treeNode8,
            treeNode10} );
            this.treeView_modes.Size = new System.Drawing.Size( 204, 430 );
            this.treeView_modes.TabIndex = 3;
            this.treeView_modes.BeforeLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler( this.treeView_modes_BeforeLabelEdit );
            this.treeView_modes.AfterLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler( this.treeView_modes_AfterLabelEdit );
            this.treeView_modes.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler( this.treeView_modes_BeforeSelect );
            this.treeView_modes.AfterSelect += new System.Windows.Forms.TreeViewEventHandler( this.treeView_modes_AfterSelect );
            this.treeView_modes.MouseClick += new System.Windows.Forms.MouseEventHandler( this.treeView_modes_MouseClick );
            // 
            // contextMenuStrip3
            // 
            this.contextMenuStrip3.Items.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.ContextMenuStrip3_insert_mode,
            this.ContextMenuStrip3_insert_step,
            this.delete_mode,
            this.ContextMenuStrip3_update} );
            this.contextMenuStrip3.Name = "contextMenuStrip3";
            this.contextMenuStrip3.Size = new System.Drawing.Size( 224, 92 );
            // 
            // ContextMenuStrip3_insert_mode
            // 
            this.ContextMenuStrip3_insert_mode.Name = "ContextMenuStrip3_insert_mode";
            this.ContextMenuStrip3_insert_mode.Size = new System.Drawing.Size( 223, 22 );
            this.ContextMenuStrip3_insert_mode.Text = "Вставить режим";
            this.ContextMenuStrip3_insert_mode.Click += new System.EventHandler( this.ContextMenuStrip3_insert_mode_Click );
            // 
            // ContextMenuStrip3_insert_step
            // 
            this.ContextMenuStrip3_insert_step.Name = "ContextMenuStrip3_insert_step";
            this.ContextMenuStrip3_insert_step.Size = new System.Drawing.Size( 223, 22 );
            this.ContextMenuStrip3_insert_step.Text = "Вставить шаг";
            this.ContextMenuStrip3_insert_step.Click += new System.EventHandler( this.ContextMenuStrip3_insert_step_Click );
            // 
            // delete_mode
            // 
            this.delete_mode.Name = "delete_mode";
            this.delete_mode.Size = new System.Drawing.Size( 223, 22 );
            this.delete_mode.Text = "Удалить режим";
            this.delete_mode.Click += new System.EventHandler( this.ContextMenuStrip3_delete_mode_Click );
            // 
            // ContextMenuStrip3_update
            // 
            this.ContextMenuStrip3_update.Name = "ContextMenuStrip3_update";
            this.ContextMenuStrip3_update.Size = new System.Drawing.Size( 223, 22 );
            this.ContextMenuStrip3_update.Text = "Обновить дерево режимов";
            this.ContextMenuStrip3_update.Click += new System.EventHandler( this.ContextMenuStrip3_update_Click );
            // 
            // dataGridView1
            // 
            this.dataGridView1.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
                        | System.Windows.Forms.AnchorStyles.Left )
                        | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.dataGridView1.BackgroundColor = System.Drawing.SystemColors.ControlDark;
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Columns.AddRange( new System.Windows.Forms.DataGridViewColumn[] {
            this.number,
            this.par_name,
            this.val} );
            this.dataGridView1.Location = new System.Drawing.Point( 3, 31 );
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.Size = new System.Drawing.Size( 594, 152 );
            this.dataGridView1.TabIndex = 5;
            this.dataGridView1.RowsAdded += new System.Windows.Forms.DataGridViewRowsAddedEventHandler( this.dataGridView1_RowsAdded );
            this.dataGridView1.KeyDown += new System.Windows.Forms.KeyEventHandler( this.dataGridView1_KeyDown );
            // 
            // number
            // 
            dataGridViewCellStyle1.Format = "N0";
            dataGridViewCellStyle1.NullValue = null;
            this.number.DefaultCellStyle = dataGridViewCellStyle1;
            this.number.HeaderText = "№";
            this.number.Name = "number";
            this.number.Width = 30;
            // 
            // par_name
            // 
            this.par_name.HeaderText = "Название параметра";
            this.par_name.Name = "par_name";
            this.par_name.Width = 400;
            // 
            // val
            // 
            this.val.HeaderText = "Значение по умолчанию";
            this.val.Name = "val";
            // 
            // numericUpDown1
            // 
            this.numericUpDown1.Location = new System.Drawing.Point( 139, 6 );
            this.numericUpDown1.Name = "numericUpDown1";
            this.numericUpDown1.Size = new System.Drawing.Size( 49, 20 );
            this.numericUpDown1.TabIndex = 6;
            this.numericUpDown1.ValueChanged += new System.EventHandler( this.numericUpDown1_ValueChanged );
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point( 3, 8 );
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size( 130, 13 );
            this.label1.TabIndex = 7;
            this.label1.Text = "Количество параметров";
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.tabControl1.Controls.Add( this.tabPage1 );
            this.tabControl1.Controls.Add( this.tabPage2 );
            this.tabControl1.Location = new System.Drawing.Point( 2, 453 );
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size( 608, 215 );
            this.tabControl1.TabIndex = 9;
            // 
            // tabPage1
            // 
            this.tabPage1.BackColor = System.Drawing.Color.LightSkyBlue;
            this.tabPage1.Controls.Add( this.label1 );
            this.tabPage1.Controls.Add( this.numericUpDown1 );
            this.tabPage1.Controls.Add( this.dataGridView1 );
            this.tabPage1.ForeColor = System.Drawing.SystemColors.ControlText;
            this.tabPage1.Location = new System.Drawing.Point( 4, 22 );
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding( 3 );
            this.tabPage1.Size = new System.Drawing.Size( 600, 189 );
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Сбрасываемые параметры";
            // 
            // tabPage2
            // 
            this.tabPage2.BackColor = System.Drawing.SystemColors.HighlightText;
            this.tabPage2.Controls.Add( this.label2 );
            this.tabPage2.Controls.Add( this.numericUpDown2 );
            this.tabPage2.Controls.Add( this.dataGridView2 );
            this.tabPage2.Location = new System.Drawing.Point( 4, 22 );
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding( 3 );
            this.tabPage2.Size = new System.Drawing.Size( 600, 189 );
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Сохраняемые параметры";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point( 3, 8 );
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size( 130, 13 );
            this.label2.TabIndex = 10;
            this.label2.Text = "Количество параметров";
            // 
            // numericUpDown2
            // 
            this.numericUpDown2.Location = new System.Drawing.Point( 139, 6 );
            this.numericUpDown2.Name = "numericUpDown2";
            this.numericUpDown2.Size = new System.Drawing.Size( 49, 20 );
            this.numericUpDown2.TabIndex = 9;
            this.numericUpDown2.ValueChanged += new System.EventHandler( this.numericUpDown2_ValueChanged );
            // 
            // dataGridView2
            // 
            this.dataGridView2.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
                        | System.Windows.Forms.AnchorStyles.Left )
                        | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.dataGridView2.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView2.Columns.AddRange( new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn1,
            this.dataGridViewTextBoxColumn2,
            this.dataGridViewTextBoxColumn3} );
            this.dataGridView2.Location = new System.Drawing.Point( 1, 29 );
            this.dataGridView2.Name = "dataGridView2";
            this.dataGridView2.Size = new System.Drawing.Size( 492, 202 );
            this.dataGridView2.TabIndex = 8;
            this.dataGridView2.RowsAdded += new System.Windows.Forms.DataGridViewRowsAddedEventHandler( this.dataGridView2_RowsAdded );
            this.dataGridView2.KeyDown += new System.Windows.Forms.KeyEventHandler( this.dataGridView2_KeyDown );
            // 
            // dataGridViewTextBoxColumn1
            // 
            dataGridViewCellStyle2.Format = "N0";
            dataGridViewCellStyle2.NullValue = null;
            this.dataGridViewTextBoxColumn1.DefaultCellStyle = dataGridViewCellStyle2;
            this.dataGridViewTextBoxColumn1.HeaderText = "№";
            this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            this.dataGridViewTextBoxColumn1.Width = 30;
            // 
            // dataGridViewTextBoxColumn2
            // 
            this.dataGridViewTextBoxColumn2.HeaderText = "Название параметра";
            this.dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
            this.dataGridViewTextBoxColumn2.Width = 300;
            // 
            // dataGridViewTextBoxColumn3
            // 
            this.dataGridViewTextBoxColumn3.HeaderText = "Значение по умолчанию";
            this.dataGridViewTextBoxColumn3.Name = "dataGridViewTextBoxColumn3";
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.выбратьИзСпискаToolStripMenuItem,
            this.выбратьНаСхемеToolStripMenuItem} );
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size( 178, 48 );
            // 
            // выбратьИзСпискаToolStripMenuItem
            // 
            this.выбратьИзСпискаToolStripMenuItem.Name = "выбратьИзСпискаToolStripMenuItem";
            this.выбратьИзСпискаToolStripMenuItem.Size = new System.Drawing.Size( 177, 22 );
            this.выбратьИзСпискаToolStripMenuItem.Text = "Выбрать из списка";
            // 
            // выбратьНаСхемеToolStripMenuItem
            // 
            this.выбратьНаСхемеToolStripMenuItem.Name = "выбратьНаСхемеToolStripMenuItem";
            this.выбратьНаСхемеToolStripMenuItem.Size = new System.Drawing.Size( 177, 22 );
            this.выбратьНаСхемеToolStripMenuItem.Text = "Выбрать на схеме";
            // 
            // treeView_prop
            // 
            this.treeView_prop.HideSelection = false;
            this.treeView_prop.Location = new System.Drawing.Point( 212, 21 );
            this.treeView_prop.Name = "treeView_prop";
            treeNode11.Name = "Node11";
            treeNode11.Text = "Время_работы_режима";
            treeNode12.Name = "Node12";
            treeNode12.Text = "Номер_следующего_режима";
            treeNode13.Name = "Parameters";
            treeNode13.Text = "Параметры";
            treeNode14.Name = "Node13";
            treeNode14.Text = "Включать";
            treeNode15.Name = "Node14";
            treeNode15.Text = "Выключать";
            treeNode16.Name = "Node23";
            treeNode16.Text = "Верхний_флип";
            treeNode17.Name = "Node24";
            treeNode17.Text = "Нижний_флип";
            treeNode18.Name = "Node27";
            treeNode18.Text = "Включать_по_завершению";
            treeNode19.Name = "Devices";
            treeNode19.Text = "Устройства";
            treeNode20.Name = "Node15";
            treeNode20.Text = "Включающие_режим";
            treeNode21.Name = "Node16";
            treeNode21.Text = "Выключающие_режим";
            treeNode22.Name = "Node29";
            treeNode22.Text = "Необходимые_для_включения";
            treeNode23.Name = "Node30";
            treeNode23.Text = "Отправляемые_режимом";
            treeNode24.Name = "Node31";
            treeNode24.Text = "Управляющие_устройствами";
            treeNode25.Name = "Node33";
            treeNode25.Text = "Импульсные";
            treeNode26.Name = "Signals";
            treeNode26.Text = "Сигналы";
            treeNode27.Name = "Node17";
            treeNode27.Text = "Блокирующие_режимы_гребенки";
            treeNode28.Name = "Node18";
            treeNode28.Text = "Блокирующие_режимы_танков";
            treeNode29.Name = "Node34";
            treeNode29.Text = "Блокирующие_устройства";
            treeNode30.Name = "Conditions";
            treeNode30.Text = "Ограничения";
            treeNode31.Name = "Proporties";
            treeNode31.Text = "Proporties";
            this.treeView_prop.Nodes.AddRange( new System.Windows.Forms.TreeNode[] {
            treeNode31} );
            this.treeView_prop.Size = new System.Drawing.Size( 264, 430 );
            this.treeView_prop.TabIndex = 10;
            this.treeView_prop.AfterSelect += new System.Windows.Forms.TreeViewEventHandler( this.treeView_prop_AfterSelect );
            this.treeView_prop.KeyDown += new System.Windows.Forms.KeyEventHandler( this.treeView_prop_KeyDown );
            this.treeView_prop.MouseClick += new System.Windows.Forms.MouseEventHandler( this.treeView_prop_MouseClick );
            // 
            // dev_list
            // 
            this.dev_list.FormattingEnabled = true;
            this.dev_list.Location = new System.Drawing.Point( 680, 19 );
            this.dev_list.Name = "dev_list";
            this.dev_list.Size = new System.Drawing.Size( 185, 433 );
            this.dev_list.Sorted = true;
            this.dev_list.TabIndex = 11;
            this.dev_list.DoubleClick += new System.EventHandler( this.dev_list_DoubleClick );
            // 
            // treeView2
            // 
            this.treeView2.CheckBoxes = true;
            this.treeView2.Location = new System.Drawing.Point( 490, 21 );
            this.treeView2.Name = "treeView2";
            treeNode32.Name = "Node0";
            treeNode32.Text = "TreeView";
            this.treeView2.Nodes.AddRange( new System.Windows.Forms.TreeNode[] {
            treeNode32} );
            this.treeView2.Size = new System.Drawing.Size( 183, 430 );
            this.treeView2.TabIndex = 15;
            this.treeView2.Visible = false;
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point( 11, 4 );
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size( 95, 13 );
            this.label16.TabIndex = 18;
            this.label16.Text = "Режимы объекта";
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point( 223, 4 );
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size( 133, 13 );
            this.label17.TabIndex = 18;
            this.label17.Text = "Характеристики режима";
            // 
            // contextMenuStrip4
            // 
            this.contextMenuStrip4.Items.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.ContextMenuStrip4_select_on_map} );
            this.contextMenuStrip4.Name = "contextMenuStrip4";
            this.contextMenuStrip4.Size = new System.Drawing.Size( 173, 26 );
            // 
            // ContextMenuStrip4_select_on_map
            // 
            this.ContextMenuStrip4_select_on_map.Name = "ContextMenuStrip4_select_on_map";
            this.ContextMenuStrip4_select_on_map.Size = new System.Drawing.Size( 172, 22 );
            this.ContextMenuStrip4_select_on_map.Text = "Выбрать на схеме";
            this.ContextMenuStrip4_select_on_map.Click += new System.EventHandler( this.ContextMenuStrip4_select_on_map_Click );
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point( 500, 4 );
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size( 127, 13 );
            this.label3.TabIndex = 18;
            this.label3.Text = "Режимы всех объектов";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point( 693, 3 );
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size( 111, 13 );
            this.label4.TabIndex = 18;
            this.label4.Text = "Устройства проекта";
            // 
            // Form_modes
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size( 872, 670 );
            this.Controls.Add( this.label16 );
            this.Controls.Add( this.treeView_modes );
            this.Controls.Add( this.treeView2 );
            this.Controls.Add( this.label17 );
            this.Controls.Add( this.tabControl1 );
            this.Controls.Add( this.dev_list );
            this.Controls.Add( this.treeView_prop );
            this.Controls.Add( this.label3 );
            this.Controls.Add( this.label4 );
            this.Name = "Form_modes";
            this.Text = "Заполнение режимов объекта";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler( this.Form_modes_FormClosing );
            this.Shown += new System.EventHandler( this.Form_modes_Shown );
            this.contextMenuStrip2.ResumeLayout( false );
            this.contextMenuStrip3.ResumeLayout( false );
            ( ( System.ComponentModel.ISupportInitialize ) ( this.dataGridView1 ) ).EndInit();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.numericUpDown1 ) ).EndInit();
            this.tabControl1.ResumeLayout( false );
            this.tabPage1.ResumeLayout( false );
            this.tabPage1.PerformLayout();
            this.tabPage2.ResumeLayout( false );
            this.tabPage2.PerformLayout();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.numericUpDown2 ) ).EndInit();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.dataGridView2 ) ).EndInit();
            this.contextMenuStrip1.ResumeLayout( false );
            this.contextMenuStrip4.ResumeLayout( false );
            this.ResumeLayout( false );
            this.PerformLayout();

            }

        #endregion

        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.NumericUpDown numericUpDown1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.NumericUpDown numericUpDown2;
        private System.Windows.Forms.DataGridView dataGridView2;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn3;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem выбратьИзСпискаToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem выбратьНаСхемеToolStripMenuItem;
        private System.Windows.Forms.ListBox dev_list;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip2;
        private System.Windows.Forms.TreeView treeView2;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.DataGridViewTextBoxColumn number;
        private System.Windows.Forms.DataGridViewTextBoxColumn par_name;
        private System.Windows.Forms.DataGridViewTextBoxColumn val;
        private System.Windows.Forms.ToolStripMenuItem add_prop;
        private System.Windows.Forms.ToolStripMenuItem Devices;
        private System.Windows.Forms.ToolStripMenuItem Signals;
        private System.Windows.Forms.ToolStripMenuItem Conditions;
        private System.Windows.Forms.ToolStripMenuItem Parameters;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip3;
        private System.Windows.Forms.ToolStripMenuItem ContextMenuStrip3_insert_mode;
        private System.Windows.Forms.ToolStripMenuItem delete_mode;
        public System.Windows.Forms.TreeView treeView_prop;
        private System.Windows.Forms.ToolStripMenuItem ContextMenuStrip3_insert_step;
        private System.Windows.Forms.ToolStripMenuItem set_def_prop;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip4;
        private System.Windows.Forms.ToolStripMenuItem ContextMenuStrip4_select_on_map;
        public System.Windows.Forms.TreeView treeView_modes;
        private System.Windows.Forms.ToolStripMenuItem ContextMenuStrip3_update;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        }
    }