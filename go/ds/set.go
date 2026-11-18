package ds

import "sort"

// Set — АТД «Множество» (задание 2). Построено на хеш-таблице ChainTable,
// поэтому добавление, удаление и проверка наличия выполняются за O(1)
// в среднем.
type Set struct {
	t *ChainTable[struct{}]
}

// NewSet создаёт пустое множество.
func NewSet() *Set {
	return &Set{t: NewChainTable[struct{}](16)}
}

// Add добавляет элемент; возвращает true, если его не было.
func (s *Set) Add(v string) bool {
	if s.Has(v) {
		return false
	}
	s.t.Put(v, struct{}{})
	return true
}

// Delete удаляет элемент; возвращает true, если он был во множестве.
func (s *Set) Delete(v string) bool {
	return s.t.Delete(v)
}

// Has проверяет, входит ли элемент во множество.
func (s *Set) Has(v string) bool {
	_, ok := s.t.Get(v)
	return ok
}

// Len возвращает количество элементов.
func (s *Set) Len() int {
	return s.t.Len()
}

// Values возвращает элементы в лексикографическом порядке.
func (s *Set) Values() []string {
	out := make([]string, 0, s.t.Len())
	s.t.Each(func(k string, _ struct{}) { out = append(out, k) })
	sort.Strings(out)
	return out
}
