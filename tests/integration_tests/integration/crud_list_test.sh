#!/bin/bash

SCRIPT_DIR="$1"
TEST_PORT=$2
EXPECTED_RESPONSE_FILE="expected_crud_list_response.txt"
ACTUAL_RESPONSE_FILE="actual_crud_list_response.txt"
METHOD="GET"
CRUD_API_PATH="api/Dresses"

# create file that we want to list
mkdir "./mnt/crud/Dresses"
touch "./mnt/crud/Dresses/1"
touch "./mnt/crud/Dresses/2"
touch "./mnt/crud/Dresses/3"
touch "./mnt/crud/Dresses/4"

# Send a request to the web server and capture the output
curl -k -o "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE" -X "${METHOD}" -H "Content-Type: text/plain" -H "User-Agent:" -i -s -S https://localhost:${TEST_PORT}/${CRUD_API_PATH}

# Verify that the response from the web server is as expected
# -b option ignores line endings (Windows vs. Linux)
diff -b "${SCRIPT_DIR}/$EXPECTED_RESPONSE_FILE" "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE"
RESPONSE_DIFF_EXIT_CODE=$?

# Continue to next test or exit with failure (1)
if [ $RESPONSE_DIFF_EXIT_CODE -eq 0 ]; then
  echo "PASS"
else
  echo "crud list test FAIL"
fi