#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;

vector<int> piMap;


void initializePiMap() {
    const string PI = "14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316527120190914564856692346034861045432664821339360726024914127372458700660631558817488152092096282925409171536436789259036001133053054882046652138414695194151160943305727036575959195309218611738193261179310511854807446237996274956735188575272489122793818301194912983367336244";
    for (uint32_t i = 0; i < PI.size() - 2; ++i) {
        int piVal = 0;
        for (int j = 0; j < 31; ++j) {
            piVal *= 10;
            piVal += (PI[(i + j) % (PI.size() - 2)] - '0');
        }
        piMap.push_back(abs(piVal % 511));
    }
}

int main(){
    initializePiMap();

    for(auto it : piMap){
        cout << it << ", ";
    }
    return 0;
}