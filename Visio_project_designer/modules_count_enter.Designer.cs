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
            this.ok_button = new System.Windows.Forms.Button();
            this.modules_count_label = new System.Windows.Forms.Label();
            this.modules_count_nupdown = new System.Windows.Forms.NumericUpDown();
            this.modules_type_label = new System.Windows.Forms.Label();
            this.modules_type_cbox = new System.Windows.Forms.ComboBox();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.modules_count_nupdown ) ).BeginInit();
            this.SuspendLayout();
            // 
            // ok_button
            // 
            this.ok_button.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.ok_button.Location = new System.Drawing.Point( 12, 77 );
            this.ok_button.Name = "ok_button";
            this.ok_button.Size = new System.Drawing.Size( 152, 28 );
            this.ok_button.TabIndex = 3;
            this.ok_button.Text = "Ок";
            this.ok_button.UseVisualStyleBackColor = true;
            // 
            // modules_count_label
            // 
            this.modules_count_label.AutoSize = true;
            this.modules_count_label.Location = new System.Drawing.Point( 9, 43 );
            this.modules_count_label.Name = "modules_count_label";
            this.modules_count_label.Size = new System.Drawing.Size( 85, 13 );
            this.modules_count_label.TabIndex = 0;
            this.modules_count_label.Text = "Число модулей";
            // 
            // modules_count_nupdown
            // 
            this.modules_count_nupdown.Location = new System.Drawing.Point( 100, 43 );
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
            this.modules_type_label.Location = new System.Drawing.Point( 9, 14 );
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
            this.modules_type_cbox.Location = new System.Drawing.Point( 100, 11 );
            this.modules_type_cbox.Name = "modules_type_cbox";
            this.modules_type_cbox.Size = new System.Drawing.Size( 64, 21 );
            this.modules_type_cbox.Sorted = true;
            this.modules_type_cbox.TabIndex = 2;
            this.modules_type_cbox.Text = "504";
            this.modules_type_cbox.SelectedIndexChanged += new System.EventHandler( this.modules_type_cbox_SelectedIndexChanged );
            this.modules_type_cbox.TextChanged += new System.EventHandler( this.modules_type_cbox__TextChanged );
            // 
            // modules_count_enter
            // 
            this.AcceptButton = this.ok_button;
            this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size( 170, 117 );
            this.Controls.Add( this.modules_type_cbox );
            this.Controls.Add( this.modules_count_nupdown );
            this.Controls.Add( this.modules_type_label );
            this.Controls.Add( this.modules_count_label );
            this.Controls.Add( this.ok_button );
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "modules_count_enter";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Модули";
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
        }
    }