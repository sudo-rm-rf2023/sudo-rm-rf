#!/bin/bash

SCRIPT_DIR="$1"
TEST_PORT=$2
EXPECTED_RESPONSE_FILE="expected_invalid_response.txt"
ACTUAL_RESPONSE_FILE="actual_invalid_response.txt"


# Send a request to the web server and capture the output
echo -ne "INVALID REQUEST\r\n\r\n" | nc localhost 8080 > "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE"

# Verify that the response from the web server is as expected
diff -b "${SCRIPT_DIR}/$EXPECTED_INVALID_RESPONSE_FILE" "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE"
DIFF_EXIT_CODE=$?

# Exit with an exit code indicating success (0) or failure (1)
if [ $DIFF_EXIT_CODE -eq 0 ]; then
  echo "PASS"
else
  echo "invalid test FAIL"
fi