import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os


def load_data(path):
    n, matrix_t = [], []
    with open(path, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#') or line.startswith('n '):
                continue
            parts = line.split()
            n.append(int(parts[0]))
            matrix_t.append(float(parts[1]))
    return n, matrix_t


def plot_test(test_id):
    input_path = f"prims_experiment_test{test_id}.txt"
    output_path = f"prims_experiment_test{test_id}.png"

    n, matrix_t = load_data(input_path)

    # Benchmark line: scaled n^2 reference for matrix-based Prim's.
    if n:
        scale = matrix_t[-1] / (n[-1] * n[-1]) if n[-1] > 0 else 0.0
        benchmark_t = [scale * (x * x) for x in n]
    else:
        benchmark_t = []

    plt.figure(figsize=(10, 6))
    plt.plot(n, matrix_t, linewidth=1.8, label="Prim's Matrix O(n^2)")
    plt.plot(n, benchmark_t, linestyle='--', linewidth=2.0, color='red', label='Benchmark ~ n^2')
    plt.xlabel('Input Size (number of vertices, n)')
    plt.ylabel('Average Time (milliseconds)')
    plt.title(f"Prim's Algorithm: Time vs Size (Test {test_id})")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(output_path, dpi=140)
    plt.close()
    print(f"Saved: {output_path}")


if __name__ == '__main__':
    for test in range(1, 5):
        if os.path.exists(f"prims_experiment_test{test}.txt"):
            plot_test(test)
        else:
            print(f"Missing input for test {test}")
