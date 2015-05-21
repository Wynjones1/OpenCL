GENERATOR  := "Unix Makefiles"
BUILD_DIR  := ./build
CMAKE_ROOT := $(shell pwd)
all:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -G$(GENERATOR) $(CMAKE_ROOT)
	cd $(BUILD_DIR) && make

eclipse: GENERATOR := "Eclipse CDT4 - Unix Makefiles"
eclipse: BUILD_DIR := ../build
eclipse: all

run: all
	$(BUILD_DIR)/src/out

clean:
	rm -Rf $(BUILD_DIR)
