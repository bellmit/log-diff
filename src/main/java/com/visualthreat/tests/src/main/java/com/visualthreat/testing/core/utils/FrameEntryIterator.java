package com.visualthreat.testing.core.utils;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.visualthreat.logservice.asc.parser.ASCLazyParser;
import com.visualthreat.logservice.blf.parser.BLFLazyFrameOnlyParser;
import com.visualthreat.logservice.parsers.CANFrameIterator;
import com.visualthreat.logservice.parsers.EmptyIterator;
import com.visualthreat.platform.common.can.CANLogEntry;

import java.util.List;
import org.apache.commons.io.FileUtils;
import org.apache.commons.io.LineIterator;

import java.io.File;
import java.io.IOException;
import java.util.NoSuchElementException;

import lombok.extern.slf4j.Slf4j;

/**
 * The instance of this class is used to parse a log file
 */
@Slf4j
public class FrameEntryIterator implements CANFrameIterator {
  private static final ObjectMapper json = new ObjectMapper();
  private final LineIterator lineIterator;

  static {
    json.configure(JsonParser.Feature.ALLOW_UNQUOTED_FIELD_NAMES, true);
  }

  public FrameEntryIterator(final File logFile) throws IOException {
    this.lineIterator = FileUtils.lineIterator(logFile);
  }

  public boolean hasNext() {
    return this.lineIterator.hasNext();
  }

  public CANLogEntry next() {
    return this.nextEntry();
  }

  public CANLogEntry nextEntry() {
    if (!this.hasNext()) {
      throw new NoSuchElementException("No more lines");
    } else {
      final String currentLine = this.lineIterator.nextLine().trim();
      if (currentLine.isEmpty()) {
        return null;
      }
      return CANLogEntry.fromString(currentLine);
    }
  }

  public void close() {
    LineIterator.closeQuietly(this.lineIterator);
  }

  public void remove() {
    throw new UnsupportedOperationException("Remove unsupported on LineIterator");
  }

  public static boolean isRaw(final File logFile) throws IOException {
    try {
      final LineIterator iter = FileUtils.lineIterator(logFile);
      try {
        final String line = iter.nextLine().trim();
        final CANLogEntry entry = CANLogEntry.fromString(line);
        return entry != null;
      } catch (final Exception e) {
    	  log.error("Exception:", e);
        LineIterator.closeQuietly(iter);
        return false;
      }
    } catch (final IOException ioe) {
    	log.error("Exception:", ioe);
      return false;
    } catch (Exception e2) {
    	log.error("Exception:", e2);
    	return false;
	}
  }

  public static CANFrameIterator GetCANFrameIterator(final File logfile) throws IOException {
    final String path = logfile.getAbsolutePath();
    if (BLFLazyFrameOnlyParser.isBlf(path)) {
      return new BLFLazyFrameOnlyParser(path);
    } else if (FrameEntryIterator.isRaw(logfile)) {
      return new FrameEntryIterator(logfile);
    } else if (ASCLazyParser.isAsc(logfile)) {
      return ASCLazyParser.fromFile(logfile);
    }
    return new EmptyIterator();
  }
}
