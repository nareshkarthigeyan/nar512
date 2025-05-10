
/* /usr/bin/clang++ -std=gnu++14 -stdlib=libc++ /Users/naresh/naresh/cs/code/nar512/nar512.cpp -o /Users/naresh/naresh/cs/code/nar512/nar512 */

#include <iostream>
#include <iomanip>
#include <bitset>
#include <math.h>
#include <chrono>
#include <sstream>
#include <vector>

#define ROTL(x, n, w) (((x) << (n)) | ((x) >> ((w) - (n)))) & UINT32_MAX
#define ROTR(x, n, w) (((x) >> (n)) | ((x) << ((w) - (n)))) & UINT32_MAX

using namespace std;

bitset<32> logisticMap(bitset<32> block, float r);


vector<bitset<32>> states = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
vector<bitset<8>> padded;
vector<bitset<32>> compartment;

const string PI = "14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316527120190914564856692346034861045432664821339360726024914127372458700660631558817488152092096282925409171536436789259036001133053054882046652138414695194151160943305727036575959195309218611738193261179310511854807446237996274956735188575272489122793818301194912983367336244";

int nPi(uint32_t index)
{
    int piVal = 0;
    for (int i = 0; i < 31; i++)
    {   piVal *= 10;
        piVal += (PI[index++ % PI.size() - 2] - '0');
    }
    return abs(piVal % 511);
}

void padding(string input){
    padded.clear();
    int len = input.size();
    unsigned int salt = 0;
    for (int i = 0; i < len; i++){
        padded.push_back(bitset<8> (input[i]));
        salt += (salt << 5) + salt + input[i];
    }
    padded.push_back(bitset<8>(0b10000000));

    int soFar = input.size();
    int n = 9;
    int targetSize = 0;

    while(targetSize < soFar){
        targetSize = pow(2, n++);
    }

    int lenToFinish = targetSize - 8;
    int i = 0;
    while (padded.size() < targetSize - 8) {
        int numb = 0;
        for (int j = 0; j < 4; j++)
        numb += 10 * ((PI[(len + i++ + salt) % (PI.size() - 1)]) - '0');
        padded.push_back(bitset<8>((numb) % 255));
    }

    uint64_t bitLen = len;
    
    for (int i = 7; i >= 0; i--) {
        padded.push_back(bitset<8>((bitLen >> (i * 8)) & 0xFF));
    }

}

void compartmentalize(){
    compartment.clear();
    int size = padded.size();
    for(int i = 0; i < size; i += 4){
        bitset<32> block {0};
        for(int j = 0; j < 4; j++){
            block <<= 8;
            block |= padded[i + j].to_ulong();
        }
        compartment.push_back(block);
    }
}

bitset<32> logisticMap(bitset<32> block, float r){
    double x = (double) block.to_ullong() / (double) UINT64_MAX;
    x = r * x * (1.0 - x);
    block = bitset<32>(static_cast<uint64_t>(x * UINT64_MAX));
    return block;
}


bitset<32> sigma(bitset<32> block){

    int piVal = nPi(block.to_ulong());
    block ^= ROTL(block.to_ulong(), 17, 32);
    block ^= ROTR(block.to_ulong(), 11, 32);
    block ^= ((block.to_ulong() * piVal) + 3149757) % INT32_MAX;
    block |= (block.to_ulong() ^ (piVal >> 2) * 37) % INT32_MAX;
    block ^= (piVal << 3);

    return block;
}

bitset<32> M(bitset<32> block){
    int piVal = nPi(block.to_ulong());
    uint32_t num = block.to_ulong();
    block = logisticMap(block, abs(piVal / 3.14));
    block ^= block << 17;
    block ^= block >> 19;
    block |= ROTL(num * piVal, abs(piVal - 15), 32);

    return block;
}

uint32_t fibonacci(uint32_t n, int prev1, int prev2, uint32_t &answer){
    if(n < 3) return 0;
    int current = prev1 + prev2;
    answer += current;
    return fibonacci(n - 1, prev2, current, answer);
}

void salting(){
    int size = compartment.size();

    for (int i = 0; i < size; i++){
        compartment[i] = logisticMap(compartment[i], 3.99);
        uint32_t fb;
        fibonacci(compartment[i].to_ulong() % 511, 0, 1, fb);  
        compartment[(i + 1) % size] = compartment[i] ^ sigma(compartment[(i + 2) % size]) ^ bitset<32>(fb) ^ M(compartment[(i + 7) % size]) ;
    }

    //  for(int i = 0; i < size; i++){
    //     cout << hex << compartment[i].to_ulong();
    // }
    // cout << "\" , ";
}

void stateCompression(){
    vector<bitset<32>> temp1;
    for(int i = 0; i < compartment.size(); i += 2){
        uint32_t fb = 0;
        fibonacci(compartment[i].to_ulong() % INT8_MAX, 0, 1, fb);
        temp1.push_back(logisticMap(compartment[i].to_ulong() ^ compartment[(i + 3) % compartment.size()].to_ulong(), 3.99));
    }
    // cout << "Compartment: " << compartment.size() << "\nTemp1: " << temp1.size() << endl;
    vector<bitset<32>> temp2;
    for(int i = 0; i < temp1.size(); i+= 2){
        uint32_t fb = 0;
        fibonacci(temp1[(i + 1) % temp1.size()].to_ulong() % INT8_MAX, (int) temp1[(i + 3) % temp1.size()].to_ulong() % INT8_MAX, 1, fb);
        uint32_t piVal = nPi(fb);
        bitset<32> block ((uint32_t) temp1[i].to_ulong() ^ temp1[(i + 2) % temp1.size()].to_ulong() | (uint32_t) (fb / piVal));
        temp2.push_back(block);
    }

    for(int i = 0; i < states.size(); i++){
        int iterations = (nPi((int) temp2[i % states.size()].to_ulong() % INT8_MAX) % 277);
        iterations = iterations < 64 ? 64 : iterations; 
        int max = temp2.size();
        for(int j = 0; j < iterations; j++){
            bitset<32> block = temp2[(i + 1) % max] ^ sigma(temp2[(i + 7) % max]) | M(temp2[(i + 11) % max]) ^ logisticMap(temp2[(i + 5) % max], ((nPi(17) % 4) / 4 == 0 ? 3.99 : (nPi(17) % 4) / 4));
            uint32_t fb = temp2[i].to_ulong();
            fibonacci(fb % 511, 0, 1, fb);
            block = block.to_ulong() ^ fb;
            states[i] = block;
        }
    }

    // for (auto i: states){
    //     cout << hex << setw(8) << setfill('0') << i.to_ulong();
    // }
    // cout << endl;

}

string toHex (vector<bitset<32>> vec){
    ostringstream oss;
    for (auto i: vec){
        oss << hex  << setw(8) << setfill('0') << i.to_ulong();
    }
    return oss.str();
}

string nar512(string input){
    padding(input);
    compartmentalize();
    salting();
    stateCompression();
    return toHex(states);
    // return "";
}

int main(void){
    string str = "";
    getline(cin, str);
    
    auto start = chrono::high_resolution_clock::now();
    
    string output;
    cout << nar512(str) << endl;
    // uint32_t nonce = 0;
    // string target = "00"; // Adjust the difficulty by changing the number of leading zeros
    // do {
    //     cout << nonce << ": ";
    //     string attempt = str + to_string(nonce);
    //     cout << attempt << endl;
    //     output = nar512(attempt);
    //     cout << output << endl;
    //     nonce++;
    // } while (output.substr(0, target.size()) != target);

    // cout << "Hash: " << output << endl;
    // cout << "Nonce: " << nonce - 1 << endl;

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "Time: " << duration.count() / 1e6 << " seconds" << endl;
    return 0;   
}