package com.visualthreat.testing.core;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;
import java.util.Random;
import java.util.concurrent.Callable;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import com.visualthreat.platform.common.can.CANLogEntry;
import com.visualthreat.testing.ICANBus;

import lombok.extern.slf4j.Slf4j;

/**
 * Execute CAN Requests
 */
@Slf4j
public class TestCANRequestExecutor {
  public static final int MIN_CAN_MESSAGE_SENT_INTERVAL = 0; // 5 milli-seconds
  private static final int MAX_BATCH_SIZE = 32;
  private static final int SLEEP_UNTIL_TASK_DONE = 50;
  private static final int WAIT_FOR_COMPLETE_BEFORE_SHUTDOWN = 2000;
  private static final byte[] HASH_BYTE_ARRAY = new byte[]
      {0x67,0x52,(byte)0xba,(byte)0x1e,0x0,0x0,0x0,0x0};
  private static final Random RANDOM_GENERATOR = new Random();

  private final ExecutorService executorService = Executors.newSingleThreadExecutor();
  private final Queue<TestCANRequest> cmds = new ConcurrentLinkedQueue<>();
  private final Queue<Future> submittedTasks = new ConcurrentLinkedQueue<>();
  private final ICANBus canDevice;
  private long receivedCmds = 0;
  private CanTrafficIndex index = null;
 
  private volatile boolean enableRequestFilter = false;
  private Object sendTrafficLock = new Object();

  TestCANRequestExecutor(final ICANBus canDevice, final CanTrafficIndex index) {
    this.index = index;
    this.canDevice = canDevice;
  }
  
  public ICANBus getCanDevice(){
	  return canDevice;
  }

  public void sendTestTraffic(TestCANRequest request) throws TestException {
      if (request == null) {
        return;
      }

      receivedCmds += request.getCmds().size();
      if (request.getCmds().size() == 0 ||
          (index != null && index.isResumeMode() && receivedCmds <= index.getResumeStateCmdIndex())) {
        // in resume mode, we skip all cmds before resumeStartIndex
        return;
      }

      receivedCmds += addHashSignature(request);
      cmds.offer(new TestCANRequest(request));
      if (request.getCmds().size() > MAX_BATCH_SIZE || cmds.size() > MAX_BATCH_SIZE) {
        flush();
      }
      if (index != null) {
        index.setCurCANTrafficIndex(receivedCmds);
      }
 }

  public static int addHashSignature(TestCANRequest request) {
    if(RANDOM_GENERATOR.nextInt(100) <= 8
        && request.getCmds().size() > 0
        && request.getCmds().get(0).getId() > 0) {
      CANLogEntry canLogEntry = new CANLogEntry();
      canLogEntry.setId(request.getCmds().get(0).getId());
      canLogEntry.setDlc(8);
      canLogEntry.setData(HASH_BYTE_ARRAY);
      request.getCmds().add(canLogEntry);
      return 1;
    }
    return 0;
  }

  void flush() {
    final List<TestCANRequest> workItems = new ArrayList<>();
    while (!cmds.isEmpty()) {
      final TestCANRequest request = cmds.poll();
      if (request == null) {
        break;
      }
      workItems.add(request);
    }
    if (workItems.isEmpty()) {
      return;
    }
    final Future<Integer> task = this.executorService.submit(new LocalRequestExecutor(workItems));
    submittedTasks.offer(task);
  }

  public boolean waitForComplete(final int timeoutInMilliSeconds) {
    this.flush();
    final long startTime = System.currentTimeMillis();
    try {
      while (!submittedTasks.isEmpty()) {
        final Future task = submittedTasks.peek();
        if (task.isDone()) {
          submittedTasks.poll();
        } else {
          Thread.sleep(SLEEP_UNTIL_TASK_DONE);
        }
        if (timeoutInMilliSeconds >= 0 &&
            startTime + timeoutInMilliSeconds < System.currentTimeMillis()) {
          // time out
          return false;
        }
      }
    } catch (final InterruptedException ignored) {
    	log.error("Exception:", ignored);
      Thread.currentThread().interrupt();
      return false;
    } catch (Exception e) {
    	log.error("Exception:", e);
	}
    return true;
  }

  public void sendCANFramesNow(CANLogEntry frame) throws TestException {
    try {
      synchronized (sendTrafficLock) {
        frame.setType(CANLogEntry.FrameType.request);
        frame.setTimeStamp(System.currentTimeMillis());
        canDevice.publishCANEvent(frame);
        canDevice.sendCANFrame(frame);
      }
    } catch (IOException ioe) {
    	log.error("Exception:", ioe);
      throw new TestException(ioe);
    } catch (Exception e) {
    	log.error("Exception:", e);
	}
  }

  private class LocalRequestExecutor implements Callable<Integer> {
    private final List<TestCANRequest> workItems;

    LocalRequestExecutor(final List<TestCANRequest> items) {
      this.workItems = items;
    }

    @Override
    public Integer call() throws Exception {

      final int result = this.workItems.size();
      try {
        for (final TestCANRequest request : workItems) {
          for (final CANLogEntry frame : request.getCmds()) {
            // send request into log queue
            sendCANFramesNow(frame);
            final int waitTime = Math.max(request.getWaitTimeInMilliSecs(),
                MIN_CAN_MESSAGE_SENT_INTERVAL);
            Thread.sleep(waitTime);
          }
        }
      } catch (final Exception ex) {
        if (!(ex instanceof InterruptedException)) {
          log.error("Can't send CAN messages", ex);
        } else if(ex instanceof InterruptedException) {
          Thread.currentThread().interrupt();
        }
        log.error("Exception:", ex);
        return 0;
      }

      return result;
    }
  }

  public void shutdown() {
    waitForComplete(WAIT_FOR_COMPLETE_BEFORE_SHUTDOWN);
    this.executorService.shutdownNow();
    log.info("TestCANRequestExecutor exiting ...");
  }

  void enableRequstFilter(final boolean status) {
    this.enableRequestFilter = status;
  }
}
