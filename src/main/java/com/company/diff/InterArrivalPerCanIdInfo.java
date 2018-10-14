package com.company.diff;

import lombok.Getter;
import lombok.Setter;
import lombok.ToString;

import java.util.List;

@Setter
@Getter
@ToString
public class InterArrivalPerCanIdInfo {
  Integer id;
  double average;
  double deviation;
  long min;
  long max;
  long count;

  public InterArrivalPerCanIdInfo(Integer id,
                                  double average,
                                  double deviation,
                                  long min,
                                  long max,
                                  long count) {
    this.id = id;
    this.average = average;
    this.deviation = deviation;
    this.min = min;
    this.max = max;
    this.count = count;
  }


  public static InterArrivalPerCanIdInfo fromLogs(Integer id,
                                                  List<CANLogEntry> logs) {
    BasicStatsInfo basicStats = BasicStatsInfo.fromLogs(logs);

    double deviation = 0;
    long count = 0;
    long lastTimeStamp = -1;

    for (CANLogEntry entry : logs) {
      if (lastTimeStamp > 0) {
        long tmpInterval = entry.getTimeStamp() - lastTimeStamp;
        deviation = deviation + Math.pow(tmpInterval - basicStats.getInterArrivalMean(), 2);
        count++;
      }
      lastTimeStamp = entry.getTimeStamp();
    }

    deviation = Math.sqrt(deviation / (count - 1));

    return new InterArrivalPerCanIdInfo(
        id,
        basicStats.getInterArrivalMean(),
        deviation,
        basicStats.getInterArrivalMin(),
        basicStats.getInterArrivalMax(),
        count
    );

  }

}
