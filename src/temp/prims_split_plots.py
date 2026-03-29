from pathlib import Path
import math

try:
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
except ModuleNotFoundError:
    print("Missing dependency: matplotlib")
    print("Install it with: python3 -m pip install matplotlib")
    raise SystemExit(1)

# Manual absolute input path
DATA_DIR = Path("/Users/amiteshwarsingh/Documents/ADA/prims/data")
# Output in src only
OUT_DIR = Path("/Users/amiteshwarsingh/Documents/ADA/prims/src/temp/graphs")


def scaled_benchmark(n_vals, measured_vals, complexity_fn):
    if not n_vals or not measured_vals:
        return []
    base_c = complexity_fn(n_vals[0])
    if base_c == 0:
        return [0.0 for _ in n_vals]
    scale = measured_vals[0] / base_c
    return [scale * complexity_fn(n) for n in n_vals]


def load_data(path: Path):
    dense_n, dense_matrix_t, dense_heap_t = [], [], []
    sparse_n, sparse_matrix_t, sparse_heap_t = [], [], []
    section = None

    with path.open("r") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            if line.startswith("#"):
                if "SECTION 1: DENSE GRAPH" in line:
                    section = "dense"
                elif "SECTION 2: SPARSE GRAPH" in line:
                    section = "sparse"
                continue

            if line.startswith("n "):
                continue

            parts = line.split()
            if len(parts) < 3 or section is None:
                continue

            n_val = int(parts[0])
            matrix_val = float(parts[1])
            heap_val = float(parts[2])

            if section == "dense":
                dense_n.append(n_val)
                dense_matrix_t.append(matrix_val)
                dense_heap_t.append(heap_val)
            elif section == "sparse":
                sparse_n.append(n_val)
                sparse_matrix_t.append(matrix_val)
                sparse_heap_t.append(heap_val)

    return {
        "dense": {"n": dense_n, "matrix": dense_matrix_t, "heap": dense_heap_t},
        "sparse": {"n": sparse_n, "matrix": sparse_matrix_t, "heap": sparse_heap_t},
    }


def plot_on_axis(ax, n_vals, y_vals, title, y_label, color: str, bench_fn, bench_label: str):
    ax.plot(n_vals, y_vals, linewidth=2.0, color=color, label="Measured")
    bench = scaled_benchmark(n_vals, y_vals, bench_fn)
    ax.plot(n_vals, bench, "--", linewidth=1.8, color="black", label=bench_label)
    ax.set_xlabel("n")
    ax.set_ylabel(y_label)
    ax.set_title(title)
    ax.grid(True, linestyle="--", alpha=0.5)
    ax.legend()


def plot_test(test_id: int):
    input_path = DATA_DIR / f"prims_experiment_test{test_id}.txt"
    if not input_path.exists():
        print(f"Missing input: {input_path}")
        return

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    data = load_data(input_path)

    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle(f"Prim's Test {test_id} - All Four Graphs", fontsize=14)

    plot_on_axis(
        axes[0][0],
        data["dense"]["n"],
        data["dense"]["matrix"],
        f"Prim Dense - Matrix O(n^2) (Test {test_id})",
        "Matrix Average Time (ms)",
        "#1f77b4",
        lambda n: n * n,
        "Benchmark ~ n^2",
    )

    plot_on_axis(
        axes[0][1],
        data["dense"]["n"],
        data["dense"]["heap"],
        f"Prim Dense - Heap O((n + E) log n) (Test {test_id})",
        "Heap Average Time (ms)",
        "#ff7f0e",
        lambda n: n * n * math.log2(max(n, 2)),
        "Benchmark ~ n^2 log n",
    )

    plot_on_axis(
        axes[1][0],
        data["sparse"]["n"],
        data["sparse"]["matrix"],
        f"Prim Sparse - Matrix O(n^2) (Test {test_id})",
        "Matrix Average Time (ms)",
        "#2ca02c",
        lambda n: n * n,
        "Benchmark ~ n^2",
    )

    plot_on_axis(
        axes[1][1],
        data["sparse"]["n"],
        data["sparse"]["heap"],
        f"Prim Sparse - Heap O((n + E) log n) (Test {test_id})",
        "Heap Average Time (ms)",
        "#d62728",
        lambda n: n * math.log2(max(n, 2)),
        "Benchmark ~ n log n",
    )

    plt.tight_layout(rect=(0, 0, 1, 0.96))
    out_file = OUT_DIR / f"prims_test{test_id}_combined.png"
    plt.savefig(out_file, dpi=140)
    plt.close()
    print(f"Saved: {out_file}")


if __name__ == "__main__":
    for test in range(1, 5):
        plot_test(test)
