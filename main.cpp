#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <map>
#include <numeric>
using namespace std;
using int128 = __int128;

int128 parseBase(const string& s, int base) {
    int128 r = 0;
    for (char c : s) {
        int d = isdigit(c) ? c - '0' : toupper(c) - 'A' + 10;
        r = r * base + d;
    }
    return r;
}
string to_string128(int128 n) {
    if (n == 0) return "0";
    string s;
    while (n) { s += '0' + n % 10; n /= 10; }
    reverse(s.begin(), s.end());
    return s;
}
int128 lagrange(const vector<pair<int128, int128>>& pts, int k) {
    int128 res = 0;
    for (int i = 0; i < k; ++i) {
        int128 num = 1, den = 1;
        for (int j = 0; j < k; ++j) if (i != j) {
            num *= -pts[j].first;
            den *= pts[i].first - pts[j].first;
        }
        res += pts[i].second * (num / den);
    }
    return res;
}
void parse(const string& fname, int& k, vector<pair<int128, int128>>& pts) {
    ifstream f(fname); string l;
    while (getline(f, l)) {
        l.erase(remove_if(l.begin(), l.end(), ::isspace), l.end());
        if (l.find("\"k\"") != string::npos) k = stoi(l.substr(l.find(":")+1));
        else if (!l.empty() && l[0] == '"' && l.find(":{") != string::npos) {
            size_t q1 = l.find('"');
            size_t q2 = l.find('"', q1+1);
            string xstr = l.substr(q1+1, q2-q1-1);
            // Only process if xstr is all digits
            if (!xstr.empty() && all_of(xstr.begin(), xstr.end(), ::isdigit)) {
                int x = stoi(xstr);
                // base line
                getline(f, l); l.erase(remove_if(l.begin(), l.end(), ::isspace), l.end());
                size_t qb1 = l.find('"', l.find(":"));
                size_t qb2 = l.find('"', qb1+1);
                int base = stoi(l.substr(qb1+1, qb2-qb1-1));
                // value line
                getline(f, l); l.erase(remove_if(l.begin(), l.end(), ::isspace), l.end());
                size_t qv1 = l.find('"', l.find(":"));
                size_t qv2 = l.find('"', qv1+1);
                string val = l.substr(qv1+1, qv2-qv1-1);
                pts.push_back({x, parseBase(val, base)});
            }
        }
    }
}
int main() {
    for (string file : {"input1.json", "input2.json"}) {
        int k; vector<pair<int128, int128>> pts;
        parse(file, k, pts);
        string ans;
        int n = pts.size(); vector<int> idx(n); iota(idx.begin(), idx.end(), 0);
        vector<int> c;
        map<string, int> freq; int mx = 0; int128 best = 0;
        function<void(int,int)> bt = [&](int s, int d) {
            if (d==k) {
                vector<pair<int128,int128>> sel;
                for (int i : c) sel.push_back(pts[i]);
                int128 v = lagrange(sel, k);
                string vs = to_string128(v);
                if (++freq[vs] > mx) mx = freq[vs], best = v;
                return;
            }
            for (int i=s; i<=n-(k-d); ++i) c.push_back(i), bt(i+1,d+1), c.pop_back();
        };
        bt(0,0);
        cout << to_string128(best) << endl;
    }
}
