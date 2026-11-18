package ds

import (
	"fmt"
	"math/rand"
	"reflect"
	"testing"
)

// table — общий интерфейс обеих хеш-таблиц для сравнения с map.
type table interface {
	Put(string, int)
	Get(string) (int, bool)
	Delete(string) bool
	Len() int
}

func TestTablesAgainstBuiltinMap(t *testing.T) {
	impls := map[string]func() table{
		"chain":  func() table { return NewChainTable[int](8) },
		"linear": func() table { return NewLinearTable[int](8) },
		"double": func() table { return NewDoubleHashTable[int](8) },
	}
	for name, mk := range impls {
		t.Run(name, func(t *testing.T) {
			rng := rand.New(rand.NewSource(42))
			tb, ref := mk(), map[string]int{}
			for step := 0; step < 200000; step++ {
				k := fmt.Sprintf("key%d", rng.Intn(5000))
				switch rng.Intn(3) {
				case 0:
					v := rng.Int()
					tb.Put(k, v)
					ref[k] = v
				case 1:
					_, want := ref[k]
					delete(ref, k)
					if got := tb.Delete(k); got != want {
						t.Fatalf("Delete(%q) = %v, want %v", k, got, want)
					}
				default:
					wantV, wantOK := ref[k]
					if gotV, gotOK := tb.Get(k); gotV != wantV || gotOK != wantOK {
						t.Fatalf("Get(%q) = %v,%v, want %v,%v", k, gotV, gotOK, wantV, wantOK)
					}
				}
				if tb.Len() != len(ref) {
					t.Fatalf("step %d: Len = %d, want %d", step, tb.Len(), len(ref))
				}
			}
		})
	}
}

func TestSet(t *testing.T) {
	s := NewSet()
	if !s.Add("a") || s.Add("a") {
		t.Fatal("Add must report whether the element was new")
	}
	s.Add("b")
	if !s.Has("a") || s.Has("c") {
		t.Fatal("Has is wrong")
	}
	if !s.Delete("a") || s.Delete("a") {
		t.Fatal("Delete must report whether the element existed")
	}
	if got := s.Values(); !reflect.DeepEqual(got, []string{"b"}) {
		t.Fatalf("Values = %v", got)
	}
}

func TestArrayStackLists(t *testing.T) {
	var a Array[int]
	a.Push(1)
	a.Push(3)
	if err := a.Insert(1, 2); err != nil {
		t.Fatal(err)
	}
	if err := a.Insert(5, 9); err == nil {
		t.Fatal("Insert out of range must fail")
	}
	if err := a.Remove(0); err != nil || !reflect.DeepEqual(a.Values(), []int{2, 3}) {
		t.Fatalf("Remove: %v %v", err, a.Values())
	}

	var st Stack[int]
	if _, err := st.Pop(); err == nil {
		t.Fatal("Pop on empty stack must fail")
	}
	st.Push(1)
	st.Push(2)
	if v, _ := st.Pop(); v != 2 {
		t.Fatalf("Pop = %d", v)
	}

	var sl SinglyList[int]
	sl.PushBack(2)
	sl.PushFront(1)
	sl.PushBack(3)
	sl.RemoveFirst(func(x *int) bool { return *x == 3 })
	sl.PushBack(4) // хвост должен быть обновлён после удаления
	var got []int
	sl.Each(func(x *int) { got = append(got, *x) })
	if !reflect.DeepEqual(got, []int{1, 2, 4}) {
		t.Fatalf("SinglyList = %v", got)
	}

	var dl DoublyList[int]
	n1 := dl.PushBack(1)
	n2 := dl.PushBack(2)
	dl.PushFront(0)
	dl.Remove(n1)
	if dl.Len() != 2 || dl.Front().Value != 0 {
		t.Fatal("DoublyList broken")
	}
	dl.Remove(n2)
	dl.Remove(dl.Front())
	if dl.Len() != 0 || dl.Front() != nil {
		t.Fatal("DoublyList must become empty")
	}
}

func TestBSTInOrder(t *testing.T) {
	var tr BST
	for _, v := range []int{7, 3, 2, 1, 9, 5, 4, 6, 8} {
		tr.Insert(v)
	}
	if tr.Insert(7) {
		t.Fatal("duplicate must be rejected")
	}
	var got []int
	tr.InOrder(func(n *BSTNode) { got = append(got, n.Value) })
	if !reflect.DeepEqual(got, []int{1, 2, 3, 4, 5, 6, 7, 8, 9}) {
		t.Fatalf("InOrder = %v", got)
	}
}

func TestMoveToBack(t *testing.T) {
	var dl DoublyList[int]
	a := dl.PushBack(1)
	dl.PushBack(2)
	c := dl.PushBack(3)
	dl.MoveToBack(a) // 2 3 1
	dl.MoveToBack(c) // 2 1 3
	dl.MoveToBack(c) // без изменений
	var got []int
	for n := dl.Front(); n != nil; n = n.next {
		got = append(got, n.Value)
	}
	if !reflect.DeepEqual(got, []int{2, 1, 3}) || dl.Len() != 3 {
		t.Fatalf("order = %v, len = %d", got, dl.Len())
	}
	dl.Remove(dl.Front())
	dl.Remove(dl.Front())
	dl.Remove(dl.Front())
	dl.MoveToBack(dl.PushBack(9)) // единственный узел
	if dl.Len() != 1 || dl.Front().Value != 9 {
		t.Fatal("single node broken")
	}
}

// Удаление «надгробиями» не должно ломать поиск ключей, лежащих дальше по цепочке проб.
func TestOpenTableTombstones(t *testing.T) {
	for _, double := range []bool{false, true} {
		tb := NewOpenTable[int](double, 8, 0.9)
		for i := 0; i < 6; i++ {
			tb.Put(fmt.Sprint("k", i), i)
		}
		for i := 0; i < 6; i += 2 {
			tb.Delete(fmt.Sprint("k", i))
		}
		for i := 1; i < 6; i += 2 {
			if v, ok := tb.Get(fmt.Sprint("k", i)); !ok || v != i {
				t.Fatalf("double=%v: key k%d lost", double, i)
			}
		}
		// Постоянные вставки и удаления только чистят «надгробия» и не раздувают таблицу.
		for round := 0; round < 1000; round++ {
			k := fmt.Sprint("t", round)
			tb.Put(k, round)
			tb.Delete(k)
		}
		if tb.Len() != 3 || len(tb.slots) > 16 {
			t.Fatalf("double=%v: len=%d, slots=%d (table must not keep growing)", double, tb.Len(), len(tb.slots))
		}
	}
}
