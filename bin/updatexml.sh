#!/bin/bash
ASSETS_BASE_URL="http://kg.tyrantonline.com/assets/"

for file in cards.xml raids.xml missions.xml quests.xml achievements.xml; do
    LINE="wget ${ASSETS_BASE_URL}${file} -O ${file}"
    echo ${LINE}
    ${LINE}
done
