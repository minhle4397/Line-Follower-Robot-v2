#!/usr/bin/env python3
"""Plot a CSV trace from the simulation or from the car's telemetry.

Usage:
    ./build/host/sim --csv > sim.csv
    python3 tools/plot_csv.py sim.csv t y_mm
"""
import csv
import sys

import matplotlib.pyplot as plt

def main() -> int:
    if len(sys.argv) < 4:
        print(__doc__)
        return 1
    path, x_col, y_cols = sys.argv[1], sys.argv[2], sys.argv[3:]

    with open(path, newline="") as f:
        rows = list(csv.DictReader(f))
    if not rows:
        print("empty file")
        return 1

    x = [float(r[x_col]) for r in rows]
    for col in y_cols:
        plt.plot(x, [float(r[col]) for r in rows], label = col)
    plt.xlabel(x_col)
    plt.grid(True)
    plt.legend()
    plt.title(path)
    plt.show()
    return 0
if __name__ == "__main__":
    sys.exit(main())
