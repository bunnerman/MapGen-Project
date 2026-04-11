//-------------------------------//
//        HEADERS & USING        //
//-------------------------------//

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath>
#include <cstdint> //? For convenient typedef names
#include <typeinfo> //? keep for debugging

//-------------------------------//
//              RNG              //
//-------------------------------//

struct PCG32 //! DONT TOUCH until algorithm learnt
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
//       PERMUTATION TABLE       //
//-------------------------------//

void initPermutation(PCG32& rng, std::vector<int>& perm)
{
    std::vector<int> p(256);

    for (int i = 0; i < 256; i++)
        p[i] = i;

    for (int i = 255; i > 0; i--)
    {
        int j = rng.next() % (i + 1);
        std::swap(p[i], p[j]);
    }

    for (int i = 0; i < 512; i++)
        perm[i] = p[i & 255];
}

//-------------------------------//
//           FUNCTIONS           //
//-------------------------------//

float dotProduct(int hash, float x, float y)
{
    switch(hash & 7)
    {
        case 0: 
			return x + y;
        case 1: 
			return -x + y;
        case 2: 
			return x - y;
        case 3: 
			return -x - y;
        case 4: 
			return x;
        case 5: 
			return -x;
        case 6: 
			return y;
        case 7: 
			return -y;
    }

    return 0;
}

float perlinFadeFunction(float t)
{
    return (6*t*t*t*t*t - 15*t*t*t*t + 10*t*t*t);
}

float linearInterpolation(float a, float b, float t)
{
    return (a + t * (b - a));
}

//-------------------------------//
//        PERLIN FUNCTION        //
//-------------------------------//

float generatePerlinValue(float x, float y, std::vector<int>& perm)
{
	// Grid Cell
    int x0 = (int) floor(x);
    int y0 = (int) floor(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

	// Local Coordinates
    float lx = x - x0;
    float ly = y - y0;

    float u = perlinFadeFunction(lx);
    float v = perlinFadeFunction(ly);

	/* //? Distance Vector Components - Omitted for micro-optimization
	float dvx00 = lx;
	float dvy00 = ly;

	float dvx10 = lx - 1;
	float dvy10 = ly;

	float dvx01 = lx;
	float dvy01 = ly - 1;

	float dvx11 = lx - 1;
	float dvy11 = ly - 1;
	*/

	// Hashing corner gradient indices
    int h00 = perm[(perm[x0 & 255] + y0) & 255];
    int h10 = perm[(perm[x1 & 255] + y0) & 255];
    int h01 = perm[(perm[x0 & 255] + y1) & 255];
    int h11 = perm[(perm[x1 & 255] + y1) & 255];

	// Dot Producting
    float dot00 = dotProduct(h00, lx, ly);
    float dot10 = dotProduct(h10, lx - 1, ly);
    float dot01 = dotProduct(h01, lx, ly - 1);
    float dot11 = dotProduct(h11, lx - 1, ly - 1);

	// Lerping
    float ix0 = linearInterpolation(dot00, dot10, u);
    float ix1 = linearInterpolation(dot01, dot11, u);
	
	// Final Value after interpolation on both axes
    return linearInterpolation(ix0, ix1, v);
}

//-------------------------------//

void writeNoiseGridToFile(std::vector<std::vector<float>> &noiseGrid)
{
    std::ofstream outputFile("outputFile.txt");
    if (!outputFile)
    {
        std::cout << "ERROR! FILE NOT ABLE TO BE OPENED";
        return;
    }

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
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    PCG32 rng(seed);

    std::vector<int> perm(512);
    initPermutation(rng, perm);

    int resolution = 512;
    int octaves = 6;
    float persistence = 0.5f;
    float lacunarity = 2.0f;
	float mapSize = 5.0f;

    std::vector<std::vector<float>> noiseGrid(resolution);

    float minValue = 1000.0f;
    float maxValue = -1000.0f;

    for (int i = 0; i < resolution; i++)
    {
        noiseGrid[i].resize(resolution, 0.0f);

        for (int j = 0; j < resolution; j++)
        {
            float amplitude = 1.0f;
            float frequency = 1.0f;
            float value = 0.0f;

            for (int k = 0; k < octaves; k++)
            {
                value += generatePerlinValue
				(
                    (float)j / resolution * mapSize * frequency,
                    (float)i / resolution * mapSize * frequency,
                    perm
                ) * amplitude;

                amplitude *= persistence;
                frequency *= lacunarity;
            }

            if (value < minValue) 
				minValue = value;
            if (value > maxValue) 
				maxValue = value;

            noiseGrid[i][j] = value;
        }
    }

    // Normalize values between 0.0 and 1.0
    for (int i = 0; i < resolution; i++)
        for (int j = 0; j < resolution; j++)
            noiseGrid[i][j] = (noiseGrid[i][j] - minValue) / (maxValue - minValue);

    writeNoiseGridToFile(noiseGrid);

    return 0;
}
