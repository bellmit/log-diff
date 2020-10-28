package com.visualthreat.testing.core;

import com.visualthreat.platform.common.can.CANLogEntry;
import com.visualthreat.platform.common.model.proto.generated.TcpApiProtos;

import java.util.HashSet;
import java.util.Set;

/**
 * This class implements {@link CANLogEntryFilter}. Following is Filter PB definitions
 *
 * message CANIDFilter {
 *  optional int32 min = 1;
 *  optional int32 max = 2;
 *  repeated int32 value = 3 [(nanopb).max_count = 4];
 * }
 *
 * message CANFilter {
 *  required CANIDFilter idFilter = 1;
 * }
 */
public class CANLogEntryFilterImpl implements CANLogEntryFilter {
  private int minId = Integer.MIN_VALUE;
  private int maxId = Integer.MAX_VALUE;
  private Set<Integer> idSet = new HashSet<>();
  private TcpApiProtos.CANFilter canFilter;

  /**
   * Allow All CAN Log Entries Filter
   */
  public CANLogEntryFilterImpl() {
  }

  public CANLogEntryFilterImpl(TcpApiProtos.CANFilter pbCANFilter) {
    canFilter = pbCANFilter;
    if(pbCANFilter.hasIdFilter()){
      TcpApiProtos.CANIDFilter canIDFilter = pbCANFilter.getIdFilter();
      if(canIDFilter.hasMax()){
        maxId = canIDFilter.getMax();
      }
      if(canIDFilter.hasMin()) {
        minId = canIDFilter.getMin();
      }

      idSet.addAll(canIDFilter.getValueList());
    }
  }

  /**
   *
   * @param event
   * @return True when a CAN log entry meets current filter condition
   */
  @Override
  public boolean filter(CANLogEntry event) {
    boolean hasRangeFilter = false;
    if(this.maxId != Integer.MAX_VALUE || this.minId != Integer.MIN_VALUE) {
      hasRangeFilter = true;
    }
    if(this.minId != Integer.MIN_VALUE && event.getId() < this.minId){
      return false;
    }
    if(this.maxId != Integer.MAX_VALUE && event.getId() > this.maxId) {
      return false;
    }
    if(!hasRangeFilter && idSet.size() > 0 && !idSet.contains(event.getId())){
      return false;
    }

    return true;
  }

  @Override
  public TcpApiProtos.CANFilter getCANFilter() {
    return canFilter;
  }
}
