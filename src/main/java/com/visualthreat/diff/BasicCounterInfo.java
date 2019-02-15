package com.visualthreat.diff;

import lombok.Getter;
import lombok.Setter;
import lombok.ToString;

@Setter
@Getter
@ToString
public class BasicCounterInfo {
  int id;
  long count;

  public BasicCounterInfo(int id, long count) {
    this.id = id;
    this.count = count;
  }
}
