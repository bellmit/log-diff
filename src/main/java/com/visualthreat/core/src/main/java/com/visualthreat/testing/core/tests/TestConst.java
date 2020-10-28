package com.visualthreat.testing.core.tests;

public class TestConst {
  public static byte SET_BIT(byte val, int bitIndex){
    int val_0 = 1 << bitIndex;
    val = (byte)(val | val_0);
    return val;
  }
  public static byte CLEAR_BIT(byte val, int bitIndex){
    int val_0 = ~(1 << bitIndex);
    val = (byte)(val & val_0);
    return val;
  }
  public static byte TOGGLE_BIT(byte val, int bitIndex){
    int val_0 = 1 << bitIndex;
    val = (byte)(val ^ val_0);
    return val;
  }
  public static byte IS_SET(byte val, int bitIndex){
    int val_0 = 1 << bitIndex;
    val = (byte)(val & val_0);
    return val;
  }

  // GM READ DIAG SUB FUNCS
  public static int UDS_READ_STATUS_BY_MASK           = 0x81;

  // Periodic Data Message types */
  public static int PENDING_READ_DATA_BY_ID_GM        = 1;

  // Configuration
  public static final String CONF_FILE_NAME = "conf.properties";
  public static final String RESUME_MODE = "resumeMode";
  public static final String STATE_FOLDER_CONF_KEY = "stateFolder";
  public static final String TEST_ARGUMENT_DELIMITER = ",";
  public static final String RESPONSE_WAIT_TIME = "response_wait_time";
  public static final String DIAGNOSTIC_SESSION = "diagnostic_session";
  public static final String RANDOM_RESET_ECU = "random_reset_ecu";
  public static final String SLOW_EXECUTION_MODE = "slow_execution_mode";
  public static final String VULNERABLE_EXECUTION_MODE = "vulnerable_execution_mode";
  public static final String EXTENSIVE_EXECUTION_MODE = "extensive_execution_mode";
  public static final String ECU_ID = "ecu_id";
  public static final String BAUD_RATE = "baud_rate";

  public static int CAN_RTR_FLAG 			  = 0x40000000;

  public static final Integer MIN_PAYLOAD_LENGTH = 1;
  public static final Integer MAX_PAYLOAD_LENGTH = 24;

//  public static Integer[] BaudRates =
//      {-1, 10000, 20000, 50000, 80000, 100000, 125000, 250000, 400000, 500000, 750000, 1000000};
public static Integer[] BaudRates =
    {-1, 5000, 10000, 20000, 25000, 33333, 40000, 50000, 75000, 83333, 100000, 125000, 200000, 250000, 400000, 500000, 600000, 750000,  1000000, };
//    {-1, 10000, 20000, 100000, 125000, 250000, 500000, 750000, 1000000};
public static String[] BaudRatesString =
    {"-1", "5k", "10k", "20k", "25k", "33.33k","40k", "50k", "75k", "83.33k", "100k", "125k", "200k", "250k", "400k", "500k", "600k", "750k", "1000k", };
//    {"-1", "10k", "20k", "100k", "125k", "250k", "500k", "750k", "1000k"};

  public enum DiagnosticSession {
    DEFAULT(((byte) 0x01)),
    PROGRAMMING(((byte) 0x02)),
    EXTENDED(((byte) 0x03));
    // SAFETY(((byte) 0x04)); do not support SAFETY for now

    private final byte value;
    private DiagnosticSession(byte sessionVal) {
      this.value = sessionVal;
    }

    public byte getSessionValue() {
      return this.value;
    }
  }

  public enum XCPResourceMask {
    PGM(("Flash programming available")),
    STIM("Stimulation available"),
    DAQ("DAQ lists available"),
    CAL_PAG("Calibration/paging available");

    private final String value;
    private XCPResourceMask(String sessionVal) {
      this.value = sessionVal;
    }

    public String getSessionValue() {
      return this.value;
    }
  }
}
