namespace CanFly
{
  partial class ConnectionDialog
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if(disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.btnConnect = new System.Windows.Forms.Button();
      this.btnCancel = new System.Windows.Forms.Button();
      this.cbAdapter = new System.Windows.Forms.ComboBox();
      this.labelControl1 = new System.Windows.Forms.Label();
      this.SuspendLayout();
      // 
      // btnConnect
      // 
      this.btnConnect.Location = new System.Drawing.Point(172, 53);
      this.btnConnect.Name = "btnConnect";
      this.btnConnect.Size = new System.Drawing.Size(75, 23);
      this.btnConnect.TabIndex = 0;
      this.btnConnect.Text = "Connect";
      this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
      // 
      // btnCancel
      // 
      this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.btnCancel.Location = new System.Drawing.Point(78, 53);
      this.btnCancel.Name = "btnCancel";
      this.btnCancel.Size = new System.Drawing.Size(75, 23);
      this.btnCancel.TabIndex = 1;
      this.btnCancel.Text = "Cancel";
      this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
      // 
      // cbAdapter
      // 
      this.cbAdapter.Location = new System.Drawing.Point(68, 12);
      this.cbAdapter.Name = "cbAdapter";
      this.cbAdapter.Size = new System.Drawing.Size(179, 21);
      this.cbAdapter.TabIndex = 2;
      this.cbAdapter.SelectedIndexChanged += new System.EventHandler(this.cbAdapter_SelectedIndexChanged);
      // 
      // labelControl1
      // 
      this.labelControl1.Location = new System.Drawing.Point(12, 15);
      this.labelControl1.Name = "labelControl1";
      this.labelControl1.Size = new System.Drawing.Size(50, 13);
      this.labelControl1.TabIndex = 3;
      this.labelControl1.Text = "Adapter:";
      // 
      // ConnectionDialog
      // 
      this.AcceptButton = this.btnConnect;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.CancelButton = this.btnCancel;
      this.ClientSize = new System.Drawing.Size(265, 91);
      this.Controls.Add(this.labelControl1);
      this.Controls.Add(this.cbAdapter);
      this.Controls.Add(this.btnCancel);
      this.Controls.Add(this.btnConnect);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "ConnectionDialog";
      this.Text = "Select Serial Can Converter";
      this.Load += new System.EventHandler(this.ConnectionDialog_Load);
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Button btnConnect;
    private System.Windows.Forms.Button btnCancel;
    private System.Windows.Forms.ComboBox cbAdapter;
    private System.Windows.Forms.Label labelControl1;
  }
}