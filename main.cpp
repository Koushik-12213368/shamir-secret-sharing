#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

struct Share {
    int id;
    long long x;
    long long y;
};

// GCD for LCM
long long gcd(long long a, long long b) {
    return b == 0 ? a : gcd(b, a % b);
}

long long lcm(long long a, long long b) {
    return (a * b) / gcd(a, b);
}

// Evaluates an expression like sum(a,b)
long long evaluate(string func, long long a, long long b) {
    if (func == "sum") return a + b;
    if (func == "multiply") return a * b;
    if (func == "divide") return a / b;
    if (func == "gcd") return gcd(a, b);
    if (func == "lcm") return lcm(a, b);
    return -1;
}

long long interpolateAtZero(const vector<Share>& shares) {
    long long result = 0;
    for (int i = 0; i < shares.size(); ++i) {
        long long xi = shares[i].x;
        long long yi = shares[i].y;

        long long num = 1, den = 1;
        for (int j = 0; j < shares.size(); ++j) {
            if (i == j) continue;
            long long xj = shares[j].x;
            num *= -xj;
            den *= (xi - xj);
        }
        result += yi * (num / den);
    }
    return result;
}

int main() {
    ifstream f("input.json");
    string line;
    vector<Share> shares;
    int id = 0;

    while (getline(f, line)) {
        if (line.find("value") != string::npos) {
            id++;
            size_t colon = line.find(':');
            size_t quote1 = line.find('"', colon);
            size_t quote2 = line.find('"', quote1 + 1);
            string expr = line.substr(quote1 + 1, quote2 - quote1 - 1);

            string func = expr.substr(0, expr.find('('));
            string inside = expr.substr(expr.find('(') + 1, expr.find(')') - expr.find('(') - 1);
            long long a = stoll(inside.substr(0, inside.find(',')));
            long long b = stoll(inside.substr(inside.find(',') + 1));

            long long y = evaluate(func, a, b);
            shares.push_back({id, id, y});
        }
    }

    // Try all combinations of 3 shares
    map<long long, int> freq;
    vector<vector<int>> usedInWrong;
    for (int i = 0; i < shares.size(); ++i)
        for (int j = i + 1; j < shares.size(); ++j)
            for (int k = j + 1; k < shares.size(); ++k) {
                vector<Share> combo = {shares[i], shares[j], shares[k]};
                long long secret = interpolateAtZero(combo);
                freq[secret]++;
            }

    // Most common secret = real one
    long long correctSecret = max_element(freq.begin(), freq.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; })->first;

    // Check which share is invalid
    vector<int> possibleInvalids;
    for (auto& s : shares) {
        int count = 0;
        for (int i = 0; i < shares.size(); ++i)
            for (int j = i + 1; j < shares.size(); ++j) {
                if (s.id == shares[i].id || s.id == shares[j].id) continue;
                vector<Share> combo = {s, shares[i], shares[j]};
                if (combo.size() == 3 && interpolateAtZero(combo) != correctSecret)
                    count++;
            }
        if (count >= 2) possibleInvalids.push_back(s.id);
    }

    // Final output
    ofstream out("output.txt");
    out << "Secret: " << correctSecret << endl;
    out << "Invalid Share IDs: ";
    for (auto id : possibleInvalids) out << id << " ";
    out << endl;

    cout << "Secret: " << correctSecret << endl;
    cout << "Invalid Share IDs: ";
    for (auto id : possibleInvalids) cout << id << " ";
    cout << endl;

    return 0;
}
