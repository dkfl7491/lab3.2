// Задание 5, вариант 6: листья бинарного дерева поиска.
//
// Ввод: целые числа, оканчивающиеся нулём (0 в дерево не входит).
// Вывод: листья по возрастанию.
package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"

	"lab2/tasks"
)

func main() {
	in := bufio.NewReader(os.Stdin)
	var seq []int
	for {
		var v int
		if _, err := fmt.Fscan(in, &v); err != nil || v == 0 {
			break
		}
		seq = append(seq, v)
	}
	leaves := tasks.Leaves(seq)
	parts := make([]string, len(leaves))
	for i, v := range leaves {
		parts[i] = strconv.Itoa(v)
	}
	fmt.Println(strings.Join(parts, " "))
}
