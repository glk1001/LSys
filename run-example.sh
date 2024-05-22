#!/bin/bash

set -u

declare -r THIS_SCRIPT_PATH="$(cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P)"

declare -r EXE_FILE=${THIS_SCRIPT_PATH}/build-clion-clang-18-RelWithDebInfo/lsys-gen

declare -r OUTPUT_DIR="$1"
declare -r INPUT_FILE="$2"
declare -r INPUT_NAME=$(basename "${INPUT_FILE}")

if [[ ! -d "${OUTPUT_DIR}" ]] ; then
  echo ERROR: "Could not find output dir \"${OUTPUT_DIR}\"."
  exit 1
fi

declare -r OUTPUT_FILE=${OUTPUT_DIR}/${INPUT_NAME}.out
declare -r BOUNDS_FILE=${OUTPUT_DIR}/${INPUT_NAME}.bnds

echo ""
${EXE_FILE} -o "${OUTPUT_FILE}" -b "${BOUNDS_FILE}" "${INPUT_FILE}"
if [[ $? != 0 ]] ; then
  echo ERROR: "\"${INPUT_FILE}\" failed."
  exit 1
fi
