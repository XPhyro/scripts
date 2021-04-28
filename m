#!/usr/bin/env sh
# Mark directory.

logerrq() {
    printf "[ERROR]: %s\n" "$*"
    exit 1
}

[ "$#" -eq 0 ] && logerrq "No mark given, exiting."
[ "$#" -eq 1 ] || logerrq "Only one mark is accepted, exiting."

mark="$1"

printf "%s\n" "$mark" | grep -qs "\s" && logerrq "Mark cannot contain whitespace, exiting."

mrkfl="$(getfl mrk)"

if [ -n "$2" ]
then
    val="$(realpath "$2")"
else
    val="$PWD"
fi

grep -Eq "^$mark\s" "$mrkfl" && { 
    cm "$mark" "$val"
    exit 0
}

printf "%s %s\n" "$mark" "$val" >> "$mrkfl"
