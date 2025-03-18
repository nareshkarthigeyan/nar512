#include <iostream>
#include <iomanip>
#include <bitset>
#include <math.h>
#include <chrono>
#include <sstream>
#include <vector>
#include <unordered_map>

#define ROTL(x, n, w) (((x) << (n)) | ((x) >> ((w) - (n)))) & UINT32_MAX
#define ROTR(x, n, w) (((x) >> (n)) | ((x) << ((w) - (n)))) & UINT32_MAX

using namespace std;

uint32_t logisticMap(uint32_t block, float r);

vector<uint32_t> states = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
vector<uint8_t> padded(512, 0);
vector<uint32_t> compartment(128, 0);

vector<int> piMap = {341, 296, 232, 252, 324, 90, 292, 315, 458, 381, 95, 416, 484, 295, 292, 159, 497, 254, 463, 33, 271, 163, 508, 126, 280, 289, 243, 260, 424, 132, 277, 449, 276, 176, 348, 311, 60, 74, 155, 420, 0, 166, 430, 124, 411, 24, 128, 202, 334, 358, 21, 323, 166, 39, 390, 297, 395, 333, 265, 100, 492, 298, 168, 395, 292, 313, 80, 324, 69, 451, 193, 144, 158, 434, 260, 83, 183, 259, 130, 376, 379, 151, 47, 468, 46, 147, 406, 361, 420, 502, 202, 81, 337, 343, 223, 38, 326, 130, 402, 175, 347, 276, 461, 141, 358, 452, 339, 218, 65, 25, 197, 290, 244, 259, 510, 154, 49, 54, 31, 254, 155, 55, 503, 416, 470, 62, 470, 54, 476, 15, 417, 58, 36, 199, 395, 380, 448, 136, 299, 326, 202, 412, 18, 378, 389, 313, 484, 118, 19, 408, 497, 240, 183, 179, 207, 56, 54, 472, 510, 414, 39, 290, 240, 305, 176, 166, 119, 406, 458, 137, 207, 138, 216, 419, 433, 284, 322, 396, 264, 64, 436, 329, 420, 504, 163, 481, 392, 188, 286, 176, 145, 404, 427, 159, 451, 320, 14, 459, 59, 477, 72, 341, 191, 371, 481, 452, 349, 327, 393, 265, 19, 197, 105, 501, 299, 324, 472, 486, 273, 142, 165, 485, 362, 113, 499, 173, 171, 151, 142, 275, 361, 396, 297, 128, 410, 503, 216, 489, 267, 19, 405, 193, 134, 305, 465, 25, 208, 439, 214, 50, 357, 475, 42, 365, 52, 450, 147, 184, 209, 8, 80, 368, 21, 137, 292, 257, 399, 256, 98, 392, 350, 107, 411, 47, 177, 345, 256, 388, 213, 483, 344, 275, 399, 270, 409, 51, 57, 492, 269, 64, 450, 388, 264, 84, 235, 260, 13, 388, 274, 137, 245, 243, 320, 126, 92, 304, 108, 446, 312, 469, 4, 41, 353, 449, 295, 129, 329, 196, 241, 231, 129, 148, 204, 166, 20, 382, 428, 348, 360, 438, 228, 324, 111, 59, 46, 211, 55, 489, 236, 215, 441, 261, 480, 388, 320, 453, 123, 329, 155, 446, 307, 47, 97, 136, 268, 406, 473, 435, 366, 20, 209, 50, 90, 240, 243, 238, 265, 496, 290, 253, 441, 197, 234, 259, 100, 427, 61, 29, 287, 261, 18, 354, 399, 153, 72, 106, 89, 237, 251, 325, 104, 500, 367, 435, 126, 283, 226, 456, 383, 367, 500, 159, 465, 42, 402, 325, 119, 392, 257, 138, 182, 142, 318, 476, 80, 272, 13, 434, 240, 348, 113, 442, 191, 202, 413, 491, 325, 382, 108, 13, 108, 471, 455, 354, 339, 278, 91, 248, 353, 411, 59, 486, 125, 438, 237, 209, 483, 337, 286, 274, 129, 181, 262, 35, 320, 77, 371, 50, 412, 49, 482, 319, 483, 295, 149, 365, 450, 236, 237, 248, 365, 1, 396, 189, 208, 69, 128, 384, 189, 263, 499, 156, 12, 498, 299, 170, 81, 297, 173, 160, 43, 154, 66, 140, 231, 263, 509, 76, 378, 161, 494, 299, 200, 159, 491, 255, 146, 324, 456, 353};

// void initializePiMap() {
//     const string PI = "14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316527120190914564856692346034861045432664821339360726024914127372458700660631558817488152092096282925409171536436789259036001133053054882046652138414695194151160943305727036575959195309218611738193261179310511854807446237996274956735188575272489122793818301194912983367336244";
//     for (uint32_t i = 0; i < PI.size() - 2; ++i) {
//         int piVal = 0;
//         for (int j = 0; j < 31; ++j) {
//             piVal *= 10;
//             piVal += (PI[(i + j) % (PI.size() - 2)] - '0');
//         }
//         piMap[i] = abs(piVal % 511);
//     }
// }

int nPi(uint32_t index) {
    return piMap[index % (piMap.size() - 1)];
}

void padding(string input) {
    padded.resize(512, 0);
    int len = input.size();
    unsigned int salt = 0;
    for (int i = 0; i < len; i++) {
        padded.push_back(uint8_t(input[i]));
        salt += (salt << 5) + salt + input[i];
    }
    padded.push_back(uint8_t(0b10000000));

    int soFar = input.size();
    int n = 9;
    int targetSize = 0;

    while (targetSize < soFar) {
        targetSize = pow(2, n++);
    }

    int lenToFinish = targetSize - 8;
    int i = 0;
    while (padded.size() < targetSize - 8) {
        int numb = 0;
        for (int j = 0; j < 4; j++)
            numb += 10 * ((piMap[(len + i++ + salt) % (piMap.size() - 1)]) - '0');
        padded.push_back(uint8_t((numb) % 255));
    }

    uint64_t bitLen = len;

    for (int i = 7; i >= 0; i--) {
        padded.push_back(uint8_t((bitLen >> (i * 8)) & 0xFF));
    }
}

void compartmentalize() {
    compartment.resize(128, 0);
    int size = padded.size();
    for (int i = 0; i < size; i += 4) {
        uint32_t block{0};
        for (int j = 0; j < 4; j++) {
            block <<= 8;
            block = block | padded[i + j];
        }
        compartment.push_back(block);
    }
}

uint32_t logisticMap(uint32_t block, float r) {
    double x = (double)(uint64_t)block / (double)UINT64_MAX;
    x = r * x * (1.0 - x);
    block = uint32_t(static_cast<uint64_t>(x * UINT64_MAX));
    return block;
}

uint32_t sigma(uint32_t block) {
    int piVal = nPi(block);
    block ^= ROTL(block, 17, 32) ^ ROTR(block, 11, 32) ^ (block * piVal) ^ 3149757;
    block ^= (block ^ (piVal >> 2) * 37);
    block ^= (piVal << 3);
    return block; 
}

uint32_t M(uint32_t block) {
    int piVal = nPi(block);
    uint32_t num = block;
    block = logisticMap(block, abs(piVal / 3.14));
    block ^= block << 17;
    block ^= block >> 19;
    block |= ROTL(num * piVal, abs(piVal - 15), 32);

    return block;
}

uint32_t fibonacci(uint32_t n) {
    if (n < 2) return n;
    uint32_t a = 0, b = 1;
    for (uint32_t i = 2; i <= n; i++) {
        uint32_t temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

void salting() {
    int size = compartment.size();

    for (int i = 0; i < size; i++) {
        compartment[i] = logisticMap(compartment[i], 3.99);
        uint32_t fb;
        fb = fibonacci(compartment[i] % 511);
        compartment[(i + 1) % size] = compartment[i] ^ sigma(compartment[(i + 2) % size]) ^ uint32_t(fb) ^ M(compartment[(i + 7) % size]);
    }
}

void stateCompression() {
    vector<uint32_t> temp1;
    for (int i = 0; i < compartment.size(); i += 2) {
        uint32_t fb = 0;
        fb = fibonacci(compartment[i] % INT8_MAX);
        temp1.push_back(logisticMap(compartment[i] ^ compartment[(i + 3) % compartment.size()], 3.99));
    }
    vector<uint32_t> temp2;
    for (int i = 0; i < temp1.size(); i += 2) {
        uint32_t fb = 0;
        fb = fibonacci(temp1[(i + 1) % temp1.size()] % INT8_MAX);
        uint32_t piVal = nPi(fb);
        uint32_t block((uint32_t)temp1[i] ^ temp1[(i + 2) % temp1.size()] | (uint32_t)(fb / piVal));
        temp2.push_back(block);
    }

    for (int i = 0; i < states.size(); i++) {
        int iterations = (nPi((int)temp2[i % states.size()] % INT8_MAX) % 277);
        iterations = iterations < 64 ? 64 : iterations;
        int max = temp2.size();
        for (int j = 0; j < iterations; j++) {
            uint32_t block = temp2[(i + 1) % max] ^ sigma(temp2[(i + 7) % max]) | M(temp2[(i + 11) % max]) ^ logisticMap(temp2[(i + 5) % max], ((nPi(17) % 4) / 4 == 0 ? 3.99 : (nPi(17) % 4) / 4));
            uint32_t fb = temp2[i];
            fb = fibonacci(fb % 511);
            block = block ^ fb;
            states[i] = block;
        }
    }
}

string toHex(vector<uint32_t> vec) {
    ostringstream oss;
    for (auto i : vec) {
        oss << hex << setw(8) << setfill('0') << i;
    }
    return oss.str();
}

string nar512(string input) {
    padding(input);
    compartmentalize();
    salting();
    stateCompression();
    return toHex(states);
}

// int main(void) {
//     initializePiMap();
//     string str = "";
//     getline(cin, str);

//     auto start = chrono::high_resolution_clock::now();

//     string output;
//     cout << nar512(str) << endl;

//     auto end = chrono::high_resolution_clock::now();
//     auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
//     cout << "Time: " << duration.count() / 1e6 << " seconds" << endl;
//     return 0;
// }
