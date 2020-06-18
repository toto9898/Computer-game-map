//the file with the description must not have an empty new line at the end
//and every object which is made from more than one word, must be in ""

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_set>
#include "Map.h"

using std::string;

int main()
{
	string filename, startZone;
	std::cin >> filename;

	std::ifstream in(filename);
	if (!in)
	{
		std::cout << "Failed to open file: " << filename << ".\n";
		return 0;
	}

	Map map(in);
	in.close();

	std::unordered_set<string> inventory;
	map.bfs(nullptr, inventory);

	std::ofstream out("graph.txt");
	if (!out)
	{
		std::cout << "Failed to open file: graph.txt.\n";
		return 0;
	}

	map.createDOT(out);

	out.close();


	return 0;
}