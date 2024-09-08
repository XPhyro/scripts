package main

import (
	"fmt"
	"os"
	"strconv"
	"unicode"
)

func sign(x int) int {
	if x > 0 {
		return 1
	} else if x < 0 {
		return -1
	}
	return 0
}

func charToInt(s string) (int, error) {
	if len(s) == 1 && unicode.IsLetter(rune(s[0])) {
		return int(s[0]), nil
	}
	return strconv.Atoi(s)
}

func main() {
	args := os.Args[1:]

	if len(args) < 1 || len(args) > 3 {
		fmt.Println("Usage: seq LAST")
		fmt.Println("       seq FIRST LAST")
		fmt.Println("       seq FIRST INCREMENT LAST")
		return
	}

	var first, increment, last int
	var err error

	switch len(args) {
	case 1:
		first = 1
		increment = 1
		last, err = charToInt(args[0])
	case 2:
		first, err = charToInt(args[0])
		increment = 1
		last, err = charToInt(args[1])
	case 3:
		first, err = charToInt(args[0])
		increment, err = strconv.Atoi(args[1])
		last, err = charToInt(args[2])
	}

	if err != nil {
		fmt.Println("Error:", err)
		os.Exit(1)
	}

	if increment == 0 {
		fmt.Println("Step cannot be zero")
		os.Exit(1)
	}

	deltaSign := sign(last - first)
	stepSign := sign(increment)

	if deltaSign == 0 || deltaSign != stepSign {
		return
	}

	for i := first; 0 <= i && (deltaSign == 1 && i <= last || deltaSign == -1 && i >= last) && i <= 255; i += increment {
		fmt.Printf("%c\n", i)
	}
}
