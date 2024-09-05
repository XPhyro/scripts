package main

import (
	"flag"
	"fmt"
	"math/rand"
	"strings"
	"unicode"
)

func toAlternatingCase(input string) string {
	var result strings.Builder
	for i, r := range input {
		if i%2 == 0 {
			result.WriteRune(unicode.ToUpper(r))
		} else {
			result.WriteRune(unicode.ToLower(r))
		}
	}
	return result.String()
}

func toCamelCase(input string) string {
	words := splitWords(input)
	for i := range words {
		if i > 0 {
			words[i] = strings.Title(words[i])
		} else {
			words[i] = strings.ToLower(words[i])
		}
	}
	return strings.Join(words, "")
}

func toPascalCase(input string) string {
	words := splitWords(input)
	for i := range words {
		words[i] = strings.Title(words[i])
	}
	return strings.Join(words, "")
}

func toRandomCase(input string) string {
	var result strings.Builder
	for _, r := range input {
		if rand.Intn(100) < 60 { // 50% doesn't feel random
			result.WriteRune(unicode.ToUpper(r))
		} else {
			result.WriteRune(unicode.ToLower(r))
		}
	}
	return result.String()
}

func toSnakeCase(input string) string {
	words := splitWords(input)
	for i := range words {
		words[i] = strings.ToLower(words[i])
	}
	return strings.Join(words, "_")
}

func splitWords(input string) []string {
	return strings.FieldsFunc(input, func(r rune) bool {
		return r == ' ' || r == '-' || r == '_' || r == '.'
	})
}

func main() {
	alternating := flag.Bool("alternating", false, "Convert to AlTeRnAtInG CaSe")
	camel := flag.Bool("camel", false, "Convert to camelCase")
	pascal := flag.Bool("pascal", false, "Convert to PascalCase")
	random := flag.Bool("random", false, "Convert to RaNDOm CAsE")
	snake := flag.Bool("snake", false, "Convert to snake_case")
	flag.Parse()

	if len(flag.Args()) == 0 {
		return
	}

	input := strings.Join(flag.Args(), " ")

	if *alternating {
		fmt.Println(toAlternatingCase(input))
	} else if *camel {
		fmt.Println(toCamelCase(input))
	} else if *pascal {
		fmt.Println(toPascalCase(input))
	} else if *random {
		fmt.Println(toRandomCase(input))
	} else if *snake {
		fmt.Println(toSnakeCase(input))
	} else {
		fmt.Println("Please specify a conversion type: --camel, --pascal, --snake, or --random")
	}
}
