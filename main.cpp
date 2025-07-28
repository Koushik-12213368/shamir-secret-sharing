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

// Safely parse expressions like sum(123, 456)
bool parseExpression(const string& expr, string& func, long long& a, long long& b) {
    size_t open = expr.find('(');
    size_t comma = expr.find(',', open);
    size_t close = expr.find(')', comma);
    if (open == string::npos || comma == string::npos || close == string::npos) return false;

    func = expr.substr(0, open);

    try {
        a = stoll(expr.substr(open + 1, comma - open - 1));
        b = stoll(expr.substr(comma + 1, close - comma - 1));
        return true;
    } catch (...) {
        return false;
    }
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
    ifstream file("input.json");
    string line;
    vector<Share> shares;
    int id = 0;

    while (getline(file, line)) {
        if (line.find("value") != string::npos) {
            size_t quote1 = line.find('"', line.find(":"));
            size_t quote2 = line.find('"', quote1 + 1);
            string expr = line.substr(quote1 + 1, quote2 - quote1 - 1);

            string func;
            long long a, b;
            if (parseExpression(expr, func, a, b)) {
                long long y = evaluate(func, a, b);
                shares.push_back({++id, id, y});
            }
        }
    }

    // Try all combinations of 3 shares
    map<long long, int> freq;
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

    // Detect invalid shares
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
