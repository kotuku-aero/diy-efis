using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CanFly
{
  public partial class ConnectionDialog : Form
  {
    public string Adpater { get; private set; }
    public ConnectionDialog()
    {
      InitializeComponent();
    }

    private void ConnectionDialog_Load(object sender, EventArgs e)
    {
      foreach(string adapter in CANaerospace.EnumerateDevices())
        cbAdapter.Items.Add(adapter);

      btnConnect.Enabled = false;
    }

    private void btnConnect_Click(object sender, EventArgs e)
    {
      Adpater = cbAdapter.SelectedItem.ToString();
      DialogResult = System.Windows.Forms.DialogResult.OK;
    }

    private void btnCancel_Click(object sender, EventArgs e)
    {
      DialogResult = System.Windows.Forms.DialogResult.Cancel;
      Hide();
    }

    private void cbAdapter_SelectedIndexChanged(object sender, EventArgs e)
    {
      btnConnect.Enabled = cbAdapter.SelectedIndex > -1;
    }
  }
}