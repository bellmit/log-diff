package com.visualthreat.testing.core;

import lombok.Getter;
import lombok.Setter;

public class TestStopException extends TestException {
  private static final long serialVersionUID = 3235936123283185016L;
  @Getter
  @Setter
  private boolean paused = false;

  public TestStopException() {}

  public TestStopException(String message) {
    super(message);
  }

  public TestStopException(Throwable cause) {
    super(cause);
  }

  public TestStopException(String message, Throwable cause) {
    super(message, cause);
  }

  public TestStopException(String message, Throwable cause,
                           boolean enableSuppression, boolean writableStackTrace) {
    super(message, cause, enableSuppression, writableStackTrace);
  }
}
