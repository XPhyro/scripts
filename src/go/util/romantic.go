package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"
)

var romanDigits = map[rune]int{
	'I': 1,
	'V': 5,
	'X': 10,
	'L': 50,
	'C': 100,
	'D': 500,
	'M': 1000,
}

var romanBlocks = []struct {
	value int
	char  string
}{
	{1000, "M"},
	{900, "CM"},
	{500, "D"},
	{400, "CD"},
	{100, "C"},
	{90, "XC"},
	{50, "L"},
	{40, "XL"},
	{10, "X"},
	{9, "IX"},
	{5, "V"},
	{4, "IV"},
	{1, "I"},
}

var underscoreMultiplier = 1000

type NumeralType int

const (
	Invalid NumeralType = iota
	Roman
	Arabic
)

func main() {
	args := os.Args[1:]

	if len(args) == 0 {
		return
	}

	for _, arg := range args {
		uppercaseArg := strings.ToUpper(arg)

		numeralType := peekNumeralType(uppercaseArg)

		var converted string

		switch numeralType {
		case Roman:
			converted = convertRomanToArabic(uppercaseArg)
		case Arabic:
			converted = convertArabicToRoman(uppercaseArg)
		case Invalid:
			fmt.Fprintln(os.Stderr, "invalid numeral given:", arg)
			os.Exit(1)
		}

		fmt.Println(converted)
	}
}

func peekNumeralType(numeral string) NumeralType {
	for _, char := range numeral {
		if _, ok := romanDigits[char]; ok {
			return Roman
		}

		if char == '_' {
			return Roman
		}

		if '0' <= char && char <= '9' {
			return Arabic
		}
	}

	return Invalid
}

func convertRomanToArabic(roman string) string {
	var total int
	var prev int

	runes := []rune(roman)

	for i := len(runes) - 1; i >= 0; i-- {
		value, ok := romanDigits[runes[i]]
		if !ok {
			fmt.Fprintf(os.Stderr, "invalid Roman digit [%c] in [%s]", runes[i], roman)
			os.Exit(1)
		}

		if i >= 1 && runes[i-1] == '_' {
			value *= underscoreMultiplier
			i--
		}

		if value < prev {
			total -= value
		} else {
			total += value
		}

		prev = value
	}

	arabic := strconv.Itoa(total)

	if convertArabicToRoman(arabic) != roman {
		fmt.Fprintln(os.Stderr, "invalid Roman numeral:", roman)
		os.Exit(1)
	}

	return arabic
}

func convertArabicToRoman(arabic string) string {
	number, err := strconv.Atoi(arabic)
	if err != nil || number <= 0 {
		fmt.Fprintln(os.Stderr, "invalid Arabic numeral:", arabic)
		os.Exit(1)
	}

	var result strings.Builder

	// TODO: support proper underscore notation. currently we're
	//       outputting invalid roman numerals when arabic>3999.
	for _, numeral := range romanBlocks {
		for number >= numeral.value {
			result.WriteString(numeral.char)
			number -= numeral.value
		}
	}

	return result.String()
}
