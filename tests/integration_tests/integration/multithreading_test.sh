#!/bin/bash

SCRIPT_DIR="$1"
TEST_PORT=$2
EXPECTED_RESPONSE_FILE="${SCRIPT_DIR}/../test_static_root/test_index.html"
ACTUAL_RESPONSE_FILE="${SCRIPT_DIR}/actual_multithreading_static_html_response.html"

# Send a sleep request to the web server
http GET localhost:8080/block > /dev/null &
BLOCK_PID=$!

# wait for the request to start
sleep 2

# Send a request to the web server to get the static file and save the output to ACTUAL_RESPONSE_FILE
curl -o "$ACTUAL_RESPONSE_FILE" -s -S http://localhost:$TEST_PORT/static/test_index.html -H "User-Agent:"

# Compare the received file (ACTUAL_RESPONSE_FILE) with the expected file (EXPECTED_RESPONSE_FILE)
# -b option ignores line endings (Windows vs. Linux)
diff -b "$EXPECTED_RESPONSE_FILE" "$ACTUAL_RESPONSE_FILE"
DIFF_EXIT_CODE=$?

# wait for the sleep request to shut down
sleep 2

# Check if the sleep request is still running
if ! ps -p "$BLOCK_PID" > /dev/null; then
    echo "Sleep request is not running. FAIL"
fi

# kill the block request
kill -9 $BLOCK_PID

if [ $DIFF_EXIT_CODE -eq 0 ]; then
  echo "PASS"
else
  echo "Multithreaded static html file test FAIL"
fi
