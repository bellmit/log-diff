package com.visualthreat.testing.core.status;

import com.visualthreat.platform.common.can.CANLogEntry;
import com.visualthreat.platform.common.model.TestStatus;

public interface RunProgressListener {
  int testPointCount();
  void activeTestPoint(int index);
  void process(int index, CANLogEntry canLogEntry);
  void updateStatus(int index, String statusInfo);
  void doneTestPoint(int index, TestStatus status, int canbus_num);
}
