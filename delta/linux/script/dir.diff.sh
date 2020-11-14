#!/bin/bash
#
usage(){
	echo "${0} <dir1> <dir2> <delta_dir>"
}
#
if [ -z "${1}" ] || [ ! -d "${1}" ]; then
	echo "<dir1> is null or does not exist"
	usage;
	exit -1
fi
#
if [ -z "${2}" ] || [ ! -d "${2}" ]; then
	echo "<dir2> is null or does not exist"
	usage;
	exit -1
fi
#
if [ -z "${3}" ]; then
	echo "<delta_dir> is null"
	usage;
	exit -1
fi
if [ -d "${3}" ] || [ -f "${3}" ]; then
	rm -fr "${3}"
fi
if [ ! -d "${3}" ]; then
	mkdir -p "${3}"
fi
#
dir1="${1}"
dir2="${2}"
delta_dir="${3}"
#
filepaths2=$(find ${dir2} -type f)
for filepath2 in ${filepaths2}; do
	filename=${filepath2##${dir2}}
	filepath1=${dir1}${filename}
	delta_filepath=${delta_dir}${filename}.delta.lzma
	delta_file_dir=${delta_filepath%/*}
	#
	#echo filepath1=${filepath1}
	#echo filepath2=${filepath2}
	#echo delta_filepath=${delta_filepath}
	#echo delta_file_dir=${delta_file_dir}
	#
	mkdir -p ${delta_file_dir}
	./diff.sh ${filepath1} ${filepath2} ${delta_filepath}
done
#
filesizes=$(find ${delta_dir} -type f -exec ls -l {} \; | cut -d' ' -f5)
totalsize=0
for filesize in ${filesizes} ; do
	totalsize=$((totalsize + filesize))
done
echo totalsize=${totalsize}

