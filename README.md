# GVNS for the Graph 2-Burning Problem

Research implementation of a **General Variable Neighborhood Search (GVNS)** for the **graph 2-burning problem**, developed as part of my undergraduate thesis in Computer Science at the Federal University of Ceará (UFC), Quixadá Campus, under the supervision of **Prof. Dr. Atílio Gomes Luiz**.

## About the problem

Graph 2-burning is a discrete diffusion process on a graph. Initially, every vertex is unburned. At each round:

1. every unburned vertex with at least two burned neighbors from the previous round becomes burned; and
2. one still-unburned vertex may be selected as a new burning source.

The goal is to find a sequence of sources that burns the whole graph in as few rounds as possible.

This repository explores the problem using **General Variable Neighborhood Search (GVNS)** and **Variable Neighborhood Descent (VND)**.

## Approach

The algorithm starts from constructive heuristics based on:

- relative vertex degree; and
- a combination of relative degree and trigger information.

Both initial solutions are evaluated and the best one is used as the starting point.

The GVNS then alternates between perturbation and local search. The VND explores five neighborhood structures:

1. **removal** of a burning source;
2. **substitution** of a source by a vertex outside the current solution;
3. **relocation** of a source inside the sequence;
4. **swap** of two source positions;
5. **addition** of a new source.

Candidate sequences are evaluated through an explicit simulation of the 2-burning process. Incomplete sequences are rejected instead of being automatically repaired.

## Repository structure

```text
.
├── gvns-2queima.cpp               # Command-line executable
├── GVNS.cpp/.h                    # Main GVNS procedure
├── VND.cpp/.h                     # Variable Neighborhood Descent
├── Vizinhancas.cpp/.h             # Neighborhood structures and perturbation
├── Avaliador.cpp/.h               # Candidate-solution evaluation
├── Solucao.h                      # Solution representation
├── SimulacaoQueima.h              # 2-burning simulation
├── Graph.cpp/.h                   # Graph representation and parser
├── heuristica_inicial/            # Constructive initial heuristics
├── tests/                         # Automated tests
├── benchmark/                     # Experimental results
└── Makefile
```

## Requirements

- C++17-compatible compiler
- GNU Make

The current Makefile uses `g++` with optimization flags enabled.

## Building

Clone the repository and compile the executable:

```bash
git clone https://github.com/Leudes/gvns-2queima.git
cd gvns-2queima
make
```

This generates:

```text
gvns-2queima
```

To remove generated files:

```bash
make clean
```

## Tests

The repository includes tests for the evaluator, initial heuristics, neighborhood moves, VND, perturbation, rejection of incomplete solutions, and the complete GVNS procedure.

Run them with:

```bash
make teste
```

A successful run ends with:

```text
Todos os testes do GVNS passaram.
```

## Input format

Graph instances are read as an **edge list**. Each non-comment line must contain two integer vertex identifiers:

```text
0 1
1 2
2 3
0 3
```

Lines beginning with `#` or `%` are ignored, as are self-loops. The graph is treated as undirected and duplicate edges are removed.

For the intended experiments, vertex identifiers should be contiguous and start at `0`.

## Running

Basic usage:

```bash
./gvns-2queima <graph_file> [options]
```

Example:

```bash
./gvns-2queima instancias/example.txt --tempo 30 --trials 10 --seed 42 --output resultados_gvns.csv
```

Available options:

```text
--tempo SEGUNDOS
--kmax VALOR
--sem-melhoria VALOR
--avaliacoes-vizinhanca VALOR
--avaliacoes-vnd VALOR
--trials VALOR
--seed VALOR
--output ARQUIVO
--irace
```

Default GVNS parameters are:

| Parameter | Default |
| --- | ---: |
| Time limit | 10 s |
| Maximum shaking intensity | 5 |
| Iterations without improvement | 100 |
| Evaluations per neighborhood | 100 |
| Evaluations per VND call | 2000 |
| Trials | 1 |
| Seed | 0 |

The output CSV records the graph size and density, objective value, execution time, number of sources, iterations, evaluations, GVNS/local-search improvements, initial heuristic selected, and random seed. The corresponding source sequence is also written after each result.

## Algorithm outline

```text
build two initial solutions
select the best one
apply VND

while time remains and the no-improvement limit is not reached:
    for increasing shaking intensity:
        perturb the current solution
        apply VND to the perturbed solution

        if the candidate improves the current solution:
            accept it
            reset the shaking intensity
        else:
            increase the shaking intensity
```

The search uses reproducible pseudo-random seeds and can execute multiple independent trials from the command line.

## Related implementation

A separate implementation using **Biased Random-Key Genetic Algorithm (BRKGA)** is available at:

https://github.com/Leudes/brkga-2queima

## Academic context

This repository is part of the undergraduate thesis:

> **Meta-heurísticas para o Problema da 2-Queima em Grafos**

**Author:** Francisco Leudes Bezerra Neto  
**Institution:** Federal University of Ceará (UFC) — Quixadá Campus  
**Advisor:** Prof. Dr. Atílio Gomes Luiz

The research investigates BRKGA and GVNS as metaheuristic approaches for the graph 2-burning problem, with experimental comparison against known values, theoretical bounds, and exact methods when available.
