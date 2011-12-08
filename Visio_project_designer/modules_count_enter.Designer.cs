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
            this.TV_modules_list = new System.Windows.Forms.TreeView();
            this.LB_modules = new System.Windows.Forms.ListBox();
            this.modules_type_label = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.modules_count_nupdown ) ).BeginInit();
            this.SuspendLayout();
            // 
            // ok_button
            // 
            this.ok_button.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.ok_button.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.ok_button.Location = new System.Drawing.Point( 239, 564 );
            this.ok_button.Name = "ok_button";
            this.ok_button.Size = new System.Drawing.Size( 230, 28 );
            this.ok_button.TabIndex = 3;
            this.ok_button.Text = "Ок";
            this.ok_button.UseVisualStyleBackColor = true;
            // 
            // modules_count_label
            // 
            this.modules_count_label.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left ) ) );
            this.modules_count_label.AutoSize = true;
            this.modules_count_label.Location = new System.Drawing.Point( 8, 573 );
            this.modules_count_label.Name = "modules_count_label";
            this.modules_count_label.Size = new System.Drawing.Size( 85, 13 );
            this.modules_count_label.TabIndex = 0;
            this.modules_count_label.Text = "Число модулей";
            // 
            // modules_count_nupdown
            // 
            this.modules_count_nupdown.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left ) ) );
            this.modules_count_nupdown.Location = new System.Drawing.Point( 110, 570 );
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
            // TV_modules_list
            // 
            this.TV_modules_list.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
                        | System.Windows.Forms.AnchorStyles.Left )
                        | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.TV_modules_list.Font = new System.Drawing.Font( "Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ( ( byte ) ( 204 ) ) );
            this.TV_modules_list.Location = new System.Drawing.Point( 2, 22 );
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
            this.TV_modules_list.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.TV_modules_list.Size = new System.Drawing.Size( 234, 537 );
            this.TV_modules_list.TabIndex = 4;
            this.TV_modules_list.AfterSelect += new System.Windows.Forms.TreeViewEventHandler( this.TV_modules_list_AfterSelect );
            this.TV_modules_list.DoubleClick += new System.EventHandler( this.treeView1_DoubleClick );
            // 
            // LB_modules
            // 
            this.LB_modules.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
                        | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.LB_modules.FormattingEnabled = true;
            this.LB_modules.Location = new System.Drawing.Point( 239, 22 );
            this.LB_modules.Name = "LB_modules";
            this.LB_modules.Size = new System.Drawing.Size( 230, 537 );
            this.LB_modules.TabIndex = 5;
            // 
            // modules_type_label
            // 
            this.modules_type_label.AutoSize = true;
            this.modules_type_label.Location = new System.Drawing.Point( 13, 5 );
            this.modules_type_label.Name = "modules_type_label";
            this.modules_type_label.Size = new System.Drawing.Size( 66, 13 );
            this.modules_type_label.TabIndex = 0;
            this.modules_type_label.Text = "Тип модуля";
            // 
            // label1
            // 
            this.label1.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point( 247, 5 );
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size( 124, 13 );
            this.label1.TabIndex = 6;
            this.label1.Text = "Модули в составе узла";
            // 
            // modules_count_enter
            // 
            this.AcceptButton = this.ok_button;
            this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size( 473, 595 );
            this.Controls.Add( this.label1 );
            this.Controls.Add( this.LB_modules );
            this.Controls.Add( this.TV_modules_list );
            this.Controls.Add( this.modules_count_nupdown );
            this.Controls.Add( this.modules_type_label );
            this.Controls.Add( this.modules_count_label );
            this.Controls.Add( this.ok_button );
            this.Name = "modules_count_enter";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Добавление модуля";
            this.Shown += new System.EventHandler( this.modules_count_enter_Shown );
            ( ( System.ComponentModel.ISupportInitialize ) ( this.modules_count_nupdown ) ).EndInit();
            this.ResumeLayout( false );
            this.PerformLayout();

            }

        #endregion

        private System.Windows.Forms.Button ok_button;
        private System.Windows.Forms.Label modules_count_label;
        private System.Windows.Forms.NumericUpDown modules_count_nupdown;
		private System.Windows.Forms.ListBox LB_modules;
		private System.Windows.Forms.Label modules_type_label;
		private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TreeView TV_modules_list;
        }
    }