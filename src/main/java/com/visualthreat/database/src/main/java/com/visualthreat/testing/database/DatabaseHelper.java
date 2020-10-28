package com.visualthreat.testing.database;

import java.sql.Connection;
import java.sql.SQLException;
import java.util.Properties;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.ForkJoinPool;
import java.util.function.Function;

import org.apache.commons.dbcp.BasicDataSource;
import org.flywaydb.core.Flyway;
import org.jooq.DSLContext;
import org.jooq.impl.DSL;

import lombok.extern.slf4j.Slf4j;

@Slf4j
public final class DatabaseHelper {
  private static final ExecutorService executor = new ForkJoinPool(1);
  private static final BasicDataSource ds = new BasicDataSource();

  public static void init() {
    log.debug("Loading properties");
    System.setProperty("java.util.concurrent.ForkJoinPool.common.parallelism", "20");

    final Properties properties = new Properties();
    properties.put("db.path", "data/db;AUTO_SERVER=TRUE");
    properties.put("db.username", "sa");
    properties.put("db.password", "");
//    try {
//      properties.load(DatabaseHelper.class.getResourceAsStream("/app.properties"));
//    } catch (final IOException e) {
//      log.error("Failed to load the properties", e);
//    } catch (Exception e) {
//    	log.error("Exception:", e);
//	}

    log.debug("Creating the data source");
    ds.setDriverClassName("org.h2.Driver");
    ds.setUrl("jdbc:h2:" + properties.getProperty("db.path"));
    ds.setUsername(properties.getProperty("db.username"));
    ds.setPassword(properties.getProperty("db.password"));
    ds.setDefaultAutoCommit(true);

    log.debug("Executing Flyway (database migration)");
    final Flyway flyway = new Flyway();
    flyway.setDataSource(ds);
    flyway.migrate();
    registerShutdownHook();

  }

  public static ExecutorService getReportExecutor() {
    return executor;
  }

  public static synchronized Connection getConnection() throws SQLException {
    return ds.getConnection();
  }

  public static <T> CompletableFuture<T> runDbQuery(Function<DSLContext, T> dbQuery) {
    return runDbQuery(dbQuery, true, null);
  }

  public static <T> CompletableFuture<T> runDbQuery(Function<DSLContext, T> dbQuery, T defaultValue) {
    return runDbQuery(dbQuery, true, defaultValue);
  }

  private static <T> CompletableFuture<T> runDbQuery(Function<DSLContext, T> dbQuery, boolean needDefault, T defaultValue) {
    final CompletableFuture<T> future = new CompletableFuture<>();
    getReportExecutor().submit(() -> {
      try (Connection conn = DatabaseHelper.getConnection();
           DSLContext dsl = DSL.using(conn)) {
        future.complete(dbQuery.apply(dsl));
      } catch (final RuntimeException | SQLException e) {
        log.error("Can't run db query", e);
        if (needDefault) {
          future.complete(defaultValue);
        } else {
          future.cancel(false);
        }
      } catch (final Throwable e) {
        log.error("Uncaught exception in db query", e);
        future.cancel(false);
        throw e;
      }
    });

    return future;
  }

  private static void registerShutdownHook() {
    Runtime.getRuntime().addShutdownHook(new Thread(() -> {
      log.info("Closing the data source");
      executor.shutdown();
      try {
        ds.close();
      } catch (final SQLException e) {
        log.error("Failed to close the data source", e);
      } catch (Exception e2) {
    	  log.error("Exception:", e2);
	}
    }));
  }

  public static BasicDataSource getDb() {
    return ds;
  }
}
