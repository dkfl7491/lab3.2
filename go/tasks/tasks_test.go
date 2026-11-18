package tasks

import (
	"path/filepath"
	"reflect"
	"sort"
	"strings"
	"testing"
)

func TestDFS(t *testing.T) {
	edges := [][2]int{{1, 2}, {1, 3}, {2, 4}, {2, 5}, {3, 6}}
	got, err := DFS(6, edges, 1, false)
	if err != nil || !reflect.DeepEqual(got, []int{1, 2, 4, 5, 3, 6}) {
		t.Fatalf("tree DFS = %v, %v", got, err)
	}
	// Граф с циклом и ориентированными рёбрами.
	got, _ = DFS(4, [][2]int{{1, 2}, {2, 3}, {3, 1}, {1, 4}}, 1, true)
	if !reflect.DeepEqual(got, []int{1, 2, 3, 4}) {
		t.Fatalf("digraph DFS = %v", got)
	}
	// Недостижимые вершины не попадают в обход.
	got, _ = DFS(5, [][2]int{{1, 2}, {4, 5}}, 1, false)
	if !reflect.DeepEqual(got, []int{1, 2}) {
		t.Fatalf("disconnected DFS = %v", got)
	}
	if _, err := DFS(3, nil, 4, false); err == nil {
		t.Fatal("start out of range must fail")
	}
	if _, err := DFS(3, [][2]int{{1, 9}}, 1, false); err == nil {
		t.Fatal("edge out of range must fail")
	}
}

func TestSetDB(t *testing.T) {
	file := filepath.Join(t.TempDir(), "sets.data")
	run := func(q string) (string, error) {
		db := NewSetDB()
		if err := db.Load(file); err != nil {
			t.Fatal(err)
		}
		out, err := db.Execute(q)
		if err == nil {
			if err := db.Save(file); err != nil {
				t.Fatal(err)
			}
		}
		return out, err
	}

	for _, q := range []string{"SETADD s 1", "SETADD s 2", "SETADD s 2", `SETADD s a"b`} {
		if _, err := run(q); err != nil {
			t.Fatalf("%s: %v", q, err)
		}
	}
	cases := map[string]string{"SET_AT s 1": "TRUE", "SET_AT s 3": "FALSE", `SET_AT s a"b`: "TRUE", "SETREAD s": `1 2 a"b`}
	for q, want := range cases {
		if got, err := run(q); err != nil || got != want {
			t.Fatalf("%s = %q, %v; want %q", q, got, err, want)
		}
	}
	if _, err := run("SETDEL s 1"); err != nil {
		t.Fatal(err)
	}
	if got, _ := run("SET_AT s 1"); got != "FALSE" {
		t.Fatal("deleted element is still there")
	}
	for _, q := range []string{"SETDEL s 1", "SETDEL none 1", "SET_AT none 1", "SETADD s", "NOPE s 1", ""} {
		if _, err := run(q); err == nil {
			t.Fatalf("%q must fail", q)
		}
	}
}

func TestLeaves(t *testing.T) {
	if got := Leaves([]int{7, 3, 2, 1, 9, 5, 4, 6, 8}); !reflect.DeepEqual(got, []int{1, 4, 6, 8}) {
		t.Fatalf("example = %v", got)
	}
	if got := Leaves([]int{5}); !reflect.DeepEqual(got, []int{5}) {
		t.Fatalf("single node = %v", got)
	}
	if got := Leaves([]int{5, 5, 5}); !reflect.DeepEqual(got, []int{5}) {
		t.Fatalf("duplicates = %v", got)
	}
	if got := Leaves(nil); len(got) != 0 {
		t.Fatalf("empty = %v", got)
	}
	// Вырожденное дерево (список) — не должно падать на глубине.
	seq := make([]int, 200000)
	for i := range seq {
		seq[i] = i + 1
	}
	if got := Leaves(seq[:5000]); !reflect.DeepEqual(got, []int{5000}) {
		t.Fatalf("degenerate = %v", got)
	}
}

func TestIsomorphic(t *testing.T) {
	cases := []struct {
		a, b string
		want bool
	}{
		{"fall", "redd", true},
		{"mad", "odd", false},
		{"dad", "ada", true},
		{"ab", "aa", false},
		{"aa", "ab", false},
		{"abc", "ab", false},
		{"", "", true},
		{"привет", "абвгде", true},
	}
	for name, mk := range map[string]func() StringMap{"linear": NewLinearMap, "double": NewDoubleMap} {
		for _, c := range cases {
			if got := Isomorphic(c.a, c.b, mk); got != c.want {
				t.Errorf("%s: Isomorphic(%q, %q) = %v, want %v", name, c.a, c.b, got, c.want)
			}
		}
	}
}

func sumsOf(parts [][]int) []int {
	out := make([]int, len(parts))
	for i, p := range parts {
		for _, v := range p {
			out[i] += v
		}
	}
	return out
}

func TestPartitionEqualSum(t *testing.T) {
	s := []int{4, 10, 5, 1, 3, 7}

	parts, err := PartitionEqualSum(s, 15)
	if err != nil || !reflect.DeepEqual(sumsOf(parts), []int{15, 15}) {
		t.Fatalf("target 15: %v, %v", parts, err)
	}
	parts, err = PartitionEqualSum(s, 10)
	if err != nil || !reflect.DeepEqual(sumsOf(parts), []int{10, 10, 10}) {
		t.Fatalf("target 10: %v, %v", parts, err)
	}
	// Без суммы — наименьшая возможная (3 подмножества по 10).
	parts, err = PartitionEqualSum(s, 0)
	if err != nil || len(parts) != 3 {
		t.Fatalf("auto target: %v, %v", parts, err)
	}
	// Каждое число должно попасть ровно в одно подмножество.
	var all []int
	for _, p := range parts {
		all = append(all, p...)
	}
	sort.Ints(all)
	if !reflect.DeepEqual(all, []int{1, 3, 4, 5, 7, 10}) {
		t.Fatalf("elements are lost or duplicated: %v", all)
	}

	for _, target := range []int{5, 7, 8, 11, 12} {
		if _, err := PartitionEqualSum(s, target); err == nil {
			t.Fatalf("target %d must be impossible", target)
		}
	}
	if parts, err := PartitionEqualSum(s, 30); err != nil || len(parts) != 1 {
		t.Fatalf("whole set: %v, %v", parts, err)
	}
	// Нужен перебор с возвратом: 5+4+... = 9, жадность по убыванию не годится.
	if parts, err := PartitionEqualSum([]int{5, 4, 3, 2, 1, 6, 7, 8}, 12); err != nil || len(parts) != 3 {
		t.Fatalf("backtracking case: %v, %v", parts, err)
	}
	if _, err := PartitionEqualSum([]int{1, 2, 2}, 3); err == nil {
		t.Fatal("duplicates are not allowed in a set")
	}
	if _, err := PartitionEqualSum([]int{1, 0}, 1); err == nil {
		t.Fatal("non-natural numbers must be rejected")
	}
	if _, err := PartitionEqualSum(nil, 1); err == nil {
		t.Fatal("empty set must be rejected")
	}
	if got := FormatSubsets([][]int{{1, 4, 10}, {3}}); !reflect.DeepEqual(got, []string{"{1, 4, 10}", "{3}"}) {
		t.Fatalf("format = %v", got)
	}
}

func TestDistinctSubarrays(t *testing.T) {
	subs, err := DistinctSubarrays([]string{"x", "y", "z"})
	if err != nil {
		t.Fatal(err)
	}
	want := "[{}, {x}, {y}, {z}, {x, y}, {x, z}, {y, z}, {x, y, z}]"
	if got := FormatSubarrays(subs); got != want {
		t.Fatalf("got %s, want %s", got, want)
	}
	// Повторы в массиве не дают одинаковых подмассивов.
	subs, _ = DistinctSubarrays([]string{"1", "1"})
	if got := FormatSubarrays(subs); got != "[{}, {1}, {1, 1}]" {
		t.Fatalf("duplicates: %s", got)
	}
	subs, _ = DistinctSubarrays(nil)
	if got := FormatSubarrays(subs); got != "[{}]" {
		t.Fatalf("empty array: %s", got)
	}
	// Порядок элементов внутри подмассива сохраняется: {b, a} и {a, b} — разные.
	subs, _ = DistinctSubarrays([]string{"b", "a", "b"})
	if len(subs) != 7 { // {} {b} {a} {b,a} {b,b} {a,b} {b,a,b}
		t.Fatalf("got %d subarrays: %s", len(subs), FormatSubarrays(subs))
	}
	long := make([]string, MaxSubarrayElems+1)
	if _, err := DistinctSubarrays(long); err == nil {
		t.Fatal("too long array must be rejected")
	}
}

func TestLRU(t *testing.T) {
	run := func(input string) string {
		out, err := RunLRU(strings.NewReader(input))
		if err != nil {
			t.Fatal(err)
		}
		return strings.Join(out, " ")
	}
	if got := run("2 2 SET 1 2 GET 1"); got != "2" {
		t.Fatalf("example 1 = %q", got)
	}
	if got := run("2 8 SET 1 2 SET 2 3 SET 1 5 SET 4 5 SET 6 7 GET 4 SET 1 2 GET 3"); got != "5 -1" {
		t.Fatalf("example 2 = %q", got)
	}
	// GET освежает запись: вытесняется 2, а не 1.
	if got := run("2 6\nSET 1 1\nSET 2 2\nGET 1\nSET 3 3\nGET 1\nGET 2"); got != "1 1 -1" {
		t.Fatalf("GET refresh = %q", got)
	}
	if got := run("0 2 SET 1 1 GET 1"); got != "-1" {
		t.Fatalf("zero capacity = %q", got)
	}
	if _, err := RunLRU(strings.NewReader("2 1 PUT 1 1")); err == nil {
		t.Fatal("unknown command must fail")
	}
}
