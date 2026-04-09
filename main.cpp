//-------------------------------//
//        HEADERS & USING        //
//-------------------------------//

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <cstdint>
#include <typeinfo> //? For Debugging

// RNG Algorithm with balance of fast and good
struct PCG32 //! No touching, write documentation later
{
	uint64_t state;
    uint64_t inc;

    PCG32(uint64_t seed, uint64_t seq = 1) 
	{
        state = 0;
        inc = (seq << 1u) | 1u;
        next();
        state += seed;
        next();
    }

    uint32_t next() 
	{
        uint64_t oldstate = state;
        state = oldstate * 6364136223846793005ULL + inc;

        uint32_t xorshifted =
            (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
        uint32_t rot = oldstate >> 59u;

        return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
    }
};

//-------------------------------//
//           FUNCTIONS           //
//-------------------------------//

std::vector<std::vector<int>> generateGradientGrid(PCG32& rng, float res)
{
	int resolution = (int) res;
    std::vector<std::vector<int>> gradientGrid(resolution, std::vector<int>(resolution));

    for (int i = 0; i < resolution; i++)
        for (int j = 0; j < resolution; j++)
            gradientGrid[i][j] = rng.next() & 15; // bitwise is faster for powers of 2

    return gradientGrid;
}



void displayGradientVectors(std::vector<std::vector<int>> &gradientGrid)
{
	int resolution = gradientGrid.size();
	for (std::vector<int> &i : gradientGrid)
	{
		for (int &j : i)
			std::cout << j << " ";
		std::cout << "\n";
	}
}



float dotProduct(int gradientIndex, float dx, float dy)
{
	/*
		gx, gy = (cos θ, sin θ)
		cos θ represents x component as it has it's peak values at 0 and 180
		sin θ represents y component as it has it's peak values at 90 and 270

		θ is derived from the gradient index, 0 = 0 degrees, 1 = 22.5 degrees, so on. 
		East (Positive X) = 0 degrees
	*/
	float gradientX;
	float gradientY;
	switch(gradientIndex)
	{
		case 0:  gradientX =  1.00000000f; gradientY =  0.00000000f; break;
		case 1:  gradientX =  0.92387953f; gradientY =  0.38268343f; break;
		case 2:  gradientX =  0.70710678f; gradientY =  0.70710678f; break;
		case 3:  gradientX =  0.38268343f; gradientY =  0.92387953f; break;
		case 4:  gradientX =  0.00000000f; gradientY =  1.00000000f; break;
		case 5:  gradientX = -0.38268343f; gradientY =  0.92387953f; break;
		case 6:  gradientX = -0.70710678f; gradientY =  0.70710678f; break;
		case 7:  gradientX = -0.92387953f; gradientY =  0.38268343f; break;
		case 8:  gradientX = -1.00000000f; gradientY =  0.00000000f; break;
		case 9:  gradientX = -0.92387953f; gradientY = -0.38268343f; break;
		case 10: gradientX = -0.70710678f; gradientY = -0.70710678f; break;
		case 11: gradientX = -0.38268343f; gradientY = -0.92387953f; break;
		case 12: gradientX =  0.00000000f; gradientY = -1.00000000f; break;
		case 13: gradientX =  0.38268343f; gradientY = -0.92387953f; break;
		case 14: gradientX =  0.70710678f; gradientY = -0.70710678f; break;
		case 15: gradientX =  0.92387953f; gradientY = -0.38268343f; break;
	}

	return gradientX * dx + gradientY * dy;
}



float perlinFadeFunction(float t)
{
	return (6 * (t*t*t*t*t) - 15 * (t*t*t*t) + 10 * (t*t*t));
}

float linearInterpolation(float a, float b, float t)
{
	return (a + t * (b - a));
}



float generatePerlinValue(float x, float y, std::vector<std::vector<int>> &gradientGrid, int mapSize)
{
	int gradientResolution = gradientGrid.size();
	float scale = (float) gradientResolution / mapSize;

	// Coordinates in Gradient Grid
	float sx = x * scale;
	float sy = y * scale;

	// Grid Cell
	int gx = (int) floor(sx);
	int gy = (int) floor(sy);

	// Local Coordinates
	float lx = sx - gx;
	float ly = sy - gy;

	// Grid Cell Corner Coordinates relative to Gradient Grid | (0, 0) = Bottom-Left Grid Corner
	int x0 = gx % gradientResolution;
	int y0 = gy % gradientResolution;
	int x1 = (gx + 1) % gradientResolution;
	int y1 = (gy + 1) % gradientResolution;

	// Distance Vectors | (0, 0) = Bottom-Left Grid Corner
	float dvx00 = lx;
	float dvy00 = ly;

	float dvx10 = lx - 1;
	float dvy10 = ly;

	float dvx01 = lx;
	float dvy01 = ly - 1;

	float dvx11 = lx - 1;
	float dvy11 = ly - 1;

	// Dot Products
	float dotProduct00 = dotProduct(gradientGrid[y0][x0], dvx00, dvy00);
	float dotProduct10 = dotProduct(gradientGrid[y0][x1], dvx10, dvy10);
	float dotProduct01 = dotProduct(gradientGrid[y1][x0], dvx01, dvy01);
	float dotProduct11 = dotProduct(gradientGrid[y1][x1], dvx11, dvy11);

	// Perlin Fade Function
	float u = perlinFadeFunction(lx);
	float v = perlinFadeFunction(ly);

	// Interpolation and FINAL RESULT
	float ix0 = linearInterpolation(dotProduct00, dotProduct10, u);
	float ix1 = linearInterpolation(dotProduct01, dotProduct11, u);

	float value = linearInterpolation(ix0, ix1, v);

	return value;
}

void writeNoiseGridToFile(std::vector<std::vector<float>> &noiseGrid)
{
	std::ofstream outputFile("outputFile.txt");
	if (!outputFile)
	{
		std::cout << "ERROR! FILE NOT ABLE TO BE OPENED";
		return;
	}
    int mapSize = noiseGrid.size();

    for (auto &i : noiseGrid)
    {
        for (float &j : i)
            outputFile << j << " ";
        outputFile << "\n";
    }

    outputFile.close();
}

//-------------------------------//
//              MAIN             //
//-------------------------------//

int main()
{
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count(); // long long int
	PCG32 rng(seed);
	int baseGradientResolution = 4; // 4
	int mapSize = 512; // 512
	int octaves = 6; // 64
	float persistence = 0.5f; // 0.5f - amplitude, influence
	float lacunarity = 2.0f; // 2.0f - frequency, detail-gain

	std::vector<std::vector<std::vector<int>>> gradientGrids(octaves);
	std::vector<std::vector<float>> noiseGrid(mapSize);


	float currentRes = baseGradientResolution;
	for (int i = 0; i < octaves; i++)
	{
		gradientGrids[i] = generateGradientGrid(rng, currentRes);
		currentRes *= lacunarity;
	}

	float minValue = 1000.0f;
	float maxValue = -1000.0f;

	for (int i = 0; i < mapSize; i++)
	{
		noiseGrid[i].resize(mapSize, 0.0f);
		for (int j = 0; j < mapSize; j++)
		{
			float amplitude = 1.0f;
			float value = 0.0f;
			for (int k = 0; k < octaves; k++)
			{
				value += generatePerlinValue((float) j, (float) i, gradientGrids[k], mapSize) * amplitude;
				// i and j swapped, see documentation
				amplitude *= persistence;
			}
			if (value < minValue)
				minValue = value;
			if (value > maxValue)
				maxValue = value;
			noiseGrid[i][j] = value;
	
		}
	}

	// Normalizing values between 0.0 and 1.0
	for (int i = 0; i < mapSize; i++)
		for (int j = 0; j < mapSize; j++)
			noiseGrid[i][j] = (noiseGrid[i][j] - minValue) / (maxValue - minValue);

	writeNoiseGridToFile(noiseGrid);
	
	return 0;
}
