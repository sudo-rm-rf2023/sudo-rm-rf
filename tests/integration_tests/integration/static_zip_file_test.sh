#!/bin/bash


SCRIPT_DIR="$1"
TEST_PORT=$2
EXPECTED_RESPONSE_FILE="${SCRIPT_DIR}/../test_static_root/test_sudo-rm-rf.zip"
ACTUAL_RESPONSE_FILE="${SCRIPT_DIR}/actual_static_txt_response.zip"

# Send a request to the web server to get the static file and save the output to ACTUAL_RESPONSE_FILE
curl -k -o "$ACTUAL_RESPONSE_FILE" -s -S https://localhost:$TEST_PORT/static/test_sudo-rm-rf.zip -H "User-Agent:"

# Compare the received file (ACTUAL_RESPONSE_FILE) with the expected file (EXPECTED_RESPONSE_FILE)
# -b option ignores line endings (Windows vs. Linux)
diff -b "$EXPECTED_RESPONSE_FILE" "$ACTUAL_RESPONSE_FILE"
DIFF_EXIT_CODE=$?

# If the diff command exits with a 0 status, the test passes; otherwise, it fails.
if [ $DIFF_EXIT_CODE -eq 0 ]; then
  echo "PASS"
else
  echo "static html file test FAIL"
fi