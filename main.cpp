#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <numeric>
#include <iomanip>

using namespace std;

typedef __int128 int128;

// Helper to convert base-encoded string to int128
int128 parseBaseValue(const std::string& value, int base) {
    int128 result = 0;
    for (char digit : value) {
        int d;
        if (isdigit(digit)) d = digit - '0';
        else if (isalpha(digit)) d = toupper(digit) - 'A' + 10;
        else continue;
        result = result * base + d;
    }
    return result;
}

// Convert int128 to printable string
string to_string128(int128 num) {
    if (num == 0) return "0";
    string result;
    bool negative = false;
    if (num < 0) {
        negative = true;
        num = -num;
    }
    while (num > 0) {
        result += (char)('0' + (num % 10));
        num /= 10;
    }
    if (negative) result += '-';
    reverse(result.begin(), result.end());
    return result;
}

// Lagrange interpolation for f(0)
int128 lagrangeConstant(const vector<pair<int128, int128>>& points) {
    int128 result = 0;
    int k = points.size();
    for (int i = 0; i < k; i++) {
        int128 xi = points[i].first;
        int128 yi = points[i].second;
        int128 num = 1, den = 1;
        for (int j = 0; j < k; j++) {
            if (i != j) {
                int128 xj = points[j].first;
                num *= -xj;
                den *= (xi - xj);
            }
        }
        result += yi * (num / den);
    }
    return result;
}

// Minimal JSON parser for this specific input format
struct TestCase {
    int n, k;
    std::vector<std::pair<int128, int128>> points;
};

TestCase parseInput(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    TestCase tc;
    tc.n = 0; tc.k = 0;
    while (std::getline(file, line)) {
        // Remove whitespace
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (line.find("\"n\"") != std::string::npos) {
            size_t pos = line.find(":");
            tc.n = std::stoi(line.substr(pos + 1));
        } else if (line.find("\"k\"") != std::string::npos) {
            size_t pos = line.find(":");
            tc.k = std::stoi(line.substr(pos + 1));
        } else if (!line.empty() && line[0] == '"' && line.find(":{") != std::string::npos) {
            // This is a key line: "1": {
            size_t q1 = line.find('"');
            size_t q2 = line.find('"', q1 + 1);
            std::string xstr = line.substr(q1 + 1, q2 - q1 - 1);
            // Only parse if xstr is all digits
            if (!xstr.empty() && std::all_of(xstr.begin(), xstr.end(), ::isdigit)) {
                int128 x = std::stoll(xstr);
                // Next two lines are base and value
                std::getline(file, line); // base line
                line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
                size_t qbase1 = line.find('"', line.find(":"));
                size_t qbase2 = line.find('"', qbase1 + 1);
                std::string base_str = line.substr(qbase1 + 1, qbase2 - qbase1 - 1);
                int base = std::stoi(base_str);
                std::getline(file, line); // value line
                line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
                size_t qval1 = line.find('"', line.find(":"));
                size_t qval2 = line.find('"', qval1 + 1);
                std::string ystr = line.substr(qval1 + 1, qval2 - qval1 - 1);
                int128 y = parseBaseValue(ystr, base);
                tc.points.push_back({x, y});
            }
        }
    }
    return tc;
}

// Find most common constant across all combinations
int128 solve(const TestCase& tc) {
    int n = tc.points.size();
    int k = tc.k;
    std::vector<int> indices(n);
    std::iota(indices.begin(), indices.end(), 0);
    std::vector<int> comb;
    std::map<std::string, int> freq;
    int max_count = 0;
    int128 best = 0;
    std::function<void(int, int)> backtrack = [&](int start, int depth) {
        if (depth == k) {
            std::vector<std::pair<int128, int128>> chosen;
            for (int idx : comb) chosen.push_back(tc.points[idx]);
            try {
                int128 val = lagrangeConstant(chosen);
                std::string valstr = to_string128(val);
                freq[valstr]++;
                if (freq[valstr] > max_count) {
                    max_count = freq[valstr];
                    best = val;
                }
            } catch (...) {}
            return;
        }
        for (int i = start; i <= n - (k - depth); ++i) {
            comb.push_back(i);
            backtrack(i + 1, depth + 1);
            comb.pop_back();
        }
    };
    backtrack(0, 0);
    return best;
}

int main() {
    TestCase tc1 = parseInput("input1.json");
    TestCase tc2 = parseInput("input2.json");

    // Lagrange interpolation for f(0)
    auto lagrangeConstant = [](const std::vector<std::pair<int128, int128>>& points, int k) -> int128 {
        int128 result = 0;
        for (int i = 0; i < k; i++) {
            int128 xi = points[i].first;
            int128 yi = points[i].second;
            int128 num = 1, den = 1;
            for (int j = 0; j < k; j++) {
                if (i != j) {
                    int128 xj = points[j].first;
                    num *= -xj;
                    den *= (xi - xj);
                }
            }
            result += yi * (num / den);
        }
        return result;
    };

    // Find most common constant across all k-combinations
    auto solve = [&](const TestCase& tc) -> int128 {
        int n = tc.points.size();
        int k = tc.k;
        std::vector<int> indices(n);
        std::iota(indices.begin(), indices.end(), 0);
        std::vector<int> comb;
        std::map<std::string, int> freq;
        int max_count = 0;
        int128 best = 0;
        std::function<void(int, int)> backtrack = [&](int start, int depth) {
            if (depth == k) {
                std::vector<std::pair<int128, int128>> chosen;
                for (int idx : comb) chosen.push_back(tc.points[idx]);
                try {
                    int128 val = lagrangeConstant(chosen, k);
                    std::string valstr = to_string128(val);
                    freq[valstr]++;
                    if (freq[valstr] > max_count) {
                        max_count = freq[valstr];
                        best = val;
                    }
                } catch (...) {}
                return;
            }
            for (int i = start; i <= n - (k - depth); ++i) {
                comb.push_back(i);
                backtrack(i + 1, depth + 1);
                comb.pop_back();
            }
        };
        backtrack(0, 0);
        return best;
    };

    int128 secret1 = solve(tc1);
    int128 secret2 = solve(tc2);

    std::ofstream fout("output.txt");
    fout << "Output for TestCase - 1\n" << to_string128(secret1) << "\n\n";
    fout << "Output for TestCase - 2\n" << to_string128(secret2) << "\n";
    fout.close();

    std::cout << "Secrets written to output.txt\n";
    return 0;
}
