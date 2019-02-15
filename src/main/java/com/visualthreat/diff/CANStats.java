package com.visualthreat.diff;

import lombok.Getter;

import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.Set;
import java.util.TreeMap;

/**
 * The instance of class captures statistics of all byte arrays of a same CAN ID
 */
public class CANStats {
  int canId;
  // byte position -> stats
  @Getter
  TreeMap<Integer, DataArrayStats> dataArrayStats = new TreeMap<>();
  @Getter
  Set<CANLogEntry> rawEntries = new LinkedHashSet<>();

  @Getter
  public static class DataArrayStats {
    long count;
    long sum = 0;
    int min = Short.MAX_VALUE;
    int max = Short.MIN_VALUE;
    int intMin = Integer.MAX_VALUE;
    int intMax = Integer.MIN_VALUE;
    Set<Byte> values = new HashSet<>();

    public void addDataArray(byte data) {
      count++;
      if (min > (data & 0xFF)) {
        this.min = data & 0xFF;
      }
      if (max < (data & 0xFF)) {
        this.max = data & 0xFF;
      }

      if ((data & 0xFF) < intMin) {
        this.intMin = data & 0xFF;
      }

      if ((data & 0xFF) > intMax) {
        this.intMax = data & 0xFF;
      }

      this.sum = this.sum + (data & 0xFF);
      values.add(data);
    }
  }

  public CANStats(int canId) {
    this.canId = canId;
  }

  public void addCANLogEntry(CANLogEntry entry) {
    if (entry.getId() != this.canId) {
      throw new IllegalArgumentException("Wrong CAN ID is passed in");
    }
    byte[] canData = entry.getData();
    for (int i = 0; i < canData.length; i++) {
      if (canData[i] == 0) {
        continue;
      }
      DataArrayStats byteStats = dataArrayStats
          .computeIfAbsent(i, k -> new DataArrayStats());
      byteStats.addDataArray(canData[i]);
    }
    rawEntries.add(entry);
  }

  public Integer count() {
    return rawEntries.size();
  }

  public Set<Integer> getUsedBytePositions() {
    return new HashSet<>(dataArrayStats.keySet());
  }
}
