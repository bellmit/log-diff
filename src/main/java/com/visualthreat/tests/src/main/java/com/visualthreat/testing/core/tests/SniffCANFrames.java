package com.visualthreat.testing.core.tests;

import static com.visualthreat.testing.core.TestParameters.CAN_IDS;
import static com.visualthreat.testing.core.TestParameters.TEST_DESCRIPTION;
import static com.visualthreat.testing.core.TestParameters.TIME_INTERVAL;

import com.visualthreat.platform.common.model.proto.generated.TcpApiProtos;
import com.visualthreat.testing.core.AbstractScenario;
import com.visualthreat.testing.core.CANLogEntryFilterImpl;
import com.visualthreat.testing.core.AppControl;
import com.visualthreat.testing.core.TestException;

import lombok.extern.slf4j.Slf4j;

/**
 * The instance of this class is used to Sniff CAN Frames
 */
@RunningMode(tags = {RunningMode.Mode.Local, RunningMode.Mode.Cloud})
@Slf4j
public class SniffCANFrames extends AbstractScenario {
  private String[] ids;
  private long timeInterval = 30;

  public SniffCANFrames(AppControl control) {
    super(control);
  }

  @Override
  public void run() throws TestException {
    control.disableAndClear();
    ids = control.getParam(CAN_IDS).split(",");
    timeInterval = control.getParam(TIME_INTERVAL);
    String description = control.getParam(TEST_DESCRIPTION);
    if (this.ids != null && this.ids.length > 0) {
      TcpApiProtos.CANFilter.Builder filterBuilder = TcpApiProtos.CANFilter.newBuilder();
      TcpApiProtos.CANIDFilter.Builder builder = TcpApiProtos.CANIDFilter.newBuilder();
      builder.setMin(Integer.decode(ids[0]));
      builder.setMax(Integer.decode(ids[1]));
      filterBuilder.setIdFilter(builder.build());

      // set local filter
      control.setFilter(new CANLogEntryFilterImpl(filterBuilder.build()));
    }

    if(description != null && description.length() > 0) {
      control.comment(description);
    }
    control.enableListen();
    try {
      Thread.sleep(timeInterval * 1000);
    } catch (InterruptedException e) {
    	log.error("Exception:", e);
      Thread.currentThread().interrupt();
    } catch (Exception e) {
    	log.error("Exception:", e);
	}

    control.disableAndClear();
  }
}
