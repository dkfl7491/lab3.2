package tasks

import (
	"fmt"
	"io"
	"math"
	"math/rand"
	"time"

	"lab2/ds"
)

func randomKeys(n int, prefix string, seed int64) []string {
	rng := rand.New(rand.NewSource(seed))
	const alphabet = "abcdefghijklmnopqrstuvwxyz0123456789"
	keys := make([]string, n)
	buf := make([]byte, 12)
	for i := range keys {
		for j := range buf {
			buf[j] = alphabet[rng.Intn(len(alphabet))]
		}
		keys[i] = prefix + string(buf)
	}
	return keys
}

func timePerOp(n int, f func()) float64 {
	start := time.Now()
	f()
	return float64(time.Since(start).Nanoseconds()) / float64(n)
}

// Теоретическое среднее число проб (Кнут) при заполнении a для равномерного хеширования.
func theoryLinear(a float64) (hit, miss float64) {
	return 0.5 * (1 + 1/(1-a)), 0.5 * (1 + 1/((1-a)*(1-a)))
}

func theoryDouble(a float64) (hit, miss float64) {
	return math.Log(1/(1-a)) / a, 1 / (1 - a)
}

// RunHashBenchmark (задание 6.1) — эмпирическое сравнение линейного
// апробирования и двойного хеширования.
//
// Часть 1: таблица фиксированного размера заполняется до заданной доли.
// Измеряется время (нс на операцию) и среднее число проб при успешном и
// неуспешном поиске, рядом — теоретическое значение.
// Часть 2: обычное использование — таблица растёт сама, при заполнении 0.7.
func RunHashBenchmark(w io.Writer) {
	const capacity = 1 << 19

	fmt.Fprintf(w, "Часть 1. Таблица на %d ячеек, перестроение отключено\n", capacity)
	fmt.Fprintf(w, "%-10s %-6s %8s %8s %8s %18s %18s %10s\n",
		"метод", "альфа", "вставка", "поиск+", "поиск-", "проб при поиске+", "проб при поиске-", "коллизий")
	fmt.Fprintf(w, "%-10s %-6s %8s %8s %8s %18s %18s %10s\n",
		"", "", "нс", "нс", "нс", "изм. (теор.)", "изм. (теор.)", "")

	for _, load := range []float64{0.5, 0.7, 0.9} {
		n := int(load * capacity)
		keys := randomKeys(n, "k", 1)
		missing := randomKeys(n, "m", 2) // другой префикс: таких ключей в таблице нет

		for _, double := range []bool{false, true} {
			t := ds.NewOpenTable[int](double, capacity, 0.99)
			ins := timePerOp(n, func() {
				for i, k := range keys {
					t.Put(k, i)
				}
			})
			p0 := t.Probes
			hit := timePerOp(n, func() {
				for _, k := range keys {
					if _, ok := t.Get(k); !ok {
						panic("key lost")
					}
				}
			})
			hitProbes := float64(t.Probes-p0) / float64(n)
			p0 = t.Probes
			miss := timePerOp(n, func() {
				for _, k := range missing {
					if _, ok := t.Get(k); ok {
						panic("phantom key")
					}
				}
			})
			missProbes := float64(t.Probes-p0) / float64(n)

			name, th, tm := "линейное", 0.0, 0.0
			if double {
				name = "двойное"
				th, tm = theoryDouble(load)
			} else {
				th, tm = theoryLinear(load)
			}
			fmt.Fprintf(w, "%-10s %-6.2f %8.0f %8.0f %8.0f %9.2f (%5.2f) %9.2f (%5.2f) %10d\n",
				name, load, ins, hit, miss, hitProbes, th, missProbes, tm, t.Collisions)
		}
	}

	fmt.Fprintf(w, "\nЧасть 2. Обычное использование: таблица растёт сама (предел заполнения 0.7)\n")
	fmt.Fprintf(w, "%-10s %-8s %8s %8s %10s %10s %12s\n",
		"метод", "N", "вставка", "поиск+", "коллизий", "перестроек", "проб/поиск+")
	for _, n := range []int{10_000, 100_000, 500_000} {
		keys := randomKeys(n, "k", 1)
		for _, double := range []bool{false, true} {
			t := ds.NewOpenTable[int](double, 16, 0.7)
			ins := timePerOp(n, func() {
				for i, k := range keys {
					t.Put(k, i)
				}
			})
			p0 := t.Probes
			hit := timePerOp(n, func() {
				for _, k := range keys {
					if _, ok := t.Get(k); !ok {
						panic("key lost")
					}
				}
			})
			name := "линейное"
			if double {
				name = "двойное"
			}
			fmt.Fprintf(w, "%-10s %-8d %8.0f %8.0f %10d %10d %12.2f\n",
				name, n, ins, hit, t.Collisions, t.Resizes, float64(t.Probes-p0)/float64(n))
		}
	}
}
