#include <iostream>
#include <string>

#include <vector>
#include <algorithm>

#include <cmath>
#include <ctime>
#include <cstdlib>

using std::cout;
using std::cin;
using std::vector;
using std::string;



vector<vector<int>> generateGradientVectors(int resolution)
{
    vector<vector<int>> gradientGrid(resolution);
	for (int i = 0; i < resolution; i++)
		gradientGrid[i].resize(resolution);

    for (int i = 0; i < resolution; i++)
        for (int j = 0; j < resolution; j++)
            gradientGrid[i][j] = rand() % 8;

    return gradientGrid;
}



void displayGradientVectors(vector<vector<int>> gradientGrid)
{
	int resolution = gradientGrid.size();
	for (int i = 0; i < resolution; i++)
	{
		for (int j = 0; j < resolution; j++)
			cout << gradientGrid[i][j] << " ";
		cout << "\n";
	}
}

vector<vector<int>> generatePerlinNoise(vector<vector<int>> gradientGrid, int perlinResolution)
{
	vector<vector<int>> sampleGrid;
	
}

void displayPerlinNoiseGrid(vector<vector<int>>)
{

}

int main()
{
	std::srand(std::time(nullptr));

	int gradientResolution = 64; // how big the grid/map is
	int perlinResolution = 256; // how detailed Noise will be (past a certain point there're no noticable gains)
	
	auto gradientGrid = generateGradientVectors(gradientResolution);
	displayGradientVectors(gradientGrid);
	auto perlinGrid = generatePerlinNoise(gradientGrid, perlinResolution);
	
	return 0;
}

/* Example Code - Zipped - YouTube - from https://www.youtube.com/watch?v=kCIaHqb60Cw
#include <iostream>
#include <math.h>
#include "SFML/Graphics.hpp"
 
 
typedef struct {
    float x, y;
} vector2;
 
vector2 randomGradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; 
    unsigned a = ix, b = iy;
    a *= 3284157443;
 
    b ^= a << s | a >> w - s;
    b *= 1911520717;
 
    a ^= b << s | b >> w - s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
    
    // Create the vector from the angle
    vector2 v;
    v.x = sin(random);
    v.y = cos(random);
 
    return v;
}
 
// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(int ix, int iy, float x, float y) {
    // Get gradient from integer coordinates
    vector2 gradient = randomGradient(ix, iy);
 
    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;
 
    // Compute the dot-product
    return (dx * gradient.x + dy * gradient.y);
}
 
float interpolate(float a0, float a1, float w)
{
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}
 
 
// Sample Perlin noise at coordinates x, y
float perlin(float x, float y) {
    
    // Determine grid cell corner coordinates
    int x0 = (int)x; 
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;
 
    // Compute Interpolation weights
    float sx = x - (float)x0;
    float sy = y - (float)y0;
    
    // Compute and interpolate top two corners
    float n0 = dotGridGradient(x0, y0, x, y);
    float n1 = dotGridGradient(x1, y0, x, y);
    float ix0 = interpolate(n0, n1, sx);
 
    // Compute and interpolate bottom two corners
    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    float ix1 = interpolate(n0, n1, sx);
 
    // Final step: interpolate between the two previously interpolated values, now in y
    float value = interpolate(ix0, ix1, sy);
    
    return value;
}
 
int main()
{
    const int windowWidth = 1920;
    const int windowHeight = 1080;
 
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight, 32), "Perlin");
 
    sf::Uint8* pixels = new sf::Uint8[windowWidth * windowHeight * 4];
    
    
    const int GRID_SIZE = 400;
 
 
    for (int x = 0; x < windowWidth; x++)
    {
        for (int y = 0; y < windowHeight; y++)
        {
            int index = (y * windowWidth + x) * 4;
 
            
            float val = 0;
 
            float freq = 1;
            float amp = 1;
 
            for (int i = 0; i < 12; i++)
            {
                val += perlin(x * freq / GRID_SIZE, y * freq / GRID_SIZE) * amp;
 
                freq *= 2;
                amp /= 2;
 
            }
 
            // Contrast
            val *= 1.2;
            
            // Clipping
            if (val > 1.0f)
                val = 1.0f;
            else if (val < -1.0f)
                val = -1.0f;
 
            // Convert 1 to -1 into 255 to 0
            int color = (int)(((val + 1.0f) * 0.5f) * 255);
 
            // Set pixel color
            pixels[index] = color;
            pixels[index + 1] = color;
            pixels[index + 2] = color;
            pixels[index + 3] = 255;
        }
    }
 
    sf::Texture texture;
    sf::Sprite sprite;
 
    texture.create(windowWidth, windowHeight);
 
    texture.update(pixels);
 
    sprite.setTexture(texture);
 
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
 
        window.clear();
        window.draw(sprite);
 
        window.display();
    }
 
    return 0;
}

// Ken Perlin Code - JAVA REFERENCE IMPLEMENTATION OF IMPROVED NOISE - COPYRIGHT 2002 KEN PERLIN.

public final class ImprovedNoise {
   static public double noise(double x, double y, double z) {
      int X = (int)Math.floor(x) & 255,                  // FIND UNIT CUBE THAT
          Y = (int)Math.floor(y) & 255,                  // CONTAINS POINT.
          Z = (int)Math.floor(z) & 255;
      x -= Math.floor(x);                                // FIND RELATIVE X,Y,Z
      y -= Math.floor(y);                                // OF POINT IN CUBE.
      z -= Math.floor(z);
      double u = fade(x),                                // COMPUTE FADE CURVES
             v = fade(y),                                // FOR EACH OF X,Y,Z.
             w = fade(z);
      int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,      // HASH COORDINATES OF
          B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;      // THE 8 CUBE CORNERS,

      return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),  // AND ADD
                                     grad(p[BA  ], x-1, y  , z   )), // BLENDED
                             lerp(u, grad(p[AB  ], x  , y-1, z   ),  // RESULTS
                                     grad(p[BB  ], x-1, y-1, z   ))),// FROM  8
                     lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),  // CORNERS
                                     grad(p[BA+1], x-1, y  , z-1 )), // OF CUBE
                             lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                     grad(p[BB+1], x-1, y-1, z-1 ))));
   }
   static double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
   static double lerp(double t, double a, double b) { return a + t * (b - a); }
   static double grad(int hash, double x, double y, double z) {
      int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
      double u = h<8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
             v = h<4 ? y : h==12||h==14 ? x : z;
      return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
   }
   static final int p[] = new int[512], permutation[] = { 151,160,137,91,90,15,
   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
   };
   static { for (int i=0; i < 256 ; i++) p[256+i] = p[i] = permutation[i]; }
}
*/