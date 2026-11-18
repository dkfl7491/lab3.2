// Задание 6: хеш-таблица (вариант 1: линейное апробирование и двойное
// хеширование), подзадание 6.2 — вариант 6: изоморфные строки.
//
//	./task6                      две строки на stdin (по одной в строке)
//	./task6 --table double       то же, но на таблице с двойным хешированием
//	./task6 --bench              эмпирический анализ методов (подзадание 6.1)
package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"strings"

	"lab2/tasks"
)

func main() {
	table := flag.String("table", "linear", "реализация хеш-таблицы: linear | double")
	bench := flag.Bool("bench", false, "запустить эмпирический анализ хеширования")
	flag.Parse()

	if *bench {
		tasks.RunHashBenchmark(os.Stdout)
		return
	}

	factory := tasks.NewLinearMap
	switch *table {
	case "linear":
	case "double":
		factory = tasks.NewDoubleMap
	default:
		fmt.Fprintln(os.Stderr, "ERROR: --table must be linear or double")
		os.Exit(1)
	}

	in := bufio.NewReader(os.Stdin)
	var lines [2]string
	for i := range lines {
		line, err := in.ReadString('\n')
		if err != nil && line == "" && i == 0 {
			fmt.Fprintln(os.Stderr, "ERROR: expected two lines")
			os.Exit(1)
		}
		lines[i] = strings.TrimRight(line, "\r\n")
	}

	if tasks.Isomorphic(lines[0], lines[1], factory) {
		fmt.Println("Изоморфны")
	} else {
		fmt.Println("Не изоморфны")
	}
}
