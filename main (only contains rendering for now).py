import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors

def build_terrain_colormap():
    terrain_colors = [
        (0.00, "#0a1628"),   # deep ocean
        (0.30, "#1a3a6b"),   # ocean
        (0.42, "#2d6a9f"),   # shallow sea
        (0.48, "#4aa3d9"),   # coast
        (0.50, "#e8d5a3"),   # beach
        (0.52, "#91c87a"),   # dry sand
        (0.54, "#73c44e"),   # light grass
        (0.62, "#4caa42"),   # grass
        (0.70, "#2B8928"),   # forest
        (0.78, "#6b7c52"),   # highland
        (0.84, "#7a6b52"),   # rocky
        (0.90, "#a0a0a0"),   # mountain
        (0.96, "#d0d8e0"),   # snow slope
        (1.00, "#ffffff"),   # peak snow
    ]
    positions = [c[0] for c in terrain_colors]
    colors    = [c[1] for c in terrain_colors]
    return mcolors.LinearSegmentedColormap.from_list(
        "terrain_custom",
        list(zip(positions, colors))
    )

col = build_terrain_colormap()
data = np.loadtxt("outputFile.txt")
plt.imshow(data, cmap=col, interpolation='nearest')
plt.colorbar()
plt.show()
