package com.visualthreat.testing.core;

import com.visualthreat.platform.common.can.CANLogEntry;
import com.visualthreat.platform.common.model.proto.generated.TcpApiProtos.CANFilter;

/**
 * Used to filter CAN Log Entries
 */
public interface CANLogEntryFilter {
  boolean filter(CANLogEntry event);
  CANFilter getCANFilter();
}
