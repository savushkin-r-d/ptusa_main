/// @file modules_count_enter.Designer.cs
/// @brief Визуальные компоненты формы @ref visio_prj_designer::modules_count_enter.
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
    partial class modules_count_enter
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
            if( disposing && ( components != null ) )
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
			System.Windows.Forms.TreeNode treeNode1 = new System.Windows.Forms.TreeNode( "T_402" );
			System.Windows.Forms.TreeNode treeNode2 = new System.Windows.Forms.TreeNode( "Node2" );
			System.Windows.Forms.TreeNode treeNode3 = new System.Windows.Forms.TreeNode( "Node3" );
			System.Windows.Forms.TreeNode treeNode4 = new System.Windows.Forms.TreeNode( "Node000", new System.Windows.Forms.TreeNode[] {
            treeNode1,
            treeNode2,
            treeNode3} );
			System.Windows.Forms.TreeNode treeNode5 = new System.Windows.Forms.TreeNode( "Node5" );
			System.Windows.Forms.TreeNode treeNode6 = new System.Windows.Forms.TreeNode( "Node6" );
			System.Windows.Forms.TreeNode treeNode7 = new System.Windows.Forms.TreeNode( "Node7" );
			System.Windows.Forms.TreeNode treeNode8 = new System.Windows.Forms.TreeNode( "Node4", new System.Windows.Forms.TreeNode[] {
            treeNode5,
            treeNode6,
            treeNode7} );
			this.ok_button = new System.Windows.Forms.Button();
			this.modules_count_label = new System.Windows.Forms.Label();
			this.modules_count_nupdown = new System.Windows.Forms.NumericUpDown();
			this.modules_type_label = new System.Windows.Forms.Label();
			this.modules_type_cbox = new System.Windows.Forms.ComboBox();
			this.TV_modules_list = new System.Windows.Forms.TreeView();
			this.LB_modules = new System.Windows.Forms.ListBox();
			( ( System.ComponentModel.ISupportInitialize ) ( this.modules_count_nupdown ) ).BeginInit();
			this.SuspendLayout();
			// 
			// ok_button
			// 
			this.ok_button.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.ok_button.Location = new System.Drawing.Point( 266, 468 );
			this.ok_button.Name = "ok_button";
			this.ok_button.Size = new System.Drawing.Size( 152, 28 );
			this.ok_button.TabIndex = 3;
			this.ok_button.Text = "Ок";
			this.ok_button.UseVisualStyleBackColor = true;
			// 
			// modules_count_label
			// 
			this.modules_count_label.AutoSize = true;
			this.modules_count_label.Location = new System.Drawing.Point( 263, 434 );
			this.modules_count_label.Name = "modules_count_label";
			this.modules_count_label.Size = new System.Drawing.Size( 85, 13 );
			this.modules_count_label.TabIndex = 0;
			this.modules_count_label.Text = "Число модулей";
			// 
			// modules_count_nupdown
			// 
			this.modules_count_nupdown.Location = new System.Drawing.Point( 354, 434 );
			this.modules_count_nupdown.Maximum = new decimal( new int[] {
            50,
            0,
            0,
            0} );
			this.modules_count_nupdown.Minimum = new decimal( new int[] {
            1,
            0,
            0,
            0} );
			this.modules_count_nupdown.Name = "modules_count_nupdown";
			this.modules_count_nupdown.Size = new System.Drawing.Size( 64, 20 );
			this.modules_count_nupdown.TabIndex = 1;
			this.modules_count_nupdown.Value = new decimal( new int[] {
            1,
            0,
            0,
            0} );
			this.modules_count_nupdown.ValueChanged += new System.EventHandler( this.modules_count_nupdown__ValueChanged );
			// 
			// modules_type_label
			// 
			this.modules_type_label.AutoSize = true;
			this.modules_type_label.Location = new System.Drawing.Point( 263, 405 );
			this.modules_type_label.Name = "modules_type_label";
			this.modules_type_label.Size = new System.Drawing.Size( 26, 13 );
			this.modules_type_label.TabIndex = 0;
			this.modules_type_label.Text = "Тип";
			// 
			// modules_type_cbox
			// 
			this.modules_type_cbox.FormattingEnabled = true;
			this.modules_type_cbox.Items.AddRange( new object[] {
            "402",
            "466",
            "504"} );
			this.modules_type_cbox.Location = new System.Drawing.Point( 354, 402 );
			this.modules_type_cbox.Name = "modules_type_cbox";
			this.modules_type_cbox.Size = new System.Drawing.Size( 64, 21 );
			this.modules_type_cbox.Sorted = true;
			this.modules_type_cbox.TabIndex = 2;
			this.modules_type_cbox.Text = "504";
			this.modules_type_cbox.TextChanged += new System.EventHandler( this.modules_type_cbox__TextChanged );
			// 
			// TV_modules_list
			// 
			this.TV_modules_list.Location = new System.Drawing.Point( 13, 13 );
			this.TV_modules_list.Name = "TV_modules_list";
			treeNode1.Name = "T_402";
			treeNode1.Text = "T_402";
			treeNode2.Name = "Node2";
			treeNode2.Text = "Node2";
			treeNode3.Name = "Node3";
			treeNode3.Text = "Node3";
			treeNode4.Name = "Node000";
			treeNode4.Text = "Node000";
			treeNode5.Name = "Node5";
			treeNode5.Text = "Node5";
			treeNode6.Name = "Node6";
			treeNode6.Text = "Node6";
			treeNode7.Name = "Node7";
			treeNode7.Text = "Node7";
			treeNode8.Name = "Node4";
			treeNode8.Text = "Node4";
			this.TV_modules_list.Nodes.AddRange( new System.Windows.Forms.TreeNode[] {
            treeNode4,
            treeNode8} );
			this.TV_modules_list.Size = new System.Drawing.Size( 202, 434 );
			this.TV_modules_list.TabIndex = 4;
			this.TV_modules_list.AfterSelect += new System.Windows.Forms.TreeViewEventHandler( this.TV_modules_list_AfterSelect );
			this.TV_modules_list.DoubleClick += new System.EventHandler( this.treeView1_DoubleClick );
			// 
			// LB_modules
			// 
			this.LB_modules.FormattingEnabled = true;
			this.LB_modules.Location = new System.Drawing.Point( 244, 13 );
			this.LB_modules.Name = "LB_modules";
			this.LB_modules.Size = new System.Drawing.Size( 206, 381 );
			this.LB_modules.TabIndex = 5;
			// 
			// modules_count_enter
			// 
			this.AcceptButton = this.ok_button;
			this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size( 462, 523 );
			this.Controls.Add( this.LB_modules );
			this.Controls.Add( this.TV_modules_list );
			this.Controls.Add( this.modules_type_cbox );
			this.Controls.Add( this.modules_count_nupdown );
			this.Controls.Add( this.modules_type_label );
			this.Controls.Add( this.modules_count_label );
			this.Controls.Add( this.ok_button );
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "modules_count_enter";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Модули";
			this.Shown += new System.EventHandler( this.modules_count_enter_Shown );
			( ( System.ComponentModel.ISupportInitialize ) ( this.modules_count_nupdown ) ).EndInit();
			this.ResumeLayout( false );
			this.PerformLayout();

            }

        #endregion

        private System.Windows.Forms.Button ok_button;
        private System.Windows.Forms.Label modules_count_label;
        private System.Windows.Forms.NumericUpDown modules_count_nupdown;
        private System.Windows.Forms.Label modules_type_label;
        private System.Windows.Forms.ComboBox modules_type_cbox;
		private System.Windows.Forms.TreeView TV_modules_list;
		private System.Windows.Forms.ListBox LB_modules;
        }
    }