from argparse import ArgumentParser
from pathlib import Path
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import pandas as pd

TREE_LABELS = {
    "BTree": "Árbol B",
    "Beps025": r"$B^\epsilon_{0.25}$",
    "Beps075": r"$B^\epsilon_{0.75}$",
}

TREE_COLORS = {
    "BTree": "#1f77b4",
    "Beps025": "#ff7f0e",
    "Beps075": "#2ca02c",
}


def parse_args():
    """Obtiene las carpetas de entrada y salida desde la línea de comandos."""
    parser = ArgumentParser(description=__doc__)
    parser.add_argument(
        "input_dir",
        type=Path,
        help="carpeta con los archivos CSV",
    )
    parser.add_argument(
        "output_dir",
        type=Path,
        help="carpeta donde se guardarán las figuras SVG",
    )
    return parser.parse_args()


def load_csv(input_dir, filename, columns):
    """Carga un CSV y verifica que tenga las columnas necesarias."""
    path = input_dir / filename
    data = pd.read_csv(path)
    missing = sorted(set(columns) - set(data.columns))
    if missing:
        raise ValueError(f"{path} no contiene las columnas: {', '.join(missing)}")
    return data


def configure_axes(axes, title, y_label):
    """Aplica formato común a los ejes de una figura."""
    axes.set_title(title)
    axes.set_xlabel("N")
    axes.set_ylabel(y_label)
    axes.set_xscale("log", base=2)
    axes.grid(True, which="both", alpha=0.25)


def plot_tree_metric(axes, data, metric, title, y_label):
    """Dibuja una métrica para las tres estructuras de árbol."""
    for tree, group in data.groupby("Tree", sort=False):
        axes.plot(
            group["N"],
            group[metric],
            marker="o",
            label=TREE_LABELS.get(tree, tree),
            color=TREE_COLORS.get(tree),
        )
    configure_axes(axes, title, y_label)
    axes.legend()


def plot_insertion(data, output_dir):
    """Genera las figuras de accesos y tiempo de construcción."""
    figure, axes = plt.subplots(1, 3, figsize=(18, 5), constrained_layout=True)
    plot_tree_metric(
        axes[0],
        data,
        "RAM_Reads",
        "Lecturas durante la inserción",
        "RAM_Reads",
    )
    plot_tree_metric(
        axes[1],
        data,
        "RAM_Writes",
        "Escrituras durante la inserción",
        "RAM_Writes",
    )
    plot_tree_metric(
        axes[2],
        data,
        "BuildTime",
        "Tiempo de construcción",
        "Tiempo (s)",
    )
    figure.suptitle("Costo de inserción")
    figure.savefig(output_dir / "insertion_cost.svg", format="svg")
    plt.close(figure)


def plot_size(data, output_dir):
    """Genera el tamaño del árbol en cantidad de bloques."""
    figure, axis = plt.subplots(figsize=(8, 5), constrained_layout=True)
    plot_tree_metric(axis, data, "NumNodes", "Tamaño del árbol", "Nodos (bloques)")
    figure.savefig(output_dir / "tree_size.svg", format="svg")
    plt.close(figure)


def plot_search(data, output_dir):
    """Genera la figura de lecturas de disco y tiempo de búsqueda."""
    figure, axes = plt.subplots(1, 2, figsize=(13, 5), constrained_layout=True)
    plot_tree_metric(
        axes[0],
        data,
        "AvgDiskReads",
        "Lecturas de bloques por búsqueda",
        "AvgDiskReads",
    )
    plot_tree_metric(
        axes[1],
        data,
        "AvgSearchTime",
        "Tiempo promedio de búsqueda",
        "Tiempo (s)",
    )
    figure.suptitle("Costo de búsqueda por rango")
    figure.savefig(output_dir / "search_cost.svg", format="svg")
    plt.close(figure)


def plot_alpha(data, output_dir):
    """Genera las métricas de inserción y búsqueda según alpha."""
    metrics = [
        ("BuildTime", "Tiempo de construcción", "Tiempo (s)", "#1f77b4"),
        ("RAM_Writes", "Escrituras durante la inserción", "RAM_Writes", "#d62728"),
        ("AvgSearchTime", "Tiempo promedio de búsqueda", "Tiempo (s)", "#2ca02c"),
        ("AvgDiskReads", "Lecturas por búsqueda", "AvgDiskReads", "#9467bd"),
    ]
    
    figure, axes = plt.subplots(2, 2, figsize=(13, 9), constrained_layout=True)
    for axis, (metric, title, y_label, color) in zip(axes.flat, metrics):
        axis.plot(data["Alpha"], data[metric], marker="o", color=color)
        axis.set_title(title)
        axis.set_xlabel("Alpha")
        axis.set_ylabel(y_label)
        axis.grid(True, alpha=0.25)
    figure.suptitle(r"Análisis del parámetro $\alpha$")
    figure.savefig(output_dir / "alpha_analysis.svg", format="svg")
    plt.close(figure)


def plot_range_comparison(data, output_dir):
    """Grafica temperaturas de una misma semana para los dos tamaños pedidos."""
    figure, axes = plt.subplots(2, 1, figsize=(12, 8), sharex=True,
                                constrained_layout=True)
    for axis, (n, group) in zip(axes, data.groupby("N", sort=True)):
        timestamps = pd.to_datetime(group["Key"], unit="s", utc=True)
        axis.plot(timestamps, group["Temperature"], linewidth=0.7)
        axis.set_title(f"Árbol B, N={int(n):,}")
        axis.set_ylabel("Temperatura (°C)")
        axis.grid(True, alpha=0.25)
    axes[-1].set_xlabel("Tiempo (UTC)")
    axes[-1].xaxis.set_major_formatter(mdates.DateFormatter("%d-%m %H:%M"))
    figure.suptitle("Misma búsqueda por rango para dos tamaños de entrada")
    figure.savefig(output_dir / "range_comparison.svg", format="svg")
    plt.close(figure)


def main():
    """Lee los resultados y crea todas las figuras del informe."""
    args = parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)

    experiment_columns = {
        "N",
        "Tree",
        "Alpha",
        "BuildTime",
        "RAM_Reads",
        "RAM_Writes",
        "NumNodes",
        "AvgSearchTime",
        "AvgDiskReads",
    }
    alpha_columns = {
        "Alpha",
        "BuildTime",
        "RAM_Writes",
        "AvgSearchTime",
        "AvgDiskReads",
    }
    experiment_data = load_csv(
        args.input_dir, "experiment_results.csv", experiment_columns
    )
    alpha_data = load_csv(
        args.input_dir, "alpha_bonus_results.csv", alpha_columns
    )
    comparison_data = load_csv(
        args.input_dir,
        "range_comparison.csv",
        {"N", "Key", "Temperature"},
    )

    plot_insertion(experiment_data, args.output_dir)
    plot_size(experiment_data, args.output_dir)
    plot_search(experiment_data, args.output_dir)
    plot_alpha(alpha_data, args.output_dir)
    plot_range_comparison(comparison_data, args.output_dir)
    print(f"Figuras guardadas en {args.output_dir}")


if __name__ == "__main__":
    main()