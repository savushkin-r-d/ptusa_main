namespace Visio_project_designer
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
            this.button_ok = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.numericUpDown1 = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.comboBox_type = new System.Windows.Forms.ComboBox();
            ( ( System.ComponentModel.ISupportInitialize ) ( this.numericUpDown1 ) ).BeginInit();
            this.SuspendLayout();
            // 
            // button_ok
            // 
            this.button_ok.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button_ok.Location = new System.Drawing.Point( 12, 77 );
            this.button_ok.Name = "button_ok";
            this.button_ok.Size = new System.Drawing.Size( 152, 28 );
            this.button_ok.TabIndex = 3;
            this.button_ok.Text = "Ок";
            this.button_ok.UseVisualStyleBackColor = true;
            this.button_ok.Click += new System.EventHandler( this.button_ok_Click );
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point( 9, 43 );
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size( 85, 13 );
            this.label1.TabIndex = 0;
            this.label1.Text = "Число модулей";
            // 
            // numericUpDown1
            // 
            this.numericUpDown1.Location = new System.Drawing.Point( 100, 43 );
            this.numericUpDown1.Maximum = new decimal( new int[] {
            50,
            0,
            0,
            0} );
            this.numericUpDown1.Minimum = new decimal( new int[] {
            1,
            0,
            0,
            0} );
            this.numericUpDown1.Name = "numericUpDown1";
            this.numericUpDown1.Size = new System.Drawing.Size( 64, 20 );
            this.numericUpDown1.TabIndex = 1;
            this.numericUpDown1.Value = new decimal( new int[] {
            1,
            0,
            0,
            0} );
            this.numericUpDown1.ValueChanged += new System.EventHandler( this.numericUpDown1_ValueChanged );
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point( 9, 14 );
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size( 26, 13 );
            this.label2.TabIndex = 0;
            this.label2.Text = "Тип";
            // 
            // comboBox_type
            // 
            this.comboBox_type.FormattingEnabled = true;
            this.comboBox_type.Items.AddRange( new object[] {
            "402",
            "466",
            "504"} );
            this.comboBox_type.Location = new System.Drawing.Point( 100, 11 );
            this.comboBox_type.Name = "comboBox_type";
            this.comboBox_type.Size = new System.Drawing.Size( 64, 21 );
            this.comboBox_type.Sorted = true;
            this.comboBox_type.TabIndex = 2;
            this.comboBox_type.Text = "504";
            this.comboBox_type.TextChanged += new System.EventHandler( this.comboBox_type_TextChanged );
            // 
            // modules_count_enter
            // 
            this.AcceptButton = this.button_ok;
            this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size( 170, 117 );
            this.Controls.Add( this.comboBox_type );
            this.Controls.Add( this.numericUpDown1 );
            this.Controls.Add( this.label2 );
            this.Controls.Add( this.label1 );
            this.Controls.Add( this.button_ok );
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "modules_count_enter";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Модули";
            ( ( System.ComponentModel.ISupportInitialize ) ( this.numericUpDown1 ) ).EndInit();
            this.ResumeLayout( false );
            this.PerformLayout();

            }

        #endregion

        private System.Windows.Forms.Button button_ok;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown numericUpDown1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox comboBox_type;
        }
    }