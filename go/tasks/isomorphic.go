package tasks

import "lab2/ds"

// StringMap — то, что нужно задаче от хеш-таблицы. Ему удовлетворяют обе
// реализации из задания 6.1: линейное апробирование и двойное хеширование.
type StringMap interface {
	Put(key, val string)
	Get(key string) (string, bool)
}

// NewLinearMap и NewDoubleMap — фабрики таблиц для Isomorphic.
func NewLinearMap() StringMap { return ds.NewLinearTable[string](16) }
func NewDoubleMap() StringMap { return ds.NewDoubleHashTable[string](16) }

// Isomorphic (задание 6.2, вариант 6) — проверяет, изоморфны ли строки:
// символы a можно заменить символами b, причём соответствие взаимно
// однозначно, а порядок сохраняется. Используются две хеш-таблицы
// (прямое и обратное соответствие). Сложность O(n) в среднем.
func Isomorphic(a, b string, newMap func() StringMap) bool {
	ra, rb := []rune(a), []rune(b)
	if len(ra) != len(rb) {
		return false
	}
	forward, backward := newMap(), newMap()
	for i := range ra {
		x, y := string(ra[i]), string(rb[i])
		if m, ok := forward.Get(x); ok {
			if m != y {
				return false
			}
		} else {
			forward.Put(x, y)
		}
		if m, ok := backward.Get(y); ok {
			if m != x {
				return false
			}
		} else {
			backward.Put(y, x)
		}
	}
	return true
}
