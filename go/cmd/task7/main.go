// Задание 7, вариант 1: LRU-кэш.
//
// Ввод: "cap Q", затем Q запросов "SET x y" / "GET x" (можно в одну строку
// или по одному в строке). Вывод: результаты GET через пробел.
package main

import (
	"fmt"
	"os"
	"strings"

	"lab2/tasks"
)

func main() {
	out, err := tasks.RunLRU(os.Stdin)
	if err != nil {
		fmt.Fprintln(os.Stderr, "ERROR:", err)
		os.Exit(1)
	}
	fmt.Println(strings.Join(out, " "))
}
