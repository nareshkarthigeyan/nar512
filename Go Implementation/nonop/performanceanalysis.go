package main

import (
	"fmt"
	"math/rand"
	"strings"
	"time"
)

// Assume this function exists in nar512.go
// func Hash(input string) []uint32

func generateRandomString(length int) string {
	const charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+[]{}|;:',.<>/?`~\"\\\t\r\n"
	var builder strings.Builder
	builder.Grow(length)

	for i := 0; i < length; i++ {
		builder.WriteByte(charset[rand.Intn(len(charset))])
	}

	return builder.String()
}

func uint32sToHexString(data []uint32) string {
	var builder strings.Builder
	for _, num := range data {
		builder.WriteString(fmt.Sprintf("%08x", num))
	}
	return builder.String()
}

type Times struct {
	count       int
	totalTime   time.Duration
	averageTime time.Duration
}

func (t *Times) Add(d time.Duration) {
	t.totalTime += d
	t.count++
	t.averageTime = t.totalTime / time.Duration(t.count)
}

// func main() {
// 	rand.Seed(time.Now().UnixNano())

// 	sizes := []int{1, 10, 100, 250, 500, 1024, 5000, 10000}
// 	timesMap := make(map[int]Times)

// 	fmt.Printf("%-15s%-20s%-20s\n", "Char Length", "Average Time (s)", "Total Time (s)")
// 	fmt.Println(strings.Repeat("-", 55))

// 	for _, size := range sizes {
// 		var times Times
// 		for i := 0; i < 1000; i++ {
// 			input := generateRandomString(size)
// 			start := time.Now()
// 			hash := nar512(input)
// 			_ = uint32sToHexString(hash) // Just to simulate usage
// 			elapsed := time.Since(start)
// 			times.Add(elapsed)
// 		}
// 		timesMap[size] = times

// 		fmt.Printf("%-15d%-20.5f%-20.5f\n",
// 				size,
// 				times.averageTime.Seconds(),
// 				times.totalTime.Seconds())
// 	}
// }