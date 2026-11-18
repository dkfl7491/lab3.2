package tasks

import (
	"errors"
	"fmt"
	"sort"
	"strconv"
	"strings"

	"lab2/ds"
)

// ErrNoPartition — множество нельзя разбить на подмножества с равной суммой.
var ErrNoPartition = errors.New("Set cannot be partitioned into subsets with equal sums")

// PartitionEqualSum (задание 3, вариант 1) — разбивает множество натуральных
// чисел на непересекающиеся подмножества с одинаковой суммой.
//
// Если target > 0, сумма каждого подмножества должна быть равна target
// (как в примерах из задания: 15 или 10). Если target == 0, выбирается
// наименьшая возможная сумма, то есть разбиение на максимальное число
// подмножеств (в крайнем случае — всё множество целиком).
//
// Подмножества хранятся в множествах Set из задания 2. Поиск — перебор
// с возвратом: числа по убыванию раскладываются по «корзинам» ёмкости
// target; корзины с одинаковым остатком равноценны, поэтому пробуется
// только одна из них. Задача NP-трудная, перебор рассчитан на небольшие
// множества.
func PartitionEqualSum(nums []int, target int) ([][]int, error) {
	if len(nums) == 0 {
		return nil, errors.New("Set is empty")
	}
	unique := ds.NewSet()
	sum, largest := 0, 0
	for _, x := range nums {
		if x <= 0 {
			return nil, errors.New("Elements must be natural numbers")
		}
		if !unique.Add(strconv.Itoa(x)) {
			return nil, fmt.Errorf("Duplicate element in set: %d", x)
		}
		sum += x
		if x > largest {
			largest = x
		}
	}
	if target < 0 {
		return nil, errors.New("Target sum must be positive")
	}

	sorted := append([]int(nil), nums...)
	sort.Sort(sort.Reverse(sort.IntSlice(sorted)))

	if target > 0 {
		if parts, ok := partitionInto(sorted, sum, target); ok {
			return parts, nil
		}
		return nil, ErrNoPartition
	}
	for t := largest; t <= sum; t++ {
		if sum%t != 0 {
			continue
		}
		if parts, ok := partitionInto(sorted, sum, t); ok {
			return parts, nil
		}
	}
	return nil, ErrNoPartition // недостижимо: t == sum всегда подходит
}

// partitionInto раскладывает числа (по убыванию) по sum/target подмножествам.
func partitionInto(sorted []int, sum, target int) ([][]int, bool) {
	if sorted[0] > target || sum%target != 0 {
		return nil, false
	}
	k := sum / target
	buckets := make([]*ds.Set, k)
	remaining := make([]int, k)
	for i := range buckets {
		buckets[i] = ds.NewSet()
		remaining[i] = target
	}

	var place func(i int) bool
	place = func(i int) bool {
		if i == len(sorted) {
			return true
		}
		x := sorted[i]
		key := strconv.Itoa(x)
		tried := ds.NewSet() // остатки, которые уже пробовали на этом шаге
		for b := range buckets {
			if remaining[b] < x || !tried.Add(strconv.Itoa(remaining[b])) {
				continue
			}
			buckets[b].Add(key)
			remaining[b] -= x
			if place(i + 1) {
				return true
			}
			remaining[b] += x
			buckets[b].Delete(key)
		}
		return false
	}
	if !place(0) {
		return nil, false
	}

	parts := make([][]int, k)
	for b, set := range buckets {
		for _, s := range set.Values() {
			v, _ := strconv.Atoi(s)
			parts[b] = append(parts[b], v)
		}
		sort.Ints(parts[b])
	}
	sort.Slice(parts, func(i, j int) bool { return parts[i][0] < parts[j][0] })
	return parts, true
}

// FormatSubsets записывает подмножества в виде "{1, 4, 10}" по одному на строку.
func FormatSubsets(parts [][]int) []string {
	lines := make([]string, len(parts))
	for i, p := range parts {
		items := make([]string, len(p))
		for j, v := range p {
			items[j] = strconv.Itoa(v)
		}
		lines[i] = "{" + strings.Join(items, ", ") + "}"
	}
	return lines
}
