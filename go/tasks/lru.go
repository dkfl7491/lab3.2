package tasks

import (
	"bufio"
	"errors"
	"fmt"
	"io"

	"lab2/ds"
)

type lruItem struct {
	key, val string
}

// LRUCache (задание 7, вариант 1) — кэш Least Recently Used с операциями
// Set и Get за O(1).
//
// order — двусвязный список записей от самой давней (в начале) до самой
// свежей (в конце); items — хеш-таблица «ключ → узел списка». Любое
// обращение (Get, а также Set существующего ключа) переносит узел в конец
// списка. При переполнении удаляется первый узел.
type LRUCache struct {
	capacity int
	order    *ds.DoublyList[lruItem]
	items    *ds.ChainTable[*ds.DNode[lruItem]]
}

// NewLRUCache создаёт кэш заданной ёмкости.
func NewLRUCache(capacity int) *LRUCache {
	return &LRUCache{
		capacity: capacity,
		order:    &ds.DoublyList[lruItem]{},
		items:    ds.NewChainTable[*ds.DNode[lruItem]](16),
	}
}

// Get возвращает значение по ключу; ok == false, если ключа нет.
func (c *LRUCache) Get(key string) (string, bool) {
	node, ok := c.items.Get(key)
	if !ok {
		return "", false
	}
	c.order.MoveToBack(node)
	return node.Value.val, true
}

// Set устанавливает значение ключа, при переполнении вытесняя самую давнюю запись.
func (c *LRUCache) Set(key, val string) {
	if c.capacity <= 0 {
		return
	}
	if node, ok := c.items.Get(key); ok {
		node.Value.val = val
		c.order.MoveToBack(node)
		return
	}
	if c.order.Len() >= c.capacity {
		oldest := c.order.Front()
		c.order.Remove(oldest)
		c.items.Delete(oldest.Value.key)
	}
	c.items.Put(key, c.order.PushBack(lruItem{key, val}))
}

// RunLRU читает ёмкость, число запросов Q и сами запросы (SET x y / GET x),
// выполняет их и возвращает результаты всех GET ("-1", если ключа нет).
// Запросы разделяются любыми пробельными символами.
func RunLRU(r io.Reader) ([]string, error) {
	in := bufio.NewReader(r)
	var capacity, q int
	if _, err := fmt.Fscan(in, &capacity, &q); err != nil {
		return nil, errors.New("Expected: <capacity> <Q> and then Q queries")
	}
	cache := NewLRUCache(capacity)
	var out []string
	for i := 0; i < q; i++ {
		var cmd string
		if _, err := fmt.Fscan(in, &cmd); err != nil {
			return nil, errors.New("Not enough queries")
		}
		switch cmd {
		case "SET":
			var k, v string
			if _, err := fmt.Fscan(in, &k, &v); err != nil {
				return nil, errors.New("SET needs key and value")
			}
			cache.Set(k, v)
		case "GET":
			var k string
			if _, err := fmt.Fscan(in, &k); err != nil {
				return nil, errors.New("GET needs key")
			}
			if v, ok := cache.Get(k); ok {
				out = append(out, v)
			} else {
				out = append(out, "-1")
			}
		default:
			return nil, errors.New("Unknown command: " + cmd)
		}
	}
	return out, nil
}
