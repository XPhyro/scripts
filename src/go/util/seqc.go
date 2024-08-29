package main

import (
	"fmt"
	"os"
	"strconv"
)

func sign(x int) int {
	if x > 0 {
		return 1
	} else if x < 0 {
		return -1
	}
	return 0
}

func main() {
	args := os.Args[1:]

	if len(args) < 1 || len(args) > 3 {
		fmt.Println("Usage: seq LAST")
		fmt.Println("       seq FIRST LAST\n")
		fmt.Println("       seq FIRST INCREEMENT LAST")
		return
	}

	var first, increment, last int
	var err error

	switch len(args) {
	case 1:
		first = 1
		increment = 1
		last, err = strconv.Atoi(args[0])
		break
	case 2:
		first, err = strconv.Atoi(args[0])
		increment = 1
		last, err = strconv.Atoi(args[1])
		break
	case 3:
		first, err = strconv.Atoi(args[0])
		increment, err = strconv.Atoi(args[1])
		last, err = strconv.Atoi(args[2])
		break
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
