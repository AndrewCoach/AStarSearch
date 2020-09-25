#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <vector>
#include <utility>
#include <queue>
#include <tuple>
#include <algorithm>
#include <cstdlib>
#include "olcConsoleGameEngine.h"

// test definition for breadth first search;
struct SimpleGraph {
	std::unordered_map<char, std::vector<char> > edges;

	std::vector<char> neighbors(char id) {
		return edges[id];
	}
};

//test declaration for breadth first search.
SimpleGraph example_graph{ {
	{'A', {'B'}},
	{'B', {'A', 'C', 'D'}},
	{'C', {'A'}},
	{'D', {'E', 'A'}},
	{'E', {'B'}}
  } };

//the basic element of the grid with two coordinates
struct GridLocation {
	int x, y;
};

// helper hash for basic gridlocation element.
namespace std {
	/* implement hash function so we can put GridLocation into an unordered_set */
	template <> struct hash<GridLocation> {
		typedef GridLocation argument_type;
		typedef std::size_t result_type;
		std::size_t operator()(const GridLocation& id) const noexcept {
			return std::hash<int>()(id.x ^ (id.y << 4));
		}
	};
}

// Represents a grid comprised of all basic elements (gridlocation) and other functions.
struct SquareGrid {
	//array of 4 basic elements representing - directions (left, right, up, down)...
	// USED TO BE STATIC!
	std::array<GridLocation, 8> DIRS =
	{ GridLocation{1, 0}, GridLocation{0, -1},
	 GridLocation{-1, 0}, GridLocation{0, 1},
	 GridLocation{1, 1}, GridLocation{1, -1},
	 GridLocation{-1, 1}, GridLocation{-1, -1}
	};

	//width and height of the grid of elements (width * height = for amount of elements)
	int width, height;

	//All the walls represented by elements.
	std::unordered_set<GridLocation> walls;

	//ctor to fill the width and height of grid
	SquareGrid(int width_, int height_)
		: width(width_), height(height_) {}

	//checks to see, if the supplied element is in the grid
	bool in_bounds(GridLocation id) const {
		return 0 <= id.x && id.x < width
			&& 0 <= id.y && id.y < height;
	}
	//CHECKS if the supplied element is a wall, I have added diagonal position agnostic pathfinding.
	bool passable(GridLocation dest, GridLocation whereIAm) const {
		if (dest.x == whereIAm.x + 1 && dest.y == whereIAm.y + 1)
		{
			if (walls.find({ whereIAm.x + 1 , whereIAm.y }) != walls.end() || walls.find({ whereIAm.x  , whereIAm.y + 1 }) != walls.end())
				return false;
		}
		else if (dest.x == whereIAm.x + 1 && dest.y == whereIAm.y - 1)
		{
			if (walls.find({ whereIAm.x + 1 , whereIAm.y }) != walls.end() || walls.find({ whereIAm.x  , whereIAm.y - 1 }) != walls.end())
				return false;
		}
		else if (dest.x == whereIAm.x - 1 && dest.y == whereIAm.y + 1)
		{
			if (walls.find({ whereIAm.x - 1 , whereIAm.y }) != walls.end() || walls.find({ whereIAm.x  , whereIAm.y + 1 }) != walls.end())
				return false;
		}
		else if (dest.x == whereIAm.x - 1 && dest.y == whereIAm.y - 1)

		{
			if (walls.find({ whereIAm.x - 1 , whereIAm.y }) != walls.end() || walls.find({ whereIAm.x  , whereIAm.y - 1 }) != walls.end())
				return false;
		}

		return walls.find(dest) == walls.end();
	}
	//A function returning a vector with all elements that are neighbors to the one element passed in (maximum should be 4 neighbors).
	std::vector<GridLocation> neighbors(GridLocation id) const {
		std::vector<GridLocation> results;

		for (GridLocation dir : DIRS) {
			GridLocation next{ id.x + dir.x, id.y + dir.y };
			if (in_bounds(next) && passable(next, id)) {
				results.push_back(next);
			}
		}

		if ((id.x + id.y) % 2 == 0) {
			// aesthetic improvement on square grids
			std::reverse(results.begin(), results.end());
		}

		return results;
	}
};

// Helpers for GridLocation
//operator overloading for gridlocation elements
bool operator == (GridLocation a, GridLocation b) {
	return a.x == b.x && a.y == b.y;
}

bool operator != (GridLocation a, GridLocation b) {
	return !(a == b);
}

bool operator < (GridLocation a, GridLocation b) {
	return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}
//formatting for ostream.
std::basic_iostream<char>::basic_ostream& operator<<(std::basic_iostream<char>::basic_ostream& out, const GridLocation& loc) {
	out << '(' << loc.x << ',' << loc.y << ')';
	return out;
}

// This outputs a grid. Pass in a distances map if you want to print
// the distances, or pass in a point_to map if you want to print
// arrows that point to the parent location, or pass in a path vector
// if you want to draw the path.
template<class Graph>

//this is here for drawing a walked path in a grid to console
void draw_grid(const Graph& graph,
	std::unordered_map<GridLocation, double>* distances = nullptr,
	std::unordered_map<GridLocation, GridLocation>* point_to = nullptr,
	std::vector<GridLocation>* path = nullptr,
	GridLocation* start = nullptr,
	GridLocation* goal = nullptr) {
	const int field_width = 3;
	std::cout << std::string(field_width * graph.width, '_') << '\n';
	for (int y = 0; y != graph.height; ++y) {
		for (int x = 0; x != graph.width; ++x) {
			GridLocation id{ x, y };
			if (graph.walls.find(id) != graph.walls.end()) {
				std::cout << std::string(field_width, '#');
			}
			else if (start && id == *start) {
				std::cout << " A ";
			}
			else if (goal && id == *goal) {
				std::cout << " Z ";
			}
			else if (point_to != nullptr && point_to->count(id)) {
				GridLocation next = (*point_to)[id];
				if (next.x == x + 1) { std::cout << " > "; }
				else if (next.x == x - 1) { std::cout << " < "; }
				else if (next.y == y + 1) { std::cout << " v "; }
				else if (next.y == y - 1) { std::cout << " ^ "; }
				else { std::cout << " * "; }
			}
			else if (distances != nullptr && distances->count(id)) {
				std::cout << ' ' << std::left << std::setw(field_width - 1) << (*distances)[id];
			}
			else if (path != nullptr && find(path->begin(), path->end(), id) != path->end()) {
				std::cout << " @ ";
			}
			else {
				std::cout << " . ";
			}
		}
		std::cout << '\n';
	}
	std::cout << std::string(field_width * graph.width, '~') << '\n';
}

//function adds rectangular obstacles to supplied grid. X coordinates from x1 to x2 any Y coordinates from y1 to y2.
void add_rect(SquareGrid& grid, int x1, int y1, int x2, int y2) {
	for (int x = x1; x < x2; ++x) {
		for (int y = y1; y < y2; ++y) {
			grid.walls.insert(GridLocation{ x, y });
		}
	}
}

//function returns a grid of size 30, 15 and four rectangular walls
SquareGrid make_diagram1() {
	SquareGrid grid(30, 15);
	add_rect(grid, 3, 3, 5, 12);
	add_rect(grid, 13, 4, 15, 15);
	add_rect(grid, 21, 0, 23, 7);
	add_rect(grid, 23, 5, 26, 7);
	return grid;
}

// Weighted struct inheriting from basic grid.
struct GridWithWeights : SquareGrid {
	// this is a collection of basic elements that will have more cost to them.
	std::unordered_set<GridLocation> forests;
	//better collection to hold arbitrary weights for nodes. If node is not included, weight is presumed to be 1.
	std::unordered_map<GridLocation, int> forestWeights;

	//calls a inherited constructor to set size of grid.
	GridWithWeights(int w, int h) : SquareGrid(w, h) {}

	//this method returns the weight for supplied current and desired element to move to. currently only considering the latter.
	//also only considering values 5 for forrest and 1 for normal.
	double cost(GridLocation from_node, GridLocation to_node) const {
		return forestWeights.find(to_node) != forestWeights.end() ? forestWeights.find(to_node)->second : 1;
	}
};

//function returns a weighted grid of size 10,10, a filled forrest and one obstacle. More info about forrest filling inside.
GridWithWeights make_diagram4() {
	//setting size
	GridWithWeights grid(100, 100);
	//adding wall
	add_rect(grid, 1, 7, 4, 9);
	typedef GridLocation L;

	//fills the forrest set with 27 basic elements with coordinates provided.
	grid.forests = std::unordered_set<GridLocation>{
	  L{3, 4}, L{3, 5}, L{4, 1}, L{4, 2},
	  L{4, 3}, L{4, 4}, L{4, 5}, L{4, 6},
	  L{4, 7}, L{4, 8}, L{5, 1}, L{5, 2},
	  L{5, 3}, L{5, 4}, L{5, 5}, L{5, 6},
	  L{5, 7}, L{5, 8}, L{6, 2}, L{6, 3},
	  L{6, 4}, L{6, 5}, L{6, 6}, L{6, 7},
	  L{7, 3}, L{7, 4}, L{7, 5}
	};
	return grid;
}

//A priority structure holding a queue of pairs and a vector of pairs.
template<typename T, typename priority_t>
struct PriorityQueue {
	typedef std::pair<priority_t, T> PQElement;

	//make the queue return the smallest element.
	std::priority_queue<PQElement, std::vector<PQElement>,
		std::greater<PQElement>> elements;

	//TELLS US IF THE QUEUE IS EMPTY.
	inline bool empty() const {
		return elements.empty();
	}
	//puts the supplied element with its priority in queue
	inline void put(T item, priority_t priority) {
		elements.emplace(priority, item);
	}

	//returnsitem with best priority.
	T get() {
		T best_item = elements.top().second;
		elements.pop();
		return best_item;
	}
};

//Search without a heuristic for djikstra
template<typename Location, typename Graph>
void dijkstra_search
(Graph graph,
	Location start,
	Location goal,
	std::unordered_map<Location, Location>& came_from,
	std::unordered_map<Location, double>& cost_so_far)
{
	PriorityQueue<Location, double> frontier;
	frontier.put(start, 0);

	came_from[start] = start;
	cost_so_far[start] = 0;

	while (!frontier.empty()) {
		Location current = frontier.get();

		if (current == goal) {
			break;
		}

		for (Location next : graph.neighbors(current)) {
			double new_cost = cost_so_far[current] + graph.cost(current, next);
			if (cost_so_far.find(next) == cost_so_far.end()
				|| new_cost < cost_so_far[next]) {
				cost_so_far[next] = new_cost;
				came_from[next] = current;
				frontier.put(next, new_cost);
			}
		}
	}
}

//reconstruction of the followed path.
template<typename Location>
std::vector<Location> reconstruct_path(Location start, Location goal, std::unordered_map<Location, Location> came_from)
{
	std::vector<Location> path;
	Location current = goal;
	while (current != start) {
		path.push_back(current);
		current = came_from[current];
	}
	path.push_back(start); // optional
	std::reverse(path.begin(), path.end());
	return path;
}

// A heuristic for a*

inline double heuristic(GridLocation a, GridLocation b)
{
	return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

//a star algorithm
template<typename Location, typename Graph>
void a_star_search(Graph graph, Location start, Location goal,
	std::unordered_map<Location, Location>& came_from,
	std::unordered_map<Location, double>& cost_so_far)
{
	PriorityQueue<Location, double> frontier;
	frontier.put(start, 0);

	came_from[start] = start;
	cost_so_far[start] = 0;

	while (!frontier.empty()) {
		Location current = frontier.get();

		if (current == goal) {
			break;
		}

		for (Location next : graph.neighbors(current)) {
			double new_cost = cost_so_far[current] + graph.cost(current, next);
			if (cost_so_far.find(next) == cost_so_far.end()
				|| new_cost < cost_so_far[next]) {
				cost_so_far[next] = new_cost;
				double priority = new_cost + heuristic(next, goal);
				frontier.put(next, priority);
				came_from[next] = current;
			}
		}
	}
}

class AStar
{
public:
	/// <summary>
	/// Initializes a new instance of the Weighted grid for aStar search - default size (100x100) change with "addEmptyWeightedGraph"<see cref="AStar"/> class.
	/// </summary>
	AStar()

	{
		forrestGrid.width = 100;
		forrestGrid.height = 100;
	};

	bool addEmptyWeightedGraph(int width, int height)
	{
		forrestGrid.width = width;
		forrestGrid.height = height;
		return true;
	}
	/// <summary>
	/// Adds one rectangular wall to both types of graph.
	/// </summary>
	/// <param name="xStart">The x start.</param>
	/// <param name="xEnd">The x end.</param>
	/// <param name="yStart">The y start.</param>
	/// <param name="yEnd">The y end.</param>
	/// <returns>bool if succeeded</returns>
	bool addWallToGraph(int xStart, int xEnd, int yStart, int yEnd)
	{
		GridLocation elementx{ xStart, yStart };
		GridLocation elementy{ xEnd, yEnd };
		if (!forrestGrid.in_bounds(elementx) || !forrestGrid.in_bounds(elementy))
			return false;
		add_rect(forrestGrid, xStart, yStart, xEnd, yEnd);
		return true;
	}
	/// <summary>
	/// Adds the whole colection of wall locations into both graphs. Wall is represented by Gridlocation element with an X and Y coordinate;
	/// </summary>
	/// <param name="inWalls">The in walls.</param>
	/// <returns></returns>
	bool addWallCollectionToGraph(std::unordered_set<GridLocation> inWalls)
	{
		for (auto& element : inWalls)
		{
			if (!forrestGrid.in_bounds(element))
				return false;
		}

		forrestGrid.walls = inWalls;
		return true;
	}

	/// <summary>
	/// Adds the forest map to weighted graph. Forest map is a map of pairs of Gridlocation elements and their values as int.
	/// Any value can be passed, but should be above zero. Tiles with a value of 1 are
	/// treated as default and do not have to be supplied to forest map.
	/// </summary>
	/// <param name="inForestWeights">The in forest weights.</param>
	/// <returns>Bool if succeeded.</returns>
	bool addForestMapToWeightedGraph(std::unordered_map<GridLocation, int> inForestWeights)
	{
		for (auto& element : inForestWeights)
		{
			if (!forrestGrid.in_bounds(element.first))
				return false;
		}

		this->forrestGrid.forestWeights = inForestWeights;
		return true;
	}

	std::vector<GridLocation> searchForWholePath(GridLocation start, GridLocation end)
	{
		if (!forrestGrid.in_bounds(start) || !forrestGrid.in_bounds(end))
		{
			std::vector<GridLocation> empty;
			return empty;
		}

		//do the whole thing.
		a_star_search(this->forrestGrid, start, end, this->came_from, this->cost_so_far);
		//reconstruct and return the walked path.
		return reconstruct_path(start, end, this->came_from);
	}

public:
	//The grid of elements with added collection for forrests.
	GridWithWeights forrestGrid = GridWithWeights(0, 0);
	// Some things we need for search;
	std::unordered_map<GridLocation, GridLocation> came_from;
	std::unordered_map<GridLocation, double> cost_so_far;
};
