import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Crear directorio de salida
output_dir = "plots"
os.makedirs(output_dir, exist_ok=True)

# Configuración estética general
plt.rcParams.update({
    'font.sans-serif': 'DejaVu Sans',
    'axes.edgecolor': '#333333',
    'axes.linewidth': 0.8,
    'grid.color': '#cccccc',
    'grid.linestyle': '--',
    'grid.alpha': 0.6,
    'figure.autolayout': True
})

# Cargar datos
df = pd.read_csv("results_experimental.csv")

# Funciones de Cotas Teóricas
def theoretical_binomial(v, e):
    return e * np.log2(v)

def theoretical_fibonacci(v, e):
    return e + v * np.log2(v)

def fit_constant(x_measured, y_theory):
    """ Enombra la constante c tal que c * y_theory ~ x_measured """
    return np.sum(x_measured * y_theory) / np.sum(y_theory**2)


# COSTO TOTAL: Series A y B
for serie_name in ["Serie A", "Serie B"]:
    sub_df = df[df["Serie"] == serie_name].sort_values(by="E" if serie_name == "Serie A" else "V")
    
    v_vals = sub_df["V"].values
    e_vals = sub_df["E"].values
    x_axis = e_vals if serie_name == "Serie A" else v_vals
    x_label = "Cantidad de Aristas (E)" if serie_name == "Serie A" else "Cantidad de Vértices (V)"
    
    time_bin = sub_df["TimeBinomial"].values
    time_fib = sub_df["TimeFibonacci"].values
    
    # Cotas Teóricas
    theory_bin = theoretical_binomial(v_vals, e_vals)
    theory_fib = theoretical_fibonacci(v_vals, e_vals)
    
    c_bin = fit_constant(time_bin, theory_bin)
    c_fib = fit_constant(time_fib, theory_fib)
    
    # Mismo límite Y para poder comparar ambas estructuras
    y_max = max(np.max(time_bin), np.max(time_fib)) * 1.1
    y_min = 0

    # Plot Binomial
    fig, ax = plt.subplots(figsize=(6, 4))
    ax.plot(x_axis, time_bin, 'o-', color='#1f77b4', label='Medido (Binomial)', linewidth=2)
    ax.plot(x_axis, c_bin * theory_bin, '--', color='#d62728', label=r'Cota $c \cdot E \log_2 V$', linewidth=1.5)
    ax.set_title(f"Cola Binomial - {serie_name}", fontsize=12, fontweight='bold')
    ax.set_xlabel(x_label)
    ax.set_ylabel("Tiempo Total (s)")
    ax.set_ylim(y_min, y_max)
    ax.grid(True)
    ax.legend()
    plt.savefig(f"{output_dir}/total_{serie_name.lower().replace(' ', '_')}_bin.svg")
    plt.close()

    # Plot Fibonacci
    fig, ax = plt.subplots(figsize=(6, 4))
    ax.plot(x_axis, time_fib, 's-', color='#2ca02c', label='Medido (Fibonacci)', linewidth=2)
    ax.plot(x_axis, c_fib * theory_fib, '--', color='#d62728', label=r'Cota $c \cdot (E + V \log_2 V)$', linewidth=1.5)
    ax.set_title(f"Cola de Fibonacci - {serie_name}", fontsize=12, fontweight='bold')
    ax.set_xlabel(x_label)
    ax.set_ylabel("Tiempo Total (s)")
    ax.set_ylim(y_min, y_max)
    ax.grid(True)
    ax.legend()
    plt.savefig(f"{output_dir}/total_{serie_name.lower().replace(' ', '_')}_fib.svg")
    plt.close()


# COSTO AMORTIZADO: Series C y D
for serie_name in ["Serie C", "Serie D"]:
    sub_df = df[df["Serie"] == serie_name].sort_values(by="E" if serie_name == "Serie C" else "V")
    
    v_vals = sub_df["V"].values
    e_vals = sub_df["E"].values
    x_axis = e_vals if serie_name == "Serie C" else v_vals
    x_label = "Cantidad de Aristas (E)" if serie_name == "Serie C" else "Cantidad de Vértices (V)"
    
    # Tiempos acumulados y operaciones
    t_bin = sub_df["TimeBinomial"].values
    t_fib = sub_df["TimeFibonacci"].values
    ops_bin = sub_df["OpsBinomial"].values
    ops_fib = sub_df["OpsFibonacci"].values
    
    theory_bin = theoretical_binomial(v_vals, e_vals)
    theory_fib = theoretical_fibonacci(v_vals, e_vals)
    
    # Escalas unificadas para pares de gráficos
    max_t = max(np.max(t_bin), np.max(t_fib)) * 1.1
    max_ops = max(np.max(ops_bin), np.max(ops_fib)) * 1.1

    # Binomial Time
    c_tb = fit_constant(t_bin, theory_bin)
    fig, ax = plt.subplots(figsize=(6, 4))
    ax.plot(x_axis, t_bin, 'o-', color='#1f77b4', label='Tiempo Medido')
    ax.plot(x_axis, c_tb * theory_bin, '--', color='#d62728', label=r'Cota $c \cdot E \log_2 V$')
    ax.set_title(f"Tiempo Acumulado Binomial - {serie_name}", fontweight='bold')
    ax.set_xlabel(x_label)
    ax.set_ylabel("Tiempo (s)")
    ax.set_ylim(0, max_t)
    ax.grid(True)
    ax.legend()
    plt.savefig(f"{output_dir}/amort_time_{serie_name.lower().replace(' ', '_')}_bin.svg")
    plt.close()

    # Fibonacci Time
    c_tf = fit_constant(t_fib, theory_fib)
    fig, ax = plt.subplots(figsize=(6, 4))
    ax.plot(x_axis, t_fib, 's-', color='#2ca02c', label='Tiempo Medido')
    ax.plot(x_axis, c_tf * theory_fib, '--', color='#d62728', label=r'Cota $c \cdot (E + V \log_2 V)$')
    ax.set_title(f"Tiempo Acumulado Fibonacci - {serie_name}", fontweight='bold')
    ax.set_xlabel(x_label)
    ax.set_ylabel("Tiempo (s)")
    ax.set_ylim(0, max_t)
    ax.grid(True)
    ax.legend()
    plt.savefig(f"{output_dir}/amort_time_{serie_name.lower().replace(' ', '_')}_fib.svg")
    plt.close()


    # Binomial Ops
    c_ob = fit_constant(ops_bin, theory_bin)
    fig, ax = plt.subplots(figsize=(6, 4))
    ax.plot(x_axis, ops_bin, 'o-', color='#ff7f0e', label='Operaciones Medidas')
    ax.plot(x_axis, c_ob * theory_bin, '--', color='#d62728', label=r'Cota $c \cdot E \log_2 V$')
    ax.set_title(f"Conteo Operaciones Binomial - {serie_name}", fontweight='bold')
    ax.set_xlabel(x_label)
    ax.set_ylabel("N° Operaciones Estructurales")
    ax.set_ylim(0, max_ops)
    ax.grid(True)
    ax.legend()
    plt.savefig(f"{output_dir}/amort_ops_{serie_name.lower().replace(' ', '_')}_bin.svg")
    plt.close()

    # Fibonacci Ops
    c_of = fit_constant(ops_fib, theory_fib)
    fig, ax = plt.subplots(figsize=(6, 4))
    ax.plot(x_axis, ops_fib, 's-', color='#9467bd', label='Operaciones Medidas')
    ax.plot(x_axis, c_of * theory_fib, '--', color='#d62728', label=r'Cota $c \cdot (E + V \log_2 V)$')
    ax.set_title(f"Conteo Operaciones Fibonacci - {serie_name}", fontweight='bold')
    ax.set_xlabel(x_label)
    ax.set_ylabel("N° Operaciones Estructurales")
    ax.set_ylim(0, max_ops)
    ax.grid(True)
    ax.legend()
    plt.savefig(f"{output_dir}/amort_ops_{serie_name.lower().replace(' ', '_')}_fib.svg")
    plt.close()