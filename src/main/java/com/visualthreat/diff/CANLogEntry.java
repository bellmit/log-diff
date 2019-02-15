package com.visualthreat.diff;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.Data;
import lombok.Getter;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.Arrays;
import java.util.Comparator;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Stream;

@Data
@JsonIgnoreProperties(ignoreUnknown = true)
public class CANLogEntry implements Comparable<CANLogEntry>, Serializable {
  public static final int MAX_DATA_LENGTH = 8;

  private static final long serialVersionUID = -1980267790L;

  private static ObjectMapper JSON = null;
  private static ObjectMapper getJson() {
    if (JSON == null) {
      JSON = new ObjectMapper();
    }

    return JSON;
  }

  public enum SortBy {
    TIMESTAMP,
    ID,
    SEQNUM
  }

  public enum FrameType {
    response,
    request,
    comment
  }

  public enum CategoryType {
    HFCDF,
    LFCDF,
    SDF
  }

  @JsonProperty("timestamp")
  @Getter
  private long timeStamp = 0; 
  @JsonProperty("seq")
  private int seqNum = 0;
  private boolean is_extended_id = false;
  private int dlc = 8;
  private int id;
  private byte[] data = new byte[8];
  @JsonProperty("type")
  private FrameType type = FrameType.response;
  private boolean no_data = false;
  private CategoryType category = CategoryType.SDF;
  private String comment = "";

  public static Comparator<CANLogEntry> sortBy(final SortBy sortBy) {
    switch (sortBy) {
      case TIMESTAMP:
        return Comparator.comparingLong(CANLogEntry::getTimeStamp);
      case ID:
        return Comparator.comparingInt(CANLogEntry::getId);
      case SEQNUM:
        return Comparator.comparingInt(CANLogEntry::getSeqNum);
    }

    throw new IllegalArgumentException("Can't sort by: " + sortBy.toString());
  }

  // Default constructor used by ObjectMapper
  public CANLogEntry() {
    this.timeStamp = System.currentTimeMillis();
  }

  public CANLogEntry(final long time, final int id, final int dlc, final byte[] data) {
    this.timeStamp = time;
    setId(id);
    setDlc(dlc);
    setData(data);
  }

  public CANLogEntry(final int seqNum, final int id, final int dlc, final String type, final byte[] data) {
    this(System.currentTimeMillis(), id, dlc, data);
    this.seqNum = seqNum;

    try {
      this.type = FrameType.valueOf(type);
    } catch (final IllegalArgumentException e) {
      this.type = FrameType.response;
    }
  }

  public CANLogEntry(final long time, final int seqNum, final int id,
                     final int dlc, final String type, final byte[] data) {
    this(time, id, dlc, data);
    this.seqNum = seqNum;

    try {
      this.type = FrameType.valueOf(type);
    } catch (final IllegalArgumentException e) {
      this.type = FrameType.response;
    }
  }


  @Override
  public int compareTo(final CANLogEntry o2) {
    if (o2 == null) {
      return 1;
    }

    if (this.equals(o2)) {
      return 0;
    }
    return this.timeStamp < o2.timeStamp ? -1 : 1;
  }

  public void setDlc(final int dlc) {
    if (dlc >= 0 && dlc <= MAX_DATA_LENGTH) { 
      this.dlc = dlc;
    } else {
      throw new IllegalArgumentException("dlc must be between 0 and 8");
    }
  }

  public void setData(final byte[] newData) {
    if (newData == null || newData.length == 0) {
      return;
    }
    if (newData.length > MAX_DATA_LENGTH) { 
      throw new IllegalArgumentException("CAN data cannot contain more than 8 bytes");
    }
    System.arraycopy(newData, 0, this.data, 0, newData.length);
  }

  public void setId(final int newId) {
    // ensure standard id is in range
    if (newId >= 0 && newId <= 0x7FF) {
      this.id = newId;
      this.is_extended_id = false;
    } 
  }

  @Override
  public boolean equals(final Object other) {
    if (other == null) return false;
    if (other == this) return true;
    if (!(other instanceof CANLogEntry)) return false;
    final CANLogEntry otherCANLogEntry = (CANLogEntry) other;

    if (type != otherCANLogEntry.type) {
      return false;
    }

    if (type == FrameType.comment) {
      return comment.equals(otherCANLogEntry.comment) &&
          timeStamp == otherCANLogEntry.timeStamp;
    }

    if (id == otherCANLogEntry.id && dlc == otherCANLogEntry.dlc) {
      for (int i = 0; i < data.length; i++) {
        if (data[i] != otherCANLogEntry.data[i]) {
          return false;
        }
      }
    } else {
      return false;
    }

    if (is_extended_id != otherCANLogEntry.is_extended_id) {
      return false;
    }

    if (category != otherCANLogEntry.category) {
      return false;
    }

    return true;
  }

  @Override
  public String toString() {
    final StringBuilder sb = new StringBuilder();
    if (type == FrameType.comment) {
      sb.append("{comment:\"").append(comment).append("\"}");
    } else {
      sb.append("{id:0x").append(Integer.toHexString(id))
          .append(",dlc:").append(dlc)
          .append(",data:[0x").append(byteToHex(data[0]))
          .append(",0x").append(byteToHex(data[1]))
          .append(",0x").append(byteToHex(data[2]))
          .append(",0x").append(byteToHex(data[3]))
          .append(",0x").append(byteToHex(data[4]))
          .append(",0x").append(byteToHex(data[5]))
          .append(",0x").append(byteToHex(data[6]))
          .append(",0x").append(byteToHex(data[7]))
          .append("]}");
    }

    return sb.toString();
  }

  @Override
  public int hashCode() {
    return this.toString().hashCode();
  }

  public static CANLogEntry fromString(String line) {
    final String original = line;
    try {
      for (int indexX = line.indexOf("0x"); indexX >= 0; indexX = line.indexOf("0x")) {
        int indexComma = indexX + line.substring(indexX).indexOf(',');
        if (indexComma < indexX) {
          if (line.substring(indexX).contains("]")) {
            indexComma = indexX + line.substring(indexX).indexOf(']');
          } else {
            indexComma = indexX + line.substring(indexX).indexOf('}');
          }
        }

        if (indexComma < indexX) {
          System.out.println("Unknown CANLogEntry command format: " + line);
        }

        final String num = Integer.valueOf(line.substring(indexX + "0x".length(), indexComma).trim(), 16).toString();
        line = line.substring(0, indexX) + num + line.substring(indexComma);
      }

      final int iDataStart = line.indexOf('[');
      final int iDataEnd = line.indexOf(']');
      if (iDataStart >= 0 && iDataEnd > iDataStart) {
        Stream<String> dataStream = Arrays.stream(
            line.substring(iDataStart + 1, iDataEnd).split(","))
            .map(String::trim);

        dataStream = dataStream.map(Integer::valueOf).map(value -> {
          if (value > Byte.MAX_VALUE) {
            value = value - Byte.MAX_VALUE + Byte.MIN_VALUE - 1;
          }

          return String.valueOf(value);
        });

        final String data = dataStream.reduce((a, b) -> a + "," + b).orElse("");
        line = line.substring(0, iDataStart + 1) + data + line.substring(iDataEnd);
      }

      final Pattern noQuoteParams = Pattern.compile("[{,](\\w+)\\s*:\\s*");
      final Matcher m = noQuoteParams.matcher(line);

      final StringBuffer sb = new StringBuffer(line.length());
      while (m.find()) {
        final String text = m.group(0).replace(m.group(1), "\"" + m.group(1) + "\"");
        m.appendReplacement(sb, Matcher.quoteReplacement(text));
      }

      m.appendTail(sb);
      line = sb.toString();

      if (!line.isEmpty()) {
        try {
          final CANLogEntry logEntry = getJson().readValue(line, CANLogEntry.class);
          if (!logEntry.comment.isEmpty()) {
            logEntry.setType(FrameType.comment);
          }
          return logEntry;
        } catch (final IOException e) {
          System.err.println("Wrong CAN messages format: " + original);
        }
      }
    } catch (final RuntimeException e) {
      System.out.println("Can not decode a line: " + original);
    }

    return null;
  }


  public String toLogLine() {
    return toLogLine(false);
  }

  public String toLogLine(final boolean hex) {
    return toLogLine(timeStamp, type, comment, id, dlc, data, hex);
  }


  public static String toLogLine(final long timeStamp, final FrameType type, final String comment, final int id,
                                 final int dlc, final byte[] data, final boolean dataInHex) {
    final StringBuilder sb = new StringBuilder();
    toLogLine(timeStamp, type, comment, id, dlc, data, dataInHex, sb);
    return sb.toString();
  }

  public static void toLogLine(final long timeStamp, final FrameType type, final String comment, final int id,
                               final int dlc, final byte[] data, final boolean dataInHex, final StringBuilder sb) {
    toLogLine(timeStamp, type, comment, id, dlc, data, dataInHex, sb, "");
  }

  public static void toLogLine(final long timeStamp, final FrameType type, final String comment, final int id,
                               final int dlc, final byte[] data, final boolean dataInHex, final StringBuilder sb,
                               final String optExtra) {
    // timestamp
    sb.append("{\"timestamp\":").append(timeStamp);

    // optExtra
    if (!optExtra.isEmpty()) {
      sb.append(",").append(optExtra);
    }

    // type
    if (type != FrameType.response) {
      sb.append(",\"type\":\"").append(type).append("\"");
    }

    // data
    if (type == FrameType.comment) {
      sb.append(",\"comment\":\"").append(comment).append("\"");
    } else {
      if (dataInHex) {
        sb.append(",\"id\":0x")
            .append(Integer.toHexString(id))
            .append(",\"dlc\":")
            .append(dlc)
            .append(",\"data\":[");
        if (data != null && data.length > 0) {
          sb.append("0x").append(byteToHex(data[0]));
          for (int i = 1; i < data.length; i++) {
            sb.append(",0x").append(byteToHex(data[i]));
          }
        }
        sb.append("]");
      } else {
        sb.append(",\"id\":")
            .append(id)
            .append(",\"dlc\":")
            .append(dlc)
            .append(",\"data\":[");
        if(data != null && data.length > 0) {
          for(int i = 0; i < data.length ; i++) {
            if(i==0) {
              sb.append(data[0]);
            } else {
              sb.append(",").append(data[i]);
            }
          }
        }
        sb.append("]");
      }
    }

    // close
    sb.append("}");
  }

  public int getSize(boolean hex) {
    return CANLogEntry.getEntrySize(type, comment, id, data, hex);
  }

  public static int getEntrySize(final FrameType type, final CharSequence comment, final int id, final byte[] data,
                                 final boolean hex) {

    int overallLength = 26;
    if (type == FrameType.comment) {

      overallLength += 29 + 2;
      overallLength += comment.length();

    } else {
      if (type.equals(FrameType.request)) {
        overallLength += 17;
      }

      overallLength += 6;
      if (hex) {
        //0x
        overallLength += 2;
        if (id <= 0xf) {
          overallLength += 1;
        } else if (id > 0xf && id <= 0xff) {
          overallLength += 2;
        } else if (id > 0xff && id <= 0xfff) {
          overallLength += 3;
        } else {
          overallLength += 4;
        }
      } else {
        overallLength += getPositiveIntLength(id);
      }
      //,"dlc":_,"data":[
      overallLength += 17;

      for (byte b : data) {
        overallLength += getByteLength(b, hex);
      }

      overallLength += 7 + 2;
  
    }
    return overallLength;
  }

  private static int getPositiveIntLength(final int i) {
    if (i < 10) {
      return 1;
    } else if (i < 100) {
      return 2;
    } else if (i < 1000) {
      return 3;
    } else if (i < 10000) {
      return 4;
    }

    return (int) (Math.log10(i) + 1);
  }

  private static int getByteLength(final byte b, final boolean hex) {
    if (b > 0) {
      if (hex) {
        return 2 + (b > 0xf ? 2 : 1);
      } else {
        return getPositiveIntLength(b);
      }
    } else if (b < 0) {
      if (hex) {
        return 4;
      } else {
        return 1 + getPositiveIntLength(Math.abs(b));
      }
    } else {
      //0x0 or 0
      return hex ? 3 : 1;
    }
  }

  private static String byteToHex(final int n) {
    return Integer.toHexString(n & 0xFF);
  }

  private void writeObject(final ObjectOutputStream oos) throws IOException {
    oos.writeObject(this.toLogLine());
  }

  private void readObject(final ObjectInputStream ois) throws ClassNotFoundException, IOException {
    // default deserialization
    final String logLine = (String) ois.readObject();
    final CANLogEntry entry = fromString(logLine);
    if (entry != null) {
      this.timeStamp = entry.timeStamp;
      this.comment = entry.comment;
      this.data = entry.data;
      this.dlc = entry.dlc;
      this.id = entry.id;
      this.type = entry.type;
      this.seqNum = entry.seqNum;
      this.category = entry.category;
      this.is_extended_id = entry.is_extended_id;
      this.no_data = entry.no_data;
    }
  }
}

