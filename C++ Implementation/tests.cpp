#include <iostream>
#include <iomanip>
#include <bitset>
#include <chrono>
#include <random>
#include <algorithm>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <cassert>
#include "nar512.cpp"
#include <math.h>

using namespace std;

string generateRandomString(size_t length) {
    const string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+[]{}|;:',.<>/?`~\"\\\t\r\n";
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<> dist(0, charset.size()-1);
    
    string str;
    str.reserve(length);
    for(size_t i = 0; i < length; ++i)
    str += charset[dist(generator)];
    
    return str;
}

class Times {
public:
    int count;
    double totalTime;
    double averageTime;

    Times() : count(0), totalTime(0.0), averageTime(0.0) {}

    void addTime(double time) {
        totalTime += time;
        count++;
        averageTime = totalTime / count;
    }

    void printTimes() const {
        cout << "Total Time Taken: " << fixed << totalTime << setprecision(5) << " sec" << endl;
        cout << "Average Time Taken: " << fixed << averageTime << setprecision(5) << " sec" << endl;
    }
};

int main() {
    vector<int> sizes = {1, 10, 100, 250, 500, 1024, 5000, 10000};
    map<int, Times> timesMap;

    cout << left << setw(15) << "Char Length" 
         << setw(20) << "Average Time (sec)" 
         << setw(20) << "Total Time (sec)" << endl;
    cout << string(55, '-') << endl;

    for (int size : sizes) {
        Times times;
        for (int i = 0; i < 1000; i++) {
            clock_t start = clock();
            string input = generateRandomString(size);
            nar512(input);
            clock_t end = clock();
            double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
            times.addTime(time_taken);
        }
        timesMap[size] = times;

        cout << left << setw(15) << size 
             << setw(20) << fixed << setprecision(5) << times.averageTime 
             << setw(20) << fixed << setprecision(5) << times.totalTime << endl;
    }

    return 0;
}
