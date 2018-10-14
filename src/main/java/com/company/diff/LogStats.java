package com.company.diff;

import com.fasterxml.jackson.databind.ObjectMapper;

import java.io.File;
import java.io.IOException;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.stream.Collectors;
import java.util.stream.Stream;

import lombok.Getter;


public class LogStats {

  private static final int sniffCompareStatusFrameThreshHold = 10;
  private static final int minStatusChangesThreshHold = 2;

  private static final ObjectMapper json = new ObjectMapper();
  @Getter
  private TreeMap<Integer, CANStats> canLogEntries = new TreeMap<>();

  /**
   * Test Program
   *
   * @param args
   */
  public static void main(String[] args) {
    try {
      File f1 = new File("log.base");
      File f2 = new File("log.new");

      LogStats stats1 = LogStats.loadLogFile(f1);
      LogStats stats2 = LogStats.loadLogFile(f2);


      LogStats diffNewIds = stats2.diffWithNewBytePositionChange(stats1);
      System.out.println(diffNewIds);

      LogStats diffNewFrames = stats2.diffWithNewCANFrames(stats1);
      Set<Integer> changes = new HashSet<>();
      changes.add(2);
      LogStats statusChangeCANFrames = diffNewFrames.getStatusChange(changes, null);

      System.out.println(statusChangeCANFrames);
    } catch (IOException ex) {
      ex.printStackTrace();
    }
  }

  public LogStats() {
  }

  public LogStats(LogStats other) {
    for (CANStats tmpCANStats : other.canLogEntries.values()) {
      for (CANLogEntry entry : tmpCANStats.rawEntries) {
        this.addStats(entry);
      }
    }
  }

  public static LogStats loadLogFile(File file) throws IOException {
    LogStats result = new LogStats();
    LogEntryIterator logEntryIterator = null;
    try {
      logEntryIterator = new LogEntryIterator(file);
      while (logEntryIterator.hasNext()) {
        // Process line
        CANLogEntry entry = logEntryIterator.nextEntry();
        if (entry != null) {
          result.addStats(entry);
        }
      }

    } finally {
      if (logEntryIterator != null) {
        logEntryIterator.close();
      }
    }
    return result;
  }

  public static LogStats loadLog(Iterator<CANLogEntry> logEntryIterator) throws IOException {
    LogStats result = new LogStats();
    while (logEntryIterator.hasNext()) {
      // Process each entry
      CANLogEntry entry = logEntryIterator.next();
      if (entry != null) {
        result.addStats(entry);
      }
    }
    return result;
  }

  public static LogStats loadLogList(Stream<CANLogEntry> logs) {
    LogStats result = new LogStats();
    logs.forEach(result::addStats);
    return result;
  }

  public static LogStats loadLogFile(String filePath) throws IOException {
    File file = new File(filePath);
    return loadLogFile(file);
  }


  public LogStats diffWithNewIDs(LogStats baseStats) {
    LogStats result = new LogStats();
    for (Integer id : this.canLogEntries.keySet()) {
      if (!baseStats.canLogEntries.containsKey(id)) {
        for (CANLogEntry entry : this.canLogEntries.get(id).rawEntries)
          result.addStats(entry);
      }
    }
    return result;
  }



  public List<BasicCounterInfo> countTopN(Integer n) {
    return this.canLogEntries.entrySet().stream()
        .sorted((e1, e2) -> Integer.compare(e2.getValue().count(), e1.getValue().count()))
        .limit(n)
        .map(e -> new BasicCounterInfo(e.getKey(), e.getValue().count()))
        .collect(Collectors.toList());
  }

  public List<Integer> getNonEmptyCanIds() {
    return this.canLogEntries.entrySet().stream()
        .filter((e) -> e.getValue().getRawEntries().size() > 1)
        .map((e) -> e.getKey())
        .collect(Collectors.toList());
  }

  public List<InterArrivalPerCanIdInfo> countInterArrivalPerCanId() {
    List<InterArrivalPerCanIdInfo> canLogEntries = new LinkedList<>();
    Set<Map.Entry<Integer, CANStats>> entries = this.canLogEntries.entrySet();

    for (Map.Entry<Integer, CANStats> entry : entries) {

      List<CANLogEntry> logs = new LinkedList<>();
      logs.addAll(entry.getValue().getRawEntries());

      InterArrivalPerCanIdInfo info = InterArrivalPerCanIdInfo.fromLogs(entry.getKey(), logs);
      if (info.count != 0)  {
        canLogEntries.add(info);
      }
    }

    return canLogEntries;
  }


  public LogStats diffWithNewCANFrames(LogStats baseStats) {
    LogStats result = new LogStats();
    for (Integer id : this.canLogEntries.keySet()) {
      if (baseStats.canLogEntries.containsKey(id)) {
        for (CANLogEntry entry : this.canLogEntries.get(id).rawEntries) {
          if (!baseStats.canLogEntries.get(id).rawEntries.contains(entry)) {
            result.addStats(entry);
          }
        }
      } else {
        for (CANLogEntry entry : this.canLogEntries.get(id).rawEntries) {
          result.addStats(entry);
        }
      }
    }
    return result;
  }

  public LogStats diffWithNewBytePositionChange(LogStats baseStats) {
    LogStats result = new LogStats();
    LogStats diffWithNewCANFrames = diffWithNewCANFrames(baseStats);

    for (Integer id : diffWithNewCANFrames.canLogEntries.keySet()) {
      CANStats stats = diffWithNewCANFrames.canLogEntries.get(id);
      if (baseStats.canLogEntries.containsKey(id)) {
        Set<Integer> baseBytePostions = baseStats.canLogEntries.get(id).getUsedBytePositions();
        if (!baseBytePostions.containsAll(stats.getUsedBytePositions())) {
          for (CANLogEntry entry : stats.rawEntries) {
            for (int i = 0; i < entry.getData().length; i++) {
              if (entry.getData()[i] != 0 && !baseBytePostions.contains(i)) {
                result.addStats(entry);
              }
            }
          }
        } else if(
            (baseStats.canLogEntries.get(id).rawEntries.size() + minStatusChangesThreshHold) <
                this.canLogEntries.get(id).rawEntries.size()){
    
          Set<CANLogEntry> newCANFrames =
              filterOutStatusByte(diffWithNewCANFrames.canLogEntries.get(id));
          newCANFrames.forEach(result::addStats);
        }
      } else {
        for (CANLogEntry entry : stats.rawEntries) {
          result.addStats(entry);
        }
      }
    }


    for (Integer canId : result.canLogEntries.keySet()) {
      CANStats curIDStats = result.canLogEntries.get(canId);
      if (curIDStats.rawEntries.size() >= sniffCompareStatusFrameThreshHold) {
        // status frame
        Set<CANLogEntry> logEntries = curIDStats.rawEntries;
   
        curIDStats.rawEntries = new LinkedHashSet<>();
        logEntries.stream().limit(2).forEach(entry -> curIDStats.rawEntries.add(entry));
      }
    }

    if(diffWithNewCANFrames.getCanLogEntries().containsKey(0)){

      diffWithNewCANFrames.canLogEntries.get(0).rawEntries.forEach(
          e -> result.addStats(e));
    }

    return result;
  }

  private Set<CANLogEntry> filterOutStatusByte(CANStats stats) {
  
    long maxChanges = 0;
    int bytePos = 0;
    for(Integer pos : stats.getDataArrayStats().keySet()){
      if(stats.getDataArrayStats().get(pos).getValues().size() > maxChanges) {
        maxChanges = stats.getDataArrayStats().get(pos).getValues().size();
        bytePos = pos;
      }
    }
    Set<CANLogEntry> result =new HashSet<>();

    Set<String> selectedEntries = new HashSet<>();
    for(CANLogEntry entry : stats.getRawEntries()){
      String newEntryData = "";
      for(int i = 0; i < entry.getData().length; i++) {
        if( i == bytePos) {
          continue;
        }
        newEntryData += String.format("%d,", entry.getData()[i]);
      }
      if(!selectedEntries.contains(newEntryData)) {
        selectedEntries.add(newEntryData);
        result.add(entry);
      }
    }

    return result;
  }


  public LogStats getStatusChange(Set<Integer> changeNums, LogStats baseStats) {
    LogStats combinedStats = new LogStats(this);

    if (baseStats != null) {
 
      for (Integer id : baseStats.canLogEntries.keySet()) {
        for (CANLogEntry entry : baseStats.canLogEntries.get(id).rawEntries) {
          combinedStats.addStats(entry);
        }
      }
    }

    LogStats result = new LogStats();
    for (Integer id : combinedStats.canLogEntries.keySet()) {
      CANStats stats = combinedStats.canLogEntries.get(id);
      if (changeNums.contains(stats.rawEntries.size())) {
        for (CANLogEntry entry : stats.rawEntries) {
          result.addStats(entry);
        }
      }
    }

    return result;
  }


  public void addStats(String jsonLogLine) {
    // Process line
    try {
      CANLogEntry newCAN = json.readValue(jsonLogLine, CANLogEntry.class);
      addStats(newCAN);
    } catch (IOException e) {
    }
  }

  public void addStats(CANLogEntry entry) {
    if (entry == null
        || (entry.getType() != CANLogEntry.FrameType.response &&
        entry.getType() != CANLogEntry.FrameType.comment)) {
      return;
    }

    if (!this.canLogEntries.containsKey(entry.getId())) {
      CANStats canStats = new CANStats(entry.getId());
      this.canLogEntries.put(entry.getId(), canStats);
    }
    this.canLogEntries.get(entry.getId()).addCANLogEntry(entry);
  }

  public Stream<CANLogEntry> canEntries() {
    return canLogEntries.values().stream()
        .flatMap(stats -> stats.rawEntries.stream());
  }

  @Override
  public String toString() {
    StringBuilder sb = new StringBuilder();
    for (Integer id : canLogEntries.keySet()) {
      for (CANLogEntry frame : canLogEntries.get(id).rawEntries)
        sb.append(frame.toString()).append("\n");
    }
    return sb.toString();
  }

  @Override
  public int hashCode() {
    return this.toString().hashCode();
  }
}
