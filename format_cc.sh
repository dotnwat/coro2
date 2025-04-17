#!/bin/bash

git ls-files --full-name "*.cc" "*.h" "*.proto" | xargs -n1 clang-format -i -style=file -fallback-style=none
