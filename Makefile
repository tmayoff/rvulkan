.PHONY: build run

build:
	meson compile -C build

run:
	$(MAKE) build
	./build/sandbox/Sandbox
