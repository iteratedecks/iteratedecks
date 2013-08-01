#!/bin/bash

# Creates VERSION.h. To be used only if you don't use makepp to build
# the project.

# You may change the output location of the file. The path is relative to the
# root of the project.

OUTPUT="builddir/VERSION.h"

cd ..

./createVersionH.sh > ${OUTPUT}
