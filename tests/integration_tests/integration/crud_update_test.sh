#!/bin/bash

SCRIPT_DIR="$1"
TEST_PORT=$2
EXPECTED_RESPONSE_FILE="expected_crud_update_response.txt"
ACTUAL_RESPONSE_FILE="actual_crud_update_response.txt"
METHOD="PUT"
DATA="{'some': 'new_json_fields'}"
CRUD_API_PATH="api/Pants/123"

# create file that we want to update
mkdir "./mnt/crud/Pants"
touch "./mnt/crud/Pants/123"
echo "{some: json_that_exists}" > "./mnt/crud/Pants/123"

# Send a request to the web server and capture the output
curl -o "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE" -X "${METHOD}" -H "Content-Type: text/plain" -H "User-Agent:" -d "${DATA}" -i -s -S http://localhost:${TEST_PORT}/${CRUD_API_PATH}

# Verify that the response from the web server is as expected
# -b option ignores line endings (Windows vs. Linux)
diff -b "${SCRIPT_DIR}/$EXPECTED_RESPONSE_FILE" "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE"
RESPONSE_DIFF_EXIT_CODE=$?

# Verify that the file is actually updated
diff -b "./mnt/crud/Pants/123" <(echo "$DATA")
FILE_DIFF_EXIT_CODE=$?

# Continue to next test or exit with failure (1)
if [ $RESPONSE_DIFF_EXIT_CODE -eq 0 ] && [ $FILE_DIFF_EXIT_CODE -eq 0 ]; then
  echo "PASS"
else
  echo "crud update test FAIL"
fi