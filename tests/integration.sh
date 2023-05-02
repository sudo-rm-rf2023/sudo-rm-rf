#!/bin/bash

# COLOR
USE_COLOR=false

for arg in "$@"
do
  if [ "$arg" = "-c" ]; then
    USE_COLOR=true
    shift  # Remove the "color" argument from the list
  fi
done

RED="31"
GREEN="32"
YELLOW="33"
BLUE="34"
BOLDRED=$([ "$USE_COLOR" = "true" ] && echo "\e[1;${RED}m" || echo "")
BOLDGREEN=$([ "$USE_COLOR" = "true" ] && echo "\e[1;${GREEN}m" || echo "")
BOLDYELLOW=$([ "$USE_COLOR" = "true" ] && echo "\e[1;${YELLOW}m" || echo "")
BOLDBLUE=$([ "$USE_COLOR" = "true" ] && echo "\e[1;${BLUE}m" || echo "")
ITALICRED=$([ "$USE_COLOR" = "true" ] && echo "\e[3;${RED}m" || echo "")
ENDCOLOR=$([ "$USE_COLOR" = "true" ] && echo "\e[0m" || echo "")


# Configuration
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"  # Get the directory of this script
TEST_CONFIG="${SCRIPT_DIR}/integration/test.conf"
TEST_PORT=8080

echo -e "${BOLDBLUE}Script directory: ${SCRIPT_DIR}${ENDCOLOR}"
echo -e "${BOLDBLUE}Config: ${TEST_CONFIG}${ENDCOLOR}"

# Initialize variables for test results
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Check for the existence of the executable in both locations
if [ -f "${SCRIPT_DIR}/../build/bin/server" ]; then
  COMMAND_NAME="${SCRIPT_DIR}/../build/bin/server"
elif [ -f "${SCRIPT_DIR}/../build_coverage/bin/server" ]; then
  COMMAND_NAME="${SCRIPT_DIR}/../build_coverage/bin/server"
else
  echo "${ITALICRED}Server executable not found in both build and build_coverage directories.${ENDCOLOR}"
  exit 1
fi

echo -e "${BOLDBLUE}${COMMAND_NAME}${ENDCOLOR}"


# Run the web server binary with the test config file in the background
# Assume script is in the tests directory
$COMMAND_NAME "$TEST_CONFIG" &
SERVER_PID=$!

# Give the server some time to start
sleep 1


# Execute each test script and collect results
for TEST_SCRIPT in ${SCRIPT_DIR}/integration/*_test.sh; do
  ((TOTAL_TESTS++))
  echo -e "${BOLDYELLOW}Running test $TEST_SCRIPT${ENDCOLOR}"
  RESULT=$(bash "$TEST_SCRIPT" "${SCRIPT_DIR}/integration" $TEST_PORT)
  # echo -e "${BOLDBLUE}Result: $RESULT${ENDCOLOR}"
  if [ "$RESULT" == "PASS" ]; then
    ((PASSED_TESTS++))
    echo -e "Test ${BOLDGREEN}$(basename $TEST_SCRIPT): PASS${ENDCOLOR}"
  else
    ((FAILED_TESTS++))
    echo -e "Test ${TEST_SCRIPT}: ${ITALICRED}FAIL ${ENDCOLOR}"
    echo -e "${ITALICRED}${RESULT}${ENDCOLOR}"
  fi
done


# Shut down the web server
kill $SERVER_PID
# Remove the temporary files
# rm -f ${SCRIPT_DIR}/integration/actual_*


# Calculate and print pass rate
PASS_RATE=$(echo "${PASSED_TESTS} ${TOTAL_TESTS}" | awk '{printf "%.2f", ($1 * 100 / $2)}')

if [ $(echo "$PASS_RATE" | awk '{if ($1 < 100) print 1; else print 0}') -eq 1 ]; then
  echo -e "${BOLDRED}Passed $PASSED_TESTS out of $TOTAL_TESTS tests ($PASS_RATE%)${ENDCOLOR}"
else
  echo -e "${BOLDGREEN}Passed $PASSED_TESTS out of $TOTAL_TESTS tests ($PASS_RATE%)${ENDCOLOR}"
fi

if [ $FAILED_TESTS -eq 0 ]; then
  exit 0
else
  exit 1
fi

