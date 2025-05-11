package main

import (
    "fmt"
    "math/bits"
    "math/rand"
    "time"
)


// Custom test context instead of *testing.T
type TestContext struct {
    name   string
    failed bool
}

func (tc *TestContext) Errorf(format string, args ...any) {
    tc.failed = true
    fmt.Printf("[ERROR] %s: ", tc.name)
    fmt.Printf(format, args...)
    fmt.Println()
}

func (tc *TestContext) Fatalf(format string, args ...any) {
    tc.failed = true
    fmt.Printf("[FATAL] %s: ", tc.name)
    fmt.Printf(format, args...)
    fmt.Println()
}

func (tc *TestContext) Failed() bool {
    return tc.failed
}

// Test functions
func TestAvalancheEffect(t *TestContext) {
    original := "The quick brown fox jumps over the lazy dog"
    flipped := "the quick brown fox jumps over the lazy dog" // T -> t

    hash1 := nar512(original)
    hash2 := nar512(flipped)

    differenceBits := 0
    for i := 0; i < len(hash1); i++ {
        diff := hash1[i] ^ hash2[i]
        differenceBits += bits.OnesCount32(diff)
    }

    totalBits := 32 * len(hash1)
    percentage := float64(differenceBits) / float64(totalBits) * 100

    if percentage < 40 || percentage > 60 {
        t.Errorf("Avalanche effect outside expected range: %.2f%%\nOriginal Input: %q\nFlipped Input: %q\nOriginal Hash: %s\nFlipped Hash:  %s\nBit Difference: %d / %d bits",
            percentage, original, flipped,
            uint32sToHexString(hash1),
            uint32sToHexString(hash2),
            differenceBits, totalBits)
    }
}

func TestCollisionResistance(t *TestContext) {
    hashes := make(map[string]string)
    for i := 0; i < 10000; i++ {
        input := fmt.Sprintf("input-%d", i)
        hash := uint32sToHexString(nar512(input))

        if prev, exists := hashes[hash]; exists {
            t.Fatalf("Collision found!\nHash: %s\nInput 1: %q\nInput 2: %q",
                hash, prev, input)
            return
        }
        hashes[hash] = input
    }
}

func TestSecondPreImageResistance(t *TestContext) {
    target := "fixed_input_string"
    targetHash := uint32sToHexString(nar512(target))

    for i := 0; i < 100000; i++ {
        guess := fmt.Sprintf("guess_%d", i)
        if guess == target {
            continue
        }
        guessHash := uint32sToHexString(nar512(guess))
        if guessHash == targetHash {
            t.Fatalf("Second pre-image found!\nOriginal Input: %q\nGuess: %q\nHash: %s",
                target, guess, targetHash)
            return
        }
    }
}

func TestLengthExtensionResistance(t *TestContext) {
    base := "data|"
    extra := "extra_secret"
    h1 := nar512(base)
    h2 := nar512(base + extra)

    if uint32sToHexString(h1) == uint32sToHexString(h2) {
        t.Errorf("Length extension detected!\nBase Input: %q\nExtended Input: %q\nHash: %s",
            base, base+extra, uint32sToHexString(h1))
    }
}

func TestStatisticalRandomness(t *TestContext) {
    bitCounts := make([]int, 512)
    rand.Seed(time.Now().UnixNano())

    for i := 0; i < 10000; i++ {
        input := fmt.Sprintf("input_%d", rand.Int())
        hash := nar512(input)
        for j, val := range hash {
            for k := 0; k < 32; k++ {
                if (val>>k)&1 == 1 {
                    bitCounts[j*32+k]++
                }
            }
        }
    }

    for i, count := range bitCounts {
        ratio := float64(count) / 10000.0
        if ratio < 0.45 || ratio > 0.55 {
            t.Errorf("Bit %d shows bias: set in %.2f%% of hashes (ideal is ~50%%)", i, ratio*100)
        }
    }
}

// Wrapper for tests
func runTest(name string, test func(*TestContext)) func() error {
    return func() error {
        t := &TestContext{name: name}
        test(t)
        if t.Failed() {
            return fmt.Errorf("test failed")
        }
        return nil
    }
}

// Main entry
func main() {
    tests := map[string]func() error{
        "Avalanche Effect":            runTest("Avalanche Effect", TestAvalancheEffect),
        "Collision Resistance":        runTest("Collision Resistance", TestCollisionResistance),
        "Second Pre-image Resistance": runTest("Second Pre-image Resistance", TestSecondPreImageResistance),
        "Length Extension Resistance": runTest("Length Extension Resistance", TestLengthExtensionResistance),
        "Statistical Randomness":      runTest("Statistical Randomness", TestStatisticalRandomness),
    }

    passed, failed := 0, 0
    fmt.Println("=== NAR-512 Security Test Summary ===")
    for name, test := range tests {
        err := test()
        if err != nil {
            fmt.Printf("[FAIL] %s: %v\n", name, err)
            failed++
        } else {
            fmt.Printf("[PASS] %s\n", name)
            passed++
        }
    }

    fmt.Printf("\nTotal: %d passed, %d failed\n", passed, failed)
}