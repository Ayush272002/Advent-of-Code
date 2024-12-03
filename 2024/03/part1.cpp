#include <bits/stdc++.h>
using namespace std;

int main()
{
    ifstream input("data.txt");
    string line;

    int ans = 0;
    regex pattern(R"(mul\((\d+),(\d+)\))");
    smatch match;

    while (getline(input, line))
    {
        string::const_iterator searchStart(line.cbegin());
        while (regex_search(searchStart, line.cend(), match, pattern))
        {
            int a = stoi(match[1]);
            int b = stoi(match[2]);

            ans += a * b;
            searchStart = match.suffix().first;
        }
    }

    cout << "Ans is " << ans << endl;
    return 0;
}