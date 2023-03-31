#!/bin/bash

set -u

declare -r THIS_SCRIPT_PATH="$(cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P)"
declare -r OUTPUT_DIR=${THIS_SCRIPT_PATH}/test-output

if [[ ! -d "${OUTPUT_DIR}" ]] ; then
  mkdir -p "${OUTPUT_DIR}"
fi
rm -f ${OUTPUT_DIR}/*

declare NUM_ERRORS=0
for f in Examples/* ; do
	bash run-example.sh "${OUTPUT_DIR}" "$f"
	if [[ $? != 0 ]]; then
	  NUM_ERRORS=$(( NUM_ERRORS + 1 ))
	  echo "ERROR: Run error in \"$f\"."
	fi
done

if [[ ${NUM_ERRORS} != 0 ]]; then
  echo "ERROR: There were ${NUM_ERRORS} run errors."
  exit 1
fi

echo "SUCCESS: No run errors."
