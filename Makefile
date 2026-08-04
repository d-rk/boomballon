# Boom Balloon — developer shortcuts. Run `make` (or `make help`) for the list.
#
# Overridable variables, e.g. `make upload PORT=/dev/ttyUSB0`:
#   PIO     PlatformIO executable            (default: pio)
#   ENV     firmware PlatformIO environment  (default: nanoatmega328)
#   PORT    serial port for upload/monitor   (default: auto-detect)
#   MKDOCS  mkdocs executable                (default: .venv-docs/bin/mkdocs)

PIO    ?= pio
ENV    ?= nanoatmega328
PORT   ?=
MKDOCS ?= .venv-docs/bin/mkdocs

# firmware/ is the PlatformIO project; -d keeps recipes out of `cd`.
PIO_RUN      := $(PIO) run -d firmware
PORT_ARG      = $(if $(PORT),--upload-port $(PORT))
MON_PORT_ARG  = $(if $(PORT),--port $(PORT))

.DEFAULT_GOAL := help

.PHONY: help
help: ## Show this help
	@grep -hE '^[a-zA-Z0-9_-]+:.*?## ' $(MAKEFILE_LIST) \
	  | awk 'BEGIN{FS=":.*?## "}{printf "  \033[36m%-13s\033[0m %s\n", $$1, $$2}'

# --- Firmware ---------------------------------------------------------------

.PHONY: build
build: ## Build the firmware for the Nano (override ENV= for another board)
	$(PIO_RUN) -e $(ENV)

.PHONY: build-all
build-all: ## Build every firmware environment (Nano + Micro)
	$(PIO_RUN)

.PHONY: upload
upload: ## Build + flash the Nano (set PORT= to force the serial port)
	$(PIO_RUN) -t upload -e $(ENV) $(PORT_ARG)

.PHONY: mock
mock: ## Build + flash a full no-hardware mock build (both devices + AUTOSTART_GAME)
	PLATFORMIO_BUILD_FLAGS="-D MOCK_CODE_DETECTOR -D MOCK_OUTPUT_DEVICE -D AUTOSTART_GAME" \
	  $(PIO_RUN) -t upload -e $(ENV) $(PORT_ARG)

.PHONY: mock-code
mock-code: ## Build + flash with only the card reader mocked (real pump/valve)
	PLATFORMIO_BUILD_FLAGS="-D MOCK_CODE_DETECTOR" \
	  $(PIO_RUN) -t upload -e $(ENV) $(PORT_ARG)

.PHONY: mock-output
mock-output: ## Build + flash with only the pump/valve mocked (real card reader)
	PLATFORMIO_BUILD_FLAGS="-D MOCK_OUTPUT_DEVICE" \
	  $(PIO_RUN) -t upload -e $(ENV) $(PORT_ARG)

.PHONY: monitor
monitor: ## Open the serial console (9600 baud)
	$(PIO) device monitor -d firmware -e $(ENV) $(MON_PORT_ARG)

# --- Docs -------------------------------------------------------------------

.PHONY: docs
docs: ## Build the documentation site
	$(MKDOCS) build -f docs/mkdocs.yml

.PHONY: docs-serve
docs-serve: ## Live-preview the docs at http://localhost:8000
	$(MKDOCS) serve -f docs/mkdocs.yml

.PHONY: docs-setup
docs-setup: ## Create the docs virtualenv (.venv-docs) and install requirements
	python3 -m venv .venv-docs
	.venv-docs/bin/pip install -r docs/requirements.txt

# --- Media & housekeeping ---------------------------------------------------

.PHONY: media
media: ## Regenerate PNG assets from the SVG sources (needs inkscape + imagemagick)
	media/generate-pngs.sh

.PHONY: clean
clean: ## Remove firmware and docs build artifacts
	rm -rf firmware/.pio docs/site
