package com.visualthreat.testing.core;

import static com.visualthreat.testing.core.TestParameters.EXTENSIVE_EXECUTION_MODE;

import java.security.SecureRandom;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.Set;

import com.visualthreat.platform.common.can.CANLogEntry;
import com.visualthreat.platform.common.can.CANLogEntry.FrameType;
import com.visualthreat.platform.common.model.TestStatus;
import com.visualthreat.testing.core.status.RunProgressListener;
import com.visualthreat.testing.core.tests.TestConst;

import lombok.Getter;
import lombok.RequiredArgsConstructor;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;

@RequiredArgsConstructor
@Slf4j
abstract public class AbstractScenario {
  public static final int KEEP_ALIVE_TIME_INTERVAL = 1500;
  private static final int REQUEST_TIMEOUT_INTERVAL = 5;
  public static final String NON_EMPTY_RESULT_PREFIX = "Found";

  public static String STATUS_MESSAGE_FORMAT = "%2.0f%% in Progress";

  @Getter
  protected final AppControl control;
  private Thread keepAliveWorker = null;
  private Thread keepTesterPresentAliveWorker = null;

  @Getter @Setter
  private boolean keepAlive = false;

  @Getter @Setter
  private boolean gmlanKeepAlive = false;

  @Getter @Setter
  private TestStatus status = TestStatus.NOT_STARTED;

  @Getter @Setter
  private String errorString = "";

  @Getter @Setter
  private RunProgressListener statusUpdater;
  @Getter @Setter
  private int testPointIndex = -1;

  public static final byte[] ENTER_PROG_SESSION =
      new byte[]{0x02, 0x10, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
  private static final byte[] GENERIC_PAYLOAD =
      new byte[]{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  protected static final byte[] RESET_ECU =
      new byte[]{0x02, 0x11, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
  public static final String CONST_SERVICE_SUPPORTED = "serviceSupported";
  public static final String CONST_SERVICE_NEED_NEW_SESSIONS = "serviceNeedNewSessions";

  protected StringBuilder result = new StringBuilder();
  protected int STARTING_COUNT = 0;

  private static Random rn = new SecureRandom();

  // === execution flow methods === //

  // check params and dependencies availability here
  public void before() throws TestException {}

  abstract public void run() throws TestException;

  public void after() throws TestException {}

  public boolean canResume() {
    return false;
  }

  // === get and set state === //
  public Object getState() {
    return null;
  }
  public void setState(Object state) {}

  // === get result === //
  public String getResult() {
    return "";
  }



  public void stop() {
    this.keepAlive = false;
    this.gmlanKeepAlive = false;
  }

  // === helpers === //


  protected void stopKeepAlive() {
    this.keepAlive = false;
    if (this.keepAliveWorker == null) {
      return;
    }
    this.keepAliveWorker.interrupt();
    try {
      this.keepAliveWorker.join(1200);
    } catch (InterruptedException ignored) {
    	log.error("Exception:", ignored);
      Thread.currentThread().interrupt();
    } catch (Exception e) {
    	log.error("Exception:", e);
	}
    this.keepAliveWorker = null;
  }


  protected void gmlanStopKeepAlive() {
    this.gmlanKeepAlive = false;
    if (this.keepTesterPresentAliveWorker == null) {
      return;
    }
    this.keepTesterPresentAliveWorker.interrupt();
    try {
      this.keepTesterPresentAliveWorker.join(1200);
    } catch (InterruptedException ignored) {
    	log.error("Exception:", ignored);
      Thread.currentThread().interrupt();
    } catch (Exception e) {
    	log.error("Exception:", e);
	}
    this.keepTesterPresentAliveWorker = null;
  }


  protected static byte[] getGenericPayLoad() {
    return Arrays.copyOf(GENERIC_PAYLOAD, GENERIC_PAYLOAD.length);
  }

  protected static void addECUResetRequest(Integer requestId, TestCANRequest request ) {
    CANLogEntry packet = new CANLogEntry();
    packet.setId(requestId);
    packet.setData(RESET_ECU);
    request.getCmds().add(packet);
  }

  protected static List<CANLogEntry> getRemainingPayload(int reqId, int payLoadSize) {
    List<CANLogEntry> result = new ArrayList<>();
    if(payLoadSize <= 0) {
      return result;
    }

    int remainingSize = payLoadSize;
    int index = 0x21;
    Random rn = new Random();
    do {
      byte[] curPayLoad = getGenericPayLoad();
      curPayLoad[0] = (byte) (index & 0xFF);
      for (int bytePos = 1; bytePos <= Math.min(7, remainingSize); bytePos++) {
        curPayLoad[bytePos] = (byte) (rn.nextInt(256) & 0xFF);
      }
      remainingSize = remainingSize - 7;
      index++;
      if(index >= 0x30) {
        index = 0x20;
      }
      CANLogEntry packet = new CANLogEntry();
      packet.setId(reqId);
      packet.setData(curPayLoad);
      result.add(packet);
    } while(remainingSize > 0);

    return result;
  }

  protected CANLogEntry createCanLogEntry(int requestId, byte[] data) {
    CANLogEntry packet = new CANLogEntry();
    packet.setId(requestId);
    packet.setData(data);
    return packet;
  }

  /**
   * @param idPairsString is in format <requestId1>,<responseId1>;<requestId2>,<responseId2>;...
   */
  protected void extractInputIDs(
      String idPairsString, Map<Integer, Set<Integer>> ecuIds) throws TestException {

    // if id pairs is not set, skip
    if(idPairsString == null || idPairsString.trim().equals("")){
      return;
    }
    String[] idPairs = idPairsString.split(";");
    for(String idPair : idPairs) {
      String[] idPairArray = idPair.replace(" ","").split(",");
      if(idPairArray.length != 2) {
        throw new TestException("Wrong ID Pair String=" + idPair);
      }
      if(!ecuIds.containsKey(Integer.decode(idPairArray[0]))){
        ecuIds.put(Integer.decode(idPairArray[0]), new HashSet<>());
      }
      ecuIds.get(Integer.decode(idPairArray[0])).add(Integer.decode(idPairArray[1]));
    }
  }

  public void setExpectedRequestFilter(){
    //expectedRequestFilter.setFilter(control);
  }

  public boolean getEnableRequestFilterFlag(){
    Map<String, Object> controlParams = this.getControl().getParams();
    if(controlParams.containsKey(TestParameters.ENABLE_REQUEST_FILTER.getName())){
      return this.getControl().getParam(TestParameters.ENABLE_REQUEST_FILTER);
    }
    return false;
  }

  public List<CANLogEntry> fuzzCurrentSubFunction(
      int requestId, int serviceId, List<Byte> subFunctionBytes,
      int payLoadMinLength, int payLoadMaxLength, boolean allowVarPayloadLength) {

    List<CANLogEntry> result = new ArrayList<>();
    Random rn = new Random();

    //    // set custom payLoadLength if defined
    if(allowVarPayloadLength && control.getParams().containsKey(TestParameters.MIN_PAYLOAD_LENGTH.getName())){
      payLoadMinLength = control.getParam(TestParameters.MIN_PAYLOAD_LENGTH);
      payLoadMaxLength = control.getParam(TestParameters.MAX_PAYLOAD_LENGTH);
    }

    for(int i = payLoadMinLength ; i < payLoadMaxLength; i++) {
      int offset = 0;
      byte[] curPayLoad = new byte[] {0,0,0,0,0,0,0,0};

      if(i>=8) {
        curPayLoad[0] = (byte) 0x10;
        curPayLoad[0] = (byte) (curPayLoad[0] + (byte)((i >> 8) & 0x0F));
        curPayLoad[1] = (byte) (i & 0xFF);
        offset = 1;
      } else {
        curPayLoad[0] = (byte) (i & 0xFF);
        offset = 0;
      }
      curPayLoad[1 + offset] = (byte)serviceId;
      int bytePos = 2;
      for(Byte funcByte : subFunctionBytes) {
        curPayLoad[bytePos + offset] = funcByte;
        bytePos++;
      }
      // when payLoadLength == 4
      if(i == 4 && (bytePos + offset) <=4 ) {
        // add 5ae29fc4
        curPayLoad[bytePos + offset] = (byte)0x5a;
        curPayLoad[bytePos + offset + 1] = (byte)0xe2;
        curPayLoad[bytePos + offset + 2] = (byte)0x9f;
        curPayLoad[bytePos + offset + 3] = (byte)0xc4;
      } else {
        for (int j = bytePos + offset; j < 8; j++) {
          curPayLoad[j] = (byte) (rn.nextInt(256) & 0xFF);
        }
      }
      CANLogEntry packet = new CANLogEntry();
      packet.setId(requestId);
      packet.setData(curPayLoad);
      packet.setType(CANLogEntry.FrameType.request);
      result.add(packet);

      final byte[] byteFlowControlTraffic = new byte[]{0x30, 0, 0, 0, 0, 0, 0, 0};
      if(i >= 8){
        CANLogEntry controlPacket = new CANLogEntry();
        controlPacket.setId(requestId);
        controlPacket.setData(byteFlowControlTraffic);
        controlPacket.setType(CANLogEntry.FrameType.request);
        result.add(controlPacket);
      }

      result.addAll(getRemainingPayload(requestId, i - 6));
    }

    return result;
  }

  public static CANLogEntry fuzzCurrentSubFunction(
      int requestId, int serviceId, List<Byte> subFunctionBytes, int length) {

    Random rn = new Random();
    int offset = 0;

    byte[] curPayLoad = AbstractScenario.getGenericPayLoad();
    if (length >= 8) {
      curPayLoad[0] = (byte) 0x10;
      curPayLoad[1] = (byte) (length & 0xFF);
      curPayLoad[0] = (byte) (curPayLoad[0] + (byte) ((length >> 8) & 0x0F));
      offset = 1;
    } else {
      curPayLoad[0] = (byte) (length & 0xFF);
      offset = 0;
    }
    curPayLoad[1 + offset] = (byte) serviceId;
    int bytePos = 2;
    for (Byte funcByte : subFunctionBytes) {
      curPayLoad[bytePos + offset] = funcByte;
      bytePos++;
    }
    for (int j = bytePos + offset; j < 8; j++) {
      curPayLoad[j] = (byte) (rn.nextInt(256) & 0xFF);
    }
    CANLogEntry packet = new CANLogEntry();
    packet.setId(requestId);
    packet.setData(curPayLoad);

    return packet;
  }

  public void updateProgress(String progressInfo) {
    if(this.testPointIndex < 0 || this.statusUpdater == null) {
      return;
    }

    this.statusUpdater.updateStatus(testPointIndex, progressInfo);
  }

  /**
   * Try to run 0x7DF firstly to shorten whole test duration
   * @param targetedRequestIDs
   * @return
   */
  protected List<Integer> reOrderRequestIDs(Map<Integer, Set<Integer>> targetedRequestIDs) {
    Integer broadcastingCANID = 0x7DF;
    int maxNumRespIds = 1;
    for(Integer requestId : targetedRequestIDs.keySet()) {
      if(maxNumRespIds < targetedRequestIDs.get(requestId).size()){
        maxNumRespIds = targetedRequestIDs.get(requestId).size();
        broadcastingCANID = requestId;
      }
    }
    List<Integer> result = new ArrayList<>();
    for(Integer requestId : targetedRequestIDs.keySet()) {
      if(!requestId.equals(broadcastingCANID)){
        result.add(requestId);
      }
    }
    if(control != null && control.getParam(TestParameters.SLOW_EXECUTION_MODE)){
      result.add(broadcastingCANID);
    } else {
      result.add(0, broadcastingCANID);
    }

    return result;
  }

  protected Set<Integer> extractFinishedECUIDs(String test_point_id){
        return getControl().getDatabaseUtil() != null ?
            getControl().getDatabaseUtil().loadFinishedECUIDs(test_point_id)
            : new HashSet<>();
  }

  protected void getLastLatestResultForResume(){
    String lastResult = control.getParams().containsKey(TestParameters.RESUME_LAST_RESULT.getName()) ?
        this.control.getParam(TestParameters.RESUME_LAST_RESULT) : "";
    if(lastResult != null && lastResult.startsWith(NON_EMPTY_RESULT_PREFIX)){
      String[] resultArray = lastResult.split("\n");
      STARTING_COUNT = resultArray.length - 1;
      for(int i = 1; i < resultArray.length; i++){
        result.append(resultArray[i] +"\n");
      }
    }
  }

  /**
   * random value Integer
   * @param min value min
   * @param max value max
   * @return value random
   */
  public static int randInt(int min, int max) {
    Random rand = new Random();
    int randomNum = rand.nextInt((max - min) + 1) + min;
    return randomNum;
  }

}
