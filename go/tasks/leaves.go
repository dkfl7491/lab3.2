package tasks

import "lab2/ds"

// Leaves (задание 5, вариант 6) — строит бинарное дерево поиска по
// последовательности (повторы игнорируются) и возвращает все листья
// (вершины без потомков) по возрастанию.
//
// Вставка — O(h) на элемент, симметричный обход со стеком даёт листья
// сразу в отсортированном порядке за O(n).
func Leaves(seq []int) []int {
	var tree ds.BST
	for _, v := range seq {
		tree.Insert(v)
	}
	var leaves []int
	tree.InOrder(func(n *ds.BSTNode) {
		if n.Left == nil && n.Right == nil {
			leaves = append(leaves, n.Value)
		}
	})
	return leaves
}
