package main

import (
	"fmt"
	"os"
	"strconv"
)

func main() {
	if len(os.Args) < 2 {
		os.Exit(1)
	}

	for _, arg := range os.Args[1:] {
		asciiCode, err := strconv.Atoi(arg)
		if err != nil {
			fmt.Fprintf(os.Stderr, "%s: invalid numeric code [%s]\n", os.Args[0], err)
			os.Exit(1)
		}

		if asciiCode < 0 || asciiCode > 255 {
			fmt.Fprintf(os.Stderr, "%s: invalid ASCII code [%d]\n", os.Args[0], asciiCode)
			os.Exit(2)
		}

		fmt.Printf("%c", asciiCode)
	}
}
