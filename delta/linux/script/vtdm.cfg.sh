#!/bin/bash
#
#
# The configuration file is an executable bash script file.
# And the bash syntax must be followed.
#
export VTDM_IMPL="vtdiff01"
#export VTDM_IMPL="vtdiff02"
export VTDM_DIFF_OP="sub"
#export VTDM_LIBLZMA_SO="liblzma.so"
export VTDM_LIBLZMA_SO=
#export VTDM_LIBBZ2_SO="libbz2.so"
export VTDM_LIBBZ2_SO=
export VTDM_CHECKSUM_ALGORITHM="crc32"
export VTDM_MAX_CHUNK_SIZE="2k"
export VTDM_DATA_GRAM_SIZE="8"
export VTDM_HASH_WINDOW_SIZE="8"
export VTDM_CONTINUE_MISMATCH_LIMIT="6"
export VTDM_MAX_MISMATCH_PERCENTAGE="50"
export VTDM_MAX_IDENTICAL_FIRST_IN_MATCH="false"
export VTDM_JMP_IDENTICAL_BLOCK_IN_MATCH="true"
export VTDM_NEED_KEEP_INTERIM_TEMP_FILES="false"
#
# VTDM_FLAG_OPT only valid for merge
# VTDM_FLAG_OPT can be one of: "oemramfile" or "oemstdfile" or ""
#export VTDM_FLAG_OPT="oemramfile"
#export VTDM_FLAG_OPT="oemstdfile"
export VTDM_FLAG_OPT=""
#
export VTDM_LOG_LEVEL="DEBUG"
#export VTDM_LOG_LEVEL="INFO"
#export VTDM_LOG_LEVEL="NOTICE"
#
