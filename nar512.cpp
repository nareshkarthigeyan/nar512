#include <iostream>
#include <bitset>
#include <math.h>

using namespace std;

vector<int> states = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
vector<bitset<8>> padded;
vector<bitset<32>> compartment;

void computeSalt(string input){
    int len = input.size();
    int n = 1;
    // cout << input << endl;
    // cout << len << endl;
    while (len > (n * n)) n++;
    int matrix[n][n], index {0};
    // COMPUTING ADJACENCY MATRIX OF GIVEN INPUT, THEREBY CONVERTING IT TO A GRAPH
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(i + j >= len) matrix[i][j] = 0;
            else matrix[i][j] = (char) input[index++]; 
            cout << matrix[i][j] << '\t';
        }
        cout << endl;
    }
    //TODO: USE PRIMS ALGORITHM TO FIND LOWEST SPANNING TREE, THEREBY COMPUTING THE SALT.
}

void padding(string input){
    int len = input.size();
    for (int i = 0; i < len; i++){
        padded.push_back(bitset<8> (input[i]));
    }
    padded.push_back(bitset<8>(0b10000000));

    int soFar = input.size();
    int n = 9;
    int targetSize = 0;

    while(targetSize < soFar){
        targetSize = pow(2, n++);
    }

    // cout << targetSize;

   while (padded.size() < targetSize - 8) {
        padded.push_back(bitset<8>(0));
    }

    uint64_t bitLen = len;
    
    for (int i = 7; i >= 0; i--) {
        padded.push_back(bitset<8>((bitLen >> (i * 8)) & 0xFF));
    }
    cout << endl;

    cout << "compartments: " << endl;
    for (auto i : compartment) 
    {
        cout << i << endl;
    }
    // cout << padded.size() << endl;
}

void compartmentalize(){
    int size = padded.size();
    for(int i = 0; i < size; i += 4){
        bitset<32> block {0};
        for(int j = 0; j < 4; j++){
            block <<= 8;
            block |= padded[i + j].to_ulong();
        }
        cout << block << endl;
        compartment.push_back(block);
    }
    // cout << compartment.size() << endl;
}

void logisticMap(bitset<32> &block, float r){
    double x = (double) block.to_ullong() / (double) UINT64_MAX;
    x = r * x * (1.0 - x);
    block = bitset<32>(static_cast<uint64_t>(x * UINT64_MAX));
    return;

    /*
        0110000101100010011000111000000000000000000000000000000000000000
        1111000011000000010000011010101110011010101111101000100000000000
    
        0000000000000000000000000000000000000000000000000000000000000011
        0000000000000000000000000000000000000000000000000000000000001011
    */
}


void parsing(){
    cout << "Losgistic: " << endl;
    for (auto i : compartment){
        logisticMap(i, 3.99);
        cout << i << endl;
    }
}

int main(void){
    string str;
    getline(cin, str);
    // padding(str);
    // compartmentalize();
    // parsing();
    computeSalt(str);
}

