#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define ROTL(x, n, w) (((x) << (n)) | ((x) >> ((w) - (n))))
#define ROTR(x, n, w) (((x) >> (n)) | ((x) << ((w) - (n))))

const uint32_t LAST32BIT = 4294967295U;

const char *piDigits = "14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316271201909145648566923460348610454326648213393607260249141273";

void to_binary(uint32_t num, char *out, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        out[bits - 1 - i] = (num & (1 << i)) ? '1' : '0';
    }
    out[bits] = '\0';
}

uint16_t nPi(int n) {
    size_t len = strlen(piDigits);
    n = n % len;
    int sliceLength = (n % 7) + 4;
    char slice[11] = {0};
    strncpy(slice, piDigits + n, sliceLength);
    slice[sliceLength] = '\0'; 
    return (uint16_t)(atoi(slice) % 65536);
}

uint32_t sigma(uint32_t num) {
    uint16_t piVal = nPi(num);
    uint32_t a = ROTR(num, 19, 32) ^ ROTL(num, 13, 32);
    uint32_t b = (num * piVal + 314159) % LAST32BIT;
    uint32_t c = num ^ ((piVal >> 2) * 37);
    return (a ^ b ^ c) % LAST32BIT;
}

uint32_t bigM(uint32_t num) {
    return ROTR(num, 23, 32) ^ ROTL(num, 11, 32) ^ ((num * nPi(num)) % LAST32BIT);
}

uint32_t goldenRatio() {
    return (uint32_t)(1.6180339887 * 100000000);  // Scaled golden ratio
}

void padding(const char *input, char *output) {
    char bin[2048] = {0};
    char temp[9];

    for (size_t i = 0; input[i] != '\0'; i++) {
        to_binary((unsigned char)input[i], temp, 8);
        strcat(bin, temp);
    }

    strcat(bin, "1");

    while (strlen(bin) % 512 != 384) {
        strcat(bin, "0");
    }

    char length_bin[129] = {0};
    to_binary(strlen(input) * 8, length_bin, 128);
    strcat(bin, length_bin);

    strcpy(output, bin);
}

void compartmentalize(const char *input, char output[][33], size_t *count) {
    size_t len = strlen(input);
    *count = (len + 31) / 32;
    for (size_t i = 0; i < *count; i++) {
        strncpy(output[i], input + i * 32, 32);
        output[i][32] = '\0';
    }
}

void parsing(uint32_t *input, size_t len, uint32_t *res) {
    memcpy(res, input, len * sizeof(uint32_t));
    for (size_t i = len; i < 64; i++) {
        uint32_t val = sigma(res[i % len]) ^ bigM(res[(i + 8) % len]);
        val = (val + ROTL(val + i % 17, i % 32, 32)) & LAST32BIT;
        val = (val + res[i - 16]) % LAST32BIT;
        val ^= ROTR(res[i - 2] * 19, i % 32, 32);
        res[i] = val % LAST32BIT;
    }
}

void salting(uint32_t *input, size_t len, uint32_t *res) {
    uint32_t gr = goldenRatio();
    for (size_t i = 0; i < len; i++) {
        uint32_t a = input[i];
        uint32_t index = input[(i + 1) % len];
        uint32_t b = input[(i + index) % len];
        res[i] = (a * gr + b) % LAST32BIT;
    }
}

void compression(uint32_t *input, uint32_t *state) {
    for (int i = 0; i < 64; i++) {
        uint32_t t1 = (input[i] + sigma(state[4]) + bigM(state[6]) + ROTL(state[0], 5, 32)) % LAST32BIT;
        uint32_t t2 = bigM(state[9]) + sigma(state[11]) + ROTR(state[13], 7, 32) % LAST32BIT;

        uint32_t new_state[16];
        memcpy(new_state, state + 1, 15 * sizeof(uint32_t));
        new_state[15] = (state[11] + t1) % LAST32BIT;
        new_state[10] = (t1 + t2) % LAST32BIT;
        new_state[7] = (state[3] + t1) % LAST32BIT;
        new_state[0] = (t1 + t2) % LAST32BIT;
        memcpy(state, new_state, 16 * sizeof(uint32_t));
    }
}

void toHash(uint32_t *state, char *output) {
    for (int i = 0; i < 16; i++) {
        char temp[9];
        snprintf(temp, sizeof(temp), "%08x", state[i]);
        strcat(output, temp);
    }
}

void nar512(char *input, char *output) {
    char padded[1024] = {0};
    padding(input, padded);

    char segmented[64][33] = {0};
    size_t seg_count;
    compartmentalize(padded, segmented, &seg_count);

    uint32_t parsed[64] = {0};
    parsing((uint32_t *)segmented, seg_count, parsed);

    uint32_t salted[64] = {0};
    salting(parsed, 64, salted);

    uint32_t state[16] = {
       0xcbbb9d5d, 0x629a292a, 0x9159015a, 0x152fecd8,
       0x67332667, 0x8eb44a87, 0xdb0c2e0d, 0x47b5481d,
       0xe19b48a8, 0xc5c95a63, 0xa1e38b93, 0x29d070bb,
       0x5c26c926, 0x33a7d98e, 0x4aab5db7, 0x4f7a04dd
    };

    compression(salted, state);
    toHash(state, output);
}

int main(void){
    char * value = "Naresh Karthigeyanb hahaha";
    char hash[129] = {0};
    clock_t start = clock();
    nar512(value, hash);
    clock_t end = clock();
    printf("NAR512 for %s is: %s\n", value,  hash);
    printf("Length of hashed value: %lu\n", strlen(hash));
    printf("Time taken for NAR512: %f seconds\n", ((double)(end - start)) / CLOCKS_PER_SEC);
    return 0;
}