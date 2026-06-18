#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

IMAGE_NAME="cpp-design-talk"

docker run --rm -it \
    -v "${PROJECT_ROOT}:/workspace" \
    "${IMAGE_NAME}" \
    fish
