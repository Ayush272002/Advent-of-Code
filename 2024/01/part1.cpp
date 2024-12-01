#include <bits/stdc++.h>
using namespace std;

int main()
{
    vector<int> list1;
    vector<int> list2;

    ifstream input("data.txt");
    string line;

    while (getline(input, line))
    {
        istringstream iss(line);
        int left, right;
        if (iss >> left >> right)
        {
            list1.push_back(left);
            list2.push_back(right);
        }
    }

    sort(list1.begin(), list1.end());
    sort(list2.begin(), list2.end());

    long long ans = 0;
    for (int i = 0; i < list1.size(); i++)
    {
        ans += abs(list1[i] - list2[i]);
    }

    cout << "Ans is " << ans << endl;

    return 0;
}