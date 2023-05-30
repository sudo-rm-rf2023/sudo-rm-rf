#!/bin/bash

SCRIPT_DIR="$1"
TEST_PORT=$2
EXPECTED_RESPONSE_FILE="expected_crud_create_increment_response.txt"
ACTUAL_RESPONSE_FILE="actual_crud_create_increment_response.txt"
METHOD="POST"
DATA="{'some': 'json_fields'}"
CRUD_API_PATH="api/Socks"

# Create entitity 1
curl -X "${METHOD}" -H "Content-Type: text/plain" -H "User-Agent:" -d "${DATA}" -i -s -S http://localhost:${TEST_PORT}/${CRUD_API_PATH} > /dev/null
# Send a request to the web server and capture the output
curl -o "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE" -X "${METHOD}" -H "Content-Type: text/plain" -H "User-Agent:" -d "${DATA}" -i -s -S http://localhost:${TEST_PORT}/${CRUD_API_PATH}

# Verify that the response from the web server is as expected
# -b option ignores line endings (Windows vs. Linux)
diff -b "${SCRIPT_DIR}/$EXPECTED_RESPONSE_FILE" "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE"
DIFF_EXIT_CODE=$?

# Continue to next test or exit with failure (1)
if [ $DIFF_EXIT_CODE -eq 0 ]; then
  echo "PASS"
else
  echo "crud create increment test FAIL"
fi