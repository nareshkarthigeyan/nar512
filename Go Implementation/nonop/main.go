package main

// import (
//     "fmt"
//     "os"
// 	"testing"
// )

// type testFunc func() error

// func main() {
//     tests := map[string]func() error{
//         "Avalanche Effect":            runTest(TestAvalancheEffect),
//         "Collision Resistance":        runTest(TestCollisionResistance),
//         "Second Pre-image Resistance": runTest(TestSecondPreImageResistance),
//         "Length Extension Resistance": runTest(TestLengthExtensionResistance),
//         "Statistical Randomness":      runTest(TestStatisticalRandomness),
//     }

//     passed, failed := 0, 0
//     fmt.Println("=== NAR-512 Security Test Summary ===")
//     for name, test := range tests {
//         err := test()
//         if err != nil {
//             fmt.Printf("[FAIL] %s: %v\n", name, err)
//             failed++
//         } else {
//             fmt.Printf("[PASS] %s\n", name)
//             passed++
//         }
//     }

//     fmt.Printf("\nTotal: %d passed, %d failed\n", passed, failed)
//     if failed > 0 {
//         os.Exit(1)
//     }
// }

// func runTest(test func(*testing.T)) func() error {
//     return func() error {
//         t := &testing.T{}
//         test(t)
//         if t.Failed() {
//             return fmt.Errorf("test failed")
//         }
//         return nil
//     }
// }