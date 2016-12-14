using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Diagnostics;

namespace CanFly
{
  public struct mis_message_t
  {
    public byte memid;                // memory ID used to access this parameter
    public byte parameter_type;       // Type of parameter, see comments below
    public byte parameter_length;     // number of bytes in the parameter.
    public byte name_length;          // number of characters in name (1..16)
    public string name;               // Name of the parameter
  };

  /// <summary>
  /// This class implements a proxy for a device that is described
  /// by a series of parameters only
  /// </summary>
  public class ParameterDefinitionProxy : CanFlyProxy
  {
    private int numParameters;
    private int nextParameter;
    private MIService miService;
    private DUService duService;
    private DDService ddService;
    private int channelNumber;

    private enum LoadModuleDetailsState
    {
      NotStarted,
      ProcessModuleInformation,
      ProcessParameters,
      ProcessDone,
    };

    private LoadModuleDetailsState processState = LoadModuleDetailsState.NotStarted;

    private List<IParameterDefinition> parameterDefinitions = new List<IParameterDefinition>();

    public ParameterDefinitionProxy(string name, CANaerospace channel, int channelNumber, CANASMsg idsMessage)
      : base(name, channel, channelNumber, idsMessage)
    {
    }

    public virtual void ProcessMsg(CANASMsg msg)
    {
      if(IsValid)
      {
        foreach(IParameterDefinition defn in parameterDefinitions)
        {
          if (defn is CanFlyType0ParameterDefinition)
          {
            CanFlyType0ParameterDefinition value = defn as CanFlyType0ParameterDefinition;
            if (value.Identifier == msg.ID)
            {
              value.Value = msg;
            }
          }
        }
      }
    }

    [Browsable(false)]
    public bool IsValid { get { return processState == LoadModuleDetailsState.ProcessDone; } }

    [Browsable(false)]
    public int Count { get { return parameterDefinitions.Count; } }

    [Browsable(false)]
    public IParameterDefinition this[int index] { get { return parameterDefinitions[index]; } }

    protected override void ProxyInitialize(int channelNumber)
    {
      // monitor the channel
      Channel.CanMsgReceived += new CANaerospace.ProcessCanMsg(ProcessMsg);

      // create our services
      miService = Channel.AllocateMIService(channelNumber);
      processState = LoadModuleDetailsState.NotStarted;
      ProcessService(null);
      this.channelNumber = channelNumber;
    }

    private void ProcessStore(DDService.DDSResult result)
    {
      switch (result)
      {
        case DDService.DDSResult.InProgress:
          return;
        case DDService.DDSResult.Error:
          return;
        case DDService.DDSResult.Complete:
          nextParameter++;
          if (nextParameter >= parameterDefinitions.Count)
          {
            Channel.ReleaseService(ddService);
            return;
          }
          break;
      }

      SendDefinition();
    }

    private void SendDefinition()
    {
      IParameterDefinition defn = parameterDefinitions[nextParameter];
      while (!defn.IsDirty)
      {
        nextParameter++;
        if (nextParameter >= parameterDefinitions.Count)
        {
          Channel.ReleaseService(ddService);
          return;
        }
        defn = parameterDefinitions[nextParameter];
      }

      Trace.TraceInformation("Upload parameter {0} to node {1}", defn.Name, NodeID);
      ddService.Send(NodeID, defn.Memid, defn.PrepareForUpload(), ProcessStore);
    }

    private void ProcessService(byte[] buffer)
    {
      switch(processState)
      {
        case LoadModuleDetailsState.NotStarted:
          processState = LoadModuleDetailsState.ProcessModuleInformation;
          numParameters = -1;
          nextParameter = 0;

          Trace.TraceInformation("Request first parameter from node {0}", NodeID);
          miService.Receive(NodeID, (uint) nextParameter, 2, new MIService.ReceiveHandler(ProcessService));
          break;
        case LoadModuleDetailsState.ProcessModuleInformation:
          if(numParameters == -1)
          {
            // todo: check that a short was returned
            numParameters = (((int)buffer[0]) * 256) + ((int)buffer[1]);
            Trace.TraceInformation("Device advises there are {0} parameters", numParameters);
            nextParameter = 1;
          }
          else
          {
            mis_message_t message = new mis_message_t();

            message.memid = buffer[0];
            message.parameter_type = buffer[1];
            message.parameter_length = buffer[2];
            message.name_length = buffer[3];

            StringBuilder nameBuilder = new StringBuilder();
            // this is the name of a parameter
            for (byte i = 4; i < buffer.Length; i++)
            {
              if (buffer[i] == 0)
                break;
              nameBuilder.Append((char)buffer[i]);
            }
            message.name = nameBuilder.ToString();

            IParameterDefinition defn = null;
            switch (message.parameter_type)
            {
              case 0:
                defn = new CanFlyType0ParameterDefinition(message);
                break;
              default :
                defn = new CanFlyType1ParameterDefinition(message);
                break;
            }

            defn.IsDirty = false;

            Trace.TraceInformation("Discovered parameter named {0}", nameBuilder.ToString());
            nextParameter++;

            if (defn != null)
            {
              parameterDefinitions.Add(defn);
              new ParameterNode(Node, defn);
            }
          }

          if (nextParameter <= numParameters)
          {
            miService.Receive(NodeID, (uint) nextParameter, 20, new MIService.ReceiveHandler(ProcessService));
          }
          else
          {
            Channel.ReleaseService(miService);
            miService = null;
            nextParameter = 0;
            processState = LoadModuleDetailsState.ProcessParameters;
            duService = Channel.AllocateDUService(channelNumber);
            duService.Receive(NodeID, (uint) nextParameter, parameterDefinitions[nextParameter].DownloadBytes, new DUService.ReceiveHandler(ProcessService));
          }
          break;
        case LoadModuleDetailsState.ProcessParameters:
          Trace.TraceInformation("Receive parameter definition for {0}", nextParameter);
          parameterDefinitions[(int)nextParameter].LoadDefinition(buffer);
          nextParameter++;

          if (nextParameter < numParameters)
          {
            duService.Receive(NodeID, (uint)nextParameter, parameterDefinitions[nextParameter].DownloadBytes, new DUService.ReceiveHandler(ProcessService));
            Trace.TraceInformation("Request parameter {0}", nextParameter);
          }
          else
          {
            Trace.TraceInformation("All parameters downloaded");
            Channel.ReleaseService(duService);
            processState = LoadModuleDetailsState.ProcessDone;

            ParametersDownloaded();
          }
          break;
      }
    }

    public virtual void ParametersDownloaded()
    {
      foreach (IParameterDefinition defn in parameterDefinitions)
        defn.IsDirty = false;
    }

    public override bool Save()
    {
      // create our services
      ddService = Channel.AllocateDDService(channelNumber);
      Trace.TraceInformation("Start uploading parameters to node {0}", NodeID);
      nextParameter = 0;
      SendDefinition();
 
      return true;
    }
  }
}
