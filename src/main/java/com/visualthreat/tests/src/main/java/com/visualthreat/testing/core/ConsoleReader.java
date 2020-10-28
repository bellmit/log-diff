package com.visualthreat.testing.core;

import com.visualthreat.platform.common.can.CANLogEntry;
import com.visualthreat.testing.ICANBus;

import java.io.IOException;
import java.util.Scanner;

import lombok.RequiredArgsConstructor;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;

/**
 * Listen to console input
 */
@Slf4j @RequiredArgsConstructor
class ConsoleReader {
  private static final String COMMENT_CMD = "c";
  private static final String MARK_CMD = "m";
  private static final String SEPARATOR = ",";
  private static final String TAG_PREFIX = "Tag: ";

  private final ICANBus canBus;

  private Thread consoleReaderWorker = null;
  private int markCnt = 1;

  @Setter
  private boolean isShutDown = false;

  public ConsoleReader start() {
    consoleReaderWorker = new Thread(() -> {
      Scanner scanInput = new Scanner(System.in);
      while (!isShutDown) {
        try {
          if (scanInput.hasNextLine()) {
            String userInput = scanInput.nextLine();
            if (userInput.startsWith(COMMENT_CMD) || userInput.startsWith(MARK_CMD)) {
              CANLogEntry packet = new CANLogEntry();
              packet.setType(CANLogEntry.FrameType.comment);
              String comment = "";
              if (userInput.contains(SEPARATOR)) {
                String[] vals = userInput.split(SEPARATOR);
                if (vals.length == 2) {
                  comment = TAG_PREFIX + vals[1].toLowerCase().trim();
                  packet.setComment(comment);
                }
              } else {
                comment = TAG_PREFIX + markCnt++;
                packet.setComment(comment);
              }
              canBus.publishCANEvent(packet);
              log.info(String.format("Added tag=\"%s\" into logs.", comment));
            }
          } else {
            Thread.sleep(50);
          }
        } catch (InterruptedException ignored) {
        	log.error("Exception:", ignored);
          Thread.currentThread().interrupt();
          return;
        } catch (Exception e) {
        	log.error("Exception:", e);
        	return;
		}
      }
    });
    consoleReaderWorker.start();

    return this;
  }

  public void stop() {
    if(this.consoleReaderWorker != null) {
      try {
        System.in.close();
        this.consoleReaderWorker.interrupt();
      } catch (IOException ignored) {
    	  log.error("Exception:", ignored);
      } catch (Exception e) {
    	  log.error("Exception:", e);
	}
    }
  }
}
