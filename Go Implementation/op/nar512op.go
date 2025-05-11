package main

import (
	"fmt"
	"math"
	"math/bits"
)

const PI = "14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316527120190914564856692346034861045432664821339360726024914127372458700660631558817488152092096282925409171536436789259036001133053054882046652138414695194151160943305727036575959195309218611738193261179310511854807446237996274956735188575272489122793818301194912983367336244"

var (
	PI_DIGITS = make([]int, len(PI))
	fibCache  = make([]uint32, 512)
	nPiCache  = map[uint32]int{}
)

func init() {
	for i := range PI {
		PI_DIGITS[i] = int(PI[i] - '0')
	}
	fibCache[0], fibCache[1] = 0, 1
	for i := 2; i < 512; i++ {
		fibCache[i] = fibCache[i-1] + fibCache[i-2]
	}
}

func ROTL(x uint32, n int) uint32 {
	return bits.RotateLeft32(x, n)
}
func ROTR(x uint32, n int) uint32 {
	return bits.RotateLeft32(x, -n)
}

func nPi(index uint32) int {
	if val, ok := nPiCache[index]; ok {
		return val
	}
	piVal := 0
	for i := 0; i < 31; i++ {
		pos := (int(index) + i) % len(PI_DIGITS)
		piVal = piVal*10 + PI_DIGITS[pos]
	}
	result := int(math.Abs(float64(piVal % 511)))
	nPiCache[index] = result
	return result
}

func padding(input string) []uint8 {
	soFar := len(input)
	totalLen := ((soFar + 511) / 512) * 512
	padded := make([]uint8, totalLen)

	var salt uint = 0
	for i := 0; i < soFar; i++ {
		ch := uint8(input[i])
		padded[i] = ch
		salt += ((salt << 5) + salt) + uint(input[i])
	}
	padded[soFar] = 1

	i := 0
	for j := soFar + 1; j < totalLen; {
		numb := 0
		for k := 0; k < 4; k++ {
			index := (soFar + i + int(salt)) % (len(PI_DIGITS) - 1)
			numb += 10 * PI_DIGITS[index]
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
	x := float64(block) / float64(math.MaxUint32)
	x = r * x * (1.0 - x)
	return uint32(x * math.MaxUint32)
}

func sigma(block uint32) uint32 {
	piVal := uint32(nPi(block))
	block ^= ROTL(block, 17) ^ ROTR(block, 11) ^ (block * piVal) ^ 3149757
	block ^= (block ^ ((piVal >> 2) * 37))
	block ^= (piVal << 3)
	return block
}

func M(block uint32) uint32 {
	piVal := uint32(nPi(block))
	num := block
	block = logisticMap(block, float64(piVal)/3.14)
	block ^= block << 17
	block ^= block >> 19
	block |= ROTL(num*piVal, int(math.Abs(float64(piVal)-15)))
	return block
}

func salting(compartment []uint32) []uint32 {
	size := len(compartment)
	for i := 0; i < size; i++ {
		compartment[i] = logisticMap(compartment[i], 3.99)
		fb := fibCache[compartment[i]&511]
		compartment[(i+1)%size] = compartment[i] ^ sigma(compartment[(i+2)%size]) ^ fb ^ M(compartment[(i+7)%size])
	}
	return compartment
}

func stateCompression(compartment []uint32) []uint32 {
	temp1 := make([]uint32, len(compartment)/2)
	for i := 0; i < len(compartment); i += 2 {
		temp1[i/2] = logisticMap(compartment[i]^compartment[(i+3)%len(compartment)], 3.99)
	}

	temp2 := make([]uint32, len(temp1)/2)
	for i := 0; i < len(temp1); i += 2 {
		fb := fibCache[temp1[(i+1)%len(temp1)]%math.MaxInt8]
		piVal := nPi(fb)
		temp2[i/2] = temp1[i] ^ temp1[(i+2)%len(temp1)] | (fb / uint32(piVal))
	}

	states := make([]uint32, 16)
	for i := 0; i < 16; i++ {
		it := nPi(temp2[i%len(temp2)]) % 277
		if it < 64 {
			it = 64
		}
		for j := 0; j < it; j++ {
			lis := float64(nPi(17) % 4)
			if lis == 0 {
				lis = 3.99
			}
			block := temp2[(i+1)%len(temp2)] ^ sigma(temp2[(i+7)%len(temp2)]) | M(temp2[(i+11)%len(temp2)]) ^ logisticMap(temp2[(i+5)%len(temp2)], lis)
			fb := fibCache[temp2[i]&511]
			states[i] ^= block ^ fb
		}
	}
	return states
}

func main() {
	var input string
	fmt.Scan(&input)

	padded := padding(input)
	compartment := compartmentize(padded)
	salted := salting(compartment)
	states := stateCompression(salted)

	for _, ele := range states {
		fmt.Printf("%08x", ele)
	}
}