package com.visualthreat.testing.core;

import com.visualthreat.platform.common.can.CANLogEntry;
import com.visualthreat.platform.common.model.proto.generated.TcpApiProtos;

import java.util.ArrayList;
import java.util.List;

import lombok.Data;
/**
 * Wrapper class to send test CAN Request
 *
*/
@Data
public class TestCANRequest {
  private String name;
  private List<CANLogEntry> cmds = new ArrayList<>();

  public TcpApiProtos.CANFilter responseFilter;
  private int waitTimeInMilliSecs = 150;

  public List<CANLogEntry> getCmds() {
    return cmds;
  }

  public void setCmds(List<CANLogEntry> cmds) {
    this.cmds = cmds;
  }


  public int getWaitTimeInMilliSecs() {
    return waitTimeInMilliSecs;
  }

  public void setWaitTimeInMilliSecs(int waitTimeInMilliSecs) {
    this.waitTimeInMilliSecs = waitTimeInMilliSecs;
  }


  public TcpApiProtos.CANFilter getResponseFilter() {
    return responseFilter;
  }

  public void setResponseFilter(TcpApiProtos.CANFilter responseFilter) {
    this.responseFilter = responseFilter;
  }

  public TestCANRequest(String requestName) {
    this.name = requestName;
  }

  public TestCANRequest(TestCANRequest other) {
    this.name = other.name;
    this.cmds.addAll(other.cmds);
    this.responseFilter = other.responseFilter;
    this.waitTimeInMilliSecs = other.waitTimeInMilliSecs;
  }
}
