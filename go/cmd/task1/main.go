// Задание 1, вариант 6: обход деревьев и графов в глубину со стеком.
//
// Ввод (stdin): "n m start", затем m рёбер "u v" (вершины 1..n).
// Ключ --directed: рёбра ориентированные (по умолчанию неориентированные).
// Вывод: вершины в порядке обхода.
package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"strconv"
	"strings"

	"lab2/tasks"
)

func fail(msg string) {
	fmt.Fprintln(os.Stderr, "ERROR:", msg)
	os.Exit(1)
}

func main() {
	directed := flag.Bool("directed", false, "рёбра ориентированные")
	flag.Parse()

	in := bufio.NewReader(os.Stdin)
	var n, m, start int
	if _, err := fmt.Fscan(in, &n, &m, &start); err != nil || n < 1 || m < 0 {
		fail("expected: n m start, then m edges 'u v'")
	}
	edges := make([][2]int, m)
	for i := range edges {
		if _, err := fmt.Fscan(in, &edges[i][0], &edges[i][1]); err != nil {
			fail("not enough edges")
		}
	}

	order, err := tasks.DFS(n, edges, start, *directed)
	if err != nil {
		fail(err.Error())
	}
	parts := make([]string, len(order))
	for i, v := range order {
		parts[i] = strconv.Itoa(v)
	}
	fmt.Println(strings.Join(parts, " "))
}
