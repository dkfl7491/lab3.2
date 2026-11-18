package ds

// mix — финальное перемешивание битов (из MurmurHash3), чтобы остаток
// от деления на размер таблицы зависел от всех битов хеша.
func mix(h uint64) uint64 {
	h ^= h >> 33
	h *= 0xff51afd7ed558ccd
	h ^= h >> 33
	h *= 0xc4ceb9fe1a85ec53
	h ^= h >> 33
	return h
}

// hashFNV — хеш FNV-1a с «солью» seed (первая хеш-функция).
func hashFNV(s string, seed uint64) uint64 {
	h := uint64(14695981039346656037) ^ seed
	for i := 0; i < len(s); i++ {
		h ^= uint64(s[i])
		h *= 1099511628211
	}
	return mix(h)
}

// hashPoly — полиномиальный хеш с «солью» seed (вторая хеш-функция кукушки).
func hashPoly(s string, seed uint64) uint64 {
	h := seed
	for i := 0; i < len(s); i++ {
		h = h*1000003 + uint64(s[i]) + 1
	}
	return mix(h)
}
