#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;


//void print(ifstream &in)
//{
//    if (!in) return;
//    int a;
//    in >> a;
//    if (a > 0)
//    {
//        cout << a << endl;
//        print(in);
//    }
//        
//    if (a < 0)
//    {
//        print(in);
//        cout << a << endl;
//    }
//}


struct graph
{
    double x;
    double y;
    bool metka;
};

void create_list(ifstream& fin, graph* s, int n)
{
    s[0].metka = true;
    fin >> s[0].x >> s[0].y;
    for (int i = 1; i < n; i++)
    {
        s[i].metka = false;
        fin >> s[i].x >> s[i].y;
    }
}
void show_list(graph* s, int n)
{
    for (int i = 0; i < n; i++)
    {
        cout << s[i].x << " " << s[i].y << " " << s[i].metka << endl;
    }
    cout << endl;
}

double weight(graph tek, graph sled)
{
    double weight = sqrt((tek.x - sled.x) * (tek.x - sled.x) + (tek.y - sled.y) * (tek.y - sled.y));
    return weight;
}


// нахождение ближайшей точки и расстояние от изначальной точки до найденной
void Min_r_v(graph* s, int n, graph& tek, double mas[2])
{
    double mn = 10000;
    int index = 0;
    for (int i = 1; i < n; i++)
    {
        graph sled = s[i];
        if (sled.metka == false)
        {
            double w = weight(tek, sled);
            if (w < mn)
            {
                mn = w;
                index = i;
            }
        }
    }
    mas[0] = mn;
    mas[1] = index;
    return;
}



// проверка всех структур: все вершины пройдены или нет
bool Metka_s(graph* s, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (s[i].metka == false)
            return false;
    }
    return true;
}
// коммивояжера методом нахождения ближайшего соседа
double nearest_neighbor(graph* s, int n)
{
    graph tek = s[0];

    double mas[2] = { 0, 0 };

    double distance = 0;

    Min_r_v(s, n, tek, mas);

    while(Metka_s(s, n) == false)
    {
        tek = s[(int)mas[1]];
        s[(int)mas[1]].metka = true;
        distance += mas[0];
        Min_r_v(s, n, tek, mas);
    }
    distance += weight(tek, s[0]);
    return distance;
}

int main()
{
    
    srand(time(0));
    ifstream in("input.txt");
    ofstream fout("output.txt");
    
    int n;
    in >> n;

    graph* s = new graph[n];

    

    create_list(in, s, n);
    show_list(s, n);

    cout << nearest_neighbor(s, n) << endl;
    delete[] s;

    cout << clock() / 1000.0;
    return 0;
}