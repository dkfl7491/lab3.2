package ds

// DNode — узел двусвязного списка. Наружу отдаётся «ручка» узла,
// чтобы удалять его за O(1), не разыскивая по значению.
type DNode[T any] struct {
	Value T
	prev  *DNode[T]
	next  *DNode[T]
}

// DoublyList — двусвязный список.
type DoublyList[T any] struct {
	head, tail *DNode[T]
	count      int
}

// PushBack добавляет элемент в конец и возвращает его узел.
func (l *DoublyList[T]) PushBack(v T) *DNode[T] {
	n := &DNode[T]{Value: v, prev: l.tail}
	if l.tail != nil {
		l.tail.next = n
	} else {
		l.head = n
	}
	l.tail = n
	l.count++
	return n
}

// PushFront добавляет элемент в начало и возвращает его узел.
func (l *DoublyList[T]) PushFront(v T) *DNode[T] {
	n := &DNode[T]{Value: v, next: l.head}
	if l.head != nil {
		l.head.prev = n
	} else {
		l.tail = n
	}
	l.head = n
	l.count++
	return n
}

// Front возвращает первый узел (nil, если список пуст).
func (l *DoublyList[T]) Front() *DNode[T] {
	return l.head
}

// Remove удаляет узел n, принадлежащий этому списку, за O(1).
func (l *DoublyList[T]) Remove(n *DNode[T]) {
	if n.prev != nil {
		n.prev.next = n.next
	} else {
		l.head = n.next
	}
	if n.next != nil {
		n.next.prev = n.prev
	} else {
		l.tail = n.prev
	}
	n.prev, n.next = nil, nil
	l.count--
}

// MoveToBack переносит узел n этого списка в конец за O(1).
func (l *DoublyList[T]) MoveToBack(n *DNode[T]) {
	if l.tail == n {
		return
	}
	l.Remove(n)
	n.prev = l.tail
	if l.tail != nil {
		l.tail.next = n
	} else {
		l.head = n
	}
	l.tail = n
	l.count++
}

// Len возвращает количество элементов.
func (l *DoublyList[T]) Len() int {
	return l.count
}
