#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
using namespace std;

struct graph
{
    double x;
    double y;
};

void create_list(ifstream& fin, graph* s, int n)
{
    for (int i = 0; i < n; i++)
    {
        fin >> s[i].x >> s[i].y;
    }
}

void show_list(graph* s, int n)
{
    for (int i = 0; i < n; i++)
    {
        cout << " " << s[i].x << " " << s[i].y << endl;
    }
    cout << endl;
}


double weight(graph* p, int a, int b)
{
    double weight;
    weight = sqrt(((p[a].x - p[b].x) * (p[a].x - p[b].x)) + ((p[a].y - p[b].y) * (p[a].y - p[b].y)));
    return weight;
}

// проверка: если есть х в массиве а
bool proverka(int* a, int n, int x)
{
    for (int i = 0; i < n; i++)
        if (a[i] == x)
            return true;
    return false;
}

void create_mas(int* a, int n)
{
    int start = 0;
    a[0] = 0;
    int nn = n - 1;
    for (int i = 1; i < n; i++)
    {
        int x = rand() % (nn - start + 1) + start;
        while (proverka(a, nn, x) == true)
            x = rand() % (nn - start + 1) + start;
        a[i] = x;
    }
}


double perebor(graph* s, int* a, int n)
{
    double count = 0;
    for (int i = 0; i < n - 1; i++)
    {
        count += weight(s, a[i], a[i + 1]);
    }
    count += weight(s, a[n - 1], a[0]);
    return count;
}



int main()
{
    srand(time(0));
    ifstream fin("input.txt");
    ofstream fout("output.txt");

    int n;
    fin >> n;
    graph* s = new graph[n];

    create_list(fin, s, n);
    show_list(s, n);

    // для разного рандома
    const auto now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    int cur_time = static_cast<int>(t_c);
    srand(cur_time);
    //

    int* a = new int[n];
    create_mas(a, n);

    for (int i = 0; i < n; i++)
        cout << a[i] << " ";
    cout << endl;
    cout << perebor(s, a, n) << endl;

    cout << clock() / 1000.0;

    return 0;
}