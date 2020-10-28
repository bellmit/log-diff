package com.visualthreat.testing.core;

public interface CanTrafficIndex {
  boolean isResumeMode();
  Long getResumeStateCmdIndex();
  void setCurCANTrafficIndex(Long index);
}
