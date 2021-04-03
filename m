#!/usr/bin/env sh
# Mark directory.

mark="$1"

mrkfl="$( getfl mrk )"

if [ "$2" ]
then
    val="$( realpath "$2" )"
else
    val="$PWD"
fi

grep -Eq "$mark\s" "$mrkfl" && { 
    cm "$mark" "$val"
    exit 0
}

printf "%s %s\n" "$mark" "$val" >> "$mrkfl"
