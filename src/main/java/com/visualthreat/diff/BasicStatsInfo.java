package com.visualthreat.diff;

import lombok.Getter;
import lombok.Setter;
import lombok.ToString;

import java.util.ArrayList;
import java.util.List;

@Setter
@Getter
@ToString
public class BasicStatsInfo {
  double interArrivalMean;
  long interArrivalMin;
  long interArrivalMax;
  Integer uniqeuIds;
  double dlcMean;
  long dlcMin;
  long dlcMax;


  public BasicStatsInfo(double interArrivalMean,
                        long interArrivalMin,
                        long interArrivalMax,
                        Integer uniqeuIds,
                        double dlcMean,
                        long dlcMin,
                        long dlcMax) {
    this.interArrivalMean = interArrivalMean;
    this.interArrivalMin = interArrivalMin;
    this.interArrivalMax = interArrivalMax;
    this.uniqeuIds = uniqeuIds;
    this.dlcMean = dlcMean;
    this.dlcMin = dlcMin;
    this.dlcMax = dlcMax;
  }

  public static BasicStatsInfo fromLogs(List<CANLogEntry> logs) {
    long totalEntries = 0;
    long nonZeroEntries = 0;
    long maxInterval = Long.MIN_VALUE;
    long minInterval = Long.MAX_VALUE;
    long sumInterval = 0L;

    long maxDlc = Long.MIN_VALUE;
    long minDlc = Long.MAX_VALUE;
    long sumDlc = 0L;

    List<Integer> uniqueIds = new ArrayList<>();

    long lastTimeStamp = -1;

    for (CANLogEntry entry : logs) {

      if (!uniqueIds.contains(entry.getId())) {
        uniqueIds.add(entry.getId());
      }

      Long currentTimestamp = entry.getTimeStamp();
      Integer currentDlc = entry.getDlc();
      totalEntries++;
      sumDlc += currentDlc;

      if (currentDlc > maxDlc) {
        maxDlc = currentDlc;
      }

      if (currentDlc > 0 && currentDlc < minDlc) {
        minDlc = currentDlc;
      }

      if (lastTimeStamp > 0) {
        long tmpInterval = currentTimestamp - lastTimeStamp;
        if (tmpInterval > maxInterval) {
          maxInterval = tmpInterval;
        }
        if (tmpInterval < minInterval) {
          minInterval = tmpInterval;
        }
        if (tmpInterval > 0) {
          nonZeroEntries++;
        }
        sumInterval = sumInterval + tmpInterval;
      }

      lastTimeStamp = currentTimestamp;
    }

    double meanInterval = 0L;

    if (nonZeroEntries != 0) {
      meanInterval = sumInterval / nonZeroEntries;
    }

    double meanDlc = 0L;
    if (totalEntries != 0) {
      meanDlc = sumDlc / totalEntries;
    }

    return new BasicStatsInfo(meanInterval, minInterval, maxInterval, uniqueIds.size(), meanDlc, minDlc, maxDlc);

  }
}
