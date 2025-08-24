#!/usr/bin/env bash

SCRIPT_DIR=$(dirname "$0")
OUT_DIR=$SCRIPT_DIR/../../run/engine/shaders
OUT_VK=$OUT_DIR/vk

glslc $SCRIPT_DIR/grad.comp --target-env=vulkan -o $OUT_VK/grad.spv
