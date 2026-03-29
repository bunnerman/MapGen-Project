"""
Perlin Noise → 2D World Map
============================
Direct Python port of the C++ Perlin noise code,
extended with fractal octaves and terrain-colour rendering.

Requirements:
    pip install numpy matplotlib
"""

import math
import random
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
from matplotlib.widgets import Slider, Button


# ─────────────────────────────────────────────
#  1. GRADIENT DIRECTIONS  (same 8 as C++ code)
# ─────────────────────────────────────────────
# Each index maps to a unit vector (cos θ, sin θ).
# 0 = East (0°), 1 = NE (45°), 2 = North (90°) … 7 = SE (315°)
GRADIENTS = [
    ( 1.00000000,  0.00000000),   # 0: East
    ( 0.70710678,  0.70710678),   # 1: NE
    ( 0.00000000,  1.00000000),   # 2: North
    (-0.70710678,  0.70710678),   # 3: NW
    (-1.00000000,  0.00000000),   # 4: West
    (-0.70710678, -0.70710678),   # 5: SW
    ( 0.00000000, -1.00000000),   # 6: South
    ( 0.70710678, -0.70710678),   # 7: SE
]


# ─────────────────────────────────────────────
#  2. CORE PERLIN FUNCTIONS
# ─────────────────────────────────────────────

def generate_gradient_grid(resolution: int) -> list[list[int]]:
    """
    C++ equivalent: generateGradientVectors()
    Creates an N×N grid of random ints 0-7 (one per gradient direction).
    """
    return [
        [random.randint(0, 7) for _ in range(resolution)]
        for _ in range(resolution)
    ]


def dot_product(gradient_index: int, dx: float, dy: float) -> float:
    """
    C++ equivalent: dotProduct()
    Converts gradient index to a unit vector, then dots with (dx, dy).
    """
    gx, gy = GRADIENTS[gradient_index]
    return gx * dx + gy * dy


def fade(t: float) -> float:
    """
    C++ equivalent: perlinFadeFunction()
    Ken Perlin's improved smoothstep: 6t^5 - 15t^4 + 10t^3
    Makes transitions perfectly smooth (zero first AND second derivative at edges).
    """
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0)


def lerp(a: float, b: float, t: float) -> float:
    """
    C++ equivalent: linearInterpolation()
    Standard linear interpolation between a and b by factor t.
    """
    return a + t * (b - a)


def perlin_value(
    x: int, y: int,
    gradient_grid: list[list[int]],
    noise_resolution: int
) -> float:
    """
    C++ equivalent: generatePerlinValue()
    Generates a single Perlin noise value for pixel (x, y).

    Steps:
      1. Scale pixel coordinate into gradient-grid space.
      2. Find the surrounding grid cell and local offsets.
      3. Compute dot products at all 4 corners.
      4. Fade the offsets.
      5. Bilinear interpolation → final value ∈ roughly [-1, 1].
    """
    grad_res = len(gradient_grid)
    scale = (grad_res - 1) / noise_resolution   # same as C++

    # Position inside gradient grid
    sx = x * scale
    sy = y * scale

    # Integer cell coordinates
    gx = int(math.floor(sx))
    gy = int(math.floor(sy))

    # Fractional (local) offsets within the cell
    lx = sx - gx
    ly = sy - gy

    # Corner dot products
    # Each corner: gradient_grid[row][col] gives the direction index;
    # distance vector points FROM that corner TO the sample point.
    dp00 = dot_product(gradient_grid[gy    ][gx    ],  lx,      ly    )
    dp10 = dot_product(gradient_grid[gy    ][gx + 1],  lx - 1,  ly    )
    dp01 = dot_product(gradient_grid[gy + 1][gx    ],  lx,      ly - 1)
    dp11 = dot_product(gradient_grid[gy + 1][gx + 1],  lx - 1,  ly - 1)

    # Smooth the offsets
    u = fade(lx)
    v = fade(ly)

    # Bilinear interpolation (horizontal first, then vertical)
    ix0 = lerp(dp00, dp10, u)
    ix1 = lerp(dp01, dp11, u)
    return lerp(ix0, ix1, v)


# ─────────────────────────────────────────────
#  3. BUILD THE FULL NOISE GRID
# ─────────────────────────────────────────────

def generate_noise_grid(
    gradient_resolution: int,
    noise_resolution: int
) -> np.ndarray:
    """
    C++ equivalent: main() loop that fills noiseGrid[][].
    Returns a (noise_resolution × noise_resolution) numpy array.
    Note: we pass (j, i) — same as C++ — so x=column, y=row.
    """
    grad_grid = generate_gradient_grid(gradient_resolution)
    grid = np.zeros((noise_resolution, noise_resolution), dtype=np.float32)

    for i in range(noise_resolution):
        for j in range(noise_resolution):
            grid[i, j] = perlin_value(j, i, grad_grid, noise_resolution)

    return grid


# ─────────────────────────────────────────────
#  4. FRACTAL / OCTAVE NOISE  ← NEW in Python
# ─────────────────────────────────────────────

def generate_fractal_noise(
    noise_resolution: int,
    octaves: int        = 6,
    base_grad_res: int  = 4,
    persistence: float  = 0.5,
    lacunarity: float   = 2.0,
    seed: int | None    = None,
) -> np.ndarray:
    """
    Layers multiple octaves of Perlin noise (fBm — fractal Brownian motion).

    Each octave:
      - doubles the gradient resolution  → finer detail (lacunarity = 2.0)
      - halves the amplitude             → less weight  (persistence = 0.5)

    The final map is normalised to [0, 1].

    Parameters
    ----------
    noise_resolution : output image size in pixels (square)
    octaves          : how many noise layers to add
    base_grad_res    : gradient grid size for the first (coarsest) octave
    persistence      : amplitude multiplier per octave (< 1 = each layer quieter)
    lacunarity       : gradient-resolution multiplier per octave (> 1 = finer detail)
    seed             : fix the random seed for reproducibility (None = random)
    """
    if seed is not None:
        random.seed(seed)

    combined   = np.zeros((noise_resolution, noise_resolution), dtype=np.float64)
    amplitude  = 1.0
    max_value  = 0.0          # track sum of amplitudes for normalisation
    grad_res   = base_grad_res

    for _ in range(octaves):
        layer = generate_noise_grid(grad_res, noise_resolution).astype(np.float64)
        combined  += layer * amplitude
        max_value += amplitude

        amplitude *= persistence    # quieter each round
        grad_res   = int(grad_res * lacunarity)  # finer each round

    # Normalise to [0, 1]
    combined /= max_value
    combined = (combined - combined.min()) / (combined.max() - combined.min())
    return combined.astype(np.float32)


# ─────────────────────────────────────────────
#  5. TERRAIN COLOUR MAP  ← NEW in Python
# ─────────────────────────────────────────────

def build_terrain_colormap():
    """
    Maps noise values [0, 1] to terrain biome colours.

    Value range → Biome
    ───────────────────
    0.00 – 0.30  Deep ocean  (dark blue)
    0.30 – 0.42  Ocean       (medium blue)
    0.42 – 0.48  Shallow sea (light blue)
    0.48 – 0.52  Beach / sand
    0.52 – 0.62  Lowland grass
    0.62 – 0.72  Forest
    0.72 – 0.82  Highland / rock
    0.82 – 0.92  Mountain
    0.92 – 1.00  Snow cap
    """
    terrain_colors = [
        (0.00, "#0a1628"),   # deep ocean
        (0.30, "#1a3a6b"),   # ocean
        (0.42, "#2d6a9f"),   # shallow sea
        (0.48, "#4aa3d9"),   # coast
        (0.50, "#e8d5a3"),   # beach
        (0.52, "#c8b87a"),   # dry sand
        (0.54, "#8ab874"),   # light grass
        (0.62, "#5a9e52"),   # grass
        (0.70, "#2d7a3a"),   # forest
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


# ─────────────────────────────────────────────
#  6. RENDERING
# ─────────────────────────────────────────────

def render_greyscale(noise: np.ndarray):
    """Simple greyscale render — closest to the C++ text output."""
    fig, ax = plt.subplots(figsize=(8, 8))
    ax.imshow(noise, cmap="gray", interpolation="bilinear", vmin=0, vmax=1)
    ax.set_title("Perlin Noise — Greyscale", fontsize=14, pad=12)
    ax.axis("off")
    plt.tight_layout()
    plt.show()


def render_2d_map(noise: np.ndarray, title: str = "2D World Map"):
    """
    Renders the noise as a colour terrain map with an interactive
    sea-level slider and a regenerate button.
    """
    cmap = build_terrain_colormap()

    fig, ax = plt.subplots(figsize=(10, 9))
    plt.subplots_adjust(bottom=0.18)

    img = ax.imshow(
        noise, cmap=cmap,
        interpolation="bilinear",
        vmin=0, vmax=1,
        origin="upper"
    )

    # Colour bar / legend
    cbar = fig.colorbar(img, ax=ax, fraction=0.03, pad=0.02)
    cbar.set_ticks([0.15, 0.38, 0.50, 0.58, 0.66, 0.78, 0.90, 0.97])
    cbar.set_ticklabels(
        ["Deep Ocean", "Ocean", "Beach", "Grass", "Forest",
         "Highland", "Mountain", "Snow"],
        fontsize=8
    )

    ax.set_title(title, fontsize=16, pad=14, fontweight="bold")
    ax.axis("off")

    # ── Sea-level slider ──────────────────────────────────────────────
    ax_slider = plt.axes([0.15, 0.08, 0.55, 0.03])
    slider = Slider(ax_slider, "Sea level", 0.0, 0.7, valinit=0.48, valstep=0.01)

    def update_sea_level(val):
        sea = slider.val
        # Shift: everything below sea → deep blue; above stays as-is
        shifted = np.where(noise < sea, noise * (0.45 / max(sea, 0.01)), noise)
        shifted = np.clip(shifted, 0, 1)
        img.set_data(shifted)
        fig.canvas.draw_idle()

    slider.on_changed(update_sea_level)

    # ── Regenerate button ─────────────────────────────────────────────
    ax_btn = plt.axes([0.75, 0.065, 0.15, 0.05])
    btn    = Button(ax_btn, "Regenerate")

    def regenerate(_):
        new_noise = generate_fractal_noise(
            noise_resolution = noise.shape[0],
            octaves          = 6,
            base_grad_res    = 4,
        )
        img.set_data(new_noise)
        ax.set_title("2D World Map  (regenerated)", fontsize=16,
                     pad=14, fontweight="bold")
        fig.canvas.draw_idle()

    btn.on_clicked(regenerate)

    plt.show()


def render_comparison(noise: np.ndarray):
    """Side-by-side: greyscale raw noise vs coloured terrain map."""
    cmap = build_terrain_colormap()
    fig, axes = plt.subplots(1, 2, figsize=(14, 6))

    axes[0].imshow(noise, cmap="gray", interpolation="bilinear", vmin=0, vmax=1)
    axes[0].set_title("Raw Perlin Noise (greyscale)", fontsize=13)
    axes[0].axis("off")

    im = axes[1].imshow(noise, cmap=cmap, interpolation="bilinear", vmin=0, vmax=1)
    axes[1].set_title("2D Terrain Map", fontsize=13)
    axes[1].axis("off")
    fig.colorbar(im, ax=axes[1], fraction=0.04, pad=0.02)

    plt.suptitle("Perlin Noise → 2D World Map", fontsize=15, fontweight="bold", y=1.01)
    plt.tight_layout()
    plt.show()


# ─────────────────────────────────────────────
#  7. SAVE TO FILE  (mirrors C++ writeNoiseGridToFile)
# ─────────────────────────────────────────────

def write_to_file(noise: np.ndarray, filename: str = "outputFile.txt"):
    """C++ equivalent: writeNoiseGridToFile()"""
    rows, cols = noise.shape
    with open(filename, "w") as f:
        for i in range(rows):
            line = "  ".join(f"{noise[i, j]:+.6f}" for j in range(cols))
            f.write(line + "\n")
    print(f"Saved noise values → {filename}")


# ─────────────────────────────────────────────
#  8. MAIN
# ─────────────────────────────────────────────

if __name__ == "__main__":

    # ── Parameters (equivalent to C++ cin inputs) ────────────────────
    NOISE_RESOLUTION  = 512   # output image pixels (512×512)
    OCTAVES           = 6     # fractal layers
    BASE_GRAD_RES     = 4     # coarsest gradient grid size
    PERSISTENCE       = 0.5   # amplitude decay per octave
    LACUNARITY        = 2.0   # detail gain per octave
    SEED              = None  # set an int for reproducible maps

    print("Generating Perlin noise …")
    noise = generate_fractal_noise(
        noise_resolution = NOISE_RESOLUTION,
        octaves          = OCTAVES,
        base_grad_res    = BASE_GRAD_RES,
        persistence      = PERSISTENCE,
        lacunarity       = LACUNARITY,
        seed             = SEED,
    )
    print(f"Noise grid: {noise.shape}, min={noise.min():.3f}, max={noise.max():.3f}")

    # Uncomment whichever rendering mode you want:

    # render_greyscale(noise)          # plain greyscale
    render_comparison(noise)           # side-by-side greyscale + terrain
    # render_2d_map(noise)             # interactive map with slider & button

    # Optional: save raw float values to text (mirrors C++ output)
    # write_to_file(noise, "outputFile.txt")
