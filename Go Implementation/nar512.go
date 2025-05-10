package main

import (
	"fmt"
	"math"
	"math/bits"
	"time"
)

func ROTL(x uint32, n int) uint32 {
	return bits.RotateLeft32(x, n);
}

func ROTR(x uint32, n int) uint32 {
	return bits.RotateLeft32(x, -n);
}

const PI = "14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316527120190914564856692346034861045432664821339360726024914127372458700660631558817488152092096282925409171536436789259036001133053054882046652138414695194151160943305727036575959195309218611738193261179310511854807446237996274956735188575272489122793818301194912983367336244"

func nPi(index uint32) int {
	piVal := 0

	for i:= 0; i < 31; i++ {
		pos := (int(index) + i) % len(PI)
		digit := int(PI[pos] - '0');
		piVal = piVal * 10 + digit;
	}

	return int(math.Abs(float64(piVal % 511)))
}

func padding(input string) []uint8 {
	soFar := len(input)
	totalLen := ((soFar + 511) / 512) * 512

	var padded []uint8
	var salt uint =  0

	// copy(padded, []int8(input))

	for i := 0; i < soFar; i++ {
		padded = append(padded, uint8(input[i]))
		salt += ((salt << 5) + salt) + uint(input[i])
	}
	padded = append(padded, uint8(1))

	lenToFinish := totalLen
	i := 0
	for len(padded) < int(lenToFinish) {
		numb := 0
		for j := 0; j < 4; j++{
			index := (len(input) + i + int(salt)) % (len(PI) - 1)
			digit := int(PI[index] - '0')
			numb += 10 * digit
			i++
		}
		padded = append(padded, uint8(numb % 255))
	}

	return padded
}

func compartmentize(padded []uint8) []uint32 {
	var compartment []uint32;
	size := len(padded)
	for i := 0; i < size; i+= 4 {
		var block uint32 = 0
		for j := 0; j < 4; j++ {
			block <<= 8
			block = block | uint32(padded[i + j])
		}
		compartment = append(compartment, block)
	}
	return compartment
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

func fibonacci(n uint32) uint32 {
	if n < 2 {
		return n
	}
	var i uint32 = 0;
	var a uint32 = 0;
	var b uint32 = 1;
	for i = 0; i < n; i++ {
		temp := a + b
		a = b
		b = temp
	}
	return b
}

func salting(compartment []uint32) []uint32 {
	size := len(compartment)
	for i:= 0; i < size; i++ {
		compartment[i] = logisticMap(compartment[i], 3.99)
		fb := fibonacci(compartment[i] % 511)
		compartment[(i + 1) % size] = compartment[i] ^ sigma(compartment[(i + 2) % size]) ^ uint32(fb) ^ M(compartment[(i + 7) % size]);
 	}
	return compartment
}

func stateCompression(compartment []uint32) []uint32 {
	var temp1 []uint32;
	for i := 0; i < len(compartment); i += 2 {
		// fb := fibonacci(compartment[i] % math.MaxInt8)
		// piVal := nPi(fb)
		block := logisticMap(compartment[i] ^ compartment[(i + 3) % len(compartment)], 3.99)
		temp1 = append(temp1, block)
	}
	var temp2 []uint32;

	for i := 0; i < len(temp1); i+=2 {
		fb := fibonacci(temp1[(i + 1) % len(temp1)] % math.MaxInt8)
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
			fb := fibonacci(temp2[i] % 511)
			block = block ^ fb;
			states[i] = block;
		}
	}
	return states
}



func main(){
	var input string
	fmt.Scan(&input)

	// start := time.Now()

	padded := padding(input);
	compartment := compartmentize(padded);
	salted := salting(compartment)
	states := stateCompression(salted)

	// elapsed := time.Since(start)


	for _, ele := range states {
		fmt.Printf("%08x", ele)
	}
	// fmt.Print("\nTime Elapsed: ");
	// fmt.Println(elapsed)

}
