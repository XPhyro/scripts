#!/usr/bin/env sh

if [ "$@" ]
then
    mdv "$@"
else
    for i in *.md
    do
        mdv "$i"
    done
fi
