package com.visualthreat.testing.database;

import java.util.Set;
import java.util.concurrent.CompletableFuture;

public interface DatabaseUtil {
  Set<Integer> loadFinishedECUIDs(String test_name);
  CompletableFuture<Boolean> insertAndUpdateResumeTable(String testcase_name, int finished_ecu_id);
  CompletableFuture<Boolean> clearFinishedECUIDForTestCase(String testcase_name);
  boolean isResumeEnabled(String test_name);
  CompletableFuture<Integer> getPredefinedBaudRateSpeedFromDB();
  CompletableFuture<Boolean> insertIntoCustomBaudRate(int baudRateStr);
}