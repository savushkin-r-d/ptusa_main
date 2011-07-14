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
        public int modules_count = 0;

        public modules_count_enter()
            {
            InitializeComponent();
            }

        private void numericUpDown1_ValueChanged( object sender, EventArgs e )
            {
            NumericUpDown cnt = ( NumericUpDown ) sender;
            modules_count = ( int ) cnt.Value;
            }
        }
    }
