all: build/karg

clean:
	@rm -rf build

fmt:
	@clang-format -i $(shell find -not -path "./build/*" -name "*.[ch]")

.PHONY: docs
docs:
	@doxygen

build:
	@mkdir build
	@cmake \
		-B build \
		-D CMAKE_TOOLCHAIN_FILE=$(CMAKE_TOOLCHAIN_FILE) \
		-D PLATFORM=$(PLATFORM)

FORCE:

build/karg: FORCE | build
	@cmake --build build
