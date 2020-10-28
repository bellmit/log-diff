package com.visualthreat.testing.core.tests;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Mark Test Readiness
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface RunningMode {
  public enum Mode {
    Local, Cloud, Experiment
  }

  Mode[] tags() default Mode.Experiment;
}
