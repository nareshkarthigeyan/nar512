#include <iostream>
#include <bitset>
#include <math.h>

using namespace std;

vector<int> states = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
vector<bitset<8>> padded;
vector<bitset<64>> compartment;

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

    cout << targetSize;

   while (padded.size() < targetSize - 8) {
        padded.push_back(bitset<8>(0));
    }

    uint64_t bitLen = len * 8;

    for (int i = 7; i >= 0; i--) {
        padded.push_back(bitset<8>((bitLen >> (i * 8)) & 0xFF));
    }

    cout << endl;
    // cout << padded.size() << endl;
}

void compartmentalize(){
    int size = padded.size();
    for(int i = 0; i < size; i += 8){
        bitset<64> block {0};
        for(int j = 0; j < 8; j++){
            block <<= 8;
            block |= padded[i + j].to_ulong();
        }
        cout << block << endl;
        compartment.push_back(block);
    }
    cout << compartment.size() << endl;
}



int main(void){
    string str;
    getline(cin, str);
    padding(str);
    compartmentalize();
}