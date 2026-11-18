// Задание 4, вариант 1: все различные подмассивы массива.
//
// Ввод (stdin): элементы массива через пробелы. Вывод: [{}, {x}, {x, y}, ...].
package main

import (
	"fmt"
	"io"
	"os"
	"strings"

	"lab2/tasks"
)

func main() {
	data, err := io.ReadAll(os.Stdin)
	if err != nil {
		fmt.Fprintln(os.Stderr, "ERROR:", err)
		os.Exit(1)
	}
	subs, err := tasks.DistinctSubarrays(strings.Fields(string(data)))
	if err != nil {
		fmt.Fprintln(os.Stderr, "ERROR:", err)
		os.Exit(1)
	}
	fmt.Println(tasks.FormatSubarrays(subs))
}
