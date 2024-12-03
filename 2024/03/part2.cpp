#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main()
{
    ifstream inputFile("data.txt");
    if (!inputFile.is_open())
    {
        cerr << "Error: Could not open input file." << endl;
        return 1;
    }

    string line;
    int ans = 0;
    bool enabled = true;

    while (getline(inputFile, line))
    {
        int i = 0, j = 0;
        int n1 = 0, n2 = 0;

        while (i < line.length())
        {
            if (j == 0)
            {
                n1 = 0;
                n2 = 0;
            }

            if (line[i] == 'd' && j == 0)
            {
                while (i < line.length() && line[i] == "don't()"[j])
                {
                    i++;
                    j++;
                    if (j == 7)
                    {
                        enabled = false;
                        j = 0;
                        break;
                    }
                }

                if (j == 2)
                {
                    j = 0;
                    while (i < line.length() && line[i] == "()"[j])
                    {
                        i++;
                        j++;
                        if (j == 2)
                        {
                            enabled = true;
                            break;
                        }
                    }
                    j = 0;
                }
                else
                {
                    j = 0;
                }

                continue;
            }

            if (!enabled)
            {
                i++;
                j = 0;
                continue;
            }

            if (j == 4 && !isdigit(line[i]))
            {
                i++;
                j = 0;
                continue;
            }

            while ((j == 4 || j == 5) && isdigit(line[i]))
            {
                if (j == 4)
                {
                    n1 = n1 * 10 + (line[i] - '0');
                }
                else if (j == 5)
                {
                    n2 = n2 * 10 + (line[i] - '0');
                }
                i++;
            }

            if (j == 4)
            {
                if (line[i] == ',')
                {
                    i++;
                    j++;
                    continue;
                }
                else
                {
                    i++;
                    j = 0;
                    continue;
                }
            }

            if (j == 5)
            {
                if (line[i] == ')')
                {
                    i++;
                    j = 0;
                    ans += n1 * n2;
                    n1 = 0;
                    n2 = 0;
                    continue;
                }
                else
                {
                    i++;
                    j = 0;
                    continue;
                }
            }

            if (line[i] == "mul("[j])
            {
                j++;
            }
            else
            {
                j = 0;
            }

            i++;
        }
    }

    inputFile.close();

    cout << "Ans is " << ans << endl;
    return 0;
}
