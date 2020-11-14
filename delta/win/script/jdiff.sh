#!/bin/bash
#
# If this script is not run in the directory it is placed,
# variable THIS_SCRIPT_DIRPATH must be set to an absolute dirpath.
THIS_SCRIPT_DIRPATH=$(pwd)
THIS_PARENT_DIRPATH=$(dirname ${THIS_SCRIPT_DIRPATH})
VTDM_CFG_PATH="${THIS_SCRIPT_DIRPATH}/vtdm.cfg.sh"
#
#
if [ -z "${3}" ]; then
    echo "Usage:"
    echo "  ${0} <filepath1> <filepath2> <delta_filepath>"
    exit -1
fi
#
VTDM_FILEPATH1="${1}"
VTDM_FILEPATH2="${2}"
VTDM_DELTA_FILEPATH="${3}"
#
if [ ! -f "${VTDM_FILEPATH1}" ]; then
    echo "The filepath1: \"${VTDM_FILEPATH1}\" does not exist!"
    echo
    echo "Usage:"
    echo "  diff.sh <filepath1> <filepath2> <delta_filepath>"
    exit -1
fi
if [ ! -f "${VTDM_FILEPATH2}" ]; then
    echo "The filepath2: \"${VTDM_FILEPATH2}\" does not exist!"
    echo
    echo "Usage:"
    echo "  diff.sh <filepath1> <filepath2> <delta_filepath>"
    exit -1
fi
#
#
if [ ! -f "${VTDM_CFG_PATH}" ]; then
    echo
    echo "The file: \"${VTDM_CFG_PATH}\" does not exist!"
    echo "This file works as configuration that must be exist."
    echo
    exit -1;
fi
#
JAVA_CMD=$(which java 2>/dev/null)
if [ -z "${JAVA_CMD}" ];then
    echo
    echo "The java command is not set in path!"
    echo
    exit -1
fi
#
# import the configured values
source ${VTDM_CFG_PATH}
#
#
VTDM_IMPL_TAG="--impl"
VTDM_DIFF_OP_TAG="--diff_op"
VTDM_LIBLZMA_SO_TAG="--liblzma_so"
VTDM_LIBBZ2_SO_TAG="--libbz2_so"
VTDM_MAX_CHUNK_SIZE_TAG="--max_chunk_size"
VTDM_DATA_GRAM_SIZE_TAG="--data_gram_size"
VTDM_HASH_WINDOW_SIZE_TAG="--hash_window_size"
VTDM_CONTINUE_MISMATCH_LIMIT_TAG="--continue_mismtach_limit"
VTDM_MAX_MISMATCH_PERCENTAGE_TAG="--max_mismatch_percentage"
VTDM_MAX_IDENTICAL_FIRST_IN_MATCH_TAG="--max_identical_first_in_match"
VTDM_JMP_IDENTICAL_BLOCK_IN_MATCH_TAG="--jmp_identical_block_in_match"
VTDM_NEED_KEEP_INTERIM_TEMP_FILES_TAG="--need_keep_interim_temp_files"
VTDM_FILEPATH1_TAG="--filepath1"
VTDM_FILEPATH2_TAG="--filepath2"
VTDM_DELTA_FILEPATH_TAG="--delta_filepath"
#
VTDM_FPARAMS="\
${VTDM_IMPL_TAG} ${VTDM_IMPL} \
${VTDM_DIFF_OP_TAG} ${VTDM_DIFF_OP} \
${VTDM_LIBLZMA_SO_TAG} ${VTDM_LIBLZMA_SO} \
${VTDM_LIBBZ2_SO_TAG} ${VTDM_LIBBZ2_SO} \
${VTDM_MAX_CHUNK_SIZE_TAG} ${VTDM_MAX_CHUNK_SIZE} \
${VTDM_DATA_GRAM_SIZE_TAG} ${VTDM_DATA_GRAM_SIZE} \
${VTDM_HASH_WINDOW_SIZE_TAG} ${VTDM_HASH_WINDOW_SIZE} \
${VTDM_CONTINUE_MISMATCH_LIMIT_TAG} ${VTDM_CONTINUE_MISMATCH_LIMIT} \
${VTDM_MAX_MISMATCH_PERCENTAGE_TAG} ${VTDM_MAX_MISMATCH_PERCENTAGE} \
${VTDM_MAX_IDENTICAL_FIRST_IN_MATCH_TAG} ${VTDM_MAX_IDENTICAL_FIRST_IN_MATCH} \
${VTDM_JMP_IDENTICAL_BLOCK_IN_MATCH_TAG} ${VTDM_JMP_IDENTICAL_BLOCK_IN_MATCH} \
${VTDM_NEED_KEEP_INTERIM_TEMP_FILES_TAG} ${VTDM_NEED_KEEP_INTERIM_TEMP_FILES} \
${VTDM_FILEPATH1_TAG} ${VTDM_FILEPATH1} \
${VTDM_FILEPATH2_TAG} ${VTDM_FILEPATH2} \
${VTDM_DELTA_FILEPATH_TAG} ${VTDM_DELTA_FILEPATH} \
"
#
VTDM_LOG_LEVEL_TAG="--log.level"
VTDM_SYSLOG_IDENT_TAG="--syslog.ident"
VTDM_SYSLOG_IDENT="${VTDM_IMPL}"
VTDM_LPARAMS="${VTDM_LOG_LEVEL_TAG} ${VTDM_LOG_LEVEL} ${VTDM_SYSLOG_IDENT_TAG} ${VTDM_SYSLOG_IDENT}"
#
VTDM_FLAG="-d"
#
LD_LIBRARY_PATH_SAVED=${LD_LIBRARY_PATH}
CLASSPATH_SAVED=${CLASSPATH}
if [ -z "${LD_LIBRARY_PATH}" ]; then
    export LD_LIBRARY_PATH=${THIS_PARENT_DIRPATH}/lib
else
    export LD_LIBRARY_PATH=${THIS_PARENT_DIRPATH}/lib:${LD_LIBRARY_PATH}
fi
if [ -z "{CLASSPATH}" ]; then
    export CLASSPATH="${THIS_PARENT_DIRPATH}/lib/libvtdiffrun.jar:${THIS_PARENT_DIRPATH}/lib/libvtdiffjni.jar"
else
    export CLASSPATH="${THIS_PARENT_DIRPATH}/lib/libvtdiffrun.jar:${THIS_PARENT_DIRPATH}/lib/libvtdiffjni.jar:${CLASSPATH}"
fi
#
JAVA_MAIN_CLASS="run.com.visualthreat.util.VTDiff"
#
${JAVA_CMD} ${JAVA_MAIN_CLASS} ${VTDM_FLAG} ${VTDM_LPARAMS} ${VTDM_FPARAMS}
RETVAL="$?"
echo
if [ "$RETVAL" == "0" ]; then
    if [ -f "${VTDM_DELTA_FILEPATH}" ]; then
        echo "The generated delta filepath:"
        echo "    \"${VTDM_DELTA_FILEPATH}\""
    else
        echo "The command execute successfuly."
        echo "No generated delta filepath, due to the files are identical."
    fi
else
    echo "Something wrong, check the related log required."
fi
echo
#
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH_SAVED}
export CLASSPATH=${CLASSPATH_SAVED}
#
