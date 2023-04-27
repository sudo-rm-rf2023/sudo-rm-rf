#!/bin/bash

SCRIPT_DIR="$1"
TEST_PORT=$2
EXPECTED_RESPONSE_FILE="expected_echo_response.txt"
ACTUAL_RESPONSE_FILE="actual_echo_response.txt"

# Send a request to the web server and capture the output
curl -o "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE" -X POST -H "Content-Type: text/plain" -d "This is a test message" -s -S http://localhost:${TEST_PORT}

# Verify that the response from the web server is as expected
# -b option ignores line endings (Windows vs. Linux)
diff -b "${SCRIPT_DIR}/$EXPECTED_RESPONSE_FILE" "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE"
DIFF_EXIT_CODE=$?

# Continue to next test or exit with failure (1)
if [ $DIFF_EXIT_CODE -eq 0 ]; then
  echo "PASS"
else
  echo "echo request test FAIL"
fi