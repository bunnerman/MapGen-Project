#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>

using std::cout;
using std::cin;
using std::vector;
using std::string;

#include <string>


vector<vector<int>> generateGradientVectors(int resolution)
{
    vector<vector<int>> gradientGrid(resolution);
	for (int i = 0; i < resolution; i++)
		gradientGrid[i].resize(resolution);

    for (int i = 0; i < resolution; i++)
        for (int j = 0; j < resolution; j++)
            gradientGrid[i][j] = rand() % 8;
			// 8 Cardinal Directions || East = 0 degrees = 0 index, NE = 45 degrees = 1 index, and so on 

    return gradientGrid;
}



void displayGradientVectors(vector<vector<int>> &gradientGrid)
{
	int resolution = gradientGrid.size();
	for (int i = 0; i < resolution; i++)
	{
		for (int j = 0; j < resolution; j++)
			cout << gradientGrid[i][j] << " ";
		cout << "\n";
	}
}



float dotProduct(int gradientIndex, float dx, float dy)
{
	/*
		gx, gy = (cos θ, sin θ)
		cos θ represents x component as it has it's peak values at 0 and 180
		sin θ represents y component as it has it's peak values at 90 and 270

		θ is derived from the gradient index, 0 = 0 degrees, 1 = 45 degrees, so on. East (Positive X) = 0 degrees
	*/
	float gradientX;
	float gradientY;
	switch(gradientIndex)
	{
		case 0:
			gradientX =  1.0f; gradientY =  0.0f; break;
		case 1:
			gradientX =  0.70710678f; gradientY =  0.70710678f; break;
		case 2:
			gradientX =  0.0f; gradientY =  1.0f; break;
		case 3:
			gradientX = -0.70710678f; gradientY =  0.70710678f; break; // eg: gx = cos(135), gy = sin(135) 
		case 4:
			gradientX = -1.0f; gradientY =  0.0f; break;
		case 5:
			gradientX = -0.70710678f; gradientY = -0.70710678f; break;
		case 6:
			gradientX =  0.0f; gradientY = -1.0f; break;
		case 7:
			gradientX =  0.70710678f; gradientY = -0.70710678f; break;
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

float generatePerlinValue(float x, float y, vector<vector<int>> &gradientGrid, int mapSize)
{
	int gradientResolution = gradientGrid.size();
	float scale = (float) (gradientResolution - 1) / mapSize;

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
	float dx00 = lx;
	float dy00 = ly;

	float dx10 = lx - 1;
	float dy10 = ly;

	float dx01 = lx;
	float dy01 = ly - 1;

	float dx11 = lx - 1;
	float dy11 = ly - 1;

	// Dot Products
	float dotProduct00 = dotProduct(gradientGrid[y0][x0], dx00, dy00);
	float dotProduct10 = dotProduct(gradientGrid[y0][x1], dx10, dy10);
	float dotProduct01 = dotProduct(gradientGrid[y1][x0], dx01, dy01);
	float dotProduct11 = dotProduct(gradientGrid[y1][x1], dx11, dy11);

	// Perlin Fade Function
	float u = perlinFadeFunction(lx);
	float v = perlinFadeFunction(ly);

	// Interpolation and FINAL RESULT
	float ix0 = linearInterpolation(dotProduct00, dotProduct10, u);
	float ix1 = linearInterpolation(dotProduct01, dotProduct11, u);

	float value = linearInterpolation(ix0, ix1, v);

	return value;
}



void displayNoiseGrid(vector<vector<float>> &noiseMap)
{
	int mapSize = noiseMap.size();

	for (int i = 0; i < mapSize; i++)
	{
		for (int j = 0; j < mapSize; j++)
			cout << std::setw(7) << noiseMap[i][j] << " ";
		cout << "\n";
	}
}



void writeNoiseGridToFile(vector<vector<float>> &noiseMap)
{
	std::ofstream outputFile("outputFile.txt");
	if (!outputFile)
	{
		cout << "ERROR! FILE NOT ABLE TO BE OPENED";
		return;
	}
    int mapSize = noiseMap.size();

    for (int i = 0; i < mapSize; i++)
    {
        for (int j = 0; j < mapSize; j++)
            outputFile << std::setw(10) << noiseMap[i][j] << " ";
        outputFile << "\n";
    }

    outputFile.close();
}

void layerOctave(vector<vector<float>> &noiseMap)
{
	
}

int main()
{
	std::srand(std::time(nullptr));

	int baseGradientResolution = 4;
	int mapSize = 1024;
	int octaves = 8;
	float persistence = 0.5; // amplitude, influence
	float lacunarity = 2; // frequency, detail-gain
	float contrast = 1;

	vector<vector<int>> gradientGrid = generateGradientVectors(baseGradientResolution);
	vector<vector<float>> noiseMap(mapSize);

	for (int i = 0; i < mapSize; i++)
	{
		noiseMap[i].resize(mapSize, 0.0f);
		for (int j = 0; j < mapSize; j++)
		{
			float amplitude = 1.0f;
			float frequency = 1.0f;
			float value = 0.0f;
			float maxAmp = 0.0f;
			for (int k = 0; k < octaves; k++)
			{
				value += generatePerlinValue
				(
					(float) j * frequency, 
					(float) i * frequency, 
					gradientGrid, 
					mapSize
				) 
				* amplitude;

				maxAmp += amplitude;
				amplitude *= persistence;
				frequency *= lacunarity;
			}
			noiseMap[i][j] = (value / maxAmp) * contrast;
		}
	}
	/*  
		swapping j and i maintains expected standards in math/graphics instead of transposely generating,
		but (..i, j..) not wrong in any other way
	*/
	
	writeNoiseGridToFile(noiseMap);
	
	return 0;
}
