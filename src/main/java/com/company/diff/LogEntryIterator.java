package com.company.diff;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.apache.commons.io.FileUtils;
import org.apache.commons.io.LineIterator;

import java.io.File;
import java.io.IOException;
import java.util.Iterator;
import java.util.NoSuchElementException;

/**
 * The instance of this class is used to parse a log file
 */
public class LogEntryIterator implements Iterator<CANLogEntry> {
  private static final ObjectMapper json = new ObjectMapper();
  private final LineIterator lineIterator;

  static {
    json.configure(JsonParser.Feature.ALLOW_UNQUOTED_FIELD_NAMES, true);
  }

  public LogEntryIterator(final File logFile) throws IOException {
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
}
