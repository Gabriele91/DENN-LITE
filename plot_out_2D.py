import csv as csv_parser
import matplotlib.pyplot as plt
import os
import sys 

population = []
with open(sys.argv[1], 'r') as csvFile:
    reader = csv_parser.reader(csvFile, delimiter=';')
    for row in reader:
        for col, val in enumerate(row):
            if val.strip() == "":
                continue
            if len(population) <= col:
                population.append([])
            population[col].append(float(val))

for individual in population:
    plt.plot(individual)
plt.ylabel('cross entropy')
plt.xlabel('generation')
plt.show()
