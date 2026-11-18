package ds

type snode[T any] struct {
	value T
	next  *snode[T]
}

// SinglyList — односвязный список (с указателем на хвост).
type SinglyList[T any] struct {
	head, tail *snode[T]
	count      int
}

// PushBack добавляет элемент в конец списка за O(1).
func (l *SinglyList[T]) PushBack(v T) {
	n := &snode[T]{value: v}
	if l.head == nil {
		l.head = n
	} else {
		l.tail.next = n
	}
	l.tail = n
	l.count++
}

// PushFront добавляет элемент в начало списка за O(1).
func (l *SinglyList[T]) PushFront(v T) {
	n := &snode[T]{value: v, next: l.head}
	l.head = n
	if l.tail == nil {
		l.tail = n
	}
	l.count++
}

// Find возвращает указатель на первый элемент, для которого match вернул true.
// Через указатель значение можно изменить на месте.
func (l *SinglyList[T]) Find(match func(*T) bool) *T {
	for n := l.head; n != nil; n = n.next {
		if match(&n.value) {
			return &n.value
		}
	}
	return nil
}

// RemoveFirst удаляет первый подходящий элемент; возвращает true, если он найден.
func (l *SinglyList[T]) RemoveFirst(match func(*T) bool) bool {
	var prev *snode[T]
	for n := l.head; n != nil; prev, n = n, n.next {
		if !match(&n.value) {
			continue
		}
		if prev == nil {
			l.head = n.next
		} else {
			prev.next = n.next
		}
		if n == l.tail {
			l.tail = prev
		}
		l.count--
		return true
	}
	return false
}

// Each вызывает f для каждого элемента по порядку.
func (l *SinglyList[T]) Each(f func(*T)) {
	for n := l.head; n != nil; n = n.next {
		f(&n.value)
	}
}

// Len возвращает количество элементов.
func (l *SinglyList[T]) Len() int {
	return l.count
}
