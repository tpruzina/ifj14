TEST_SCRIPT="../scripts/testProject.sh"
TEST_DIR="./tests"
BIN="../src/ifj14"

ALL: all
all: test

test:
	$(TEST_SCRIPT) $(BIN) $(TEST_DIR)
clean:
	$(TEST_SCRIPT) -d $(TEST_DIR)
	rm -rf ./vysledkyTestu.txt
