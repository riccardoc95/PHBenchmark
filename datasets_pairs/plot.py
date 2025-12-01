import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

RESULTS_DIR = "results"
DATASETS = ["mnist", "cifar10", "imagenet", "div2k", "kather", "astro"]

fig, ax = plt.subplots(figsize=(14, 7))

x = list(range(len(DATASETS)))
offset = 0.25

colors = {
    "H0_pairs": "#1f77b4",
    "H1_pairs": "#ff7f0e",
    "total_pairs": "#2ca02c"
}

all_data_log = {k: [] for k in colors.keys()}

for ds in DATASETS:
    df = pd.read_csv(os.path.join(RESULTS_DIR, f"{ds}.csv"))

    all_data_log["H0_pairs"].append(np.log10(df["H0_pairs"] + 1))
    all_data_log["H1_pairs"].append(np.log10(df["H1_pairs"] + 1))
    all_data_log["total_pairs"].append(np.log10(df["total_pairs"] + 1))

positions_H0 = [i - offset for i in x]
positions_H1 = x
positions_total = [i + offset for i in x]

def draw_box(data, pos, color):
    ax.boxplot(
        data,
        positions=pos,
        widths=0.2,
        patch_artist=True,
        boxprops=dict(facecolor=color, alpha=0.7),
        medianprops=dict(color="black")
    )

draw_box(all_data_log["H0_pairs"], positions_H0, colors["H0_pairs"])
draw_box(all_data_log["H1_pairs"], positions_H1, colors["H1_pairs"])
draw_box(all_data_log["total_pairs"], positions_total, colors["total_pairs"])

ax.set_xticks(x)
ax.set_xticklabels([x.upper() for x in DATASETS], rotation=45)
ax.set_ylabel("log10( persistence pairs + 1 )")
#ax.set_title("Persistence Pairs Distribution (log-scale transformed)")

legend_elements = [
    plt.Line2D([0], [0], color=colors["H0_pairs"], lw=6, label="H0 pairs"),
    plt.Line2D([0], [0], color=colors["H1_pairs"], lw=6, label="H1 pairs"),
    plt.Line2D([0], [0], color=colors["total_pairs"], lw=6, label="total pairs (H0 + H1)"),
]
ax.legend(handles=legend_elements)

plt.tight_layout()
plt.savefig("pairs_boxplot_log.png", dpi=300)
plt.show()
