import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os


def load_data(path):
    dense_n, dense_matrix_t, dense_heap_t = [], [], []
    sparse_n, sparse_matrix_t, sparse_heap_t = [], [], []
    section = None

    with open(path, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            if line.startswith('#'):
                if 'SECTION 1: DENSE GRAPH' in line:
                    section = 'dense'
                elif 'SECTION 2: SPARSE GRAPH' in line:
                    section = 'sparse'
                continue

            if line.startswith('n '):
                continue

            parts = line.split()
            if len(parts) < 3 or section is None:
                continue

            n_val = int(parts[0])
            matrix_val = float(parts[1])
            heap_val = float(parts[2])

            if section == 'dense':
                dense_n.append(n_val)
                dense_matrix_t.append(matrix_val)
                dense_heap_t.append(heap_val)
            elif section == 'sparse':
                sparse_n.append(n_val)
                sparse_matrix_t.append(matrix_val)
                sparse_heap_t.append(heap_val)

    return (dense_n, dense_matrix_t, dense_heap_t,
            sparse_n, sparse_matrix_t, sparse_heap_t)


def plot_test(test_id):
    input_path = f"prims_experiment_test{test_id}.txt"
    output_path = f"prims_experiment_test{test_id}.png"

    (dense_n, dense_matrix_t, dense_heap_t,
     sparse_n, sparse_matrix_t, sparse_heap_t) = load_data(input_path)

    plt.figure(figsize=(12, 6))

    plt.subplot(1, 2, 1)
    plt.plot(dense_n, dense_matrix_t, linewidth=1.8, label="Matrix O(n^2)")
    plt.plot(dense_n, dense_heap_t, linewidth=1.8, label="Heap O(E log n)")
    plt.xlabel('n')
    plt.ylabel('Average Time (ms)')
    plt.title('Dense Graph')
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.legend()

    plt.subplot(1, 2, 2)
    plt.plot(sparse_n, sparse_matrix_t, linewidth=1.8, label="Matrix O(n^2)")
    plt.plot(sparse_n, sparse_heap_t, linewidth=1.8, label="Heap O(E log n)")
    plt.xlabel('n')
    plt.ylabel('Average Time (ms)')
    plt.title('Sparse Graph')
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.legend()

    plt.suptitle(f"Prim's Algorithm Analysis (Test {test_id})")
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
