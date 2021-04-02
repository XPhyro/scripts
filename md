#!/usr/bin/env sh

if [ "$#" -ne 0 ]
then
    mdv "$@"
else
    for i in *.md
    do
        mdv "$i"
    done
fi
