package com.visualthreat.testing.core;

import com.visualthreat.platform.common.can.CANLogEntry;
import lombok.Value;

@Value
public class CANRequest {
  private final CANLogEntry canMessage;
  private final long waitTime;
}
