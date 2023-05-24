#!/bin/bash

SCRIPT_DIR="$1"
TEST_PORT=$2
EXPECTED_RESPONSE_FILE="expected_crud_retrieve_response.txt"
ACTUAL_RESPONSE_FILE="actual_crud_retrieve_response.txt"
METHOD="GET"
CRUD_API_PATH="api/Hats/123"

# create file that we want to retrieve
mkdir "./mnt/crud/Hats"
touch "./mnt/crud/Hats/123"
echo "{some: json_to_retrieve}" > "./mnt/crud/Hats/123"

# Send a request to the web server and capture the output
curl -o "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE" -X "${METHOD}" -H "Content-Type: text/plain" -H "User-Agent:" -i -s -S http://localhost:${TEST_PORT}/${CRUD_API_PATH}

# Verify that the response from the web server is as expected
# -b option ignores line endings (Windows vs. Linux)
diff -b "${SCRIPT_DIR}/$EXPECTED_RESPONSE_FILE" "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE"
DIFF_EXIT_CODE=$?

# Continue to next test or exit with failure (1)
if [ $DIFF_EXIT_CODE -eq 0 ]; then
  echo "PASS"
else
  echo "crud retrieve test FAIL"
fi