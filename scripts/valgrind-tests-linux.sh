#!/bin/sh
cd build/bin || echo "Error: Please run this script from the project's root directory as ./scripts/valgrind-linux.sh"

echo "Started valgrind..."
valgrind --num-callers=50 \
	--leak-resolution=high \
	--leak-check=full \
    --show-leak-kinds=all \
	--track-origins=yes \
	--time-stamp=yes \
	./odbc_tests 2>&1 | tee ../../odbc_tests.grind
cd .. && cd ..
