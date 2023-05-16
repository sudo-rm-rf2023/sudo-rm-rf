#!/bin/bash
# Inputs:
# integration.sh [-c color] <path_to_server_binary>

# COLOR
USE_COLOR=false

for arg in "$@"
do
  if [ "$arg" = "-c" ]; then
    USE_COLOR=true
    shift  # Remove the "color" argument from the list
  fi
done

# Check that the first argument has to be the path to server binary
if [ ! -f "$1" ]; then
  echo -e "ERROR: First argument is ${1}"
  echo -e "Usage: integration.sh [-c] <path_to_server_binary>. FAILED."
  exit 1;
fi

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
TEST_CONFIG="${SCRIPT_DIR}/assign6_test.conf"
SERVER_BINARY="$1"
TEST_PORT=8080

echo -e "${BOLDBLUE}Script directory: ${SCRIPT_DIR}${ENDCOLOR}"
echo -e "${BOLDBLUE}Config: ${TEST_CONFIG}${ENDCOLOR}"
echo -e "${BOLDBLUE}Server binary: ${SERVER_BINARY}${ENDCOLOR}"

# Initialize variables for test results
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

echo -e "${BOLDBLUE}${COMMAND_NAME}${ENDCOLOR}"


check_server_running() {
  if ! ps -p $SERVER_PID > /dev/null; then
    echo "Web server is not running. Exiting..."
    exit 1
  fi
}

# Run the web server binary with the test config file in the background
# Assume script is in the tests directory
$SERVER_BINARY "${TEST_CONFIG}" &
SERVER_PID=$!

# Give the server some time to start
sleep 1

# Execute each test script and collect results
for TEST_SCRIPT in ${SCRIPT_DIR}/integration/*_test.sh; do
  ((TOTAL_TESTS++))
  # Check if the web server is running before executing the test
  check_server_running

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

