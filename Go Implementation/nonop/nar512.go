package main

import (
	"math"
	"math/bits"
)

func ROTL(x uint32, n int) uint32 {
	return bits.RotateLeft32(x, n);
}

func ROTR(x uint32, n int) uint32 {
	return bits.RotateLeft32(x, -n);
}

var PI = []int{
	1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3, 2, 3, 8, 4, 6, 2, 6, 4, 3, 3, 8, 3, 2, 7, 9,
	5, 0, 2, 8, 8, 4, 1, 9, 7, 1, 6, 9, 3, 9, 9, 3, 7, 5, 1, 0, 5, 8, 2, 0, 9, 7, 4, 9, 4, 4,
	5, 9, 2, 3, 0, 7, 8, 1, 6, 4, 0, 6, 2, 8, 6, 2, 0, 8, 9, 9, 8, 6, 2, 8, 0, 3, 4, 8, 2, 5,
	3, 4, 2, 1, 1, 7, 0, 6, 7, 9, 8, 2, 1, 4, 8, 0, 8, 6, 5, 1, 3, 2, 8, 2, 3, 0, 6, 6, 4, 7,
	0, 9, 3, 8, 4, 4, 6, 0, 9, 5, 5, 0, 5, 8, 2, 2, 3, 1, 7, 2, 5, 3, 5, 9, 4, 0, 8, 1, 2, 8,
	4, 8, 1, 1, 1, 7, 4, 5, 0, 2, 8, 4, 1, 0, 2, 7, 0, 1, 9, 3, 8, 5, 2, 1, 1, 0, 5, 5, 5, 9,
	6, 4, 4, 6, 2, 2, 9, 4, 8, 9, 5, 4, 9, 3, 0, 3, 8, 1, 9, 6, 4, 4, 2, 8, 8, 1, 0, 9, 7, 5,
	6, 6, 5, 9, 3, 3, 4, 4, 6, 1, 2, 8, 4, 7, 5, 6, 4, 8, 2, 3, 3, 7, 8, 6, 7, 8, 3, 1, 6, 5,
	2, 7, 1, 2, 0, 1, 9, 0, 9, 1, 4, 5, 6, 4, 8, 5, 6, 6, 9, 2, 3, 4, 6, 0, 3, 4, 8, 6, 1, 0,
	4, 5, 4, 3, 2, 6, 6, 4, 8, 2, 1, 3, 3, 9, 3, 6, 0, 7, 2, 6, 0, 2, 4, 9, 1, 4, 1, 2, 7, 3,
	7, 2, 4, 5, 8, 7, 0, 0, 6, 6, 0, 6, 3, 1, 5, 5, 8, 8, 1, 7, 4, 8, 8, 1, 5, 2, 0, 9, 2, 0,
	9, 6, 2, 8, 2, 9, 2, 5, 4, 0, 9, 1, 7, 1, 5, 3, 6, 4, 3, 6, 7, 8, 9, 2, 5, 9, 0, 3, 6, 0,
	0, 1, 1, 3, 3, 0, 5, 3, 0, 5, 4, 8, 8, 2, 0, 4, 6, 6, 5, 2, 1, 3, 8, 4, 1, 4, 6, 9, 5, 1,
	9, 4, 1, 5, 1, 1, 6, 0, 9, 4, 3, 3, 0, 5, 7, 2, 7, 0, 3, 6, 5, 7, 5, 9, 5, 9, 1, 9, 5, 3,
	0, 9, 2, 1, 8, 6, 1, 1, 7, 3, 8, 1, 9, 3, 2, 6, 1, 1, 7, 9, 3, 1, 0, 5, 1, 1, 8, 5, 4, 8,
	0, 7, 4, 4, 6, 2, 3, 7, 9, 9, 6, 2, 7, 4, 9, 5, 6, 7, 3, 5, 1, 8, 8, 5, 7, 5, 2, 7, 2, 4,
	8, 9, 1, 2, 2, 7, 9, 3, 8, 1, 8, 3, 0, 1, 1, 9, 4, 9, 1, 2, 9, 8, 3, 3, 6, 7, 3, 6, 2, 4,
	4,
}

var fibCache [512]uint32

func init() {
	fibCache[0], fibCache[1] = 0, 1
	for i := 2; i < 512; i++ {
		fibCache[i] = fibCache[i-1] + fibCache[i-2]
	}
}

func nPi(index uint32) int {
	piVal := 0

	for i:= 0; i < 31; i++ {
		pos := (int(index) + i) % len(PI)
		digit := int(PI[pos]);
		piVal = piVal * 10 + digit;
	}

	return int(math.Abs(float64(piVal % 511)))
}

func padding(input string) []uint8 {
	soFar := len(input)
	totalLen := ((soFar + 511) / 512) * 512
	padded := make([]uint8, totalLen)

	var salt uint = 0

	for i := 0; i < soFar; i++ {
		ch := input[i]
		padded[i] = ch
		salt += ((salt << 5) + salt) + uint(ch)
	}

	// Padding start marker
	if soFar < totalLen {
		padded[soFar] = 1
	}

	i := 0
	for j := soFar + 1; j < totalLen; {
		numb := 0
		for k := 0; k < 4 && j < totalLen; k++ {
			index := (soFar + i + int(salt)) % len(PI)
			if index < 0 {
				index += len(PI)
			}
			numb += 10 * PI[index]
			i++
		}
		padded[j] = uint8(numb % 255)
		j++
	}
	return padded
}

func compartmentize(padded []uint8) []uint32 {
	blocks := make([]uint32, len(padded)/4)
	for i := 0; i < len(padded); i += 4 {
		blocks[i/4] = uint32(padded[i])<<24 | uint32(padded[i+1])<<16 | uint32(padded[i+2])<<8 | uint32(padded[i+3])
	}
	return blocks
}

func logisticMap(block uint32, r float64) uint32 {
	var x float64 = float64(block) / float64(math.MaxUint32)
	x = r * x * (1.0 - x)
	block = uint32(x * math.MaxUint32)
	return block
}

func sigma(block uint32) uint32 {
	piVal := nPi(block)
	block ^= ROTL(block, 17) ^ ROTR(block, 11) ^ (block * uint32(piVal)) ^ 3149757
	block ^= (block ^ (uint32(piVal) >> 2) * 37)
	block ^= (uint32(piVal) << 3)
	return block
}

func M(block uint32) uint32 {
	piVal := nPi(block);
	num := block
	block = logisticMap(block, math.Abs(float64(piVal) / 3.14))
	block ^= block << 17
	block ^= block >> 19
	block |= ROTL(num * uint32(piVal), int(math.Abs(float64(piVal) - 15)))

	return block
}


func salting(compartment []uint32) []uint32 {
	size := len(compartment)
	for i := 0; i < size; i++ {
		block := compartment[i]
		fib := fibCache[block%512]
		piVal := uint32(nPi(block))
		mixed := M(block)
		sig := sigma(block)
		chaotic := logisticMap(block^mixed^sig, 3.89)

		compartment[i] ^= chaotic ^ fib ^ ROTL(piVal, int(block%31))
	}
	return compartment
}

func stateCompression(compartment []uint32) []uint32 {
	var temp1 []uint32;
	for i := 0; i < len(compartment); i += 2 {
		block := logisticMap(compartment[i] ^ compartment[(i + 3) % len(compartment)], 3.99)
		temp1 = append(temp1, block)
	}
	var temp2 []uint32;

	for i := 0; i < len(temp1); i+=2 {
		fb := fibCache[temp1[(i + 1) % len(temp1)] % math.MaxInt8]
		piVal := nPi(fb)
		block := temp1[i] ^ temp1[(i + 2) % len(temp1)] | (fb / uint32(piVal))
		temp2 = append(temp2, block)
	}

	var states []uint32 = make([]uint32, 16);

	for i := 0; i < 16; i++ {
		it := (nPi(temp2[i % 16]) % 277) % math.MaxUint8;
		if it < 64 {
			it = 64
		}

		max := len(temp2)
		for j := 0; j < it; j++ {
			var lis float64 = float64(nPi(17) % 4)
			if lis / 4 == 0 {
				lis = 3.99
			}
			block := temp2[(i + 1) % max] ^ sigma(temp2[(i + 7) % max]) | M(temp2[(i + 11) % max]) ^ logisticMap(temp2[(i + 5) % max], lis)
			fb := fibCache[temp2[i] % 511]
			block = block ^ fb;
			states[i] = block;
		}
	}
	return states
}

func nar512(input string) []uint32 {
	padded := padding(input);
	compartment := compartmentize(padded);
	salted := salting(compartment)
	states := stateCompression(salted)
	return states
}

// func main(){
// 	var input string;
// 	fmt.Scan(&input)

// 	start := time.Now()
// 	hash := nar512(input)
// 	elapsed := time.Since(start)


// 	for _, ele := range hash {
// 		fmt.Printf("%08x", ele)
// 	}
// 	fmt.Print("\nTime Elapsed: ");
// 	fmt.Println(elapsed)

// }
