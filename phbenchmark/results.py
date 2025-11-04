import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import glob
import os

# === Configurazione ===
INPUT_DIR = "results"
OUTPUT_DIR = "results"
DATASET_DIRS = {

}
os.makedirs(f"{OUTPUT_DIR}/plots", exist_ok=True)
os.makedirs(f"{OUTPUT_DIR}/tables", exist_ok=True)

sns.set(style="whitegrid", font_scale=1.2)

# === Lettura di tutti i CSV ===
files = glob.glob(f"{INPUT_DIR}/*.csv")

all_data = []
for f in files:
    base = os.path.basename(f)
    parts = base.replace(".csv", "").split("_")
    try:
        dataset  = parts[0]
        software = parts[1]
        dim_topo = parts[2]
    except IndexError:
        software = "unknown"
        dataset = "unknown"
        dim_topo = "unknown"

    df = pd.read_csv(f)
    df["software"] = software
    df["dataset"] = dataset
    df["dim_topo"] = dim_topo
    all_data.append(df)

df_all = pd.concat(all_data, ignore_index=True)

# === Calcolo della dimensione del dataset (.npy) ===
dataset_dims = {}
for dataset, group in df_all.groupby("dataset"):
    folder = f"datasets/{dataset}_npy"
    npy_files = glob.glob(os.path.join(folder, "*.npy"))
    if npy_files:
        # carica solo il primo file (tutti hanno stessa dimensione)
        arr = np.load(npy_files[0])
        # dimensione totale (numero di elementi)
        dim = arr.size
        # oppure se vuoi la dimensione 2D/3D:
        shape_str = "×".join(map(str, arr.shape))
        dataset_dims[dataset] = {"dim": dim, "shape": shape_str}
        print(f"📏 {dataset}: shape={shape_str}, total_elements={dim}")
    else:
        dataset_dims[dataset] = {"dim": 0, "shape": "unknown"}


df_all["dim_data"] = df_all["dataset"].map(lambda d: dataset_dims[d]["dim"])
df_all["shape"] = df_all["dataset"].map(lambda d: dataset_dims[d]["shape"])

df_all.to_csv(f"{OUTPUT_DIR}/plots/df_all.csv", index=False)
print("✅ Summary saved to output/summary.csv")

# === Statistiche riassuntive ===
summary = (
    df_all.groupby(["software", "dataset", "dim_topo", "shape", "dim_data"])
    .agg(
        mean_time_s=("time_s", "mean"),
        std_time_s=("time_s", "std"),
        mean_mem_mb=("peak_memory_mb", "mean"),
        std_mem_mb=("peak_memory_mb", "std"),
        n=("filename", "count"),
    )
    .reset_index()
)

summary.to_csv(f"{OUTPUT_DIR}/plots/summary.csv", index=False)
print("✅ Summary saved to output/summary.csv")
print(summary)
# === Grafici comparativi ===
# Tempo medio per dataset e dim_topo
plt.figure(figsize=(9, 5))
g = sns.catplot(
    data=summary,
    x="dataset",
    y="mean_time_s",
    hue="software",
    col="dim_topo",
    kind="bar",
    errorbar="sd",
    height=4,
    aspect=1.2
)
g.set_titles("dim_topo = {col_name}")
g.set_axis_labels("Dataset", "Tempo medio (s)")
plt.tight_layout()
plt.savefig(f"{OUTPUT_DIR}/plots/time_mean_by_topodim.png", dpi=300)
plt.close()

# Variante: tempo medio separato per dim_topo
for topo_dim, df_t in summary.groupby("dim_topo"):
    plt.figure(figsize=(8, 5))
    sns.barplot(data=df_t, x="dataset", y="mean_time_s", hue="software", errorbar="sd")
    plt.title(f"Tempo medio per dataset (topological dim={topo_dim})")
    plt.ylabel("Tempo medio (s)")
    plt.xlabel("Dataset")
    plt.legend(title="Software")
    plt.tight_layout()
    plt.savefig(f"{OUTPUT_DIR}/plots/time_mean_dim{topo_dim}.png", dpi=300)
    plt.close()

# Scaling log-log
df_scale = summary.dropna(subset=["dim_data"])
for topo_dim, df_t in df_scale.groupby("dim_topo"):
    plt.figure(figsize=(8, 6))
    sns.lineplot(data=df_t, x="dim_data", y="mean_time_s", hue="software", marker="o")
    plt.xscale("log")
    plt.yscale("log")
    plt.title(f"Scaling log-log (dim_topo={topo_dim})")
    plt.xlabel("Numero elementi (log)")
    plt.ylabel("Tempo medio (s, log)")
    plt.tight_layout()
    plt.savefig(f"{OUTPUT_DIR}/plots/scaling_dim{topo_dim}_loglog.png", dpi=300)
    plt.close()

# === Tabella LaTeX ===
with open(f"{OUTPUT_DIR}/tables/summary_table.tex", "w") as f:
    f.write(summary.to_latex(index=False, float_format="%.2f"))
print("✅ LaTeX table saved to output/tables/summary_table.tex")

print("🎉 Analisi completata!")