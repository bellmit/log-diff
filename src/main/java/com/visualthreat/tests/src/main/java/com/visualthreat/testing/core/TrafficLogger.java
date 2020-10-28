package com.visualthreat.testing.core;

import com.visualthreat.testing.CANEventListener;

import java.io.File;
import java.text.SimpleDateFormat;

public abstract class TrafficLogger implements CANEventListener {


  public enum TRAFFIC_LOG_STATE {
    BEFORE,
    RUNNING,
    AFTER
  }


  public static int MAX_TRAFFIC_FILES_TO_KEEP = 120;
  protected static String DEFAULT_LOG_FOLDER = "logs";
  protected static File logRootFolder = null;
  protected static SimpleDateFormat dateFormat = new SimpleDateFormat("MMM_dd_HH_mm_ss");

  public abstract void close();

  public abstract TestTrafficLogger.TRAFFIC_LOG_STATE getCurState();

  public abstract void setCurState(TestTrafficLogger.TRAFFIC_LOG_STATE curState);

  public abstract void writeStringToLogFile(long timeStamp, int id, byte[] data, int dlc);
}
