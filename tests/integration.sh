#!/bin/bash

# Configuration
TEST_CONFIG="../server_config"
TEST_PORT=8080
EXPECTED_RESPONSE_FILE="expected_response.txt"
ACTUAL_RESPONSE_FILE="actual_response.txt"


# Run the web server binary with the test config file in the background
# Assume script is in the tests directory
../build/bin/server $TEST_CONFIG &
SERVER_PID=$!

# Give the server some time to start
sleep 2

# Send a request to the web server and capture the output
curl -o $ACTUAL_RESPONSE_FILE -X POST -H "Content-Type: text/plain" -d "This is a test message" -s -S http://localhost:${TEST_PORT}

# Verify that the response from the web server is as expected
# -b option ignores line endings (Windows vs. Linux)
diff -b $EXPECTED_RESPONSE_FILE $ACTUAL_RESPONSE_FILE
DIFF_EXIT_CODE=$?

# Shut down the web server
kill $SERVER_PID

# Exit with an exit code indicating success (0) or failure (1)
if [ $DIFF_EXIT_CODE -eq 0 ]; then
  echo "Integration test: SUCCESS"
  exit 0
else
  echo "Integration test: FAILURE"
  exit 1
fi