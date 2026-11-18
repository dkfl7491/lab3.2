package ds

type slotState uint8

const (
	slotEmpty slotState = iota
	slotUsed
	slotDeleted // «надгробие»: ячейка освободилась, но цепочка проб через неё идёт дальше
)

type openSlot[V any] struct {
	state slotState
	key   string
	val   V
}

// OpenTable — хеш-таблица с открытой адресацией (задание 6.1, вариант 1).
// Все пары лежат прямо в массиве ячеек; при коллизии ищется другая ячейка
// по последовательности проб idx, idx+step, idx+2·step, … (по модулю размера):
//
//   - линейное апробирование: step = 1;
//   - двойное хеширование: step = h2(key), вторая хеш-функция (всегда нечётная).
//
// Размер таблицы — степень двойки, поэтому нечётный шаг взаимно прост с ним
// и последовательность проб обходит все ячейки. Удаление помечает ячейку
// «надгробием». При заполнении (с учётом надгробий) выше maxLoad таблица
// перестраивается: удваивается или просто очищается от надгробий.
type OpenTable[V any] struct {
	slots       []openSlot[V]
	mask        int
	live, tombs int
	double      bool
	maxLoad     float64

	Collisions int // новые ключи, попавшие не в свою первую ячейку
	Resizes    int // перестроения таблицы
	Probes     int // число просмотренных ячеек во всех вызовах Get
}

// NewLinearTable — таблица с линейным апробированием (макс. заполнение 0.7).
func NewLinearTable[V any](capacity int) *OpenTable[V] {
	return NewOpenTable[V](false, capacity, 0.7)
}

// NewDoubleHashTable — таблица с двойным хешированием (макс. заполнение 0.7).
func NewDoubleHashTable[V any](capacity int) *OpenTable[V] {
	return NewOpenTable[V](true, capacity, 0.7)
}

// NewOpenTable создаёт таблицу; capacity округляется вверх до степени двойки,
// maxLoad (0 < maxLoad < 1) — предел заполнения, после которого таблица
// перестраивается.
func NewOpenTable[V any](double bool, capacity int, maxLoad float64) *OpenTable[V] {
	size := 8
	for size < capacity {
		size <<= 1
	}
	return &OpenTable[V]{
		slots:   make([]openSlot[V], size),
		mask:    size - 1,
		double:  double,
		maxLoad: maxLoad,
	}
}

func (t *OpenTable[V]) start(key string) int {
	return int(hashFNV(key, 0) & uint64(t.mask))
}

func (t *OpenTable[V]) step(key string) int {
	if !t.double {
		return 1
	}
	return int(hashPoly(key, 0)&uint64(t.mask)) | 1
}

// Put добавляет пару или обновляет значение существующего ключа.
func (t *OpenTable[V]) Put(key string, val V) {
	first := t.start(key)
	step := t.step(key)
	idx := first
	deleted := -1 // первое встреченное «надгробие» — туда можно вставить новый ключ

	for i := 0; i <= t.mask; i++ {
		s := &t.slots[idx]
		switch s.state {
		case slotUsed:
			if s.key == key {
				s.val = val
				return
			}
		case slotDeleted:
			if deleted < 0 {
				deleted = idx
			}
		case slotEmpty:
			// Ключа в таблице нет — вставляем (предпочтительно на «надгробие»).
			target := idx
			if deleted >= 0 {
				target = deleted
				t.tombs--
			}
			t.slots[target] = openSlot[V]{slotUsed, key, val}
			t.live++
			if target != first {
				t.Collisions++
			}
			t.rebuildIfNeeded()
			return
		}
		idx = (idx + step) & t.mask
	}

	// Пустых ячеек не осталось (возможно только при maxLoad около 1).
	if deleted >= 0 {
		t.slots[deleted] = openSlot[V]{slotUsed, key, val}
		t.tombs--
		t.live++
		return
	}
	t.rebuild(len(t.slots) * 2)
	t.Put(key, val)
}

// find возвращает индекс ячейки с ключом или -1.
func (t *OpenTable[V]) find(key string, countProbes bool) int {
	idx := t.start(key)
	step := t.step(key)
	for i := 0; i <= t.mask; i++ {
		s := &t.slots[idx]
		if countProbes {
			t.Probes++
		}
		if s.state == slotEmpty {
			return -1
		}
		if s.state == slotUsed && s.key == key {
			return idx
		}
		idx = (idx + step) & t.mask
	}
	return -1
}

// Get возвращает значение по ключу.
func (t *OpenTable[V]) Get(key string) (V, bool) {
	if i := t.find(key, true); i >= 0 {
		return t.slots[i].val, true
	}
	var zero V
	return zero, false
}

// Delete удаляет ключ; возвращает true, если он был в таблице.
func (t *OpenTable[V]) Delete(key string) bool {
	i := t.find(key, false)
	if i < 0 {
		return false
	}
	t.slots[i] = openSlot[V]{state: slotDeleted}
	t.live--
	t.tombs++
	return true
}

// Len возвращает количество пар.
func (t *OpenTable[V]) Len() int {
	return t.live
}

// LoadFactor возвращает долю занятых ячеек (без «надгробий»).
func (t *OpenTable[V]) LoadFactor() float64 {
	return float64(t.live) / float64(len(t.slots))
}

func (t *OpenTable[V]) rebuildIfNeeded() {
	size := float64(len(t.slots))
	if float64(t.live+t.tombs) <= t.maxLoad*size {
		return
	}
	if float64(t.live) > t.maxLoad/2*size {
		t.rebuild(len(t.slots) * 2)
	} else {
		t.rebuild(len(t.slots)) // много надгробий — достаточно их вычистить
	}
}

// rebuild создаёт новый массив ячеек и заново вставляет в него все пары.
func (t *OpenTable[V]) rebuild(size int) {
	old := t.slots
	t.slots = make([]openSlot[V], size)
	t.mask = size - 1
	t.live, t.tombs = 0, 0
	t.Resizes++
	for i := range old {
		if old[i].state != slotUsed {
			continue
		}
		idx := t.start(old[i].key)
		step := t.step(old[i].key)
		for t.slots[idx].state != slotEmpty {
			idx = (idx + step) & t.mask
		}
		t.slots[idx] = old[i]
		t.live++
	}
}
