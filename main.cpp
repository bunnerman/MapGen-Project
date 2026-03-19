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
	float gx;
	float gy;
	switch(gradientIndex)
	{
		case 0:
			gx =  1.0f; gy =  0.0f; break;
		case 1:
			gx =  0.70710678f; gy =  0.70710678f; break;
		case 2:
			gx =  0.0f; gy =  1.0f; break;
		case 3:
			gx = -0.70710678f; gy =  0.70710678f; break; // eg: gx = cos(135), gy = sin(135) 
		case 4:
			gx = -1.0f; gy =  0.0f; break;
		case 5:
			gx = -0.70710678f; gy = -0.70710678f; break;
		case 6:
			gx =  0.0f; gy = -1.0f; break;
		case 7:
			gx =  0.70710678f; gy = -0.70710678f; break;
	}

	return gx * dx + gy * dy;
}

float perlinFadeFunction(float t)
{
	return (6 * (t*t*t*t*t) - 15 * (t*t*t*t) + 10 * (t*t*t));
}

float linearInterpolation(float a, float b, float t)
{
	return (a + t * (b - a));
}

float generatePerlinValue(int x, int y, vector<vector<int>> &gradientGrid, int noiseResolution)
{
	int gradientResolution = gradientGrid.size();
	float scale = (float) (gradientResolution - 1) / noiseResolution; // sample points

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
	int x0 = gx;
	int y0 = gy;
	int x1 = gx + 1;
	int y1 = gy + 1;

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



void displayNoiseGrid(vector<vector<float>> &noiseGrid)
{
	int noiseResolution = noiseGrid.size();

	for (int i = 0; i < noiseResolution; i++)
	{
		for (int j = 0; j < noiseResolution; j++)
			cout << std::setw(7) << noiseGrid[i][j] << " ";
		cout << "\n";
	}
}



void writeNoiseGridToFile(vector<vector<float>> &noiseGrid)
{
	std::ofstream outputFile("outputFile.txt");
	if (!outputFile)
	{
		cout << "ERROR! FILE NOT ABLE TO BE OPENED";
		return;
	}
    int noiseResolution = noiseGrid.size();

    for (int i = 0; i < noiseResolution; i++)
    {
        for (int j = 0; j < noiseResolution; j++)
            outputFile << std::setw(10) << noiseGrid[i][j] << " ";
        outputFile << "\n";
    }

    outputFile.close();
}



int main()
{
	std::srand(std::time(nullptr));

	int gradientResolution; // how big the grid/map is
	int noiseResolution; // how detailed Noise will be (past a certain point there're no noticable gains)

	cout << "Enter Gradient Grid resolution: ";
	cin >> gradientResolution;
	cout << "Enter Noise Grid resolution: ";
	cin >> noiseResolution;

	auto gradientGrid = generateGradientVectors(gradientResolution);

	vector<vector<float>> noiseGrid(noiseResolution);
	for (int i = 0; i < noiseResolution; i++)
	{
		noiseGrid[i].resize(noiseResolution);
		for (int j = 0; j < noiseResolution; j++)
			noiseGrid[i][j] = generatePerlinValue(j, i, gradientGrid, noiseResolution);
	}
	/*  
		swapping j and i maintains expected standards in math/graphics instead of transposely generating,
		but (..i, j..) not wrong in any other way
	*/

	writeNoiseGridToFile(noiseGrid);
	
	return 0;
}
