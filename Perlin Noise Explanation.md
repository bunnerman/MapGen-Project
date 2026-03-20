Perlin Noise is a Procedural Noise Generation method used to produce smooth, natural looking gradients for generating things like terrain, texture, etc.
It was invented by *Ken Perlin* in the 1980s.
## Steps
1. Divide plane into grid, each grid intersection point is assigned a **Vector with random direction but same weight** called **Gradient Vectors** (including corners and edges of entire grid)
2. Compute a Perlin Noise value for each sample point where you want an output.
   *Higher Sample Point Frequency = Higher Detail*
3. Locate **Grid Cell** containing sample point
4. Calculate **Local Distance** of the sample point relative to the grid cell
5. Compute distance between sample point and each of the 4 corner vectors, these are **Distance Vectors**
6. Get **Dot Product** of the **gradient vector** and **distance vector** for each corner
7. Apply ***Perlin* Fade Function** to the `x` and `y` **local distances** respectively and store their values `u` and `v` respectively
	$f(t) = 6t^5 - 15t^4 + 10t^3$
	This specific function defined by *Ken Perlin* smooths the output
8. Perform **Linear Interpolation** Horizontally, then Vertically (**But Cubic Interpolation yields better results**)
> [!note] Interpolation 
> Suppose 4 **dot products** are `dp00, dp10, dp01, dp11`
> `Bottom Left, Bottom Right, Top Left, Top Right`
> ```c
> p = interpolate(dp00, dp10, u)
> q = interpolate(dp01, dp11, u)
> value = interpolate(p, q, v)
> ```
9. `value` is the final ***Perlin* Noise Value** for that **sample point**
10. Repeat for other **sample points**
