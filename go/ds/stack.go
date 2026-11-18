package ds

import "errors"

// ErrEmptyStack — операция над пустым стеком.
var ErrEmptyStack = errors.New("Stack is empty")

// Stack — АТД «Стек» (LIFO).
type Stack[T any] struct {
	data []T
}

// Push кладёт элемент на вершину стека.
func (s *Stack[T]) Push(v T) {
	s.data = append(s.data, v)
}

// Pop снимает и возвращает верхний элемент.
func (s *Stack[T]) Pop() (T, error) {
	var zero T
	if len(s.data) == 0 {
		return zero, ErrEmptyStack
	}
	v := s.data[len(s.data)-1]
	s.data[len(s.data)-1] = zero
	s.data = s.data[:len(s.data)-1]
	return v, nil
}

// Peek возвращает верхний элемент, не снимая его.
func (s *Stack[T]) Peek() (T, error) {
	if len(s.data) == 0 {
		var zero T
		return zero, ErrEmptyStack
	}
	return s.data[len(s.data)-1], nil
}

// Len возвращает количество элементов.
func (s *Stack[T]) Len() int {
	return len(s.data)
}

// Empty сообщает, пуст ли стек.
func (s *Stack[T]) Empty() bool {
	return len(s.data) == 0
}

// Values возвращает элементы от дна к вершине.
func (s *Stack[T]) Values() []T {
	return append([]T(nil), s.data...)
}
