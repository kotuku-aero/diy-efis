using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.Diagnostics;
using System.Threading;

namespace CanFly
	{
	public partial class CanFlyBrowser : Form, ICanFlyBrowser
		{
		private CANaerospace canAdapter = null;
		private CANaerospace.ProcessCanMsg callbackDelegate;
		private SerialPort device;

		public CanFlyBrowser()
			{
			InitializeComponent();
			}

		private void MsgReceivedCallback(CANASMsg msg)
			{
			// decide what we are to do
			switch (msg.ID)
				{
				case CANaerospace.NodeServiceChannel0Reply:         // response to a broadcast message
					ProcessIDSMessage(msg);
					break;
				}
			}

		private Mutex deviceMutex = new Mutex();
		private int nextDevice = 1;

		private List<CanFlyProxy> discoveredDevices = new List<CanFlyProxy>();

		private int GetNextDevice()
			{
			int result;
			deviceMutex.WaitOne();
			result = nextDevice++;
			deviceMutex.ReleaseMutex();

			return result;
			}

		private void AddProxy(CanFlyProxy proxy)
			{
			deviceMutex.WaitOne();
			discoveredDevices.Add(proxy);
			deviceMutex.ReleaseMutex();
			}

		private void AddDeviceTab(Control window)
			{
			ICanFlyMonitor monitor = window as ICanFlyMonitor;
			TabIdentifier id = monitor.Device;

			string windowName = String.Format("{0} - {1}", id.DeviceName, id.NodeID);

			deviceMonitorTabControl.Invoke((MethodInvoker)delegate
					{
					deviceMonitorTabControl.TabPages.Add(new TabHolder(windowName, window));
					});
			}

		private void ProcessIDSMessage(CANASMsg msg)
			{
			// and IDS message has 2 parts.
			// Byte[0] = hardware type
			// Byte[1] = software revision
			// Byte[2] = node_id high
			// Byte[3] = node_id low
			//
			switch (msg.UCHAR2[0])
				{
			// determine the node type from the response
				case 10:
					Trace.TraceInformation("Found AHRS Device, Revision {1}, NodeID {2}", msg.UCHAR4[0], msg.UCHAR4[1], msg.UCHAR4[3]);
          // this is an AHRS so we add it to the tree
					AddDeviceNode(new ParameterDefinitionProxy("AHRS", canAdapter, GetNextDevice(), msg));
					break;
				case 20:
					{
						Trace.TraceInformation("Found EDU device, Revision {1}, NodeID {2}", msg.UCHAR4[0], msg.UCHAR4[1], msg.UCHAR4[3]);
						ParameterDefinitionProxy proxy = new ParameterDefinitionProxy("EDU", canAdapter, GetNextDevice(), msg);
						AddDeviceNode(proxy);

						TabIdentifier id;

						id.DeviceName = "EDU";
						id.DeviceType = 20;
						id.NodeID = msg.UCHAR4[3];

						EDUMonitor monitor = null;

						deviceTree.Invoke((MethodInvoker)delegate
								{
								monitor = new EDUMonitor();
								});
						monitor.AssignProxy(id, proxy);


						AddDeviceTab(monitor);
					}
					break;
				case 70:
					{
						Trace.TraceInformation("Found FDU device, Revision {1}, NodeID {2}", msg.UCHAR4[0], msg.UCHAR4[1], msg.UCHAR4[3]);
						ParameterDefinitionProxy proxy = new ParameterDefinitionProxy("FDU", canAdapter, GetNextDevice(), msg);
						AddDeviceNode(proxy);

						TabIdentifier id;

						id.DeviceName = "FDU";
						id.DeviceType = 70;
						id.NodeID = msg.UCHAR4[3];

						FDUMonitor monitor = null;

						deviceTree.Invoke((MethodInvoker)delegate
								{
								monitor = new FDUMonitor();
								});
						monitor.AssignProxy(id, proxy);


						AddDeviceTab(monitor);
					}
					break;
				case 80:
          // this is an aacu
					Trace.TraceInformation("Found ACU device, Revision {1}, NodeID {2}", msg.UCHAR4[0], msg.UCHAR4[1], msg.UCHAR4[3]);
					AddDeviceNode(new ParameterDefinitionProxy("ACU", canAdapter, GetNextDevice(), msg));
					break;
				case 128:
					{
						Trace.TraceInformation("Found EFI device, Revision {1}, NodeID {2}", msg.UCHAR4[0], msg.UCHAR4[1], msg.UCHAR4[3]);

						ParameterDefinitionProxy proxy = new ParameterDefinitionProxy("EFI", canAdapter, GetNextDevice(), msg);
						AddDeviceNode(proxy);

						EFIMonitor monitor = null;

						TabIdentifier id;

						id.DeviceName = "EFI";
						id.DeviceType = 128;
						id.NodeID = msg.UCHAR4[3];

						deviceTree.Invoke((MethodInvoker)delegate
								{
								monitor = new EFIMonitor();
								});
						monitor.AssignProxy(id, proxy);

						AddDeviceTab(monitor);
					}
					break;
				case 4:
          // this is an AIO
					Trace.TraceInformation("Found PFD device, Revision {1}, NodeID {2}", msg.UCHAR4[0], msg.UCHAR4[1], msg.UCHAR4[3]);
					AddDeviceNode(new PFDProxy(canAdapter, GetNextDevice(), msg));
					break;
				case 30:
          // scan device
					break;
				default:
					{
						// Not recognised.
					}
					break;
				}
			}

		private void AddDeviceNode(CanFlyProxy proxy)
			{
			DeviceNode node = new DeviceNode(proxy);
			proxy.Node = node;
			// add it as root node
			deviceTree.Invoke((MethodInvoker)delegate
					{
					deviceTree.Nodes.Add(node);
					});
			AddProxy(proxy);

			proxy.Initialize();
			}

		/// <summary>
		/// Save one or more of the discovered devices as a configuration script
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void saveToolStripMenuItem_Click(object sender, EventArgs e)
			{
			Save();
			}

		/// <summary>
		/// Load a canbus configuration script.
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void loadToolStripMenuItem_Click(object sender, EventArgs e)
			{

			}

		public static bool IsLinux
			{
			get
				{
				int p = (int)Environment.OSVersion.Platform;
				return (p == 4) || (p == 6) || (p == 128);
				}
			}

		public bool Connect()
			{
			if (device != null)
				{
				if (canAdapter != null)
					{
					canAdapter.Close();
					canAdapter = null;
					}

				device.Close();
				device.Dispose();

				device = null;

				refreshToolStripMenuItem.Enabled = false;
				}

			propertyGrid.SelectedObject = null;
			deviceTree.Nodes.Clear();

			// we list the CANUSB devices
			ConnectionDialog dlg = new ConnectionDialog();

			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
				{
				try
					{
          if(IsLinux)
            device = new SerialPort(dlg.Adpater, 38400, Parity.None, 8, StopBits.Two);
          else
            device = new SerialPort(dlg.Adpater, 250000, Parity.None, 8, StopBits.Two);
					device.Handshake = Handshake.None;
					// serialPort.ReadTimeout = 100;
					device.NewLine = "\r";


					if (canAdapter != null)
						{
						canAdapter.CanMsgReceived -= callbackDelegate;
						canAdapter.Close();
						canAdapter = null;
						}

					canAdapter = new CANaerospace(device, 255);
					callbackDelegate = new CANaerospace.ProcessCanMsg(MsgReceivedCallback);
					canAdapter.CanMsgReceived += callbackDelegate;

					refreshToolStripMenuItem.Enabled = true;
					// clear the tree
					refreshToolStripMenuItem_Click(null, null);
					} catch (Exception ex)
					{
					MessageBox.Show(ex.Message, "Error when connecting to SLCAN");
					return false;
					}
				return true;
				}

			return false;
			}

		private void connectToolStripMenuItem_Click(object sender, EventArgs e)
			{
			Connect();
			}

		private void exitToolStripMenuItem_Click(object sender, EventArgs e)
			{

			}

		private void contentsToolStripMenuItem_Click(object sender, EventArgs e)
			{

			}

		private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
			{

			}

		private void deviceTree_BeforeExpand(object sender, TreeViewCancelEventArgs e)
			{

			}

		private void deviceTree_BeforeSelect(object sender, TreeViewCancelEventArgs e)
			{
			//IParameterDefinition defn = propertyGrid.SelectedObject as IParameterDefinition;

			//if (defn != null && defn.IsDirty)
			//{
			//  switch(MessageBox.Show("Parameter has changed, save?", "Confirm Save", MessageBoxButtons.YesNoCancel))
			//  {
			//    case System.Windows.Forms.DialogResult.Yes :
			//      defn.Save();
			//      break;
			//    case System.Windows.Forms.DialogResult.No :
			//      defn.Revert();
			//      break;
			//    case System.Windows.Forms.DialogResult.Cancel :
			//      e.Cancel = true;
			//      break;
			//  }
			//}
			}

		private void deviceTree_AfterSelect(object sender, TreeViewEventArgs e)
			{
			// we pass the selected node into the property grid
			propertyGrid.SelectedObject = ((IPropertyBag)e.Node).PropertyBag;
			}

		private void CanFlyBrowser_Shown(object sender, EventArgs e)
			{
			Connect();
			}

		private void refreshToolStripMenuItem_Click(object sender, EventArgs e)
			{
			// work over all nodes and remove them

			deviceTree.Nodes.Clear();

			// send a discovery message to all devices
			CANASMsg discoveryMsg = new CANASMsg(0, CANaerospace.IDSService);
			for (int nodeId = 1; nodeId < 256; nodeId++)
				{
				Trace.TraceInformation("Probe for device {0}", nodeId);
				discoveryMsg.Send(canAdapter, CANaerospace.NodeServiceChannel0, (byte)nodeId, 0);              // this is a broadcast
				}

			}

		private void propertyGrid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
			{
			IParameterDefinition defn = e.ChangedItem.Value as IParameterDefinition;

      
			}

		private void fileToolStripMenuItem_Click(object sender, EventArgs e)
			{

			}

		public void EnableSave()
			{
			Invoke((MethodInvoker)delegate
					{
					saveToolStripMenuItem.Enabled = true;
					});
			}

		public void Save()
			{
			// save all changed
			foreach (CanFlyProxy proxy in discoveredDevices)
				if (!proxy.Save())
					return;

			saveToolStripMenuItem.Enabled = false;
			}
		}
	}