package com.visualthreat.testing.core;

import static com.visualthreat.testing.core.tests.TestConst.BAUD_RATE;
import static com.visualthreat.testing.core.tests.TestConst.RESUME_MODE;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicReference;

import org.apache.commons.lang3.NotImplementedException;
import org.apache.commons.lang3.StringUtils;

import com.visualthreat.platform.common.can.CANLogEntry;
import com.visualthreat.platform.common.model.TestPoint;
import com.visualthreat.platform.common.model.TestStatus;
import com.visualthreat.testing.CANBus;
import com.visualthreat.testing.CANClient;
import com.visualthreat.testing.CANEventListener;
import com.visualthreat.testing.ICANBus;
import com.visualthreat.testing.database.DatabaseUtil;

import lombok.Getter;
import lombok.RequiredArgsConstructor;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;

@RequiredArgsConstructor
@Slf4j
public class LocalControl implements AppControl, CANClient, CANEventListener, CanTrafficIndex {
  protected static volatile boolean isShutDown = false;
  @Getter
  private final ConcurrentLinkedQueue<CANEventListener> listeners = new ConcurrentLinkedQueue<>();

  @Getter
  protected volatile boolean isStopped = false;

  @Getter
  protected final TestPoint testPoint;
  protected final Properties props;
  @Getter
  protected final String deviceName;
  @Getter
  protected final String stateFolder;

  protected boolean isTestInitialized = false;

  @Getter
  protected boolean isResumeMode = false;

  @Getter
  @Setter
  protected Long resumeStateCmdIndex = 0L;
  @Setter
  protected Long curCANTrafficIndex = 0L;
  @Getter
  protected long receivedRequests = 0;

  protected AtomicReference<CANLogEntryFilter> logFilter = new AtomicReference<>();
  protected ConcurrentLinkedQueue<CANLogEntry> logs = new ConcurrentLinkedQueue<>();
  protected TestCANRequestExecutor executor = null;

  protected AbstractScenario scenario;
  @Getter
  protected ICANBus canBus;
  @Getter
  protected TrafficLogger trafficLogger;
  private ConsoleReader consoleReader;

  private static ExecutorService reportExecutor = null;

  private static int currentCanbusNum = 0;

  private static boolean currentCanbusNumSeted = false;

  @Override
  public void setCurrentCanbusNum(final int currentCanbusNum) {
    this.currentCanbusNum = currentCanbusNum;
    currentCanbusNumSeted = true;
  }

  @Override
  public int getCurrentCanbusNum() {
    return this.currentCanbusNum;
  }

  @Override
  public boolean getCurrentCanbusNumSeted() {
    return currentCanbusNumSeted;
  }

  @Override
  public void setScenario(final AbstractScenario scenario) {
    this.scenario = scenario;
  }

  @Override
  public void saveState() {
    try {
      final Object state = scenario.getState();
      final File stateFile = new File(this.stateFolder + "/" + testPoint.getId(), testPoint.getId() + ".state");
      final FileOutputStream fileStream = new FileOutputStream(stateFile);
      try (ObjectOutputStream os = new ObjectOutputStream(fileStream)) {
        os.writeObject(state);
      }
    } catch (final IOException e) {
      log.error("Can not save state for test point run: {}", testPoint.getId());
    } catch (Exception e) {
    	log.error("Exception:", e);
	}
  }

  @Override
  public Object restoreState() {
    return null; // no resume yet
  }

  @Override
  public void setFilter(final CANLogEntryFilter filter) {
    this.logFilter.set(filter);
  }
  
  
  @Override
  public void sendRequest(final TestCANRequest request) {
    if (request == null || isStopped) {
      return;
    }
    getExecutor().sendTestTraffic(request);
    request.getCmds().clear();
  }

  @Override
  public void sendRequestNow(final TestCANRequest request) {
    if (request == null || isStopped) {
      return;
    }
   
    for (final CANLogEntry entry : request.getCmds()) {
      getExecutor().getCanDevice().setDelayRequest(0);
      getExecutor().sendCANFramesNow(entry);
      final int waitTime = Math.max(request.getWaitTimeInMilliSecs(),
          TestCANRequestExecutor.MIN_CAN_MESSAGE_SENT_INTERVAL);
      try {
        Thread.sleep(waitTime);
      } catch (final InterruptedException ignored) {
          log.error("Exception:", ignored);
        Thread.currentThread().interrupt();
      } catch (Exception e) {
          log.error("Exception:", e);
    }
    }
  }

  @Override
  public void sendCANFrame(final CANLogEntry entry) throws IOException {
    if (isStopped) {
      return;
    }
    canBus.publishCANEvent(entry);
    canBus.sendCANFrame(entry);
  }

  @Override
  public void sendRequests(final Collection<CANRequest> requests) {
    throw new RuntimeException("Not implemented");
  }

  @Override
  public void flush() {
    if (isStopped) {
      return;
    }
    getExecutor().flush();
  }

  @Override
  public void comment(final String comment) {
    if (isStopped) {
      return;
    }
    final CANLogEntry packet = new CANLogEntry();
    packet.setType(CANLogEntry.FrameType.comment);
    packet.setComment(comment);
    canBus.publishCANEvent(packet);
  }

  @Override
  public Collection<CANLogEntry> readLogs() {
    return logs;
  }

  @Override
  public void clearLogs() {
    logs.clear();
  }

  @Override
  public long totalReceived() {
    return receivedRequests;
  }

  @Override
  public boolean waitForComplete() throws TestException {
    return getExecutor().waitForComplete(-1);
  }

  @Override
  public boolean waitForComplete(final int timeout) throws TestException {
    if (isStopped) {
      return true;
    }
    return getExecutor().waitForComplete(timeout);
  }

  @Override
  public void addCANListener(final CANEventListener listener) {
    listeners.add(listener);
  }

  @Override
  public void removeCANListener(final CANEventListener listener) {
    listeners.remove(listener);
  }

  @Override
  public void enableListen() {
    if (canBus != null) {
      canBus.EnableListening();
    }
  }

  @Override
  public void disableListen() {
    if (canBus != null) {
      canBus.DisableListening();
    }
  }

  /**
   * Will be run before scenario run, but after scenario before
   * So before can fail with some dependency checks and in this case
   * we don't have to start canBus and other things
   */
  @Override
  public void start() {
    this.receivedRequests = 0;
    this.curCANTrafficIndex = 0L;

    canBus = new CANBus(this);
    if (!canBus.connect(deviceName, 115200)) {
      log.error("sorry, there was an error connecting serial port=" + deviceName);
      throw new TestException("Can not connect to serial port");
    }

    log.info("Test={} Starts...", testPoint.getDescription());
    canBus.registerListener(this);

    // Detect CAN BUS BaudRate
    try {
      if (props.getProperty(BAUD_RATE) == null) {
        log.info("Detecting BaudRate...");
        canBus.autoDetectBaudRate();
        Thread.sleep(6500);
        canBus.getBaudRate();
      } else {
        log.info("Setting BaudRate...");
        canBus.stop();
        Thread.sleep(500);
        canBus.setBaudRate(
            Integer.valueOf(props.getProperty(BAUD_RATE)));
        canBus.start();
      }
      Thread.sleep(1000);
    } catch (IOException | InterruptedException ex) {
    	log.error("Exception:", ex);
      if (ex instanceof InterruptedException) {
        Thread.currentThread().interrupt();
      }
      throw new TestException("Couldn't detect BaudRate.", ex);
    } catch (Exception e) {
    	log.error("Exception:", e);
	}

    this.trafficLogger = new TestTrafficLogger(testPoint.getId());
    this.isTestInitialized = true;
    if (props.get(RESUME_MODE) != null) {
      // TODO: fully support Resume Mode
      this.isResumeMode = true;
    }
    comment(testPoint.getDescription() + " started.");

    // start ConsoleReader
    consoleReader = new ConsoleReader(canBus).start();

    Runtime.getRuntime().addShutdownHook(new Thread(() -> {
      if (consoleReader != null) {
        consoleReader.setShutDown(true);
      }
    }));
  }

  /**
   * Will be run after scenario finishes, even in case of exception
   *
   * @param status Scenario finished with this status
   */
  @Override
  public void stop(final TestStatus status) {
    if (isStopped) {
      return;
    }

    if (this.executor != null) {
      this.executor.shutdown();
    }
    if (canBus != null) {
      canBus.unRegisterListener(this);
      canBus.disconnect();
      canBus = null;
    }
    isStopped = true;

    if (this.trafficLogger != null) {
      this.trafficLogger.close();
    }
    if (consoleReader != null) {
      this.consoleReader.stop();
    }
    saveState();
    this.receivedRequests = 0;
  }

  @Override
  public void userStop(final boolean paused) {
    throw new NotImplementedException("userStop not implemented for LocalTestControl");
  }

  @Override
  public Map<String, Object> getParams() {
    final Map<String, Object> map = new HashMap<>();
    props.forEach((name, value) ->
        map.put(name.toString(), value));
    return map;
  }

  @Override
  public Object getDependency(final String testPointId) {
    try {
      final File stateFile = new File(this.stateFolder + "/" + testPointId, testPointId + ".state");
      final FileInputStream fileInputStream = new FileInputStream(stateFile);
      try (ObjectInputStream oInputStream = new ObjectInputStream(fileInputStream)) {
        return oInputStream.readObject();
      }
    } catch (final IOException e) {
      // probably no saved state (test point weren't ran before)
      log.info("No saved state for: {}", testPointId);
    } catch (final ClassNotFoundException e) {
      log.error("Unknown class while restoring {} state", testPointId);
    } catch (Exception e) {
    	log.error("Exception:", e);
	}

    return null;
  }

  @Override
  public void disconnected(final int id) {
    if (consoleReader != null) {
      consoleReader.setShutDown(true);
    }
  }

  @Override
  public void writeLog(final int id, final String text) {
    log.info(String.format("id=%d %s", id, text));
  }

  @Override
  public void writeLog(final int id, final String text, final Throwable t) {
    log.error(String.format("id=%d %s", id, text), t);
  }

  private boolean hasInfoSet = false;

  @Override
  public void setInfo(final String info) {
    if (!StringUtils.isEmpty(info) && !hasInfoSet) {
      log.info("System BaudRate=" + info.replace("I", ""));
      hasInfoSet = true;
    }
  }

  @Override
  public void complete() {
    if (this.scenario != null) {
      stop(scenario.getStatus());
    }
  }

  @Override
  public void onEvent(final CANLogEntry event) {
    if (!isTestInitialized || this.trafficLogger == null || isShutDown || isStopped) {
      return;
    }

    // log all traffic to file
    // TODO: make it optional and use addCANListener, so we can safely reuse
    
    // collect filtered logs into memory
    CANLogEntryFilter filter = null;
    if (logFilter != null) {
      filter = logFilter.get();
    }
    
    if(event.getType() == CANLogEntry.FrameType.comment) {
    	this.trafficLogger.onEvent(event);
    	return;
    }
       
    if (event.getType() == CANLogEntry.FrameType.request 
        || (event.getType() == CANLogEntry.FrameType.response &&
        (filter == null || filter.filter(event)))) {
      
      this.trafficLogger.onEvent(event);
      
      logs.offer(event);
      // LocalTestControl in GUI App
      listeners.forEach(listener -> listener.onEvent(event));
      if (event.getType() == CANLogEntry.FrameType.response) {
        if (log.isDebugEnabled()) {
          log.debug("Response:" + event.toString());
        }
      } else {
        receivedRequests++;
        if (receivedRequests % 256 == 0) {
          log.debug(String.format("Received %d requests.", receivedRequests));
        }
      }
    }
  }

  private TestCANRequestExecutor getExecutor() {
    if (executor == null) {
      executor = new TestCANRequestExecutor(canBus, this);
    }
    return executor;
  }

  @Override
  public void enableRequestFilter(final boolean status) {
    getExecutor().enableRequstFilter(status);
  }

  public ExecutorService getReportExecutor() {
    if (reportExecutor == null) {
      reportExecutor = Executors.newSingleThreadExecutor();
    }
    return reportExecutor;
  }

  @Override
  public int getBaudrate() {
    return -1;
  }


  @Override
  public DatabaseUtil getDatabaseUtil() {
    return null;
  }

  @Override
  public void setExpectedRequestFilter() {
    if (executor == null) {
      executor = new TestCANRequestExecutor(canBus, this);
    }
   // executor.setExpectedRequestFilter(this.scenario.getExpectedRequestFilter());
  }

  @Override
  public void cleanExpectedRequestFilter() {
    //executor.cleanExpectedRequestFilter();
  }

  public static void shutDown() {
    isShutDown = true;
  }
}
