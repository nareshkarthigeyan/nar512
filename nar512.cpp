#include <iostream>
#include <iomanip>
#include <bitset>
#include <math.h>

using namespace std;

void logisticMap(bitset<32> &block, float r);

vector<int> states = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
vector<bitset<8>> padded;
vector<bitset<32>> compartment;

const string PI = "1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091456485669234603486104543266482133936072602491412737245870066063155881748815209209628292540917153643678925903600113305305488204665213841469519415116094330572703657595919530921861173819326117931051185480744623799627495673518857527248912279381830119491298336733624406566430860213949463952247371907021798609437027705392171762931767523846748184676694051320005681271452635608277857713427577896091736371787214684409012249534301465495853710507922796892589235420199561121290219608640344181598136297747713099605187072113499999983729780499510597317328160963185950244594553469083026425223082533446850352619311881710100031378387528865875332083814206171776691473035982534904287554687311595628638823537875937519577818577805321712268066130019278766111959092164201989380952572010654858632788659361533818279682303019520353018529689957736225994138912497217752834791315155748572424541506959508295331168617278558890750983817546374649393192550604009277016711390098488240128583616035637076601047101819429555961989467678374494482553797747268471040475346462080466842590694912933136770289891521047521620569660240580381501935112533824300355876402474964732639141992726042699227967823547816360093417216412199245863150302861829745557067498385054945885869269956909272107975093029553211653449872027559602364806654991198818347977535663698074265425278625518184175746728909777727938000816470600161452491921732172147723501414419735685481613611573525521334757418494684385233239073941433345477624168625189835694855620992192221842725502542568876717904946016534668049886272327917860857843838279679766814541009538837863609506800642251252051173929848960841284886269456042419652850222106611863067442786220391949450471237137869609563643719172874677646575739624138908658326459958133904780275900994657640789512694683983525957098258226205224894077267194782684826014769909026401363944374553050682034962524517493996514314298091906592509372216964615157098583874105978859597729754989301617539284681382686838689427741559918559252459539594310499725246808459872736446958486538367362226260991246080512438843904512441365497627807977156914359977001296160894416948685558484063534220722258284886481584560285060168427394522674676788952521385225499546667278239864565961163548862305774564980355936345681743241125";

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

    // for (auto i: padded) cout << i << endl;
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
        cout << hex << block.to_ulong(); // PRINT HEX VALUE HAHAHAHA
        compartment.push_back(block);
    }
    cout << endl;
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
        cout << hex << i.to_ulong();
    }
    cout << endl;
}

int main(void){
    string str;
    getline(cin, str);
    padding(str);
    compartmentalize();
    parsing();

}

