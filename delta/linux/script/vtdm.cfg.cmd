@echo off
REM
REM
REM The configuration file is an executable windows batch script file.
REM And the windows batch syntax must be followed.
REM
set VTDM_IMPL=vtdiff01
REM set VTDM_IMPL=vtdiff02
set VTDM_DIFF_OP=sub
REM set VTDM_LIBLZMA_SO=lzma.dll
set VTDM_LIBLZMA_SO=
REM set VTDM_LIBBZ2_SO=bz2.dll
set VTDM_LIBBZ2_SO=
set VTDM_CHECKSUM_ALGORITHM=crc32
set VTDM_MAX_CHUNK_SIZE=4k
set VTDM_DATA_GRAM_SIZE=8
set VTDM_HASH_WINDOW_SIZE=8
set VTDM_CONTINUE_MISMATCH_LIMIT=6
set VTDM_MAX_MISMATCH_PERCENTAGE=50
set VTDM_MAX_IDENTICAL_FIRST_IN_MATCH=false
set VTDM_JMP_IDENTICAL_BLOCK_IN_MATCH=true
set VTDM_NEED_KEEP_INTERIM_TEMP_FILES=false
REM
REM VTDM_FLAG_OPT only valid for merge
REM VTDM_FLAG_OPT can be one of: "oemramfile" or "oemstdfile" or ""
REM set VTDM_FLAG_OPT=oemramfile
REM set VTDM_FLAG_OPT=oemstdfile
set VTDM_FLAG_OPT=
REM
set VTDM_LOG_LEVEL=DEBUG
REM set VTDM_LOG_LEVEL=INFO
REM set VTDM_LOG_LEVEL=NOTICE
REM
