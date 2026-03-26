#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

using namespace std;

using ll = long long;
static const int INF = numeric_limits<int>::max() / 4;

struct GraphData {
    vector<vector<int>> matrix;
};

GraphData generateRandomDenseGraph(int n, mt19937 &rng) {
    uniform_int_distribution<int> weightDist(1, 1000);
    GraphData g;
    g.matrix.assign(n, vector<int>(n, INF));
    if (n <= 1) {
        if (n == 1) g.matrix[0][0] = 0;
        return g;
    }

    for (int i = 0; i < n; ++i) {
        g.matrix[i][i] = 0;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int w = weightDist(rng);
            g.matrix[i][j] = w;
            g.matrix[j][i] = w;
        }
    }

    return g;
}

ll primsMatrixO2(const vector<vector<int>> &graph) {
    int n = static_cast<int>(graph.size());
    if (n <= 1) return 0;
    // 1) pick global minimum edge (k, l)
    // 2) maintain near[] for closest included vertex
    // 3) repeatedly pick min graph[j][near[j]]
    int k = -1, l = -1;
    int minEdge = INF;
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (graph[i][j] < minEdge) {
                minEdge = graph[i][j];
                k = i;
                l = j;
            }
        }
    }

    if (k == -1 || l == -1) return 0;

    ll cost = minEdge;
    vector<int> near(n, -1);
    vector<char> inMST(n, 0);
    inMST[k] = 1;
    inMST[l] = 1;

    for (int i = 0; i < n; ++i) {
        if (inMST[i]) continue;
        near[i] = (graph[i][k] < graph[i][l]) ? k : l;
    }

    for (int edgeCount = 1; edgeCount < n - 1; ++edgeCount) {
        int jMin = -1;
        int best = INF;
        for (int j = 0; j < n; ++j) {
            if (!inMST[j] && near[j] != -1 && graph[j][near[j]] < best) {
                best = graph[j][near[j]];
                jMin = j;
            }
        }

        if (jMin == -1) break;
        cost += graph[jMin][near[jMin]];
        inMST[jMin] = 1;

        for (int j = 0; j < n; ++j) {
            if (!inMST[j] && graph[j][jMin] < graph[j][near[j]]) {
                near[j] = jMin;
            }
        }
    }

    return cost;
}

double timeMsMatrix(const GraphData &g) {
    auto t1 = chrono::high_resolution_clock::now();
    volatile ll ans = primsMatrixO2(g.matrix);
    (void)ans;
    auto t2 = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(t2 - t1).count();
}

int main() {
    const int minN = 50;
    const int maxN = 5000;
    const int step = 50;
    const int trialsPerSize = 5;
    const int numTests = 4;

    for (int testId = 1; testId <= numTests; ++testId) {
        int seed = 100 + testId * 1234;
        mt19937 rng(seed);

        string outPath = "prims_experiment_test" + to_string(testId) + ".txt";
        ofstream out(outPath);
        if (!out.is_open()) {
            cerr << "Failed to open output file: " << outPath << '\n';
            return 1;
        }

        out << "# Prim's Algorithm Experiment\n";
        out << "# Test: " << testId << '\n';
        out << "# Seed: " << seed << '\n';
        out << "# Dense graph: complete graph with random weights\n";
        out << "# Columns: n matrix_avg_ms\n";
        out << "n matrix_avg_ms\n";

        cout << "\nTest " << testId << " seed=" << seed << '\n';
        cout << left << setw(8) << "n"
             << setw(18) << "matrix_avg_ms" << '\n';

        for (int n = minN; n <= maxN; n += step) {
            double matrixSum = 0.0;

            for (int t = 0; t < trialsPerSize; ++t) {
                GraphData g = generateRandomDenseGraph(n, rng);
                matrixSum += timeMsMatrix(g);
            }

            double matrixAvg = matrixSum / trialsPerSize;

            out << n << ' '
                << fixed << setprecision(4) << matrixAvg << '\n';

            cout << left << setw(8) << n
                 << setw(18) << fixed << setprecision(4) << matrixAvg << '\n';
        }

        out.close();
        cout << "Saved: " << outPath << '\n';
    }

    cout << "\nAll Prim's experiments complete.\n";
    return 0;
}
