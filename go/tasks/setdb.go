package tasks

import (
	"bufio"
	"errors"
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"

	"lab2/ds"
)

// SetDB (задание 2) — файловое хранилище именованных множеств.
// Формат файла (как в ЛР1): по строке на множество
//
//	U "имя" количество "элемент1" "элемент2" ...
type SetDB struct {
	sets map[string]*ds.Set
}

// NewSetDB создаёт пустое хранилище.
func NewSetDB() *SetDB {
	return &SetDB{sets: map[string]*ds.Set{}}
}

var errBadFile = errors.New("Invalid database file")

// takeQuoted отделяет от начала строки значение в кавычках.
func takeQuoted(s string) (value, rest string, err error) {
	s = strings.TrimLeft(s, " ")
	q, err := strconv.QuotedPrefix(s)
	if err != nil {
		return "", "", errBadFile
	}
	value, err = strconv.Unquote(q)
	if err != nil {
		return "", "", errBadFile
	}
	return value, s[len(q):], nil
}

// Load загружает множества из файла; отсутствие файла — не ошибка.
func (db *SetDB) Load(filename string) error {
	f, err := os.Open(filename)
	if err != nil {
		if errors.Is(err, os.ErrNotExist) {
			return nil
		}
		return err
	}
	defer f.Close()

	sc := bufio.NewScanner(f)
	sc.Buffer(make([]byte, 1<<20), 1<<26)
	for sc.Scan() {
		line := strings.TrimSpace(sc.Text())
		if line == "" {
			continue
		}
		if !strings.HasPrefix(line, "U ") {
			return errors.New("Unknown structure in database")
		}
		name, rest, err := takeQuoted(line[2:])
		if err != nil {
			return err
		}
		rest = strings.TrimLeft(rest, " ")
		end := strings.IndexByte(rest, ' ')
		if end < 0 {
			end = len(rest)
		}
		count, err := strconv.Atoi(rest[:end])
		if err != nil || count < 0 {
			return errBadFile
		}
		rest = rest[end:]

		set := ds.NewSet()
		for i := 0; i < count; i++ {
			var v string
			if v, rest, err = takeQuoted(rest); err != nil {
				return err
			}
			set.Add(v)
		}
		db.sets[name] = set
	}
	return sc.Err()
}

// Save записывает все множества в файл.
func (db *SetDB) Save(filename string) error {
	f, err := os.Create(filename)
	if err != nil {
		return errors.New("Cannot write database file")
	}
	w := bufio.NewWriter(f)

	names := make([]string, 0, len(db.sets))
	for name := range db.sets {
		names = append(names, name)
	}
	sort.Strings(names)

	for _, name := range names {
		values := db.sets[name].Values()
		fmt.Fprintf(w, "U %s %d", strconv.Quote(name), len(values))
		for _, v := range values {
			fmt.Fprintf(w, " %s", strconv.Quote(v))
		}
		w.WriteByte('\n')
	}
	if err := w.Flush(); err != nil {
		f.Close()
		return err
	}
	return f.Close()
}

func requireCount(args []string, expected int) error {
	if len(args) != expected {
		return errors.New("Incorrect number of arguments")
	}
	return nil
}

func (db *SetDB) existing(name string) (*ds.Set, error) {
	s, ok := db.sets[name]
	if !ok {
		return nil, errors.New("Structure not found")
	}
	return s, nil
}

// Execute выполняет запрос и возвращает текст для вывода (пустой, если
// команда ничего не печатает).
//
//	SETADD  <имя> <элемент>   — добавить элемент (множество создаётся само)
//	SETDEL  <имя> <элемент>   — удалить элемент
//	SET_AT  <имя> <элемент>   — TRUE/FALSE: входит ли элемент во множество
//	SETREAD <имя>             — вывести элементы (служебная команда)
func (db *SetDB) Execute(query string) (string, error) {
	fields := strings.Fields(query)
	if len(fields) == 0 {
		return "", errors.New("Empty command")
	}
	command, args := fields[0], fields[1:]

	switch command {
	case "SETADD":
		if err := requireCount(args, 2); err != nil {
			return "", err
		}
		set, ok := db.sets[args[0]]
		if !ok {
			set = ds.NewSet()
			db.sets[args[0]] = set
		}
		set.Add(args[1])
		return "", nil

	case "SETDEL":
		if err := requireCount(args, 2); err != nil {
			return "", err
		}
		set, err := db.existing(args[0])
		if err != nil {
			return "", err
		}
		if !set.Delete(args[1]) {
			return "", errors.New("Value not found")
		}
		return "", nil

	case "SET_AT":
		if err := requireCount(args, 2); err != nil {
			return "", err
		}
		set, err := db.existing(args[0])
		if err != nil {
			return "", err
		}
		if set.Has(args[1]) {
			return "TRUE", nil
		}
		return "FALSE", nil

	case "SETREAD":
		if err := requireCount(args, 1); err != nil {
			return "", err
		}
		set, err := db.existing(args[0])
		if err != nil {
			return "", err
		}
		return strings.Join(set.Values(), " "), nil
	}
	return "", errors.New("Unknown command: " + command)
}
