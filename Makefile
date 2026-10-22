# Convenience commands only. CMake owns all compilation and packaging rules.
.DEFAULT_GOAL := docker
DOCKER ?= docker
DOCKER_IMAGE ?= deltalaunch-devkitpro
DEVKITPRO_IMAGE ?= devkitpro/devkita64:latest
JOBS ?= 4
BUILD ?= build
BUILD_TYPE ?= $(if $(filter debug,$(BUILD)),Debug,Release)
SWITCH_BUILD ?= build-switch$(if $(filter debug,$(BUILD)),-debug,)
# On SELinux hosts, use DOCKER_RUN_FLAGS="--security-opt label=disable".
DOCKER_RUN_FLAGS ?=
LINUX_BUILD ?= build-linux
LINUX_DOCKER_IMAGE ?= deltalaunch-linux
LINUX_DOCKER_BUILD ?= build-linux-docker

.PHONY: all switch native docker docker-image docker-shell docker-clean clean linux linux-test linux-run linux-docker
all: docker

switch:
	cmake -S . -B $(SWITCH_BUILD) -DCMAKE_TOOLCHAIN_FILE="$(DEVKITPRO)/cmake/Switch.cmake" -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	cmake --build $(SWITCH_BUILD) -j$(JOBS)

native: switch

docker-image:
	$(DOCKER) build --pull --build-arg DEVKITPRO_IMAGE=$(DEVKITPRO_IMAGE) -t $(DOCKER_IMAGE) .

docker: docker-image
	$(DOCKER) run --rm $(DOCKER_RUN_FLAGS) --user "$$(id -u):$$(id -g)" -e HOME=/tmp --mount 'type=bind,source=$(CURDIR),target=/work' -w /work $(DOCKER_IMAGE) make switch JOBS=$(JOBS) BUILD_TYPE=$(BUILD_TYPE) SWITCH_BUILD=$(SWITCH_BUILD)

docker-shell: docker-image
	$(DOCKER) run --rm $(DOCKER_RUN_FLAGS) -it --user "$$(id -u):$$(id -g)" -e HOME=/tmp --mount 'type=bind,source=$(CURDIR),target=/work' -w /work $(DOCKER_IMAGE) bash

docker-clean: docker-image
	$(DOCKER) run --rm $(DOCKER_RUN_FLAGS) --user "$$(id -u):$$(id -g)" --mount 'type=bind,source=$(CURDIR),target=/work' -w /work $(DOCKER_IMAGE) make clean

linux:
	cmake -S . -B $(LINUX_BUILD) -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(LINUX_BUILD) -j$(JOBS)

linux-test: linux
	ctest --test-dir $(LINUX_BUILD) --output-on-failure

linux-run: linux
	./$(LINUX_BUILD)/deltalaunch-linux

linux-docker:
	$(DOCKER) build -f Dockerfile.linux -t $(LINUX_DOCKER_IMAGE) .
	$(DOCKER) run --rm $(DOCKER_RUN_FLAGS) --user "$$(id -u):$$(id -g)" -e HOME=/tmp --mount 'type=bind,source=$(CURDIR),target=/work' -w /work $(LINUX_DOCKER_IMAGE) make linux-test JOBS=$(JOBS) LINUX_BUILD=$(LINUX_DOCKER_BUILD)

clean:
	rm -rf build debug build-switch build-switch-debug build-linux build-linux-docker dist deltalaunch.elf deltalaunch.nro deltalaunch.nso deltalaunch.npdm deltalaunch.nacp deltalaunch.map deltalaunch.lst
