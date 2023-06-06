#!/bin/bash

SCRIPT_DIR="$1"
TEST_PORT=$2
EXPECTED_RESPONSE_FILE="expected_crud_delete_response.txt"
ACTUAL_RESPONSE_FILE="actual_crud_delete_response.txt"
METHOD="DELETE"
CRUD_API_PATH="api/Shirts/123"

# create file that we want to delete
mkdir "./mnt/crud/Shirts"
touch "./mnt/crud/Shirts/123"
echo "{some: json_that_exists}" > "./mnt/crud/Shirts/123"

# Send a request to the web server and capture the output
curl -k -o "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE" -X "${METHOD}" -H "Content-Type: text/plain" -H "User-Agent:" -i -s -S https://localhost:${TEST_PORT}/${CRUD_API_PATH}

# Verify that the response from the web server is as expected
# -b option ignores line endings (Windows vs. Linux)
diff -b "${SCRIPT_DIR}/$EXPECTED_RESPONSE_FILE" "${SCRIPT_DIR}/$ACTUAL_RESPONSE_FILE"
RESPONSE_DIFF_EXIT_CODE=$?

# Continue to next test or exit with failure (1)
if [ $RESPONSE_DIFF_EXIT_CODE -eq 0 ] && 
[ ! -f "./mnt/crud/Shirts/123" ]; # Verify file actually deleted
then
  echo "PASS"
else
  echo "crud delete test FAIL"
fi