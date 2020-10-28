package com.visualthreat.testing.database;

import static com.visualthreat.testing.database.DatabaseHelper.runDbQuery;
import static org.jooq.impl.DSL.field;
import static org.jooq.impl.DSL.table;

import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.CompletableFuture;
import lombok.extern.slf4j.Slf4j;
import org.jooq.Record;

@Slf4j
public class DatabaseUtilImpl implements DatabaseUtil {

  private static DatabaseUtilImpl instance;

  public static DatabaseUtilImpl getInstance(){
    if(instance == null){
      synchronized (DatabaseUtilImpl.class) {
        if (instance == null) {
          instance = new DatabaseUtilImpl();
        }
      }
    }
    return instance;
  }

  @Override
  public Set<Integer> loadFinishedECUIDs(String test_name){
    Record record = findFinishedECUIDs(test_name).join();
    // If test case is not found in DB or there is no resume ecu ids, return empty list
    Set<Integer> idList = new HashSet<>();
    if(record != null && record.size() != 0){
      String finishedECUIDStr = record.getValue(field("FINISHED_ECU_IDS")).toString();
      for(String str : finishedECUIDStr.split(":")){
        if(str == null || str.equals("")){
          continue;
        }
        idList.add(Integer.parseInt(str));
      }
    }
    return idList;
  }

  private CompletableFuture<Record> findFinishedECUIDs(String test_name){
    return runDbQuery(dsl ->{
      return dsl.select(field("FINISHED_ECU_IDS"))
          .from(table("RESUME_TEST_RUN"))
          .where(field("TESTCASE_NAME").eq(test_name))
          .limit(1)
          .fetchOne();
    });
  }

  @Override
  public CompletableFuture<Boolean> insertAndUpdateResumeTable(String testcase_name, int finished_ecu_id){
    Record record = findFinishedECUIDs(testcase_name).join();
    StringBuilder newFinishedIDs = new StringBuilder();
    // Generate the finished ecu ids string
    if(isResumeEnabled(testcase_name)){
      newFinishedIDs.append(record.getValue(field("FINISHED_ECU_IDS")).toString()).append(":");
    }
    newFinishedIDs.append(finished_ecu_id);
    return runDbQuery(dsl ->{
      try{
        return dsl.mergeInto(table("RESUME_TEST_RUN"),
            field("TESTCASE_NAME"), field("FINISHED_ECU_IDS"))
            .values(testcase_name, newFinishedIDs.toString())
            .execute() > 0;
      } catch (Exception e) {
        log.error("Can't insert into resume_test_run table", e);
        return false;
      }
    });
  }

  @Override
  public CompletableFuture<Boolean> clearFinishedECUIDForTestCase(String testcase_name){
    Record record = findFinishedECUIDs(testcase_name).join();
    return runDbQuery(dsl ->{
      try{
        if(record == null || record.size() == 0){
          return true;
        }else {
          return dsl.mergeInto(table("RESUME_TEST_RUN"),
              field("TESTCASE_NAME"), field("FINISHED_ECU_IDS"))
              .values(testcase_name, "")
              .execute() > 0;
        }
      } catch (Exception e) {
        log.error("Can't insert into resume_test_run table", e);
        return false;
      }
    });
  }

  @Override
  public boolean isResumeEnabled(String test_name){
    Record record = findFinishedECUIDs(test_name).join();
    boolean isResumeEnabled = false;
    if(record != null && record.size() != 0){
      String id = record.getValue(field("FINISHED_ECU_IDS")).toString();
      isResumeEnabled = id.equals("") ? false : true;
    }
    return isResumeEnabled;
  }

  @Override
  public CompletableFuture<Boolean> insertIntoCustomBaudRate(int baudRate) {
//    int baudRate = convertBaudRateFromStringToInt(baudRateStr);
    return runDbQuery(dsl ->{
      try{
        return dsl.mergeInto(table("CUSTOM_BAUD_RATE"),
            field("ID"), field("BAUD_RATE"))
            .values("CURRENT_BAUD_RATE", baudRate)
            .execute() > 0;

      } catch (Exception e) {
        log.error("Can't insert into custom_baud_rate table", e);
        return false;
      }
    });
  }

  @Override
  public CompletableFuture<Integer> getPredefinedBaudRateSpeedFromDB() {
    try{
      return runDbQuery(dsl ->{
        Record record = dsl.select(field("BAUD_RATE"))
            .from(table("CUSTOM_BAUD_RATE"))
            .where(field("ID").eq("CURRENT_BAUD_RATE"))
            .limit(1)
            .fetchOne();
        if(record != null){
          return Integer.parseInt(record.getValue(field("BAUD_RATE")).toString());
        }else {
          return null;
        }
      });
    }catch (Exception e){
      log.error("failed to get baudrate from custom_baud_rate table", e);
      return null;
    }
  }

  private int convertBaudRateFromStringToInt(String baudRateStr){
    int baudRate = -1;
    switch (baudRateStr){
      case "AUTO" :
        baudRate = -1;
        break;
      case "10K" :
        baudRate = 10000;
        break;
      case "20K" :
        baudRate = 20000;
        break;
      case "50K" :
        baudRate = 50000;
        break;
      case "80K" :
        baudRate = 80000;
        break;
      case "100K" :
        baudRate = 100000;
        break;
      case "125K" :
        baudRate = 125000;
        break;
      case "250K" :
        baudRate = 250000;
        break;
      case "400K" :
        baudRate = 400000;
        break;
      case "500K" :
        baudRate = 500000;
        break;
      case "750K" :
        baudRate = 750000;
        break;
      case "1000K" :
        baudRate = 1000000;
        break;
    }
    return baudRate;
  }
}
