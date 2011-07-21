using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Visio_project_designer
    {
    public partial class modules_count_enter : Form
        {
        public int modules_count    = 1;
        public string modules_type  = "504";

        public modules_count_enter()
            {
            InitializeComponent();
            }

        private void numericUpDown1_ValueChanged( object sender, EventArgs e )
            {
            NumericUpDown cnt = ( NumericUpDown ) sender;
            modules_count = ( int ) cnt.Value;
            }

        private void comboBox_type_TextChanged( object sender, EventArgs e )
            {
            modules_type = ( ( ComboBox ) sender).Text;
            }

        private void button_ok_Click( object sender, EventArgs e )
            {

            }
        }
    }
