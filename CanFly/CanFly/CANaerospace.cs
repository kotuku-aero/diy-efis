using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Diagnostics;

namespace CanFly
{
  /// <summary>
  /// Class to interface with the CANSERIAL device.
  /// </summary>
  /// <remarks>
  /// The Kotuku CANSERIAL device is a low cost serial device
  /// for connecting to the CAN bus.
  /// The protocol is compatible with the CANUSB device.
  /// </remarks>
  public class CANaerospace
  {
    private SerialPort serialPort;

    public const UInt16 NodeServiceChannel0 = 128;
    public const UInt16 NodeServiceChannel0Reply = 129;
    public const UInt16 NodeServiceChannel1 = 130;
    public const UInt16 NodeServiceChannel2 = 132;
    public const UInt16 NodeServiceChannel3 = 134;
    public const UInt16 NodeServiceChannel4 = 136;
    public const UInt16 NodeServiceChannel5 = 138;
    public const UInt16 NodeServiceChannel6 = 140;
    public const UInt16 NodeServiceChannel7 = 142;
    public const UInt16 NodeServiceChannel8 = 144;
    public const UInt16 NodeServiceChannel9 = 146;
    public const UInt16 NodeServiceChannel10 = 148;
    public const UInt16 NodeServiceChannel11 = 150;
    public const UInt16 NodeServiceChannel12 = 152;
    public const UInt16 NodeServiceChannel13 = 154;
    public const UInt16 NodeServiceChannel14 = 156;
    public const UInt16 NodeServiceChannel15 = 158;
    public const UInt16 NodeServiceChannel16 = 160;
    public const UInt16 NodeServiceChannel17 = 162;
    public const UInt16 NodeServiceChannel18 = 164;
    public const UInt16 NodeServiceChannel19 = 166;
    public const UInt16 NodeServiceChannel20 = 168;
    public const UInt16 NodeServiceChannel21 = 170;
    public const UInt16 NodeServiceChannel22 = 172;
    public const UInt16 NodeServiceChannel23 = 174;
    public const UInt16 NodeServiceChannel24 = 176;
    public const UInt16 NodeServiceChannel25 = 178;
    public const UInt16 NodeServiceChannel26 = 180;
    public const UInt16 NodeServiceChannel27 = 183;
    public const UInt16 NodeServiceChannel28 = 184;
    public const UInt16 NodeServiceChannel29 = 186;
    public const UInt16 NodeServiceChannel30 = 188;
    public const UInt16 NodeServiceChannel31 = 190;
    public const UInt16 NodeServiceChannel32 = 192;
    public const UInt16 NodeServiceChannel33 = 194;
    public const UInt16 NodeServiceChannel34 = 196;
    public const UInt16 NodeServiceChannel35 = 198;

    // service ID's
    /// <summary>
    /// The identification service is a client/server type service. It is used to
    /// obtain a “sign-of-life” indication from the addressed node and check if
    /// its identifier distribution and message header format is compliant with
    /// the network that it is attached to.
    /// </summary>
    public const byte IDSService = 0;
    /// <summary>
    /// The node synchronisation service is a connectionless service (no service
    /// response required) used to perform time synchronisation of all
    /// nodes attached to the network.
    /// </summary>
    public const byte NSSService = 1;
    /// <summary>
    /// The data download is a connection-oriented service and is used to
    /// send a block of data to another node.
    /// </summary>
    public const byte DDSService = 2;
    /// <summary>
    /// The data upload is a connection-oriented service and is used to receive
    /// a block of data from another node.
    /// </summary>
    public const byte DUSService = 3;
    /// <summary>
    /// The simulation control service is a connection-oriented service and is
    /// used to change the behaviour of the addressed node by controlling internal
    /// simulation software functions.
    /// </summary>
    public const byte SCSService = 4;
    /// <summary>
    /// The transmission interval service is a connection-oriented service and
    /// is used to set the transmission rate of a specific CAN message transmitted
    /// by the addressed node.
    /// </summary>
    public const byte TISService = 5;
    /// <summary>
    /// The FLASH programming service is a connection-oriented service
    /// used to store configuration data (i.e. settings made through BSS, NIS,
    /// CSS or similar services) into internal non-volatile memory.
    /// </summary>
    public const byte FPSService = 6;
    /// <summary>
    /// The state transmission service is a connectionless service which causes
    /// the addressed node to transmit all its CAN messages once.
    /// </summary>
    public const byte STSService = 7;
    /// <summary>
    /// The filter setting service is a connection-oriented service used to set
    /// the limit frequency for node-internal highpass, lowpass or bandpass
    /// filter functions:
    /// </summary>
    public const byte FSSService = 8;
    /// <summary>
    /// The test control is a connection-oriented service and is used to control
    /// internal test functions of the addressed node.
    /// </summary>
    public const byte TCSService = 9;
    /// <summary>
    /// The baudrate setting service is a (normally) connectionless service
    /// that modifies the CAN baudrate of the addressed node.
    /// </summary>
    public const byte BSSService = 10;
    /// <summary>
    /// The Node-ID setting service is a connection-oriented service used to
    /// set the ID of the addressed node.
    /// </summary>
    public const byte NISService = 11;
    /// <summary>
    /// The module information service is a connection-oriented service that
    /// returns information about modules installed in the addressed node.
    /// </summary>
    public const byte MISService = 12;
    /// <summary>
    /// The module configuration service is a connection-oriented service that
    /// configures modules installed in the addressed node.
    /// </summary>
    public const byte MCSService = 13;
    /// <summary>
    /// The CAN identifier setting service is a connection-oriented service
    /// used to set the CAN identifier of a specific CAN message transmitted
    /// by the addressed node.
    /// </summary>
    public const byte CSSService = 14;
    /// <summary>
    /// The CANaerospace identifier distribution setting service is a connection-
    /// oriented service used to set the identifier distribution ID of the
    /// addressed node.
    /// </summary>
    public const byte DSSService = 15;

    public byte NodeID { get; private set; }

    private ServiceChannel[] serviceChannels = new ServiceChannel[35];

    private Thread readThread;
    private bool shouldTerminate = false;
    private AutoResetEvent doorbell = new AutoResetEvent(false);
    private AutoResetEvent responseDoorbell = new AutoResetEvent(false);

    private UInt32 acceptance_code = 0x00000000;
    private UInt32 acceptance_mask = 0xFFFFFFFF;

    private readonly string canbus_flag_cmd = "F";
    private readonly string canusb_setup_cmd = "S4";
    private readonly string canusb_version_cmd = "V";
    private readonly string canusb_serial_cmd = "N";
    private readonly string canusb_open_cmd = "O";
    private readonly string canusb_code_cmd = "M{0:X2}{1:X2}{2:X2}{3:X2}";
    private readonly string canusb_mask_cmd = "m{0:X2}{1:X2}{2:X2}{3:X2}";
    private readonly string canusb_flush_cmd = "";
    private readonly string canusb_timestamp_off = "Z0";
    private readonly string canusb_close_cmd = "C";


    private enum STATE
    {
      FIRST,
      CANMSG,
      RESPONSE,
      EMPTY,
    };
    private STATE state = STATE.FIRST;
    private StringBuilder rawMessage = new StringBuilder();
    private Queue<string> responses = new Queue<string>();
    private Mutex responseMutex = new Mutex();

    /// <summary>
    /// Initializes a new instance of the <see cref="CanFly.CANaerospace"/> class.
    /// </summary>
    /// <param name="device">Device to connect to</param>
    /// <param name="nodeID">Node ID of the browser</param>
    /// <remarks>
    /// On linux the device will be something like /dev/ttUSBn assuming a USB serial port
    /// On windows will be something like COM1:
    /// </remarks>
    public CANaerospace(SerialPort device,  byte nodeID)
    {
      NodeID = nodeID;

      serialPort = device;

      serialPort.Open();

      readThread = new Thread(new ThreadStart(ReadWorker));
      readThread.Start();

      // flush the device.
      SendMessage("");
      SendMessage("");
      SendMessage("");
      SendMessage(canusb_close_cmd);

      SendMessage(canbus_flag_cmd);
      SendMessage(canusb_version_cmd);
      SendMessage(canusb_setup_cmd);
      SendMessage(canusb_serial_cmd);
      string cmd;
      cmd = String.Format(canusb_code_cmd, acceptance_code & 0xff, (acceptance_code >> 8) & 0xff, (acceptance_code >> 16) & 0xff, (acceptance_code >> 24) & 0xff);
      SendMessage(cmd);
      cmd = String.Format(canusb_mask_cmd, acceptance_mask & 0xff, (acceptance_mask >> 8) & 0xff, (acceptance_mask >> 16) & 0xff, (acceptance_mask >> 24) & 0xff);
      SendMessage(cmd);
      SendMessage(canusb_timestamp_off);
      SendMessage(canusb_open_cmd);
    }

    public void Close()
    {
      if (readThread != null)
      {
        shouldTerminate = true;

        if (!responseDoorbell.WaitOne(1000))
          readThread.Abort();

        readThread = null;
      }
    }

    /*
         How it works..
         * RECEIVED
         * ========
         * The FTDI DLL unblocks the parserTask (BackgroundWorker) when data arrives.
         * When unblocked the parserTask dequeues the string data from the FTDI driver. The string is parsed byte by byte to reassemble fragmented messages.
         * Messages are split into 'streams'. These streams are:
         *  a) RECEIVED (asynchronous) are converted into a CANMsg datatype and placed on a queue. An msgRxvd_Semaphore is given as notification.
         *  b) RESPONSE messages are written to a string. A RspRxcd_Semaphore is given as notification.
         *  const string &name,
         * TRANSMITTED
         * ===========
         * Sending will block the caller until the response is received. 
         * 
         * Here is quick reference of message strings:
         * SENT COMMANDS and their RESPONSES
         *  Sn\r, O\r, C\r, Miiiiiiii\r, miiiiiiii\r, Zn\r -> \r or \b  (Baud, Open, Close, Acceptance Code, Acceptance Mask, Timestamps)
         *  tiiin[dd...dd]\r, Tiiiiiiiin[dd..ddvvv]\r        -> Z\r or \b (Transmit standard, Transmit extended)
         *  riiin\r,          Riiiiiiiin\r                -> Z\r or \b (Transmit standard RTR, Transmit extended RTR)
         *  F\r -> Fxx\r   (STATUS{b0:RX Q Full, b1:TX Q Full, b2:ErrWrn, b3:DatOvRn, b4:na, b5:ErrPassiv, b6:ArbLost, b7:BussErr})
         *  V\r -> Vxx\r   (Version: 4 x nibbles in BCD)
         *  N\r -> Nxxxx\r (Serial Number)
         * RECEIVED (ansynchronous)
         * tiiin[dd...dd]\r, Tiiiiiiiin[dd..dd]\r  (Received standard, Received extended)
         * 
         * // Other messages seen on the CANBUS -> But NOT documented are:
         * When reading back the serial number..
         * N\r -> N****\r   (Serial number could not be read? Dont know why. Changed parser to accept '*' chars)
         */
    private void ReadWorker()
    {
      while (!shouldTerminate)
      {
        // When traffic gets heavy, expect data to arrive in random chunks with no regard for the start
        // or end of the CanUsb message.
        // Don't expect the \r to be the last char we receive.. the string often ends half way
        // through a message.. It could be the next .Read() starts with a /r at the beginning of the string.
        // If a fragment of a CanUsb message is at the end of the string, we can�t throw it away.. it
        // needs to prefixed to the next incoming message otherwise we are going to throw away data.
        // Aswell as this, when we connect to a CanUsb device that is in full flow, dont expect it
        // to start at the beginning of a CanUsb message.. 
        // - This explains some of the complexity of the code below...
        // There is the added headache that an ACK to some messages is only indicated by a \r
        // inserted between the received messages - which are themselves terminated by a \r
        //  "t12300\rt12300\r\rt12300"   - spot the ack to RTR transmit..
        // .. it could arrive like this.. "t12300\rt12300\r" then "\rt12300"
        // To avoid ugly code, I have chosen to parse incoming data byte by byte using a statemachine.
        // The number of times the timestamp has  
        // Block until we receive a signal from the FTDI driver that data has arrived..
        doorbell.WaitOne(100);

        if (shouldTerminate)
          break;

        int charsExpected = 80;

        // Check how much data there is
        while (!shouldTerminate)
        {
          // Get the data..
          char[] buffer = new char[charsExpected];

          int charsActual = serialPort.Read(buffer, 0, charsExpected);

          if (charsActual > 0)
          {
            // Split into two streams.. Incoming message - and ACKs / NAKs
            //Trace.TraceInformation("Received {0}\n", new string(buffer));
            string str;
            for (int i = 0; i < charsActual; i++)
            {
              char ch = buffer[i];

              // Extract the incoming received CAN messages ('t' or 'T')
              STATE messageType = STATE.EMPTY;
              str = ParseChar(ch, ref messageType);
              if (messageType == STATE.CANMSG)
              {
                // We have received an async CAN message
                //Trace.TraceInformation(String.Format("Message from canbus {0}\n", str));
                CanCallback(new CANASMsg(str));
              }
              else if (messageType == STATE.RESPONSE)
              {
                // We have and ACK NACK or RSP. Add it to the queue
                // AckNackRspQueue.Enqueue(str);
                // TODO: handle this?
                //if (str[0] == '\a')
                //  str = "Error\r";

                ////              trace_debug("Response from canbus %s\n", str.c_str());
                //try
                //{
                //  responseMutex.WaitOne();
                //  responses.Enqueue(str);
                //  responseDoorbell.Set();
                //}
                //finally
                //{
                //  responseMutex.ReleaseMutex();
                //}
              }
            }
          }
        }
      }

      // signal we are done.
      responseDoorbell.Set();
    }

    private string ParseChar(char ch, ref STATE msgstate)
    {
      string retval = String.Empty;
      msgstate = STATE.EMPTY;
      // Process char
      switch (state)
      {
        case STATE.FIRST:
          if (ch == '\r')
          {
            msgstate = STATE.RESPONSE;
            retval = "\r";
          }
          // If there is an \a, this is taken to be a NACK
          else if (ch == '\a')
          {
            msgstate = STATE.RESPONSE;
            retval = "\a";
          }
          // Check for a CAN message received
          else if ((ch == 't') || (ch == 'T') || (ch == 'r') || (ch == 'R'))
          {
            // Clear the string and add the byte..
            rawMessage = new StringBuilder(); // Clear string builder contents
            rawMessage.Append(ch);
            // Get the rest of the message
            state = STATE.CANMSG;
          }
          // Check for a response to see if it a response to a messages that may have been sent
          else if ((ch == 'F') || (ch == 'V') || (ch == 'N') || (ch == 'z') || (ch == 'Z'))
          {
            // Clear the string and add the byte..
            rawMessage = new StringBuilder(); // Clear string builder contents
            rawMessage.Append(ch);
            // Get the rest of the message
            state = STATE.RESPONSE;
          }
          else
          {
            // Unrecognised char. Idle.
            state = STATE.FIRST;
          }
          break;
        case STATE.CANMSG:
          // Append the received char
          if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'F')))
          {
            // The message body
            rawMessage.Append(ch);
          }
          else
          {
            state = STATE.FIRST;
            // Not a hex value
            if (ch == '\r')
            {
              // We have reached the end of the message
              msgstate = STATE.CANMSG;
              retval = rawMessage.ToString();
            }
            else if (ch == '\a')
            {
              Trace.TraceWarning("Unexpected NACK Rxved during reception of CAN Message");
            }
            else
            {
              // We have received an invalid char Exit
              Trace.TraceWarning("Invalid char Rcved during reception of CAN Message");
            }
          }
          break;
        case STATE.RESPONSE:
          // Append the received char
          if ((Char.IsLetterOrDigit(ch)) || (ch == '*'))
          {
            // The message body
            rawMessage.Append(ch);
          }
          else
          {
            state = STATE.FIRST;
            // Not a hex value
            if (ch == '\r')
            {
              // We have reached the end of the message
              msgstate = STATE.RESPONSE;
              retval = rawMessage.ToString();
            }
            else if (ch == '\a')
            {
              Trace.TraceWarning("Unexpected NACK Rcved during reception of Response");
            }
            else
            {
              // We have received an invalid char Exit
              Trace.TraceWarning("Invalid char Rxved during reception of Response");
            }
          }
          break;
      }

      return retval;
    }


    public void ReleaseService(ServiceChannel channel)
    {
      UInt16 channelIndex = channel.ChannelNumber;
      channelIndex -= NodeServiceChannel0;
      channelIndex >>= 1;

      if (serviceChannels[channelIndex] != null &&
         serviceChannels[channelIndex] == channel)
      {
        serviceChannels[channelIndex].ReleaseService();
        serviceChannels[channelIndex] = null;
      }
    }

    public DUService AllocateDUService(int channelNumber)
    {
      if (serviceChannels[channelNumber] != null)
        return null;

      DUService service = new DUService(this, (UInt16)((channelNumber << 1) + NodeServiceChannel0));
      serviceChannels[channelNumber] = service;

      return service;
    }

    public DDService AllocateDDService(int channelNumber)
    {
      if (serviceChannels[channelNumber] != null)
        return null;

      DDService service = new DDService(this, (UInt16)((channelNumber << 1) + NodeServiceChannel0));
      serviceChannels[channelNumber] = service;

      return service;
    }

    public MIService AllocateMIService(int channelNumber)
    {
      if (serviceChannels[channelNumber] != null)
        return null;

      MIService service = new MIService(this, (UInt16)((channelNumber << 1) + NodeServiceChannel0));
      serviceChannels[channelNumber] = service;

      return service;
    }

    internal void SendMessage(CANASMsg msg)
    {
      SendMessage(msg.ToString());
    }

    private void SendMessage(string msg)
    {
      string response = String.Empty;

      SendMessage(msg, 1000, ref response);
    }

    private void SendMessage(string msg, int timeout)
    {
      string response = String.Empty;

      SendMessage(msg, timeout, ref response);
    }

    private Mutex serialMutex = new Mutex();

    private void SendMessage(string msg, int timeout, ref string response)
    {
      //Trace.TraceInformation("Send {0}\n", msg);
      // write the message
      serialMutex.WaitOne();
      serialPort.WriteLine(msg);
      serialMutex.ReleaseMutex();
    }

    public delegate void ProcessCanMsg(CANASMsg msg);
    public event ProcessCanMsg CanMsgReceived;

    private void CanCallback(CANASMsg msg)
    {
      if (CanMsgReceived != null)
        CanMsgReceived(msg);
    }

    public static string[] EnumerateDevices()
    {
      return SerialPort.GetPortNames();
    }
  }
}
