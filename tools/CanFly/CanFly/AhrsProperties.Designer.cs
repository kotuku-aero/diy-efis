namespace CanFly
{
  partial class AhrsProperties
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
      if (disposing && (components != null))
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
      this.labelControl1 = new DevExpress.XtraEditors.LabelControl();
      this.textEdit1 = new DevExpress.XtraEditors.TextEdit();
      this.groupControl4 = new DevExpress.XtraEditors.GroupControl();
      this.treeList1 = new DevExpress.XtraTreeList.TreeList();
      this.columnCanIDName = new DevExpress.XtraTreeList.Columns.TreeListColumn();
      this.columnLastValue = new DevExpress.XtraTreeList.Columns.TreeListColumn();
      ((System.ComponentModel.ISupportInitialize)(this.textEdit1.Properties)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.groupControl4)).BeginInit();
      this.groupControl4.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.treeList1)).BeginInit();
      this.SuspendLayout();
      // 
      // labelControl1
      // 
      this.labelControl1.Location = new System.Drawing.Point(19, 13);
      this.labelControl1.Name = "labelControl1";
      this.labelControl1.Size = new System.Drawing.Size(33, 13);
      this.labelControl1.TabIndex = 0;
      this.labelControl1.Text = "Can ID";
      // 
      // textEdit1
      // 
      this.textEdit1.Location = new System.Drawing.Point(90, 10);
      this.textEdit1.Name = "textEdit1";
      this.textEdit1.Size = new System.Drawing.Size(143, 20);
      this.textEdit1.TabIndex = 1;
      // 
      // groupControl4
      // 
      this.groupControl4.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.groupControl4.Controls.Add(this.treeList1);
      this.groupControl4.Location = new System.Drawing.Point(13, 232);
      this.groupControl4.Name = "groupControl4";
      this.groupControl4.Size = new System.Drawing.Size(746, 236);
      this.groupControl4.TabIndex = 11;
      this.groupControl4.Text = "Data Feed";
      // 
      // treeList1
      // 
      this.treeList1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.treeList1.Columns.AddRange(new DevExpress.XtraTreeList.Columns.TreeListColumn[] {
            this.columnCanIDName,
            this.columnLastValue});
      this.treeList1.Location = new System.Drawing.Point(6, 24);
      this.treeList1.Name = "treeList1";
      this.treeList1.BeginUnboundLoad();
      this.treeList1.AppendNode(new object[] {
            "Airspeed",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Altitude",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Latitude",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Longitude",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Desired Track Angle",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Deviation from Track",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Distance to Destination",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Distance to Next",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Estimated Time of Arrival",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Estimated Time to Next",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Track",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Track Angle Error",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Outside Air Temperature",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Waypoint Latitude",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Waypoint Longitude",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Waypoint Maximum Altitude",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Waypoint Minimum Altitude",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Waypoint Name",
            null}, -1);
      this.treeList1.AppendNode(new object[] {
            "Time UTC",
            null}, -1);
      this.treeList1.EndUnboundLoad();
      this.treeList1.Size = new System.Drawing.Size(735, 207);
      this.treeList1.TabIndex = 0;
      // 
      // columnCanIDName
      // 
      this.columnCanIDName.Caption = "Name";
      this.columnCanIDName.FieldName = "canIDName";
      this.columnCanIDName.MinWidth = 34;
      this.columnCanIDName.Name = "columnCanIDName";
      this.columnCanIDName.Visible = true;
      this.columnCanIDName.VisibleIndex = 0;
      // 
      // columnLastValue
      // 
      this.columnLastValue.Caption = "Value";
      this.columnLastValue.FieldName = "lastValue";
      this.columnLastValue.Fixed = DevExpress.XtraTreeList.Columns.FixedStyle.Right;
      this.columnLastValue.Name = "columnLastValue";
      this.columnLastValue.Visible = true;
      this.columnLastValue.VisibleIndex = 1;
      // 
      // AhrsProperties
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(771, 480);
      this.Controls.Add(this.groupControl4);
      this.Controls.Add(this.textEdit1);
      this.Controls.Add(this.labelControl1);
      this.Name = "AhrsProperties";
      this.Text = "Ahrs Properties";
      ((System.ComponentModel.ISupportInitialize)(this.textEdit1.Properties)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.groupControl4)).EndInit();
      this.groupControl4.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.treeList1)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private DevExpress.XtraEditors.LabelControl labelControl1;
    private DevExpress.XtraEditors.TextEdit textEdit1;
    private DevExpress.XtraEditors.GroupControl groupControl4;
    private DevExpress.XtraTreeList.TreeList treeList1;
    private DevExpress.XtraTreeList.Columns.TreeListColumn columnCanIDName;
    private DevExpress.XtraTreeList.Columns.TreeListColumn columnLastValue;
  }
}