package com.visualthreat.testing.core;

import com.visualthreat.platform.common.model.TestRun;

import lombok.extern.slf4j.Slf4j;

import static com.visualthreat.platform.common.model.TestStatus.FAILURE;
import static com.visualthreat.platform.common.model.TestStatus.IN_PROGRESS;
import static com.visualthreat.platform.common.model.TestStatus.PAUSED;
import static com.visualthreat.platform.common.model.TestStatus.SUCCESS;

@Slf4j
public abstract class AbstractTestRunner {
  protected abstract AppControl createTestControl(TestRun testRun);

  public AbstractScenario prepare(TestRun testRun) throws TestException {
    AppControl control = createTestControl(testRun);
    AbstractScenario scenario = scenarioFactory(testRun.getTestPoint(), control);

    if (scenario == null) {
      log.error("Can not create scenario for {}", testRun);
      throw new TestException("Can not create scenario");
    }

    control.setScenario(scenario);

    Object state = control.restoreState();
    if (state != null) {
      scenario.setState(state);
    }

    // should throw TestException if
    // no required dependencies or parameters
    scenario.before();
    return scenario;
  }

  public boolean run(AbstractScenario scenario) {
    boolean success = true;
    try {
      scenario.getControl().start();
      scenario.setStatus(IN_PROGRESS);
      // Set the ExpectedRequestFilter if needed
      scenario.setExpectedRequestFilter();
      scenario.getControl().setExpectedRequestFilter();
      scenario.getControl().enableRequestFilter(scenario.getEnableRequestFilterFlag());
      scenario.run();
      if (scenario.getStatus() == IN_PROGRESS) {
        scenario.setStatus(SUCCESS);
        scenario.updateProgress(String.format(AbstractScenario.STATUS_MESSAGE_FORMAT, 100.0f));
      }
    } catch (TestStopException e) {
      if (scenario.getStatus() == IN_PROGRESS) {
        scenario.setStatus(e.isPaused() ? PAUSED : FAILURE);
        scenario.setErrorString(e.getMessage());
      }
      success = false;
      log.error("Exception:", e);
    } catch (Exception e) {
      log.error("Exception:", e);
      log.warn("Exception raised while running test scenario", e);
      if (scenario.getStatus() == IN_PROGRESS) {
        scenario.setStatus(FAILURE);
        scenario.setErrorString(e.getMessage());
      }
      success = false;
    } finally {
      scenario.after();
      scenario.getControl().enableRequestFilter(false);
      scenario.getControl().cleanExpectedRequestFilter();
      scenario.getControl().stop(scenario.getStatus());
    }

    return success;
  }

  private static AbstractScenario scenarioFactory(String testPointId,
                                                  AppControl control) {
//    TestPoints testPoint = TestPoints.getEnum(testPointId);
//
//    if (testPoint == null) {
//      return null;
//    }
//
//    switch (testPoint) {
//      case REPLAY_LOG:
//        return new ReplayLog(control);
//    }

    return null;
  }
}
