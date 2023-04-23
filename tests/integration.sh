#!/bin/bash

# Configuration
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"  # Get the directory of this script
TEST_CONFIG="../server_config"
TEST_PORT=8080
EXPECTED_RESPONSE_FILE="expected_response.txt"
EXPECTED_INVALID_RESPONSE_FILE="expected_invalid_response.txt"
ACTUAL_RESPONSE_FILE="actual_response.txt"
ACTUAL_INVALID_RESPONSE_FILE="actual_invalid_response.txt"
COMMAND_NAME="${SCRIPT_DIR}/../build/bin/server"

echo $COMMAND_NAME
# Run the web server binary with the test config file in the background
# Assume script is in the tests directory
$COMMAND_NAME "${SCRIPT_DIR}/$TEST_CONFIG" &
SERVER_PID=$!

# Give the server some time to start
sleep 1

# Send a request to the web server and capture the output
curl -o "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE" -X POST -H "Content-Type: text/plain" -d "This is a test message" -s -S http://localhost:${TEST_PORT}

# Verify that the response from the web server is as expected
# -b option ignores line endings (Windows vs. Linux)
diff -b "${SCRIPT_DIR}/$EXPECTED_RESPONSE_FILE" "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE"
DIFF_EXIT_CODE=$?

# Shut down the web server
kill $SERVER_PID

# Continue to next test or exit with failure (1)
if [ $DIFF_EXIT_CODE -eq 0 ]; then
  echo "Integration test 1: SUCCESS"
else
  echo "Integration test 1: FAILURE"
  exit 1
fi

# Run the web server binary with the test config file in the background
# Assume script is in the tests directory
$COMMAND_NAME "${SCRIPT_DIR}/$TEST_CONFIG" &
SERVER_PID=$!

# Give the server some time to start
sleep 1

# Send a request to the web server and capture the output
echo -ne "INVALID REQUEST\r\n\r\n" | nc localhost 8080 > "${SCRIPT_DIR}/$ACTUAL_INVALID_RESPONSE_FILE"

# Verify that the response from the web server is as expected
diff -b "${SCRIPT_DIR}/$EXPECTED_INVALID_RESPONSE_FILE" "${SCRIPT_DIR}/$ACTUAL_INVALID_RESPONSE_FILE"
DIFF_EXIT_CODE=$?

# Shut down the web server
kill $SERVER_PID

# Exit with an exit code indicating success (0) or failure (1)
if [ $DIFF_EXIT_CODE -eq 0 ]; then
  echo "Integration test 2: SUCCESS"
  exit 0
else
  echo "Integration test 2: FAILURE"
  exit 1
fi

