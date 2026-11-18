// Пакет tasks — решения заданий лабораторной работы №2.
package tasks

import (
	"errors"

	"lab2/ds"
)

// DFS (задание 1, вариант 6) — обход графа или дерева в глубину с помощью
// стека. Вершины нумеруются с 1, edges — список рёбер (u, v). Если
// directed == false, каждое ребро считается двунаправленным.
//
// Стек хранит вершины, ожидающие обхода. Из стека извлекается вершина,
// и если она ещё не посещена — помечается, выводится, а её соседи кладутся
// в стек в обратном порядке, чтобы первым обработался первый сосед. Порядок
// обхода совпадает с рекурсивным DFS. Сложность O(n + m).
func DFS(n int, edges [][2]int, start int, directed bool) ([]int, error) {
	if start < 1 || start > n {
		return nil, errors.New("Start vertex is out of range")
	}

	adj := make([]ds.Array[int], n+1)
	for _, e := range edges {
		u, v := e[0], e[1]
		if u < 1 || u > n || v < 1 || v > n {
			return nil, errors.New("Edge vertex is out of range")
		}
		adj[u].Push(v)
		if !directed {
			adj[v].Push(u)
		}
	}

	visited := make([]bool, n+1)
	var order []int
	var stack ds.Stack[int]
	stack.Push(start)

	for !stack.Empty() {
		v, _ := stack.Pop()
		if visited[v] {
			continue
		}
		visited[v] = true
		order = append(order, v)

		neighbours := adj[v].Values()
		for i := len(neighbours) - 1; i >= 0; i-- {
			if !visited[neighbours[i]] {
				stack.Push(neighbours[i])
			}
		}
	}
	return order, nil
}
