
/* /usr/bin/clang++ -std=gnu++14 -stdlib=libc++ /Users/naresh/naresh/cs/code/nar512/nar512.cpp -o /Users/naresh/naresh/cs/code/nar512/nar512 */

#include <iostream>
#include <iomanip>
#include <bitset>
#include <math.h>
#include <chrono>

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

    int lenToFinish = targetSize - 8;
    int i = 0;
    while (padded.size() < targetSize - 8) {
        int numb = 0;
        for (int j = 0; j < 4; j++)
        numb += 10 * ((PI[(len + i++) % (PI.size() - 1)]) - '0');
        padded.push_back(bitset<8>((numb) % 255));
    }

    uint64_t bitLen = len;
    
    for (int i = 7; i >= 0; i--) {
        padded.push_back(bitset<8>((bitLen >> (i * 8)) & 0xFF));
    }

    // // for (auto i: padded) cout << i << endl;
    // cout << "padded: " << padded.size() << endl;
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
        // cout << hex << block.to_ulong(); // PRINT HEX VALUE HAHAHAHA
        compartment.push_back(block);
    }
    // cout << compartment.size() << endl;
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
        compartment[(i + 1) % size] = compartment[i] ^ sigma(compartment[(i + 2) % size]) | bitset<32>(fb) ^ M(compartment[(i + 7) % size]) ;
    }

    //  for(int i = 0; i < size; i++){
    //     cout << hex << compartment[i].to_ulong();
    // }
    // cout << "\" , ";
}

void initializeState(){
    vector<bitset<32>> temp1;
    for(int i = 0; i < compartment.size(); i += 2){
        uint32_t fb = 0;
        fibonacci(compartment[i].to_ulong() % INT8_MAX, 0, 1, fb);
        temp1.push_back(logisticMap(compartment[i].to_ulong() ^ compartment[(i + 3) % compartment.size()].to_ulong(), 3.97));
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

    for (auto i: states){
        cout << hex << setw(8) << setfill('0') << i.to_ulong();
    }
    cout << endl;

}

int main(void){

    
    string str = "";
    getline(cin, str);
    
    auto start = chrono::high_resolution_clock::now();
    padding(str);
    compartmentalize();
    salting();
    initializeState();

    cout << endl;
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "Time: " << duration.count() / 1e6 << " seconds" << endl;

}

/*
String: "Hello there, My name is Naresh Karthigeyan";
 before salting: 
48656c6c6f2074686572652c204d79206e616d65206973204e6172657368204b6172746869676579616e80f0829623dc78bea0a04182be648c0582c89696c8bec8b450aab4be8caa8c6482a06ef08c2dd278e6bed2058caadcaaf0bec828bebefab48296a0b48cd2969650bef0786405c8a06efab48ca0be5aa464682dca06eaad2be50be8ca005f0aaaa78d264c846fa96b405fad2dcd2e678d278e6c8be8c64dcd219aab4aae6b4d2be196ebef0c8b46edcbefa96b450c8c8c89696a0bedcbe968296d2966ea0965ad25a96e6be05a0d2a0aafe6aa96dca019b496788ce6596966e8cf05ab496a08ca0c87896dcf50c8b4c8aa96c8f0b4146e78b4d28c96f06ea064d28c05f08ca0be96968296aad2d2f0f0a019a0dcc8c832e6dcbe966edcfad2d296a0d2dcb40ff08cb4d2bea0a0b405f0968c6edc96dc6e82e6c88cbeaa82dc82dc646e8264fa28f0c8aa0596f0b4b4f0c896d2e69623967864bee6e682d26496c8a0a064b482b496b48cf0d2a0aad26eb4b4f0fa5a326eaa8cbea0c8be50be6ea0d219b478a0bec8b45a96bea019befaafa96c8d2c86ea0e6be82bea023aac8a0c8a0d2e62da096c8dc8cdc78a055a0aa8c5a2d96be8c6eaaf0be969696d26ed2829646aab446faa0e60f8cc8dcaadcbe82f0c8d2f08c64bea0e6c882be82aa96b4967896bebe8296bec8dc82aad2b48ca02d02a
after salting: 
eec7efa7e5a416cde81d6206ed61b290f88b2c32b77d813f71c69ec7fbb17ad258a857883d13cd60fb2d8b7c7a8493fdd47ab338d46a9e1afd6e4f56ebe02905bd55c33ef1357b3aed52fab9fd2cc086d62dddb54d907bbc6dc98bdff878348ef734a3efb99c2308d947d622d12be916fb64f251faaba68fd622dbc5a5ed0a2afb15071f7dbd9164e47c8b73fd50a87d73c8e46619d9a744eb3c49d1fa3337e1f7685e07d564fbebdd7aecfab5502612db3a573b7bbacc74aab9f399dc35ad687dc33ee8e51bb47a7d3fd69ca99e27cefd9ac80efd091778b0620eddf1685bc57dac998fdb85d48375c79db339636cc37a319996bd3074ceecea9d87b9978acb7d8079dfdc0495049d4c3e6e7d479e61eff7c5e61d9f1409fd5aafedf0f76083f515cafaf873961c7cae3836fe9ee00ecd6f60cd5cd7ad2cfd94bccfb97c2c03bc7a5172e8fdb232ebf73b9fc4940f59f569ca1edd800605d5fd33ee8d3ac5a6fca1280f99aca4ede9a95497fd24b391f97333541dbee89cdc1fba2240898d74ed1808c3c3818d85b39fdfefdd68fee89dadd4751bc16455cd8eb9e03de1fb486f0c7f6aedf6d084aca47900da2a8953ed0b2713957f2cf1f962f9b9bf4149b9f31c0520de1cd5a1d1290cc4d52e78317d620d38feae57c1fbffc480e57c028feb102e767d9aa6ff9dd20b65e7fa1e22faf1e5c9ec836770e4208d3db95b8b71

String: "Hello there, My name is Naresh Karthigeyan!"

before salting: 
48656c6c6f2074686572652c204d79206e616d65206973204e6172657368204b6172746869676579616e2180196ebef0c8b46edcbefa96b450c8c8c89696a0bedcbe968296d2966ea0965ad25a96e6be5a0d2a0aa0fe6aa96dca019b496788ce60596966e8cf05ab496a08ca0c87896dc0f50c8b4c8aa96c8f0b4146e78b4d28c96f06ea064d28c5f08ca0be96968296aad2d2f0f0a019a0dcc8c832e6dcbe966edcfad2d296a0d2dcb40ff08cb4d2bea0a0b45f0968c6edc96dc6e82e6c88cbeaa82dc82dc646e8264fa28f0c8aa596f0b4b4f0c896d2e69623967864bee6e682d26496c8a0a064b482b496b48cf0d2a0aad26eb4b4f0fa5a326eaa8cbea0c8be50be6ea0d219b478a0bec8b45a96bea019be0faafa96c8d2c86ea0e6be82bea023aac8a0c8a0d2e62da096c8dc8cdc78a055a0aa8c5a2d96be8c6eaaf0be969696d26ed2829646aab446faa0e6f8cc8dcaadcbe82f0c8d2f08c64bea0e6c882be82aa96b4967896bebe8296bec8dc82aad2b48ca02da05a5050fa788ca0f09682a09678e6f0aac896d278c846b4be9637dcd2dcbed28c0f7896d2c8be6eaabe4ba0b4b4d296dce6f0829623dc78bea0a04182be648c0582c89696c8bec8b450aab4be8caa8c6482a06ef08c2dd278e6bed2058caadcaaf0bec828bebefab48296a0b48cd2969650bef0786405c8a06efab48ca0be02b
after salting: 
ee6bc371e5a416cde81d60e7fc614d875ccfb2b7ce951882f53ce26ef56dc7b639ae12a615e50263e59f32e7b5bc8446c543cdbbacec4d17d587ae30ed3ae744bf3b8ae8f01915e6f4830e78f33a6f41dc5950f32a2bcd15eb6da248fc03b83aeb762691c610619aed656b2dbd729630f7574e1ab997aaeeb93c627bc5728f4be468a643bec046e8f63d18ebd38408c87e465aaa79c4fdea1da46aecf4be8820eeaf8b5e3f1f4008dae35e7c653251a7ccbbdfb17dae47dc4d5c5ca9fd6de5babd55a552fcdcb671d982d0c33daae69feebe7de7ee82b35cb83cbbcbc97ad34cc937d1a9958d80913c54ffef8f38299feb82f71fb686dbd2cf6564af83175aafe14165ea0cacfaf9703517f0e46fd2d45ffc6cb47c2cba77ab27d971e60a92beafde67bd879547fd12e90b6ccd7060e7ddf6d6ed4aadcdfbb0bf52d850bb6efbf113497ac1784cad3f86c17ddcaed6ac22cbeccd4206d6fc2ef047fc58439bfdfb6e6cb0e0faeec88dfa44bee14763fb6edb9dfd7027046f288cca72ca992abf5538cfab1d71936bd289336d0bc8c0b49ddcbdcd30d230dace31f2a7a15247dd331b135dc67ccf7cc2f35972723007f4f514cf36f03f9d698f05889ef49a19d46ef034e57b7afafc704038fb95b4d0f7742f22daaaaa5a5d4b5bcf7aecb4eeb0a1cbb8b6968eadf61fd6e8bc890762fd608e3dfcb85a3578bba0e8db522727

*/