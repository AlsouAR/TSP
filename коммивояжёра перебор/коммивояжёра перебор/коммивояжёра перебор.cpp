#include <iostream>
#include <algorithm>
#include <fstream>
#include <ctime>
using namespace std;

struct graph
{
    int num;
    double x;
    double y;
};

void create_list(ifstream& fin, graph* s, int n)
{
    for (int i = 0; i < n; i++)
    {
        s[i].num = i + 1;
        fin >> s[i].x >> s[i].y;
    }
}

void show_list(graph* s, int n)
{
    for (int i = 0; i < n; i++)
    {
        cout << s[i].num << " " << s[i].x << " " << s[i].y << endl;
    }
    cout << endl;
}

int factorial(int n)
{
    int f = 1;
    for (int i = 1; i < n+1; i++)
    {
        f *= i;
    }
    return f;
}


double weight(graph* p, int a, int b)
{
    double weight;
    weight = sqrt(((p[a].x - p[b].x) * (p[a].x - p[b].x)) + ((p[a].y - p[b].y) * (p[a].y - p[b].y)));
    return weight;
}
double perebor(graph* s, int* a, int n)
{
    double min = 1000000000;
    while (next_permutation(a, a + n))
    {
        double count = 0;
        if (a[0] == 1)
        {
            for (int i = 0; i < n - 1; i++)
            {
                count += weight(s, a[i] - 1, a[i + 1] - 1);
            }
            count += weight(s, a[n - 1] - 1, a[0] - 1);
            if (count < min)
                min = count;
        }
    }
    return min;
}


int main()
{
    srand(time(0));
    ifstream fin("input.txt");
    ofstream fout("output.txt");
    graph* s = new graph;

    int n;
    fin >> n;

    create_list(fin, s, n);
    show_list(s, n);

    int* a = new int[n];
    for (int i = 0; i < n; i++)
        a[i] = i + 1;

    cout << perebor(s, a, n) << endl;

    cout << clock() / 1000.0;

    return 0;
}
