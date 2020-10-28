package com.visualthreat.testing.core;

import java.io.IOException;
import java.util.Collection;
import java.util.Map;
import java.util.concurrent.ExecutorService;

import com.visualthreat.platform.common.can.CANLogEntry;
import com.visualthreat.platform.common.model.TestParameter;
import com.visualthreat.platform.common.model.TestStatus;
import com.visualthreat.testing.CANEventListener;
import com.visualthreat.testing.database.DatabaseUtil;

public interface AppControl {
  // sets scenario
  void setScenario(AbstractScenario scenario);

  // Enable disable the request filter
  void enableRequestFilter(boolean status);
  // set expectedRequestFilter
  void setExpectedRequestFilter();
  // Clean expectedRequestFilter
  void cleanExpectedRequestFilter();
  // Get report executor service
  ExecutorService getReportExecutor();
  // Set CurrentCanbusNum
  void setCurrentCanbusNum(int currentCanbusNum);
  // Get CurrentCanbusNum
  int getCurrentCanbusNum();
  // Get set currentCanbusNum flag
  boolean getCurrentCanbusNumSeted();
  // Get the CANBUS Baudrate
  int getBaudrate();

  // Get TestReportUtil

  // Get DatabaseUtilImpl
  DatabaseUtil getDatabaseUtil();

  // Saves state (and result) for restore, will be available in restoreState
  // should use AbstractScenario::getState
  // TestRunner can enhance state with its local variables,
  // like curCANTrafficIndex for TestRunner
  // should not be used with stop, as it'll save state anyway
  void saveState();

  // returns restored state
  Object restoreState();

  // Set filter for input can log entries
  void setFilter(CANLogEntryFilter filter);

  // send requests
  void sendRequest(TestCANRequest request);
  void sendRequests(Collection<CANRequest> requests);
  void flush();
  // add comment to can log
  void comment(String comment);

  default void sendRequestNow(TestCANRequest request) {
      sendRequest(request);
      flush();
  }
  
  default void sendCANFrame(CANLogEntry entry) throws IOException {
    return;
  }

  // iterate through received responses
  Collection<CANLogEntry> readLogs();
  // clear saved logs
  void clearLogs();
  // number of received messages
  long totalReceived();

  // wait until we send all current requests and receive all logs responses
  boolean waitForComplete() throws TestException;
  boolean waitForComplete(int timeout) throws TestException;

  // add and remove can listeners
  void addCANListener(CANEventListener listener);
  void removeCANListener(CANEventListener listener);

  // enable/disable listen ability
  void enableListen();
  void disableListen();

  // start test run, sets status to IN_PROGRESS
  void start();

  // finishes test point, with success or failure, stop should save state + result too
  // shouldn't be called from AbstractScenario subclasses
  void stop(TestStatus status);
  default void complete() {
    stop(TestStatus.SUCCESS);
  }
  default void cancel() {
    stop(TestStatus.CANCELLED);
  }

  // if user cancels or pauses a test
  void userStop(boolean paused);

  // get params and result for dependencies
  Map<String, Object> getParams();
  Object getDependency(String testPointId);
  default <T> T getDependency(String testPointId, Class<T> classTag) {
    return classTag.cast(getDependency(testPointId));
  }
//  default <T> T getDependency(TestPoints testPointEnum, Class<T> classTag) {
//    return getDependency(testPointEnum.id(), classTag);
//  }

  default <T> T getParam(TestParameter<T> testParameter) {
    Object value = getParams().getOrDefault(testParameter.getName(), testParameter.getDefaultValue());
    if (testParameter.getClassTag().equals(String.class)) {
      value = value.toString();
    } else if (testParameter.getClassTag().equals(Integer.class)) {
      value = Integer.valueOf(value.toString());
    } else if (testParameter.getClassTag().equals(Boolean.class)) {
      value = Boolean.valueOf(value.toString());
    }

    try {
      return testParameter.getClassTag().cast(value);
    } catch (ClassCastException e) {
     
    }

    return testParameter.getDefaultValue();
  }


  default void disableAndClear() {
    disableListen();
    clearLogs();
  }
}
