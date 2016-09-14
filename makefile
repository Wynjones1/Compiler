BUILD_DIR := build

all: $(BUILD_DIR)/Makefile
	make -C $(BUILD_DIR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/Makefile: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=Debug

run: $(BUILD_DIR)/Makefile
	make -C $(BUILD_DIR) run

test: $(BUILD_DIR)/Makefile
	make -C $(BUILD_DIR) test

dbg: $(BUILD_DIR)/Makefile
	make -C $(BUILD_DIR) dbg

clean:
	rm -Rf $(BUILD_DIR)
