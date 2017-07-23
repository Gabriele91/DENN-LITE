import csv as csv_parser

from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import numpy as np

import os
import sys 

base_col = 0
population = []
for arg in sys.argv[1:]:
    base_col = len(population)
    #if base_col != 0:
    #    population.append([])
    #    population[base_col] = [-1.0 for x in range(len(population[0]))]
    #    base_col+=1
    with open(sys.argv[1], 'r') as csvFile:
        reader = csv_parser.reader(csvFile, delimiter=';')
        for row in reader:
            for col, val in enumerate(row):
                if val.strip() == "":
                    continue
                if len(population)-base_col <= col:
                    population.append([])
                population[col+base_col].append(float(val))

# Make data.
X = np.arange(0, len(population[0]))
Y = np.arange(0, len(population))
X, Y = np.meshgrid(X, Y)
Z = np.array(population)
# Plot data
fig = plt.figure()
ax = fig.gca(projection='3d')

# Plot the surface.
surf = ax.plot_surface(X, Y, Z,  cmap=cm.coolwarm, linewidth=0, antialiased=False)
ax.set_zlim(0.0, 1.0)
ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))
ax.set_xlabel('generations')
ax.set_ylabel('population')
ax.set_zlabel('cross entropy')

# Add a color bar which maps values to colors.
fig.colorbar(surf, shrink=0.5, aspect=5)

plt.show()
