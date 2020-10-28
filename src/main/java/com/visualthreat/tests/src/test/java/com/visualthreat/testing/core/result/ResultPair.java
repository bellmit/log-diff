package com.visualthreat.testing.core.result;

import com.visualthreat.platform.common.can.CANLogEntry;
import lombok.Value;

import java.util.Collection;

@Value
public class ResultPair<T> {
  private final String source;
  private final Collection<CANLogEntry> canLogEntries;
  private final T result;
}
