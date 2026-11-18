// Задание 2: АТД «Множество».
//
//	./task2 --file file.data --query 'SETADD имя элемент'
//
// Команды: SETADD, SETDEL, SET_AT (+ служебная SETREAD).
package main

import (
	"fmt"
	"os"

	"lab2/tasks"
)

func main() {
	filename := "file.data"
	query := ""

	args := os.Args[1:]
	for i := 0; i < len(args); i++ {
		switch {
		case args[i] == "--file" && i+1 < len(args):
			i++
			filename = args[i]
		case args[i] == "--query" && i+1 < len(args):
			i++
			query = args[i]
		default:
			fmt.Fprintln(os.Stderr, "Usage: ./task2 --file file.data --query 'COMMAND arguments'")
			os.Exit(1)
		}
	}
	if query == "" {
		fmt.Fprintln(os.Stderr, "Query was not specified")
		os.Exit(1)
	}

	db := tasks.NewSetDB()
	if err := db.Load(filename); err != nil {
		fmt.Fprintln(os.Stderr, "ERROR:", err)
		os.Exit(1)
	}
	out, err := db.Execute(query)
	if err == nil {
		err = db.Save(filename) // как в ЛР1: сохраняем после успешной команды
	}
	if err != nil {
		fmt.Fprintln(os.Stderr, "ERROR:", err)
		os.Exit(1)
	}
	if out != "" {
		fmt.Println(out)
	}
}
