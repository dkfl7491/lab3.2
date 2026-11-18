package ds

// BSTNode — узел бинарного дерева поиска (устроен так же, как узел
// дерева из ЛР1: значение + левый и правый потомок).
type BSTNode struct {
	Value       int
	Left, Right *BSTNode
}

// BST — бинарное дерево поиска для целых чисел без балансировки.
type BST struct {
	root *BSTNode
}

// Insert добавляет значение; возвращает false, если оно уже есть в дереве.
func (t *BST) Insert(v int) bool {
	if t.root == nil {
		t.root = &BSTNode{Value: v}
		return true
	}
	cur := t.root
	for {
		switch {
		case v == cur.Value:
			return false
		case v < cur.Value:
			if cur.Left == nil {
				cur.Left = &BSTNode{Value: v}
				return true
			}
			cur = cur.Left
		default:
			if cur.Right == nil {
				cur.Right = &BSTNode{Value: v}
				return true
			}
			cur = cur.Right
		}
	}
}

// InOrder обходит дерево в симметричном порядке (по возрастанию) без
// рекурсии: явный стек Stack из ЛР1 не даёт переполнить стек вызовов
// на вырожденном дереве. Для каждого узла вызывается visit.
func (t *BST) InOrder(visit func(*BSTNode)) {
	var st Stack[*BSTNode]
	cur := t.root
	for cur != nil || !st.Empty() {
		for cur != nil {
			st.Push(cur)
			cur = cur.Left
		}
		n, _ := st.Pop()
		visit(n)
		cur = n.Right
	}
}
