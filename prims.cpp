#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <vector>

using namespace std;

using ll = long long;
static const int INF = numeric_limits<int>::max() / 4;
static const size_t NPOS = numeric_limits<size_t>::max();

struct GraphData {
    vector<vector<int>> matrix;
    vector<vector<pair<int, int>>> adj;
};

// ============================================================
// GRAPH GENERATION (DETERMINISTIC)
// ============================================================

// Dense graph: complete graph on n vertices.
GraphData generateDenseConnectedGraph(int n) {
    GraphData g;
    const size_t sz = static_cast<size_t>(n);
    g.matrix.assign(sz, vector<int>(sz, INF));
    g.adj.assign(sz, {});
    if (n <= 1) {
        if (n == 1) g.matrix[0][0] = 0;
        return g;
    }

    for (size_t i = 0; i < sz; ++i) g.matrix[i][i] = 0;

    // Deterministic dense graph: complete graph.
    for (size_t u = 0; u < sz; ++u) {
        for (size_t v = u + 1; v < sz; ++v) {
            int w = static_cast<int>(((u + 1U) * (v + 1U)) % 1000U) + 1;
            g.matrix[u][v] = w;
            g.matrix[v][u] = w;
            g.adj[u].push_back({static_cast<int>(v), w});
            g.adj[v].push_back({static_cast<int>(u), w});
        }
    }

    return g;
}

// Sparse graph: path backbone + periodic skip edges.
GraphData generateSparseConnectedGraph(int n) {
    GraphData g;
    const size_t sz = static_cast<size_t>(n);
    g.matrix.assign(sz, vector<int>(sz, INF));
    g.adj.assign(sz, {});
    if (n <= 1) {
        if (n == 1) g.matrix[0][0] = 0;
        return g;
    }

    for (size_t i = 0; i < sz; ++i) g.matrix[i][i] = 0;

    // Deterministic sparse graph: path backbone for connectivity.
    for (size_t v = 1; v < sz; ++v) {
        size_t u = v - 1;
        int w = static_cast<int>(v % 1000U) + 1;
        g.matrix[u][v] = w;
        g.matrix[v][u] = w;
        g.adj[u].push_back({static_cast<int>(v), w});
        g.adj[v].push_back({static_cast<int>(u), w});
    }

    // Add a few deterministic extra edges, keeping graph sparse.
    for (size_t u = 0; u + 2 < sz; u += 10) {
        size_t v = u + 2;
        int w = static_cast<int>((u + v) % 1000U) + 1;
        g.matrix[u][v] = w;
        g.matrix[v][u] = w;
        g.adj[u].push_back({static_cast<int>(v), w});
        g.adj[v].push_back({static_cast<int>(u), w});
    }

    return g;
}

// ============================================================
// PRIM'S ALGORITHM IMPLEMENTATIONS
// ============================================================

// Matrix-based Prim's algorithm: O(n^2)
ll primsMatrixO2(const vector<vector<int>> &graph) {
    size_t n = graph.size();
    if (n <= 1) return 0;
    // 1) pick global minimum edge (k, l)
    // 2) maintain near[] for closest included vertex
    // 3) repeatedly pick min graph[j][near[j]]
    size_t k = NPOS, l = NPOS;
    int minEdge = INF;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (graph[i][j] < minEdge) {
                minEdge = graph[i][j];
                k = i;
                l = j;
            }
        }
    }

    if (k == NPOS || l == NPOS) return 0;

    ll cost = minEdge;
    vector<size_t> near(n, NPOS);
    vector<char> inMST(n, 0);
    inMST[k] = 1;
    inMST[l] = 1;

    for (size_t i = 0; i < n; ++i) {
        if (inMST[i]) continue;
        near[i] = (graph[i][k] < graph[i][l]) ? k : l;
    }

    for (size_t edgeCount = 1; edgeCount < n - 1; ++edgeCount) {
        size_t jMin = NPOS;
        int best = INF;
        for (size_t j = 0; j < n; ++j) {
            if (!inMST[j] && near[j] != NPOS && graph[j][near[j]] < best) {
                best = graph[j][near[j]];
                jMin = j;
            }
        }

        if (jMin == NPOS) break;
        cost += graph[jMin][near[jMin]];
        inMST[jMin] = 1;

        for (size_t j = 0; j < n; ++j) {
            if (!inMST[j] && graph[j][jMin] < graph[j][near[j]]) {
                near[j] = jMin;
            }
        }
    }

    return cost;
}

// Heap-based Prim's algorithm on adjacency list: O(E log n)
ll primsHeap(const vector<vector<pair<int, int>>> &adj) {
    size_t n = adj.size();
    if (n <= 1) return 0;

    vector<char> inMST(n, 0);
    priority_queue<pair<int, size_t>, vector<pair<int, size_t>>, greater<pair<int, size_t>>> pq;
    pq.push({0, 0U});
    ll cost = 0;
    size_t picked = 0;

    while (!pq.empty() && picked < n) {
        pair<int, size_t> top = pq.top();
        int w = top.first;
        size_t u = top.second;
        pq.pop();
        if (inMST[u]) continue;

        inMST[u] = 1;
        cost += w;
        picked++;

        for (const auto &edge : adj[u]) {
            size_t v = static_cast<size_t>(edge.first);
            int wt = edge.second;
            if (!inMST[v]) pq.push({wt, v});
        }
    }
    return cost;
}

// ============================================================
// TIMING HELPERS
// ============================================================

double timeMsMatrix(const GraphData &g) {
    auto t1 = chrono::high_resolution_clock::now();
    volatile ll ans = primsMatrixO2(g.matrix);
    (void)ans;
    auto t2 = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(t2 - t1).count();
}

double timeMsHeap(const GraphData &g) {
    auto t1 = chrono::high_resolution_clock::now();
    volatile ll ans = primsHeap(g.adj);
    (void)ans;
    auto t2 = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(t2 - t1).count();
}

// ============================================================
// EXPERIMENT DRIVER
// ============================================================

int main() {
    const int minN = 100;
    const int maxN = 2000;
    const int step = 100;
    const int trialsPerSize = 9;
    const int numTests = 4;

    for (int testId = 1; testId <= numTests; ++testId) {
        string outPath = "prims_experiment_test" + to_string(testId) + ".txt";
        ofstream out(outPath);
        if (!out.is_open()) {
            cerr << "Failed to open output file: " << outPath << '\n';
            return 1;
        }

        out << "# Prim's Algorithm Experiment (Matrix vs Heap)\n";
        out << "# Test: " << testId << '\n';
        out << "# Deterministic connected undirected weighted graphs\n";
        out << "# Dense: complete graph, Sparse: path + periodic skip edges\n";
        out << "# Each section format: n matrix_avg_ms heap_avg_ms\n";

        cout << "\nTest " << testId << '\n';
        cout << left << setw(8) << "n"
             << setw(18) << "matrix_avg_ms"
             << setw(18) << "heap_avg_ms" << '\n';

        // =========================
        // SECTION 1: DENSE GRAPH
        // =========================
        out << "\n# =========================\n";
        out << "# SECTION 1: DENSE GRAPH\n";
        out << "# =========================\n";
        out << "n matrix_avg_ms heap_avg_ms\n";

        cout << "\n[DENSE GRAPH]\n";
        for (int n = minN; n <= maxN; n += step) {
            double matrixSum = 0.0;
            double heapSum = 0.0;

            for (int t = 0; t < trialsPerSize; ++t) {
                GraphData g = generateDenseConnectedGraph(n);
                matrixSum += timeMsMatrix(g);
                heapSum += timeMsHeap(g);
            }

            double matrixAvg = matrixSum / trialsPerSize;
            double heapAvg = heapSum / trialsPerSize;

            out << n << ' '
                << fixed << setprecision(4) << matrixAvg << ' '
                << fixed << setprecision(4) << heapAvg << '\n';

            cout << left << setw(8) << n
                 << setw(18) << fixed << setprecision(4) << matrixAvg
                 << setw(18) << fixed << setprecision(4) << heapAvg << '\n';
        }

        // =========================
        // SECTION 2: SPARSE GRAPH
        // =========================
        out << "\n# =========================\n";
        out << "# SECTION 2: SPARSE GRAPH\n";
        out << "# =========================\n";
        out << "n matrix_avg_ms heap_avg_ms\n";

        cout << "\n[SPARSE GRAPH]\n";
        for (int n = minN; n <= maxN; n += step) {
            double matrixSum = 0.0;
            double heapSum = 0.0;

            for (int t = 0; t < trialsPerSize; ++t) {
                GraphData g = generateSparseConnectedGraph(n);
                matrixSum += timeMsMatrix(g);
                heapSum += timeMsHeap(g);
            }

            double matrixAvg = matrixSum / trialsPerSize;
            double heapAvg = heapSum / trialsPerSize;

            out << n << ' '
                << fixed << setprecision(4) << matrixAvg << ' '
                << fixed << setprecision(4) << heapAvg << '\n';

            cout << left << setw(8) << n
                 << setw(18) << fixed << setprecision(4) << matrixAvg
                 << setw(18) << fixed << setprecision(4) << heapAvg << '\n';
        }

        out.close();
        cout << "Saved: " << outPath << '\n';
    }

    cout << "\nAll Prim's experiments complete.\n";
    return 0;
}
