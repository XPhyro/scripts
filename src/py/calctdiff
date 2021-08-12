#!/usr/bin/env python3


import re
import sys
import argparse
import datetime as dt
import input_helper as ih


parser = argparse.ArgumentParser()
parser.add_argument("-e", "--exit-on-error", action="store_true")
args = parser.parse_args()


def getdate(msg):
    while True:
        s = re.sub("\s|[-/_|,.]", "", input(msg))

        l = len(s)
        if l != 4:
            if l != 8:
                if args.exit_on_error:
                    sys.exit(1)
                print("Input could not be understood.")
                continue
            else:
                try:
                    year = int(s[0:4])
                    month = int(s[4:6])
                    day = int(s[6:8])

                    return dt.datetime(year, month, day)
                except:
                    if args.exit_on_error:
                        sys.exit(1)
                    print("You must enter a valid date.")
                    continue
        else:
            try:
                year = 2020
                month = int(s[0:2])
                day = int(s[0:4])

                return dt.datetime(year, month, day)
            except:
                if args.exit_on_error:
                    sys.exit(1)
                print("You must enter a valid date.")
                continue


now = dt.datetime.now()
date = getdate("Enter date: ")

print(date - now)
