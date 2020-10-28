package com.visualthreat.testing.core;

import ch.qos.logback.classic.Logger;
import ch.qos.logback.classic.LoggerContext;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.core.Appender;
import ch.qos.logback.core.FileAppender;
import com.visualthreat.platform.common.can.CANLogEntry;
import lombok.Getter;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;

import org.apache.commons.lang3.StringUtils;
import org.slf4j.ILoggerFactory;
import org.slf4j.LoggerFactory;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;

/**
 * Log all traffics during the test
 */
@Slf4j
public class TestTrafficLogger extends TrafficLogger {


  private static String TRAFFIC_FILE_EXTENSION = ".traffic";
  public static String VIN_NUMBER = "";


  @Getter
  @Setter
  private TRAFFIC_LOG_STATE curState = TRAFFIC_LOG_STATE.BEFORE;

  private String baseLogFileName;
  private BufferedWriter bufferedWriterBefore;
  private BufferedWriter bufferedWriterRunning;
  private BufferedWriter bufferedWriterAfter;
  private boolean isClosed = false;

  public TestTrafficLogger(final String testName) {
    if (logRootFolder == null) {
      logRootFolder = getLogFileFolder();
    }
    if (logRootFolder == null) {
      throw new TestException("Couldn't find log root folder");
    }

    baseLogFileName = testName + "_" + dateFormat.format(new Date());

    String fileName = baseLogFileName +  TRAFFIC_FILE_EXTENSION;
    if (StringUtils.isNotEmpty(VIN_NUMBER)) {
      fileName = baseLogFileName + "-" + VIN_NUMBER +  TRAFFIC_FILE_EXTENSION;
    }

    final File trafficFile = new File(logRootFolder, fileName);
    try {
      final FileWriter fileWriter = new FileWriter(trafficFile);
      this.bufferedWriterRunning = new BufferedWriter(fileWriter);
    } catch (final IOException ex) {
    	log.error("Exception:", ex);
      throw new TestException("Create " + testName + " traffic logger failed.", ex);
    } catch (Exception e) {
    	log.error("Exception:", e);
	}
  }

  public static File getLogRootFolder() {
    if (logRootFolder == null) {
      logRootFolder = getLogFileFolder();
    }
    return logRootFolder;
  }

  @Override
  public void onEvent(final CANLogEntry event) {
    if (isClosed || event == null) {
      return;
    }

    BufferedWriter activeBufferedWriter = null;
    try {
      switch (this.curState) {
        case BEFORE:
          break;
        case AFTER:
          break;
        default:
          activeBufferedWriter = this.bufferedWriterRunning;
          break;
      }

      if (!this.isClosed && activeBufferedWriter != null) {
        synchronized (this) {
          activeBufferedWriter.append(event.toLogLine(true));
          activeBufferedWriter.newLine();
        }
      }
    } catch (final IOException e) {
    	log.error("Exception:", e);
      throw new TestException("Couldn't write traffic", e);
    } catch (Exception e) {
    	log.error("Exception:", e);
	}
  }

  @Override
  public void close() {
    if (this.isClosed) {
      return;
    }

    this.isClosed = true;
    try {
      // let OnEvent handler drain
      Thread.sleep(1);
    } catch (final InterruptedException e) {
    	log.error("Exception:", e);
      Thread.currentThread().interrupt();
    } catch (Exception e) {
    	log.error("Exception:", e);;
	}

    try {
      if (this.bufferedWriterAfter != null) {
        this.bufferedWriterAfter.close();
      }
      if (this.bufferedWriterBefore != null) {
        this.bufferedWriterBefore.close();
      }
      if (this.bufferedWriterRunning != null) {
        this.bufferedWriterRunning.close();
      }
    } catch (final IOException ignored) {
    	log.error("Exception:", ignored);
    } catch (Exception e) {
    	log.error("Exception:", e);
	}
  }

  @Override
  public void writeStringToLogFile(long timeStamp, int id, byte[] data, int dlc) {
    return;
  }

  private static File getLogFileFolder() {
    final FileAppender fileAppender;
    final ILoggerFactory loggerFactory = LoggerFactory.getILoggerFactory();
    if (loggerFactory instanceof LoggerContext) {
      final LoggerContext context = (LoggerContext) LoggerFactory.getILoggerFactory();
      for (final Logger logger : context.getLoggerList()) {
        final Iterator<Appender<ILoggingEvent>> index = logger.iteratorForAppenders();
        if (index.hasNext()) {
          final Object enumElement = index.next();
          if (enumElement instanceof FileAppender) {
            fileAppender = (FileAppender<?>) enumElement;
            return (new File(fileAppender.getFile())).getParentFile();
          }
        }
      }
    } else {
      final Enumeration appenders = org.apache.log4j.LogManager.getRootLogger().getAllAppenders();
      while (appenders.hasMoreElements()) {
        final Object currAppender = appenders.nextElement();
        if (currAppender instanceof org.apache.log4j.RollingFileAppender) {
          final org.apache.log4j.RollingFileAppender rollingFileAppender
              = (org.apache.log4j.RollingFileAppender) currAppender;
          return (new File(rollingFileAppender.getFile())).getParentFile();
        }
      }
    }

    return Files.exists(Paths.get(DEFAULT_LOG_FOLDER)) ? new File(DEFAULT_LOG_FOLDER) : null;
  }

  public static void cleanTrafficFiles() {
    if (logRootFolder == null) {
      logRootFolder = getLogFileFolder();
    }
    if (logRootFolder == null) {
      return;
    }

    final File[] files = logRootFolder.listFiles(new FilenameFilter() {
      public boolean accept(final File dir, final String name) {
        return name.toLowerCase().endsWith(TRAFFIC_FILE_EXTENSION);
      }
    });

    Arrays.sort(files, new Comparator<File>() {
      public int compare(final File f1, final File f2) {
        return Long.compare(f2.lastModified(), f1.lastModified());
      }
    });

    if (files.length <= MAX_TRAFFIC_FILES_TO_KEEP) {
      return;
    }

    // delete oldest files
    for (int i = MAX_TRAFFIC_FILES_TO_KEEP; i < files.length; i++) {
      files[i].delete();
    }
  }
}
