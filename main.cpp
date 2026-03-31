//-------------------------------//
//        HEADERS & USING        //
//-------------------------------//

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <random>

using std::cout;
using std::cin;
using std::vector;
using std::string;

#include <string> // unnecessary but keep for now

//-------------------------------//
//           FUNCTIONS           //
//-------------------------------//

vector<vector<int>> generateGradientGrid(float res)
{
	int resolution = (int) res;
    vector<vector<int>> gradientGrid((int)resolution);
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

		θ is derived from the gradient index, 0 = 0 degrees, 1 = 45 degrees, so on. 
		East (Positive X) = 0 degrees
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

void writeNoiseGridToFile(vector<vector<float>> &noiseGrid)
{
	std::ofstream outputFile("outputFile.txt");
	if (!outputFile)
	{
		cout << "ERROR! FILE NOT ABLE TO BE OPENED";
		return;
	}
    int mapSize = noiseGrid.size();

    for (int i = 0; i < mapSize; i++)
    {
        for (int j = 0; j < mapSize; j++)
            outputFile << noiseGrid[i][j] << " ";
        outputFile << "\n";
    }

    outputFile.close();
}

//-------------------------------//
//              MAIN             //
//-------------------------------//

int main()
{
	std::srand(std::time(nullptr));

	int baseGradientResolution = 4; // 4
	int mapSize = 512; // 512
	int octaves = 6; // 6
	float persistence = 0.5f; // 0.5f - amplitude, influence
	float lacunarity = 2.0f; // 2.0f - frequency, detail-gain

	vector<vector<vector<int>>> gradientGrids(octaves);
	vector<vector<float>> noiseGrid(mapSize);


	float currentRes = baseGradientResolution;
	for (int i = 0; i < octaves; i++)
	{
		gradientGrids[i] = generateGradientGrid(currentRes);
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
				// i and j swappred, see documentation
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
