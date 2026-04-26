#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <ctime>

using namespace std;

//// Параметры пользователя
//const int POPULATION_SIZE = 1000;
//const int MAX_GENERATIONS = 100;
//const int WR_GROUP_SIZE = 100;
//const double MUTATION_CHANCE = 0.1; // вероятность мутации
//const int CLOSE_CITIES_COUNT = 5;   // количество ближайших соседей
//const double CHANCE_CLOSE_CITIES = 50.0; // если rand() > число — жадный выбор
int POPULATION_SIZE;
int MAX_GENERATIONS;
int WR_GROUP_SIZE;
double MUTATION_CHANCE;
int CLOSE_CITIES_COUNT;
double CHANCE_CLOSE_CITIES;

struct City {
    int id;
    int x, y;
};

vector<City> cities;
vector<vector<double>> Distances;
vector<vector<int>> CloseCities;
vector<vector<int>> population;
vector<int> bestTour;
double bestDistance = numeric_limits<double>::max();

default_random_engine rng(time(0));

// Вычисление расстояния между двумя городами
double getDistance(int i, int j) {
    int dx = cities[i].x - cities[j].x;
    int dy = cities[i].y - cities[j].y;
    return sqrt(dx * dx + dy * dy);
}

// Чтение данных из файла
void readCities(const string& filename) {
    ifstream fin(filename);
    City c;
    while (fin >> c.id >> c.x >> c.y) {
        cities.push_back(c);
    }
    fin.close();
}

// матрица расстояний
void initDistances() {
    int n = cities.size();
    Distances.resize(n, vector<double>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            Distances[i][j] = getDistance(i, j);
}

// Для каждого города сохраняем его ближайших соседей
void initCloseCities() {
    int n = cities.size();
    CloseCities.resize(n);
    for (int i = 0; i < n; ++i) {
        vector<pair<double, int>> temp;
        for (int j = 0; j < n; ++j) {
            if (i != j)
                temp.push_back({ Distances[i][j], j });
        }
        sort(temp.begin(), temp.end());
        for (int k = 0; k < CLOSE_CITIES_COUNT && k < (int)temp.size(); ++k)
            CloseCities[i].push_back(temp[k].second);
    }
}

// Подсчёт длины маршрута
double calcTourLength(const vector<int>& tour) {
    double len = 0.0;
    for (int i = 0; i < (int)tour.size() - 1; ++i)
        len += Distances[tour[i]][tour[i + 1]];
    return len;
}

// Создание случайного маршрута
vector<int> generateRandomTour(int startCity) {
    vector<int> tour;
    vector<bool> used(cities.size(), false);
    tour.push_back(startCity);
    used[startCity] = true;

    uniform_int_distribution<> dist(0, 99);
    for (int i = 1; i < (int)cities.size(); ++i) {
        int next;
        do {
            next = dist(rng);
        } while (used[next]);
        tour.push_back(next);
        used[next] = true;
    }
    tour.push_back(startCity); 
    return tour;
}

// Жадное создание маршрута с частичным рандомом
vector<int> generateGreedyTour(int startCity) {
    vector<int> tour;
    vector<bool> used(cities.size(), false);
    tour.push_back(startCity);
    used[startCity] = true;

    int current = startCity;
    for (int i = 1; i < (int)cities.size(); ++i) {
        bool found = false;
        // Пробуем ближайших соседей
        for (int neighbor : CloseCities[current]) {
            if (!used[neighbor]) {
                tour.push_back(neighbor);
                used[neighbor] = true;
                current = neighbor;
                found = true;
                break;
            }
        }

        if (!found) { // Если не нашлось — случайный
            uniform_int_distribution<> dist(0, 99);
            int next;
            do {
                next = dist(rng);
            } while (used[next]);
            tour.push_back(next);
            used[next] = true;
            current = next;
        }
    }
    tour.push_back(startCity); // Замыкание
    return tour;
}

// Формирование начальной популяции
void initPopulation() {
    population.resize(POPULATION_SIZE);
    uniform_int_distribution<> city_dist(0, 99);

    for (int i = 0; i < POPULATION_SIZE; ++i) {
        int startCity = 0;
        double chance = ((double)rand() / RAND_MAX) * 100;
        if (chance > CHANCE_CLOSE_CITIES)
            population[i] = generateGreedyTour(startCity);
        else
            population[i] = generateRandomTour(startCity);

        double len = calcTourLength(population[i]);
        if (len < bestDistance) {
            bestDistance = len;
            bestTour = population[i];
        }
    }
}

// Сортировка рабочей группы пузырьком
void sortWorkingGroup(vector<vector<int>>& wrGroup, vector<double>& lengths) {
    int n = wrGroup.size();
    for (int i = 0; i < n - 1; ++i)
        for (int j = 0; j < n - i - 1; ++j)
            if (lengths[j] > lengths[j + 1]) {
                swap(wrGroup[j], wrGroup[j + 1]);
                swap(lengths[j], lengths[j + 1]);
            }
}

// Скрещивание
vector<int> crossover(const vector<int>& parent1, const vector<int>& parent2) {
    int size = parent1.size() - 1; // без последнего (он повторяет первый)
    int mid = size / 2;
    vector<int> child(size + 1, -1);
    vector<bool> used(size, false);

    // Первая половина от parent1
    for (int i = 0; i < mid; ++i) {
        child[i] = parent1[i];
        used[parent1[i]] = true;
    }

    // Вторая половина: добавляем уникальные из parent2
    int idx = mid;
    for (int i = 0; i < size; ++i) {
        int city = parent2[i];
        if (!used[city]) {
            child[idx++] = city;
            used[city] = true;
        }
    }

    child.back() = child.front(); // Замыкаем
    return child;
}

// Мутация
void mutate(vector<int>& tour) {
    uniform_real_distribution<> prob(0.0, 1.0);
    if (prob(rng) > MUTATION_CHANCE) return;

    uniform_int_distribution<> dist(1, tour.size() - 2); // Не трогаем начало/конец
    int i = dist(rng), j = dist(rng);
    swap(tour[i], tour[j]);
}

// Генетический алгоритм
void geneticAlgorithm() {
    int generation = 0;
    while (generation < MAX_GENERATIONS) {
        // Отбор рабочей группы
        uniform_int_distribution<> pop_dist(0, POPULATION_SIZE - 1);
        vector<vector<int>> wrGroup(WR_GROUP_SIZE);
        for (int i = 0; i < WR_GROUP_SIZE; ++i)
            wrGroup[i] = population[pop_dist(rng)];

        // Оценка и сортировка
        vector<double> lengths(WR_GROUP_SIZE);
        for (int i = 0; i < WR_GROUP_SIZE; ++i)
            lengths[i] = calcTourLength(wrGroup[i]);
        sortWorkingGroup(wrGroup, lengths);

        // Скрещивание и мутация
        vector<int> child = crossover(wrGroup[0], wrGroup[1]);
        mutate(child);
        double childLen = calcTourLength(child);

        // Найден лучший?
        if (childLen < bestDistance) {
            bestDistance = childLen;
            bestTour = child;
        }

        // Замена худшего маршрута в рабочей группе
        int worstIndex = -1;
        double maxLen = -1;
        for (int i = 0; i < WR_GROUP_SIZE; ++i) {
            double len = calcTourLength(wrGroup[i]);
            if (len > maxLen) {
                maxLen = len;
                worstIndex = i;
            }
        }
        wrGroup[worstIndex] = child;

        // Обновление популяции
        for (int i = 0; i < WR_GROUP_SIZE; ++i)
            population[pop_dist(rng)] = wrGroup[i];

        //cout << "Поколение " << generation << ": Лучшая длина = " << bestDistance << endl;
        generation++;
    }
}

// Сохранение результата
void saveResult() {
    ofstream fout("best_tour.txt");
    for (int city : bestTour)
        fout << city << " ";
    fout << "\nДлина: " << bestDistance << endl;
    fout.close();
}
// для сброса глобальных переменных
void resetGlobals() {
    cities.clear();
    Distances.clear();
    CloseCities.clear();
    population.clear();
    bestTour.clear();
    bestDistance = numeric_limits<double>::max();
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(0));
    //int zn[10]{ 50, 200, 500, 1000, 1500, 2000 }; POPULATION_SIZE
    //int zn[10]{ 50, 80, 100, 500, 1000, 1500 }; MAX_GENERATIONS
    //int zn[10]{ 0.01, 0.05, 0.1, 0.5, 0.7, 0.9 }; MUTATION_CHANCE
    //int zn[10]{ 5, 50, 100, 300, 500, 900 }; WR_GROUP_SIZE
    //int zn[6]{ 1, 3, 5, 7, 9, 20 }; CLOSE_CITIES_COUNT
    int zn[6]{ 5, 10, 20, 35, 50, 70 };
    for (int i = 0; i < 6; i++)    
    {                              
        resetGlobals();

        POPULATION_SIZE = 1000;
        MAX_GENERATIONS = 100;
        WR_GROUP_SIZE = 100;
        MUTATION_CHANCE = 0.1; // вероятность мутации
        CLOSE_CITIES_COUNT = 5;   // количество ближайших соседей
        CHANCE_CLOSE_CITIES = zn[i]; // если rand() > число — жадный выбор

        readCities("cities.txt");
        initDistances();
        initCloseCities();
        initPopulation();
        geneticAlgorithm();
        saveResult();
        cout << bestDistance << endl;
    }
    
    return 0;
}
