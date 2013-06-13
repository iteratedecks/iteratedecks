#!/bin/bash

# Creates VERSION.h. To be used only if you don't use makepp to build
# the project.

# You may change the output location of the file. The path is relative to the
# root of the project.

OUTPUT="builddir/VERSION.h"

cd ..

echo -n "#define ITERATEDECKS_VERSION \"" > $OUTPUT
git rev-parse HEAD | tr -d "\n" >> $OUTPUT
echo  "\"" >> $OUTPUT

echo -n "#define ITERATEDECKS_VERSION_TAGS \"" >> $OUTPUT
git tag --points-at HEAD | tr "\n" " " >> $OUTPUT
echo  "\"" >> $OUTPUT

echo -n "#define ITERATEDECKS_DIRTY_HEAD " >> $OUTPUT
if [ `git diff HEAD | wc --chars` -gt 0 ]; then \
    echo true >> $OUTPUT; \
else \
    echo false >> $OUTPUT; \
fi

echo -n "#define ITERATEDECKS_DIRTY_HASH \"" >> $OUTPUT
git diff HEAD | sha1sum | cut -d " " -f 1 | tr -d "\n" >> $OUTPUT
echo  "\"" >> $OUTPUT

echo -n "#define ITERATEDECKS_DIRTY_HEAD_CORE " >> $OUTPUT
if [ `git diff HEAD -- src/CORE | wc --chars` -gt 0 ]; then \
    echo true >> $OUTPUT; \
else \
    echo false >> $OUTPUT; \
fi

echo -n "#define ITERATEDECKS_DIRTY_HASH_CORE \"" >> $OUTPUT
git diff HEAD -- src/CORE | sha1sum | cut -d " " -f 1 | tr -d "\n" >> $OUTPUT
echo  "\"" >> $OUTPUT
