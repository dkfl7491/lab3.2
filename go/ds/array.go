// Пакет ds — структуры данных из лабораторной работы №1 (порт с C++ на Go)
// и структуры, построенные на их основе в лабораторной работе №2.
package ds

import "errors"

// ErrIndex — индекс вне границ массива.
var ErrIndex = errors.New("Invalid array index")

// Array — динамический массив (АТД «Массив»).
type Array[T any] struct {
	data []T
}

// Push добавляет элемент в конец массива.
func (a *Array[T]) Push(v T) {
	a.data = append(a.data, v)
}

// Insert вставляет элемент по индексу (0 <= i <= Len).
func (a *Array[T]) Insert(i int, v T) error {
	if i < 0 || i > len(a.data) {
		return ErrIndex
	}
	var zero T
	a.data = append(a.data, zero)
	copy(a.data[i+1:], a.data[i:])
	a.data[i] = v
	return nil
}

// Get возвращает элемент по индексу.
func (a *Array[T]) Get(i int) (T, error) {
	if i < 0 || i >= len(a.data) {
		var zero T
		return zero, ErrIndex
	}
	return a.data[i], nil
}

// Set заменяет элемент по индексу.
func (a *Array[T]) Set(i int, v T) error {
	if i < 0 || i >= len(a.data) {
		return ErrIndex
	}
	a.data[i] = v
	return nil
}

// Remove удаляет элемент по индексу.
func (a *Array[T]) Remove(i int) error {
	if i < 0 || i >= len(a.data) {
		return ErrIndex
	}
	copy(a.data[i:], a.data[i+1:])
	var zero T
	a.data[len(a.data)-1] = zero
	a.data = a.data[:len(a.data)-1]
	return nil
}

// Len возвращает количество элементов.
func (a *Array[T]) Len() int {
	return len(a.data)
}

// Values возвращает копию всех элементов.
func (a *Array[T]) Values() []T {
	return append([]T(nil), a.data...)
}
