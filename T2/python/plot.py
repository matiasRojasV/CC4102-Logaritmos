"""Genera las figuras principales a partir de los resultados experimentales."""

from argparse import ArgumentParser
from pathlib import Path

import matplotlib.pyplot as plt
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
    project_dir = Path(__file__).resolve().parents[1]
    parser = ArgumentParser(description=__doc__)
    parser.add_argument(
        "--input-dir",
        type=Path,
        default=project_dir / "out",
        help="carpeta con experiment_results.csv y alpha_bonus_results.csv",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=project_dir / "figures",
        help="carpeta donde se guardarán las figuras",
    )
    return parser.parse_args()


def load_csv(input_dir, filename, columns):
    """Carga un CSV y verifica que tenga las columnas necesarias."""
    path = input_dir / filename
    if not path.exists():
        raise FileNotFoundError(f"No se encontró el archivo de resultados: {path}")

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
    """Genera la figura de escrituras y tiempo de construcción."""
    figure, axes = plt.subplots(1, 2, figsize=(13, 5), constrained_layout=True)
    plot_tree_metric(
        axes[0],
        data,
        "RAM_Writes",
        "Escrituras durante la inserción",
        "RAM_Writes",
    )
    plot_tree_metric(
        axes[1],
        data,
        "BuildTime",
        "Tiempo de construcción",
        "Tiempo (s)",
    )
    figure.suptitle("Costo de inserción")
    figure.savefig(output_dir / "insertion_cost.png", dpi=150)
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
    figure.savefig(output_dir / "search_cost.png", dpi=150)
    plt.close(figure)


def plot_alpha(data, output_dir):
    """Genera la figura del efecto de alpha sobre inserción y búsqueda."""
    figure, axes = plt.subplots(1, 2, figsize=(13, 5), constrained_layout=True)
    axes[0].plot(data["Alpha"], data["RAM_Writes"], marker="o", color="#d62728")
    axes[0].set_title("Escrituras según alpha")
    axes[0].set_xlabel("Alpha")
    axes[0].set_ylabel("RAM_Writes")

    axes[1].plot(data["Alpha"], data["AvgDiskReads"], marker="o", color="#9467bd")
    axes[1].set_title("Lecturas de disco según alpha")
    axes[1].set_xlabel("Alpha")
    axes[1].set_ylabel("AvgDiskReads")

    for axis in axes:
        axis.grid(True, alpha=0.25)
    figure.suptitle(r"Análisis del parámetro $\alpha$")
    figure.savefig(output_dir / "alpha_analysis.png", dpi=150)
    plt.close(figure)


def main():
    """Lee los resultados y crea todas las figuras del informe."""
    args = parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)

    experiment_columns = {
        "N",
        "Tree",
        "BuildTime",
        "RAM_Writes",
        "AvgSearchTime",
        "AvgDiskReads",
    }
    alpha_columns = {"Alpha", "RAM_Writes", "AvgDiskReads"}
    experiment_data = load_csv(
        args.input_dir, "experiment_results.csv", experiment_columns
    )
    alpha_data = load_csv(
        args.input_dir, "alpha_bonus_results.csv", alpha_columns
    )

    plot_insertion(experiment_data, args.output_dir)
    plot_search(experiment_data, args.output_dir)
    plot_alpha(alpha_data, args.output_dir)
    print(f"Figuras guardadas en {args.output_dir}")


if __name__ == "__main__":
    main()