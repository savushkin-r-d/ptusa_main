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
            System.Windows.Forms.TreeNode treeNode1 = new System.Windows.Forms.TreeNode( "Node1" );
            System.Windows.Forms.TreeNode treeNode2 = new System.Windows.Forms.TreeNode( "Node2" );
            System.Windows.Forms.TreeNode treeNode3 = new System.Windows.Forms.TreeNode( "Node3" );
            System.Windows.Forms.TreeNode treeNode4 = new System.Windows.Forms.TreeNode( "Node0", new System.Windows.Forms.TreeNode[] {
            treeNode1,
            treeNode2,
            treeNode3} );
            System.Windows.Forms.TreeNode treeNode5 = new System.Windows.Forms.TreeNode( "Node5" );
            System.Windows.Forms.TreeNode treeNode6 = new System.Windows.Forms.TreeNode( "Node4", new System.Windows.Forms.TreeNode[] {
            treeNode5} );
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            this.treeView_modes = new System.Windows.Forms.TreeView();
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
            this.tabControl2 = new System.Windows.Forms.TabControl();
            this.Page_dev = new System.Windows.Forms.TabPage();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.listBox4 = new System.Windows.Forms.ListBox();
            this.listBox2 = new System.Windows.Forms.ListBox();
            this.listBox3 = new System.Windows.Forms.ListBox();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.listBox_open = new System.Windows.Forms.ListBox();
            this.listBox_close = new System.Windows.Forms.ListBox();
            this.Page_sign = new System.Windows.Forms.TabPage();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.listBox5 = new System.Windows.Forms.ListBox();
            this.listBox6 = new System.Windows.Forms.ListBox();
            this.listBox7 = new System.Windows.Forms.ListBox();
            this.listBox8 = new System.Windows.Forms.ListBox();
            this.listBox9 = new System.Windows.Forms.ListBox();
            this.listBox11 = new System.Windows.Forms.ListBox();
            this.listBox10 = new System.Windows.Forms.ListBox();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip( this.components );
            this.выбратьИзСпискаToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.выбратьНаСхемеToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.dataGridView1 ) ).BeginInit();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.numericUpDown1 ) ).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.numericUpDown2 ) ).BeginInit();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.dataGridView2 ) ).BeginInit();
            this.tabControl2.SuspendLayout();
            this.Page_dev.SuspendLayout();
            this.Page_sign.SuspendLayout();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // treeView_modes
            // 
            this.treeView_modes.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
                        | System.Windows.Forms.AnchorStyles.Left )
                        | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.treeView_modes.Location = new System.Drawing.Point( 2, 0 );
            this.treeView_modes.Name = "treeView_modes";
            treeNode1.Name = "Node1";
            treeNode1.Text = "Node1";
            treeNode2.Name = "Node2";
            treeNode2.Text = "Node2";
            treeNode3.Name = "Node3";
            treeNode3.Text = "Node3";
            treeNode4.Name = "Node0";
            treeNode4.Text = "Node0";
            treeNode5.Name = "Node5";
            treeNode5.Text = "Node5";
            treeNode6.Name = "Node4";
            treeNode6.Text = "Node4";
            this.treeView_modes.Nodes.AddRange( new System.Windows.Forms.TreeNode[] {
            treeNode4,
            treeNode6} );
            this.treeView_modes.Size = new System.Drawing.Size( 273, 581 );
            this.treeView_modes.TabIndex = 3;
            this.treeView_modes.AfterSelect += new System.Windows.Forms.TreeViewEventHandler( this.treeView_modes_AfterSelect );
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
            this.dataGridView1.Location = new System.Drawing.Point( 1, 29 );
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.Size = new System.Drawing.Size( 492, 148 );
            this.dataGridView1.TabIndex = 5;
            this.dataGridView1.CellEnter += new System.Windows.Forms.DataGridViewCellEventHandler( this.dataGridView1_CellEnter );
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
            this.par_name.Width = 300;
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
            this.tabControl1.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
                        | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.tabControl1.Controls.Add( this.tabPage1 );
            this.tabControl1.Controls.Add( this.tabPage2 );
            this.tabControl1.Location = new System.Drawing.Point( 279, 372 );
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size( 504, 211 );
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
            this.tabPage1.Size = new System.Drawing.Size( 496, 185 );
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
            this.tabPage2.Size = new System.Drawing.Size( 496, 185 );
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
            this.dataGridView2.Size = new System.Drawing.Size( 492, 148 );
            this.dataGridView2.TabIndex = 8;
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
            // tabControl2
            // 
            this.tabControl2.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.tabControl2.Controls.Add( this.Page_dev );
            this.tabControl2.Controls.Add( this.Page_sign );
            this.tabControl2.Location = new System.Drawing.Point( 280, 0 );
            this.tabControl2.Name = "tabControl2";
            this.tabControl2.SelectedIndex = 0;
            this.tabControl2.Size = new System.Drawing.Size( 503, 370 );
            this.tabControl2.TabIndex = 3;
            // 
            // Page_dev
            // 
            this.Page_dev.Controls.Add( this.label8 );
            this.Page_dev.Controls.Add( this.label7 );
            this.Page_dev.Controls.Add( this.label6 );
            this.Page_dev.Controls.Add( this.label5 );
            this.Page_dev.Controls.Add( this.label4 );
            this.Page_dev.Controls.Add( this.label3 );
            this.Page_dev.Controls.Add( this.listBox4 );
            this.Page_dev.Controls.Add( this.listBox2 );
            this.Page_dev.Controls.Add( this.listBox3 );
            this.Page_dev.Controls.Add( this.listBox1 );
            this.Page_dev.Controls.Add( this.listBox_open );
            this.Page_dev.Controls.Add( this.listBox_close );
            this.Page_dev.Location = new System.Drawing.Point( 4, 22 );
            this.Page_dev.Name = "Page_dev";
            this.Page_dev.Padding = new System.Windows.Forms.Padding( 3 );
            this.Page_dev.Size = new System.Drawing.Size( 495, 344 );
            this.Page_dev.TabIndex = 0;
            this.Page_dev.Text = "Устройства";
            this.Page_dev.UseVisualStyleBackColor = true;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font( "Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.label8.Location = new System.Drawing.Point( 254, 227 );
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size( 65, 15 );
            this.label8.TabIndex = 9;
            this.label8.Text = "Запасные";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font( "Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.label7.Location = new System.Drawing.Point( 6, 227 );
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size( 238, 15 );
            this.label7.TabIndex = 9;
            this.label7.Text = "Вкл. после выключения режима устр-ва";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font( "Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.label6.Location = new System.Drawing.Point( 254, 114 );
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size( 122, 15 );
            this.label6.TabIndex = 9;
            this.label6.Text = "Флип нижних сёдел";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font( "Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.label5.Location = new System.Drawing.Point( 6, 114 );
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size( 126, 15 );
            this.label5.TabIndex = 9;
            this.label5.Text = "Флип верхних сёдел";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font( "Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.label4.Location = new System.Drawing.Point( 254, 1 );
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size( 160, 15 );
            this.label4.TabIndex = 9;
            this.label4.Text = "Выключаемые устройства";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font( "Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.label3.Location = new System.Drawing.Point( 6, 1 );
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size( 151, 15 );
            this.label3.TabIndex = 9;
            this.label3.Text = "Включаемые устройства";
            // 
            // listBox4
            // 
            this.listBox4.Font = new System.Drawing.Font( "Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.listBox4.FormattingEnabled = true;
            this.listBox4.ItemHeight = 16;
            this.listBox4.Location = new System.Drawing.Point( 246, 248 );
            this.listBox4.Name = "listBox4";
            this.listBox4.Size = new System.Drawing.Size( 240, 84 );
            this.listBox4.TabIndex = 5;
            // 
            // listBox2
            // 
            this.listBox2.Font = new System.Drawing.Font( "Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.listBox2.FormattingEnabled = true;
            this.listBox2.ItemHeight = 16;
            this.listBox2.Location = new System.Drawing.Point( 246, 136 );
            this.listBox2.Name = "listBox2";
            this.listBox2.Size = new System.Drawing.Size( 240, 84 );
            this.listBox2.TabIndex = 7;
            // 
            // listBox3
            // 
            this.listBox3.Font = new System.Drawing.Font( "Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.listBox3.FormattingEnabled = true;
            this.listBox3.ItemHeight = 16;
            this.listBox3.Location = new System.Drawing.Point( 3, 248 );
            this.listBox3.Name = "listBox3";
            this.listBox3.Size = new System.Drawing.Size( 240, 84 );
            this.listBox3.TabIndex = 8;
            // 
            // listBox1
            // 
            this.listBox1.Font = new System.Drawing.Font( "Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.listBox1.FormattingEnabled = true;
            this.listBox1.ItemHeight = 16;
            this.listBox1.Location = new System.Drawing.Point( 3, 136 );
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size( 240, 84 );
            this.listBox1.TabIndex = 6;
            // 
            // listBox_open
            // 
            this.listBox_open.Font = new System.Drawing.Font( "Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.listBox_open.FormattingEnabled = true;
            this.listBox_open.ItemHeight = 16;
            this.listBox_open.Location = new System.Drawing.Point( 3, 23 );
            this.listBox_open.Name = "listBox_open";
            this.listBox_open.Size = new System.Drawing.Size( 240, 84 );
            this.listBox_open.TabIndex = 3;
            this.listBox_open.DoubleClick += new System.EventHandler( this.listBox_open_DoubleClick );
            this.listBox_open.Leave += new System.EventHandler( this.listBox_open_Leave );
            // 
            // listBox_close
            // 
            this.listBox_close.Font = new System.Drawing.Font( "Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.listBox_close.FormattingEnabled = true;
            this.listBox_close.ItemHeight = 16;
            this.listBox_close.Location = new System.Drawing.Point( 246, 23 );
            this.listBox_close.Name = "listBox_close";
            this.listBox_close.Size = new System.Drawing.Size( 240, 84 );
            this.listBox_close.TabIndex = 4;
            // 
            // Page_sign
            // 
            this.Page_sign.Controls.Add( this.label9 );
            this.Page_sign.Controls.Add( this.label10 );
            this.Page_sign.Controls.Add( this.label11 );
            this.Page_sign.Controls.Add( this.label15 );
            this.Page_sign.Controls.Add( this.label12 );
            this.Page_sign.Controls.Add( this.label13 );
            this.Page_sign.Controls.Add( this.label14 );
            this.Page_sign.Controls.Add( this.listBox5 );
            this.Page_sign.Controls.Add( this.listBox6 );
            this.Page_sign.Controls.Add( this.listBox7 );
            this.Page_sign.Controls.Add( this.listBox8 );
            this.Page_sign.Controls.Add( this.listBox9 );
            this.Page_sign.Controls.Add( this.listBox11 );
            this.Page_sign.Controls.Add( this.listBox10 );
            this.Page_sign.Location = new System.Drawing.Point( 4, 22 );
            this.Page_sign.Name = "Page_sign";
            this.Page_sign.Padding = new System.Windows.Forms.Padding( 3 );
            this.Page_sign.Size = new System.Drawing.Size( 495, 344 );
            this.Page_sign.TabIndex = 1;
            this.Page_sign.Text = "Сигналы";
            this.Page_sign.UseVisualStyleBackColor = true;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Font = new System.Drawing.Font( "Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.label9.Location = new System.Drawing.Point( 8, 128 );
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size( 231, 17 );
            this.label9.TabIndex = 18;
            this.label9.Text = "Сигналы включающие устройства";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font( "Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.label10.Location = new System.Drawing.Point( 256, 128 );
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size( 171, 17 );
            this.label10.TabIndex = 19;
            this.label10.Text = "Включаемые устройства";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Font = new System.Drawing.Font( "Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.label11.Location = new System.Drawing.Point( 8, 203 );
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size( 131, 17 );
            this.label11.TabIndex = 20;
            this.label11.Text = "Пары сигналов ОС";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Font = new System.Drawing.Font( "Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.label15.Location = new System.Drawing.Point( 256, 65 );
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size( 201, 17 );
            this.label15.TabIndex = 15;
            this.label15.Text = "Сигналы, при работе режима";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Font = new System.Drawing.Font( "Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.label12.Location = new System.Drawing.Point( 256, 203 );
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size( 139, 17 );
            this.label12.TabIndex = 15;
            this.label12.Text = "Пары сигналов УПР";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Font = new System.Drawing.Font( "Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.label13.Location = new System.Drawing.Point( 8, 4 );
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size( 232, 17 );
            this.label13.TabIndex = 16;
            this.label13.Text = "Сигналы, необх. для ВКЛ. режима";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Font = new System.Drawing.Font( "Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.label14.Location = new System.Drawing.Point( 8, 65 );
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size( 193, 17 );
            this.label14.TabIndex = 17;
            this.label14.Text = "Сигнал, включающий режим";
            // 
            // listBox5
            // 
            this.listBox5.FormattingEnabled = true;
            this.listBox5.Location = new System.Drawing.Point( 4, 21 );
            this.listBox5.Name = "listBox5";
            this.listBox5.Size = new System.Drawing.Size( 240, 43 );
            this.listBox5.TabIndex = 11;
            // 
            // listBox6
            // 
            this.listBox6.FormattingEnabled = true;
            this.listBox6.Location = new System.Drawing.Point( 4, 82 );
            this.listBox6.Name = "listBox6";
            this.listBox6.Size = new System.Drawing.Size( 240, 43 );
            this.listBox6.TabIndex = 13;
            // 
            // listBox7
            // 
            this.listBox7.FormattingEnabled = true;
            this.listBox7.Location = new System.Drawing.Point( 4, 145 );
            this.listBox7.Name = "listBox7";
            this.listBox7.Size = new System.Drawing.Size( 240, 56 );
            this.listBox7.TabIndex = 14;
            // 
            // listBox8
            // 
            this.listBox8.FormattingEnabled = true;
            this.listBox8.Location = new System.Drawing.Point( 247, 145 );
            this.listBox8.Name = "listBox8";
            this.listBox8.Size = new System.Drawing.Size( 240, 56 );
            this.listBox8.TabIndex = 12;
            // 
            // listBox9
            // 
            this.listBox9.FormattingEnabled = true;
            this.listBox9.Location = new System.Drawing.Point( 4, 220 );
            this.listBox9.Name = "listBox9";
            this.listBox9.Size = new System.Drawing.Size( 240, 69 );
            this.listBox9.TabIndex = 9;
            // 
            // listBox11
            // 
            this.listBox11.FormattingEnabled = true;
            this.listBox11.Location = new System.Drawing.Point( 245, 82 );
            this.listBox11.Name = "listBox11";
            this.listBox11.Size = new System.Drawing.Size( 240, 43 );
            this.listBox11.TabIndex = 10;
            // 
            // listBox10
            // 
            this.listBox10.FormattingEnabled = true;
            this.listBox10.Location = new System.Drawing.Point( 247, 220 );
            this.listBox10.Name = "listBox10";
            this.listBox10.Size = new System.Drawing.Size( 240, 69 );
            this.listBox10.TabIndex = 10;
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.выбратьИзСпискаToolStripMenuItem,
            this.выбратьНаСхемеToolStripMenuItem} );
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size( 178, 70 );
            // 
            // выбратьИзСпискаToolStripMenuItem
            // 
            this.выбратьИзСпискаToolStripMenuItem.Name = "выбратьИзСпискаToolStripMenuItem";
            this.выбратьИзСпискаToolStripMenuItem.Size = new System.Drawing.Size( 177, 22 );
            this.выбратьИзСпискаToolStripMenuItem.Text = "Выбрать из списка";
            this.выбратьИзСпискаToolStripMenuItem.Click += new System.EventHandler( this.выбратьИзСпискаToolStripMenuItem_Click );
            // 
            // выбратьНаСхемеToolStripMenuItem
            // 
            this.выбратьНаСхемеToolStripMenuItem.Name = "выбратьНаСхемеToolStripMenuItem";
            this.выбратьНаСхемеToolStripMenuItem.Size = new System.Drawing.Size( 177, 22 );
            this.выбратьНаСхемеToolStripMenuItem.Text = "Выбрать на схеме";
            this.выбратьНаСхемеToolStripMenuItem.Click += new System.EventHandler( this.выбратьНаСхемеToolStripMenuItem_Click );
            // 
            // Form_modes
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size( 787, 588 );
            this.Controls.Add( this.tabControl2 );
            this.Controls.Add( this.tabControl1 );
            this.Controls.Add( this.treeView_modes );
            this.Name = "Form_modes";
            this.Text = "Режимы объекта";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler( this.Form_modes_FormClosing );
            this.Shown += new System.EventHandler( this.Form_modes_Shown );
            ( ( System.ComponentModel.ISupportInitialize ) ( this.dataGridView1 ) ).EndInit();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.numericUpDown1 ) ).EndInit();
            this.tabControl1.ResumeLayout( false );
            this.tabPage1.ResumeLayout( false );
            this.tabPage1.PerformLayout();
            this.tabPage2.ResumeLayout( false );
            this.tabPage2.PerformLayout();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.numericUpDown2 ) ).EndInit();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.dataGridView2 ) ).EndInit();
            this.tabControl2.ResumeLayout( false );
            this.Page_dev.ResumeLayout( false );
            this.Page_dev.PerformLayout();
            this.Page_sign.ResumeLayout( false );
            this.Page_sign.PerformLayout();
            this.contextMenuStrip1.ResumeLayout( false );
            this.ResumeLayout( false );

            }

        #endregion

        private System.Windows.Forms.TreeView treeView_modes;
        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.DataGridViewTextBoxColumn number;
        private System.Windows.Forms.DataGridViewTextBoxColumn par_name;
        private System.Windows.Forms.DataGridViewTextBoxColumn val;
        private System.Windows.Forms.NumericUpDown numericUpDown1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.TabControl tabControl2;
        private System.Windows.Forms.TabPage Page_dev;
        private System.Windows.Forms.ListBox listBox4;
        private System.Windows.Forms.ListBox listBox2;
        private System.Windows.Forms.ListBox listBox3;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.ListBox listBox_open;
        private System.Windows.Forms.ListBox listBox_close;
        private System.Windows.Forms.TabPage Page_sign;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.NumericUpDown numericUpDown2;
        private System.Windows.Forms.DataGridView dataGridView2;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn3;
        private System.Windows.Forms.ListBox listBox5;
        private System.Windows.Forms.ListBox listBox6;
        private System.Windows.Forms.ListBox listBox7;
        private System.Windows.Forms.ListBox listBox8;
        private System.Windows.Forms.ListBox listBox9;
        private System.Windows.Forms.ListBox listBox10;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.ListBox listBox11;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem выбратьИзСпискаToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem выбратьНаСхемеToolStripMenuItem;
        }
    }