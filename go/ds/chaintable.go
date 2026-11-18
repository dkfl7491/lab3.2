package ds

type chainEntry[V any] struct {
	key string
	val V
}

// ChainTable — хеш-таблица с разрешением коллизий методом цепочек.
// Каждая корзина — односвязный список (SinglyList из ЛР1).
// При коэффициенте заполнения > 0.75 таблица удваивается.
type ChainTable[V any] struct {
	buckets []*SinglyList[chainEntry[V]]
	size    int

	Collisions int // вставки новых ключей в уже непустую корзину
	Resizes    int // количество увеличений таблицы
}

const chainMaxLoad = 0.75

// NewChainTable создаёт таблицу с заданным начальным числом корзин.
func NewChainTable[V any](capacity int) *ChainTable[V] {
	if capacity < 8 {
		capacity = 8
	}
	return &ChainTable[V]{buckets: make([]*SinglyList[chainEntry[V]], capacity)}
}

func (t *ChainTable[V]) index(key string) int {
	return int(hashFNV(key, 0) % uint64(len(t.buckets)))
}

func keyMatcher[V any](key string) func(*chainEntry[V]) bool {
	return func(e *chainEntry[V]) bool { return e.key == key }
}

// Put добавляет пару или обновляет значение существующего ключа.
func (t *ChainTable[V]) Put(key string, val V) {
	i := t.index(key)
	b := t.buckets[i]
	if b == nil {
		b = &SinglyList[chainEntry[V]]{}
		t.buckets[i] = b
	}
	if e := b.Find(keyMatcher[V](key)); e != nil {
		e.val = val
		return
	}
	if b.Len() > 0 {
		t.Collisions++
	}
	b.PushBack(chainEntry[V]{key, val})
	t.size++
	if float64(t.size) > chainMaxLoad*float64(len(t.buckets)) {
		t.grow()
	}
}

// Get возвращает значение по ключу.
func (t *ChainTable[V]) Get(key string) (V, bool) {
	if b := t.buckets[t.index(key)]; b != nil {
		if e := b.Find(keyMatcher[V](key)); e != nil {
			return e.val, true
		}
	}
	var zero V
	return zero, false
}

// Delete удаляет ключ; возвращает true, если он был в таблице.
func (t *ChainTable[V]) Delete(key string) bool {
	b := t.buckets[t.index(key)]
	if b == nil || !b.RemoveFirst(keyMatcher[V](key)) {
		return false
	}
	t.size--
	return true
}

// Len возвращает количество пар.
func (t *ChainTable[V]) Len() int {
	return t.size
}

// Each вызывает f для каждой пары (порядок не определён).
func (t *ChainTable[V]) Each(f func(key string, val V)) {
	for _, b := range t.buckets {
		if b != nil {
			b.Each(func(e *chainEntry[V]) { f(e.key, e.val) })
		}
	}
}

// LoadFactor возвращает коэффициент заполнения (пар на корзину).
func (t *ChainTable[V]) LoadFactor() float64 {
	return float64(t.size) / float64(len(t.buckets))
}

// Stats возвращает длину самой длинной цепочки и среднюю длину
// среди непустых корзин.
func (t *ChainTable[V]) Stats() (maxChain int, avgChain float64) {
	used := 0
	for _, b := range t.buckets {
		if b == nil || b.Len() == 0 {
			continue
		}
		used++
		if b.Len() > maxChain {
			maxChain = b.Len()
		}
	}
	if used > 0 {
		avgChain = float64(t.size) / float64(used)
	}
	return
}

// grow удваивает число корзин и заново распределяет все пары.
func (t *ChainTable[V]) grow() {
	old := t.buckets
	t.buckets = make([]*SinglyList[chainEntry[V]], len(old)*2)
	t.Resizes++
	for _, b := range old {
		if b == nil {
			continue
		}
		b.Each(func(e *chainEntry[V]) {
			i := t.index(e.key)
			if t.buckets[i] == nil {
				t.buckets[i] = &SinglyList[chainEntry[V]]{}
			}
			t.buckets[i].PushBack(*e)
		})
	}
}
