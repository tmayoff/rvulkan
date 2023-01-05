.PHONY: build run

build:
	meson compile -C build

run:
	$(MAKE) build
	./build/editor/editor

run-sandbox:
	$(MAKE) build
	./build/sandbox/Sandbox
