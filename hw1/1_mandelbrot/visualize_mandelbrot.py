import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("mandelbrot.csv", delimiter=",", skiprows=1)

z = data[:, 0] + 1j * data[:, 1]

plt.scatter(z.real, z.imag)

plt.axhline(0, linewidth=0.8)
plt.axvline(0, linewidth=0.8)

plt.xlabel("Re")
plt.ylabel("Im")
plt.title("Complex plane")

plt.grid(True)
plt.axis("equal")
plt.show()
