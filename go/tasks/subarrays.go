package tasks

import (
	"errors"
	"strconv"
	"strings"

	"lab2/ds"
)

// MaxSubarrayElems — предел длины массива: результат содержит до 2^n подмассивов.
const MaxSubarrayElems = 24

// DistinctSubarrays (задание 4, вариант 1) — все различные подмассивы массива
// (как в примере из задания, это все наборы элементов с сохранением порядка,
// не обязательно идущих подряд: для {x, y, z} есть и {x, z}). Порядок вывода:
// сначала короче, при равной длине — по позициям элементов в исходном массиве.
// Совпадающие по содержимому подмассивы (при повторах в массиве) выводятся
// один раз — для этого используется множество Set из задания 2.
func DistinctSubarrays(elems []string) ([][]string, error) {
	n := len(elems)
	if n > MaxSubarrayElems {
		return nil, errors.New("Array is too long (more than 24 elements)")
	}

	seen := ds.NewSet()
	var result ds.Array[[]string]
	current := make([]string, 0, n)

	var generate func(start, remaining int)
	generate = func(start, remaining int) {
		if remaining == 0 {
			var key strings.Builder
			for _, e := range current {
				key.WriteString(strconv.Quote(e))
				key.WriteByte(',')
			}
			if seen.Add(key.String()) {
				result.Push(append([]string(nil), current...))
			}
			return
		}
		for i := start; i <= n-remaining; i++ {
			current = append(current, elems[i])
			generate(i+1, remaining-1)
			current = current[:len(current)-1]
		}
	}
	for size := 0; size <= n; size++ {
		generate(0, size)
	}
	return result.Values(), nil
}

// FormatSubarrays записывает результат как "[{}, {x}, {x, y}]".
func FormatSubarrays(subs [][]string) string {
	items := make([]string, len(subs))
	for i, s := range subs {
		items[i] = "{" + strings.Join(s, ", ") + "}"
	}
	return "[" + strings.Join(items, ", ") + "]"
}
