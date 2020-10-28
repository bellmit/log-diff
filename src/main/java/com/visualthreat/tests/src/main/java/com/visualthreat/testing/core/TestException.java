package com.visualthreat.testing.core;

public class TestException extends RuntimeException {
  private static final long serialVersionUID = 3231659362232885016L;

  public TestException() {
  }

  public TestException(String message) {
    super(message);
  }

  public TestException(Throwable cause) {
    super(cause);
  }

  public TestException(String message, Throwable cause) {
    super(message, cause);
  }

  public TestException(String message, Throwable cause,
                             boolean enableSuppression, boolean writableStackTrace) {
    super(message, cause, enableSuppression, writableStackTrace);
  }
}