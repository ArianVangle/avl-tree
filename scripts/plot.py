import csv
import os
import sys
from collections import defaultdict

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RESULTS = os.path.join(ROOT, "results")
CSV_PATH = os.path.join(RESULTS, "results.csv")

STRUCTURES = ["AVL", "std_set"]
LABELS = {"AVL": "наше AVL-дерево", "std_set": "std::set (красно-чёрное дерево)"}
COLORS = {"AVL": "#eb6834", "std_set": "#2a78d6"}
MARKERS = {"AVL": "s", "std_set": "o"}
LINE_STYLES = {"AVL": "-", "std_set": "--"}
OPERATIONS = [("insert_ms", "вставка"), ("search_ms", "поиск"), ("remove_ms", "удаление")]
X_LABEL = "число элементов N"

TEXT = "#3a3a38"
GRID = "#e4e3df"

plt.rcParams.update({
    "font.family": "DejaVu Sans",
    "axes.edgecolor": GRID,
    "axes.labelcolor": TEXT,
    "axes.titlecolor": TEXT,
    "xtick.color": TEXT,
    "ytick.color": TEXT,
    "legend.frameon": False,
    "font.size": 10,
})


def load():
    data = defaultdict(dict)
    try:
        with open(CSV_PATH, encoding="utf-8") as file:
            for row in csv.DictReader(file):
                values = {key: float(row[key]) for key in row if key not in ("structure", "n")}
                data[row["structure"]][int(row["n"])] = values
    except FileNotFoundError:
        print("results/results.csv не найден, сначала запустите нагрузочный тест")
        sys.exit(1)
    return data


def style(axis, ylabel, title):
    axis.set_xlabel(X_LABEL)
    axis.set_ylabel(ylabel)
    axis.set_title(title, loc="left")
    axis.grid(True, which="major", color=GRID, linewidth=0.8)
    axis.set_axisbelow(True)
    for side in ("top", "right"):
        axis.spines[side].set_visible(False)


def plot_line(axis, xs, ys, structure):
    axis.plot(xs, ys, color=COLORS[structure], marker=MARKERS[structure],
              linestyle=LINE_STYLES[structure], markersize=5, linewidth=2,
              label=LABELS[structure])


def save(figure, name):
    figure.tight_layout()
    figure.savefig(os.path.join(RESULTS, name), dpi=120)
    plt.close(figure)
    print("сохранён results/" + name)


def draw_time(data):
    figure, axes = plt.subplots(1, 3, figsize=(15, 4.8))
    for axis, (key, title) in zip(axes, OPERATIONS):
        for structure in STRUCTURES:
            ns = sorted(data[structure])
            plot_line(axis, ns, [data[structure][n][key] for n in ns], structure)
        axis.set_xscale("log")
        axis.set_yscale("log")
        style(axis, "время на N операций, мс", title.capitalize() + " N элементов")
        axis.legend()
    save(figure, "time.png")


data = load()
draw_time(data)
