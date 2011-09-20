namespace Visio_project_designer
	{
	partial class Form_device
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
			this.LV_devices = new System.Windows.Forms.ListView();
			this.type = ( ( System.Windows.Forms.ColumnHeader ) ( new System.Windows.Forms.ColumnHeader() ) );
			this.number = ( ( System.Windows.Forms.ColumnHeader ) ( new System.Windows.Forms.ColumnHeader() ) );
			this.name = ( ( System.Windows.Forms.ColumnHeader ) ( new System.Windows.Forms.ColumnHeader() ) );
			this.discription = ( ( System.Windows.Forms.ColumnHeader ) ( new System.Windows.Forms.ColumnHeader() ) );
			this.sub_type = ( ( System.Windows.Forms.ColumnHeader ) ( new System.Windows.Forms.ColumnHeader() ) );
			this.SuspendLayout();
			// 
			// LV_devices
			// 
			this.LV_devices.Activation = System.Windows.Forms.ItemActivation.OneClick;
			this.LV_devices.Anchor = ( ( System.Windows.Forms.AnchorStyles ) ( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom )
						| System.Windows.Forms.AnchorStyles.Left )
						| System.Windows.Forms.AnchorStyles.Right ) ) );
			this.LV_devices.Columns.AddRange( new System.Windows.Forms.ColumnHeader[] {
            this.type,
            this.number,
            this.name,
            this.discription,
            this.sub_type} );
			this.LV_devices.FullRowSelect = true;
			this.LV_devices.GridLines = true;
			this.LV_devices.HideSelection = false;
			this.LV_devices.Location = new System.Drawing.Point( 0, 0 );
			this.LV_devices.Name = "LV_devices";
			this.LV_devices.Size = new System.Drawing.Size( 665, 490 );
			this.LV_devices.Sorting = System.Windows.Forms.SortOrder.Ascending;
			this.LV_devices.TabIndex = 0;
			this.LV_devices.UseCompatibleStateImageBehavior = false;
			this.LV_devices.View = System.Windows.Forms.View.Details;
			this.LV_devices.SelectedIndexChanged += new System.EventHandler( this.LV_devices_SelectedIndexChanged );
			this.LV_devices.DoubleClick += new System.EventHandler( this.LV_devices_DoubleClick );
			// 
			// type
			// 
			this.type.Text = "Тип";
			this.type.Width = 40;
			// 
			// number
			// 
			this.number.Text = "Номер";
			// 
			// name
			// 
			this.name.Text = "Название";
			this.name.Width = 70;
			// 
			// discription
			// 
			this.discription.Text = "Описание";
			this.discription.Width = 400;
			// 
			// sub_type
			// 
			this.sub_type.Text = "Подтип";
			// 
			// Form_device
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size( 668, 492 );
			this.Controls.Add( this.LV_devices );
			this.Name = "Form_device";
			this.Text = "Устройства проекта";
			this.Shown += new System.EventHandler( this.Form_device_Shown );
			this.ResumeLayout( false );

			}

		#endregion

		private System.Windows.Forms.ColumnHeader type;
		private System.Windows.Forms.ColumnHeader number;
		private System.Windows.Forms.ColumnHeader name;
		private System.Windows.Forms.ColumnHeader discription;
		private System.Windows.Forms.ColumnHeader sub_type;
		public System.Windows.Forms.ListView LV_devices;
		}
	}