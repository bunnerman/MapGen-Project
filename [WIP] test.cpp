// USE THIS FOR RANDOM GENERATION INSTEAD

#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <cmath>
#include <ctime>
#include <vector>
#include <chrono>

int main()
{
	auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 gen(static_cast<unsigned int>(seed));
	std::uniform_int_distribution<int> seedRange(0, 255);

	std::vector<int> v(1000);
	for (int i = 0; i < 1000; i++)
		i = seedRange(gen);

	for (int &i : v)
		std::cout << i;
}
