// Задание 3, вариант 1: разбиение множества на подмножества с равной суммой.
//
// Ввод (stdin): N, затем N натуральных чисел, затем (необязательно) сумма T
// каждого подмножества. Без T берётся наименьшая возможная сумма.
// Вывод: подмножества по одному в строке или сообщение об ошибке.
package main

import (
	"bufio"
	"fmt"
	"os"

	"lab2/tasks"
)

func fail(msg string) {
	fmt.Fprintln(os.Stderr, "ERROR:", msg)
	os.Exit(1)
}

func main() {
	in := bufio.NewReader(os.Stdin)
	var n int
	if _, err := fmt.Fscan(in, &n); err != nil || n < 0 {
		fail("expected: N, N numbers and optionally the target sum T")
	}
	nums := make([]int, n)
	for i := range nums {
		if _, err := fmt.Fscan(in, &nums[i]); err != nil {
			fail("not enough numbers")
		}
	}
	target := 0
	fmt.Fscan(in, &target) // необязательный параметр

	parts, err := tasks.PartitionEqualSum(nums, target)
	if err != nil {
		fail(err.Error())
	}
	for _, line := range tasks.FormatSubsets(parts) {
		fmt.Println(line)
	}
}
