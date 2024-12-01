#include <bits/stdc++.h>
using namespace std;

int main()
{
    vector<int> list1;
    unordered_map<int, int> list2;

    ifstream input("data.txt");
    string line;

    while (getline(input, line))
    {
        istringstream iss(line);
        int left, right;
        if (iss >> left >> right)
        {
            list1.push_back(left);
            list2[right]++;
        }
    }

    long long ans = 0;
    for (int i = 0; i < list1.size(); i++)
    {

        ans += (list1[i] * list2[list1[i]]);
    }

    cout << "Ans is " << ans << endl;

    return 0;
}