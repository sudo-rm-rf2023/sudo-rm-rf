#!/bin/bash

SCRIPT_DIR="$1"
TEST_PORT=$2
EXPECTED_RESPONSE_FILE="expected_404_response.txt"
ACTUAL_RESPONSE_FILE="actual_404_response.txt"

# Send a request to the web server and capture the output
curl -k -D - -X POST -H "Content-Type: text/plain" -H "User-Agent:" -d "This is a test message" -s -S https://localhost:${TEST_PORT}/notfoundpath > "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE"

# Verify that the response from the web server is as expected
# -b option ignores line endings (Windows vs. Linux)
diff -b "${SCRIPT_DIR}/$EXPECTED_RESPONSE_FILE" "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE"
DIFF_EXIT_CODE=$?

# Continue to next test or exit with failure (1)
if [ $DIFF_EXIT_CODE -eq 0 ]; then
  echo "PASS"
else
  echo "404 not found test FAIL"
fi