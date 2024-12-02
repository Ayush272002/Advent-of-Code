#include <bits/stdc++.h>
using namespace std;

bool isSafe(const vector<int> &r)
{
    if (!is_sorted(r.begin(), r.end()) && !is_sorted(r.rbegin(), r.rend()))
        return false;

    for (int i = 1; i < r.size(); i++)
    {
        int diff = abs(r[i] - r[i - 1]);
        if (diff < 1 || diff > 3)
            return false;
    }

    return true;
}

int main()
{
    ifstream input("data.txt");

    vector<vector<int>> data;
    string line;

    while (getline(input, line))
    {
        vector<int> row;
        stringstream ss(line);
        int num;

        while (ss >> num)
            row.push_back(num);

        data.push_back(row);
    }

    int ans = 0;

    for (const auto &r : data)
    {
        if (isSafe(r))
        {
            ans++;
            continue;
        }

        bool dampenedSafe = false;
        for (int i = 0; i < r.size(); i++)
        {
            vector<int> changedRow = r;
            changedRow.erase(changedRow.begin() + i);

            if (isSafe(changedRow))
            {
                dampenedSafe = true;
                break;
            }
        }

        if (dampenedSafe)
            ans++;
    }

    cout << "Ans is " << ans << endl;
    return 0;
}