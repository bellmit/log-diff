@echo off
REM
REM If this script is not run in the directory it is placed,
REM variable THIS_SCRIPT_DIRPATH must be set to an absolute dirpath.
set THIS_SCRIPT_DIRPATH=%cd%
set THIS_PARENT_DIRPATH=%THIS_SCRIPT_DIRPATH%\..
set VTDM_CFG_PATH=%THIS_SCRIPT_DIRPATH%\vtdm.cfg.cmd
set CLASSPATH_SAVED=%CLASSPATH%
set PATH_SAVED=%PATH%
REM
if not "%3"=="" goto next1
echo
echo Usage:
echo   %0% ^<filepath1^> ^<filepath2^> ^<delta_filepath^>
goto done
REM
REM
:next1
set VTDM_FILEPATH1=%1%
set VTDM_FILEPATH2=%2%
set VTDM_DELTA_FILEPATH=%3%
REM
REM
if exist %VTDM_FILEPATH1% goto next2
echo
echo "The filepath1: "%VTDM_FILEPATH1%" does not exist!
echo
echo Usage:
echo   %0% ^<filepath1^> ^<filepath2^> ^<delta_filepath^>
goto done
REM
REM
:next2
if exist %VTDM_DELTA_FILEPATH% goto next3
echo
echo "The delta_data_filepath: "%VTDM_DELTA_FILEPATH%" does not exist!
echo
echo Usage:
echo   %0% ^<filepath1^> ^<filepath2^> ^<delta_filepath^>
goto done
REM
REM
:next3
if exist %VTDM_CFG_PATH% goto next4
echo
echo The file: "%VTDM_CFG_PATH%" does not exist!
echo This file works as configuration that must be exist.
goto done
REM
REM
:next4
where java
if %errorlevel% == 0 goto next5
echo
echo The java command is not in the PATH.
echo
goto done
REM
REM
:next5
REM import the configured values
call %VTDM_CFG_PATH%
REM
REM
set VTDM_IMPL_TAG=--impl
set VTDM_DIFF_OP_TAG=--diff_op
set VTDM_LIBLZMA_SO_TAG=--liblzma_so
set VTDM_LIBBZ2_SO_TAG=--libbz2_so
set VTDM_DATA_GRAM_SIZE_TAG=--data_gram_size
set VTDM_HASH_WINDOW_SIZE_TAG=--hash_window_size
set VTDM_CONTINUE_MISMATCH_LIMIT_TAG=--continue_mismtach_limit
set VTDM_MAX_MISMATCH_PERCENTAGE_TAG=--max_mismatch_percentage
set VTDM_MAX_IDENTICAL_FIRST_IN_MATCH_TAG=--max_identical_first_in_match
set VTDM_JMP_IDENTICAL_BLOCK_IN_MATCH_TAG=--jmp_identical_block_in_match
set VTDM_NEED_KEEP_INTERIM_TEMP_FILES_TAG=--need_keep_interim_temp_files
set VTDM_FILEPATH1_TAG=--filepath1
set VTDM_FILEPATH2_TAG=--filepath2
set VTDM_DELTA_FILEPATH_TAG=--delta_filepath
REM
set VTDM_FPARAMS=%VTDM_IMPL_TAG% %VTDM_IMPL%
set VTDM_FPARAMS=%VTDM_FPARAMS% %VTDM_DIFF_OP_TAG% %VTDM_DIFF_OP%
set VTDM_FPARAMS=%VTDM_FPARAMS% %VTDM_LIBLZMA_SO_TAG% %VTDM_LIBLZMA_SO%
set VTDM_FPARAMS=%VTDM_FPARAMS% %VTDM_LIBBZ2_SO_TAG% %VTDM_LIBBZ2_SO%
set VTDM_FPARAMS=%VTDM_FPARAMS% %VTDM_DATA_GRAM_SIZE_TAG% %VTDM_DATA_GRAM_SIZE%
set VTDM_FPARAMS=%VTDM_FPARAMS% %VTDM_HASH_WINDOW_SIZE_TAG% %VTDM_HASH_WINDOW_SIZE%
set VTDM_FPARAMS=%VTDM_FPARAMS% %VTDM_CONTINUE_MISMATCH_LIMIT_TAG% %VTDM_CONTINUE_MISMATCH_LIMIT%
set VTDM_FPARAMS=%VTDM_FPARAMS% %VTDM_MAX_MISMATCH_PERCENTAGE_TAG% %VTDM_MAX_MISMATCH_PERCENTAGE%
set VTDM_FPARAMS=%VTDM_FPARAMS% %VTDM_MAX_IDENTICAL_FIRST_IN_MATCH_TAG% %VTDM_MAX_IDENTICAL_FIRST_IN_MATCH%
set VTDM_FPARAMS=%VTDM_FPARAMS% %VTDM_JMP_IDENTICAL_BLOCK_IN_MATCH_TAG% %VTDM_JMP_IDENTICAL_BLOCK_IN_MATCH%
set VTDM_FPARAMS=%VTDM_FPARAMS% %VTDM_NEED_KEEP_INTERIM_TEMP_FILES_TAG% %VTDM_NEED_KEEP_INTERIM_TEMP_FILES%
set VTDM_FPARAMS=%VTDM_FPARAMS% %VTDM_FILEPATH1_TAG% %VTDM_FILEPATH1%
set VTDM_FPARAMS=%VTDM_FPARAMS% %VTDM_FILEPATH2_TAG% %VTDM_FILEPATH2%
set VTDM_FPARAMS=%VTDM_FPARAMS% %VTDM_DELTA_FILEPATH_TAG% %VTDM_DELTA_FILEPATH%
REM
REM
set VTDM_LOG_LEVEL_TAG=--log.level
set VTDM_SYSLOG_IDENT_TAG=--syslog.ident
set VTDM_SYSLOG_IDENT=%VTDM_IMPL%
REM
set VTDM_LPARAMS=%VTDM_LOG_LEVEL_TAG% %VTDM_LOG_LEVEL% %VTDM_SYSLOG_IDENT_TAG% %VTDM_SYSLOG_IDENT%
REM
REM
set VTDM_FLAG=-m
REM
REM
if "%CLASSPATH%" == "" set CLASSPATH=%THIS_PARENT_DIRPATH%\lib\libvtdiffrun.jar;%THIS_PARENT_DIRPATH%\lib\libvtdiffjni.jar
if not "%CLASSPATH%" == "" set CLASSPATH=%THIS_PARENT_DIRPATH%\lib\libvtdiffrun.jar;%THIS_PARENT_DIRPATH%\lib\libvtdiffjni.jar;%CLASSPATH_SAVED%
set PATH=%THIS_PARENT_DIRPATH%\bin;%PATH%
set JAVA_MAIN_CLASS=run.com.visualthreat.util.VTDiff
REM
REM
java %JAVA_MAIN_CLASS% %VTDM_LPARAMS% %VTDM_FLAG% %VTDM_FPARAMS%
if not %errorlevel%==0 goto onerror
echo The command execute successuly.
if exist %VTDM_FILEPATH2% goto onfilegenerated
goto onerror
:onfilegenerated
echo
echo The merged filepath2:
echo     "%VTDM_FILEPATH2%"
goto done
:onerror
echo
echo Something wrong, check the related log required.
goto done
REM
:done
echo
set CLASSPATH=%CLASSPATH_SAVED%
set PATH=%PATH_SAVED%
