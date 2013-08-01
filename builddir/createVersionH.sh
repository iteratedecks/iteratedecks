#!/usr/bin/env bash
echo -n "#define ITERATEDECKS_VERSION \""
git rev-parse HEAD | tr --delete "\n"
echo  "\""
echo -n "#define ITERATEDECKS_VERSION_TAGS \""
git tag --points-at HEAD | tr "\n" " "
echo  "\""
echo -n "#define ITERATEDECKS_VERSION_DESCRIBE \""
git describe HEAD | tr --delete "\n"
echo "\""
#
echo -n "#define ITERATEDECKS_DIRTY_HEAD "
if [ `git diff HEAD | wc --chars` -gt 0 ]; then 
    echo true
else \
    echo false
fi
echo -n "#define ITERATEDECKS_DIRTY_HASH \""
git diff HEAD | sha1sum | cut -d " " -f 1 | tr -d "\n"
echo  "\""
#
echo -n "#define ITERATEDECKS_DIRTY_HEAD_CORE "
if [ `git diff HEAD -- ../../src/CORE | wc --chars` -gt 0 ]; then \
    echo true
else \
    echo false
fi
echo -n "#define ITERATEDECKS_DIRTY_HASH_CORE \""
git diff HEAD -- ../../src/CORE | sha1sum | cut -d " " -f 1 | tr -d "\n"
echo  "\""
