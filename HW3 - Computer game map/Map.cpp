#include <sstream>
#include <exception>
#include <sstream>
#include <iostream>
#include <queue>
#include "Map.h"

using std::stringstream;
using std::exception;
using std::queue;

void getZoneName(std::stringstream& ss, string& zone)
{
	size_t startPos = ss.tellg();

	char c = 0;
	ss >> c;

	if (c == '"')
		getline(ss, zone, '"');
	else
	{
		ss.seekg(startPos);
		ss >> zone;
	}
}

void getKeyName(std::stringstream& ss, string& key)
{
	char c = '\0';
	ss >> c;
	if (c == '[')
		getline(ss, key, ']');
	else
		key = "";
}
void loadRow(std::stringstream& ss, string& zone1, string& zone2, string& key)
{
	getZoneName(ss, zone1);

	ss >> zone2; // getting the arrow
	zone2 = "";

	getZoneName(ss, zone2);

	getKeyName(ss, key);

	if (!ss)
		ss.clear();
}

Map::Map(std::ifstream& in)
{
	load(in);

	string startZone;
	std::cin >> startZone;

	startingZone = zones[startZone];
}

Map::~Map()
{
	for (auto& pairZone : zones)
	{
		for (Edge*& edge : pairZone.second->edges)
			delete edge;
		delete pairZone.second;
	}
}

void Map::bfs(Zone* startZone, unordered_set<string> inventory)
{
	if (!startZone) startZone = startingZone;

	inventory.insert("");

	vector<Zone*> visited; // used to mark zones as visited
	std::queue<Zone*> to_be_visited;  // a queue for traversing

	to_be_visited.push(startZone);     // adding the start zone into the queue
	visited.push_back(startZone);	    // mark it as visited

	while (!to_be_visited.empty()) // until we got zones into the queue
	{
		Zone* current = to_be_visited.front(); // get the first one

		newBfs(current, inventory); // running a new bfs from the current zone if we find a new key

		for (Edge*& e : current->edges)  // going trough all the children
		{
			Zone* child = e->pointed;

			bool already_visited = false;

			for (Zone*& e_vis : visited) // checking if the child is already visited
				if (e_vis == child)
					already_visited = true;

			if (already_visited == false) 
				if (inventory.count(e->label) == 1) // checking if we have the key to visit the child 
					to_be_visited.push(child); // we add the child for visitting
		}

		visited.push_back(current);
		current->visited = true;

		to_be_visited.pop(); // remove the current zone from the queue
	}
}

// Creates the DOT file, which describes the graph.
void Map::createDOT(std::ofstream& out)
{

	out << "digraph map {\n";

	for (const auto& pair : zones)
	{
		describeZone(out, pair.second);
		for (Edge* e : pair.second->edges)
		{
			out << "\t" << pair.second->name << " -> " << e->pointed->name;
			if (e->label != "")
				out << " [label=\"" << e->label << "\"]";

			out << ";\n";
		}
	}
	out << "}";
}

void Map::newBfs(Zone * newStart, unordered_set<string> inventory) // runs bfs from the newStart, if there are new keys in it
{
	bool runBfsAgain = false;

	if (newStart->keys.size() > 0) // if there are any keys in the current zone
	{
		unordered_set<string> inventory_copy(inventory);

		for (const string& key : newStart->keys)
		{
			if (inventory.count(key) == 0) // if we found a new key, we need to run the bfs again
				runBfsAgain = true;

			inventory_copy.insert(key);    // addding them to the inventory
		}
		if (runBfsAgain)
			bfs(newStart, inventory_copy);
	}
}

void Map::describeZone(std::ofstream& out, Zone * zone)
{
	out << "\t" << zone->name << "[label=\"" << zone->name;
	for (const auto& key : zone->keys)
		out << "\\n" << key;
	out << "\"";

	if (zone->visited == false)
		out << ",color=red,style=filled, fillcolor=\"#ffefef\"";

	out << "];\n";
}

void Map::load(std::ifstream & in)
{
	stringstream ss;

	loadZones(in, ss);
	loadKeys(in, ss);

	in.clear();
}

void Map::loadZones(std::ifstream& in, std::stringstream & ss)
{
	string line;
	string zone1, zone2, key;

	in >> zone1; // getting the "[zones]"
	zone1 = "";
	
	bool readingZones = true;
	while (getline(in, line, ';'))	// reading the zones
	{
		ss = stringstream(line);

		if (readingZones && line.find("[keys]") == string::npos) // we read zones until we read "[keys]"
		{
			loadRow(ss, zone1, zone2, key);

			if (zones.count(zone1) == 0)
				zones.insert(std::make_pair(zone1, new Zone(zone1)));   // adding zone1

			if (zones.count(zone2) == 0)
				zones.insert(std::make_pair(zone2, new Zone(zone2)));   // adding zone2

			zones[zone1]->edges.push_back(new Edge(key, zones[zone2])); // adding the edge
		}
		else if (readingZones)
		{
			readingZones = false;
			line = zone1 = zone2 = key = "";
			break;
		}
		line = zone1 = zone2 = key = "";
	}
}

void Map::loadKeys(std::ifstream& in, std::stringstream & ss)
{
	string line;
	string zone1, zone2, key;

	ss >> zone1; // getting the "[keys]"
	zone1 = "";

	loadRow(ss, key, zone1, zone2);
	if (key != "")
		zones[zone1]->keys.insert(key);

	while (getline(in, line, ';')) // reading the keys
	{
		ss = stringstream(line);

		loadRow(ss, key, zone1, zone2);
		zones[zone1]->keys.insert(key); // adding the ke to the zone
	}
}
