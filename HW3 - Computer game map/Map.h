#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <fstream>
using std::vector;
using std::string;
using std::unordered_map;
using std::unordered_set;

struct Zone;
struct Edge
{
	Edge(string & label, Zone* pointed = nullptr) :
		label(label),
		pointed(pointed)
	{}
	// to go trough it, you need key with the same label
	string label;
	Zone* pointed;
};

struct Zone
{
	Zone(string & name) :
		name(name),
		visited(false)
	{}
	string name;
	unordered_set<string> keys;
	vector<Edge*> edges;
	bool visited;
};

class Map
{
public:
	Map(std::ifstream& in);
	~Map();

	void bfs(Zone* zone, unordered_set<string> inventory);
	void createDOT(std::ofstream& out);

private:
	void newBfs(Zone* newStart, unordered_set<string> inventory);

	void describeZone(std::ofstream& out, Zone* zone);
	void load(std::ifstream& in);
	void loadZones(std::ifstream& in, std::stringstream& ss);
	void loadKeys(std::ifstream& in, std::stringstream& ss);

private:
	Zone* startingZone;
	unordered_map<string, Zone*> zones;

	unsigned zonesCnt;
};

