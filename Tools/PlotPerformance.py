#!/usr/bin/env python3
import csv
import statistics
import sys
from collections import defaultdict
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def main():
    if len(sys.argv) not in (2, 3):
        raise SystemExit("Использование: python3 Tools/PlotPerformance.py CSV [папка_графиков]")
    source = Path(sys.argv[1])
    target = Path(sys.argv[2]) if len(sys.argv) == 3 else source.parent / "graphs"
    values = defaultdict(list)
    with source.open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(stream)
        required = {"count", "order", "operation", "milliseconds"}
        if not required.issubset(reader.fieldnames or []):
            raise SystemExit("В CSV отсутствуют необходимые столбцы")
        for row in reader:
            count = int(row["count"])
            elapsed = float(row["milliseconds"])
            if count <= 0 or elapsed < 0:
                raise SystemExit("Некорректное измерение")
            values[row["operation"], row["order"], count].append(elapsed)
    if not values:
        raise SystemExit("CSV не содержит измерений")
    target.mkdir(parents=True, exist_ok=True)
    names = {"insert": "Вставка", "search": "Поиск", "remove": "Удаление"}
    figure, axes = plt.subplots(1, 3, figsize=(15, 5), constrained_layout=True)
    for axis, (operation, title) in zip(axes.flat, names.items()):
        for order, label, color in (("random", "Случайный порядок", "#2769ad"),
                                    ("ordered", "Упорядоченный порядок", "#bf572d")):
            counts = sorted(count for op, mode, count in values if op == operation and mode == order)
            if not counts:
                continue
            medians = [statistics.median(values[operation, order, count]) for count in counts]
            minimums = [min(values[operation, order, count]) for count in counts]
            maximums = [max(values[operation, order, count]) for count in counts]
            axis.plot(counts, medians, "o-", label=label, color=color, markersize=4)
            axis.fill_between(counts, minimums, maximums, color=color, alpha=0.15)
        axis.set_xscale("log")
        axis.set_yscale("log")
        axis.set_title(title)
        axis.set_xlabel("Количество элементов")
        axis.set_ylabel("Время всей операции, мс")
        axis.grid(True, which="both", alpha=0.2)
        axis.legend()
    figure.suptitle("AVL-дерево: фактические измерения\nМедиана повторов; полоса — минимум и максимум", fontsize=14)
    for suffix in ("png", "svg"):
        path = target / f"{source.stem}.{suffix}"
        figure.savefig(path, dpi=180)
        print(path)
    plt.close(figure)


if __name__ == "__main__":
    main()
