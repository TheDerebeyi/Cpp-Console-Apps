							/* The game of hex. */
							/********************/
							/* Mert Kahraman    */
							/* October 14, 2020 */
#include <iostream>
#include <vector>
using namespace std;
const int SIZE = 11; //Default size

class Graph
{
public:
	Graph(int node_size = SIZE) : vertices(node_size* node_size), is_occupied(new bool*[node_size * node_size]), width(node_size), edges(0), graph(new bool* [node_size * node_size]), cost(new double* [node_size * node_size]), close(new bool[node_size * node_size]), open(new bool[node_size * node_size]), cost_to(new double[node_size * node_size]) //Constructor.
	{
		for (int i = 0; i < vertices; i++)
		{
			open[i] = false;               //Initializers for member functions.
			close[i] = false;
			graph[i] = new bool[vertices];
			cost[i] = new double[vertices];
			is_occupied[i] = new bool[2];
			cost_to[i] = 999.0;
		}
		for (int i = 0; i < vertices; i++)     //These loops are for connecting nodes. Corner and edge nodes has special loops.
		{
			is_occupied[i][0] = false;
			for (int j = 0; j < vertices; j++)
			{
				graph[i][j] = false;
			}
		}
		for(int i = 0; i < node_size*(node_size-2)+1; i+=node_size) //node_size for user inputted width value.
		{
			graph[i][i + node_size] = graph[i + node_size][i] = true; //0 is left top corner, node_size -1 is right top corner.
			west_nodes.push_back(i);
		}
		west_nodes.push_back(node_size * (node_size - 1));
		for (int i = node_size-1; i < node_size * (node_size-1); i += node_size) //node_size * (node_size -1) is left bottom corner, node_size * node_size -1 or vertices -1 is right bottom corner.
		{
			graph[i][i + node_size] = graph[i + node_size][i] = true;
			east_nodes.push_back(i);
		}
		east_nodes.push_back(vertices - 1);
		for(int i = 0; i <node_size - 1;i++)
		{
			graph[i][i + 1] = graph[i+1][i]= true;
			north_nodes.push_back(i);
		}
		north_nodes.push_back(node_size - 1);
		for(int i = (node_size * (node_size -1)); i < vertices - 1;i++)
		{
			graph[i][i + 1] = graph[i + 1][i] = true;
			south_nodes.push_back(i);
		}
		south_nodes.push_back(vertices - 1);
		for(int i = 1; i < node_size -1; i++) //Each inner hexagon has 6 edge.
		{
			for(int j = (node_size * i) +1; j < (node_size* (i+1)) - 1; j++)
			{
				graph[j][j - 1] = graph[j][j + 1] = graph[j - 1][j] = graph[j + 1][j] = true;
				graph[j][j - node_size] = graph[j][j + node_size] = graph[j - node_size][j] = graph[j + node_size][j] = true;
				graph[j][j - (node_size -1)] = graph[j][j + node_size - 1] = graph[j - (node_size - 1)][j] = graph[j + node_size - 1][j] = true;
			}
		}
		graph[1][node_size] = graph[node_size][1] = true;
		graph[vertices - 2][node_size * (node_size - 1) - 1] = graph[node_size * (node_size - 1) - 1][vertices - 2] = true;
		edges = (node_size) * (node_size - 1) * 2 + node_size - 1; //This calculation is for edge count.
		for(int i = node_size -2; i > 0; --i)
		{
			edges += i * 2;
		}
	}

	int v() { return vertices; }
	int e() { return edges; }
	int* neighbors(int node);
	void print();
	void print_board();
	void game();
	bool is_game_ended();
	void special_close_cost_to_reset(bool blue_or_red);
	bool is_connected(int node1, int node2);
	void visit_neighbors(int node1);
	
private:
	int edges, vertices;
	bool** graph;
	double** cost;
	bool* close;
	bool* open;
	double* cost_to;
	int width;
	bool** is_occupied;
	vector<int> north_nodes, south_nodes, east_nodes, west_nodes, visited;
};

int* Graph::neighbors(int node)
{
	int* adj = new int[vertices];
	int index = 1, count = 0; //We will use adj[0] as numbers of neighbors. Placement of nodes will start with index 1. 
	for (int i = 0; i < vertices; i++)
	{
		if (graph[node][i])
		{
			adj[index] = i;
			index++;
			count++;
		}
	}
	adj[0] = count;
	return adj;
}

void Graph::print() //Prints graph as matrix representation.
{
	cout << "Current graph has " << edges << " edges and " << vertices << " nodes." << endl << "Printing graph..." << endl << endl;
	for (int i = 0; i < vertices;i++)
	{
		for (int j = 0; j < vertices; j++)
		{
			cout << graph[i][j] << " ";
		}
		cout << endl;
	}
}

void Graph::print_board() //Prints game board.
{
	cout <<endl<< (is_occupied[0][0] ? (is_occupied[0][1] ? "O" : "X") : "."); //if is_occupied[0][1] is true blue (0) has occupied that hexagon. If false red (X) occupied.
	for (int j = 1; j < width; j++)
	{
		cout << " - "<< (is_occupied[j][0] ? (is_occupied[j][1] ? "O" : "X") : ".");
	}
	cout << endl;
	for(int i = 1; i < width; i++)
	{
		for(int k = 0; k < i; k++)
		{
			cout << " ";
		}
		cout << "\\";
		for (int j = 1; j < width; j++)
		{
			cout << " / \\";
		}
		cout << endl;
		for (int k = 0; k < i; k++)
		{
			cout << " ";
		}
		cout << (is_occupied[width*i][0] ? (is_occupied[width*i][1] ? "O" : "X") : ".");
		for(int j = 1; j < width; j++)
		{
			cout << " - " << (is_occupied[(width*i)+j][0] ? (is_occupied[(width * i) + j][1] ? "O" : "X") : ".");
		}
		cout << endl;
	}

}

void Graph::game() //Main function that controls the game.
{
	int turn = 1, temp;
	while(is_game_ended())
	{
		print_board(); //Prints board at the begining of each turn.
		cout << endl << (turn%2 > 0? "Blue":"Red") <<" player's turn. Enter a valid and unoccupied hexagon index to make your move ('.'s are available): ";
		while (1) {
			cin >> temp;
			if (temp >= 0 && temp < vertices && is_occupied[temp][0] == false)
			{
				is_occupied[temp][0] = true;
				is_occupied[temp][1] = (turn % 2 > 0); //if true blue occupied, else red occupied.
				break;
			}
			cout << "Please enter a valid and unoccupied index between 0 and " << vertices - 1 << ": ";
		}
		turn++;
	}
}

bool Graph::is_game_ended() //return false if game ended.
{
	for (auto p = north_nodes.begin(); p != north_nodes.end();++p)
	{
		if (is_occupied[*p][1] == false) //is_occupied[*p][1] = false is for red occupy. If it can't find any it won't try to calculate for red.
		{
			for (auto it = south_nodes.begin(); it != south_nodes.end();++it)
			{
				if (is_occupied[*it][1] == false) //if there is a occupied node at north it will continue with trying south nodes. If it can find any it will check if the two is connected or not.
				{
					special_close_cost_to_reset(false);
					if (is_connected(*p, *it))
					{
						print_board();
						cout << endl << "Red has won!"<<endl;
						return false;
					}
				}
			}
		}
	}
	for (auto p = east_nodes.begin(); p != east_nodes.end();++p)
	{
		if (is_occupied[*p][1] == true)
		{
			for (auto it = west_nodes.begin(); it != west_nodes.end();++it)
			{
				if (is_occupied[*it][1] == true)
				{
					special_close_cost_to_reset(true);
					if (is_connected(*p, *it))
					{
						print_board();
						cout << endl << "Blue has won!" << endl;
						return false;
					}
				}
			}
		}
	}
	return true;
}

void Graph::special_close_cost_to_reset(bool blue_or_red) //true for blue, false for red
{
	for (int i = 0; i < vertices; i++)
	{
		if (is_occupied[i][1] == blue_or_red)
		{
			close[i] = false;
		}
		else
		{
			close[i] = true;
		}
	}
}

bool Graph::is_connected(int node1, int node2) //Checks if two node is connected or not.
{
	visited.clear();
	visit_neighbors(node1);
	if(find(visited.begin(),visited.end(),node2)!= visited.end())
	{
		return true;
	}
	return false;
}

void Graph::visit_neighbors(int node1) //Visits node1's open set neighbors.
{
	int* arr = new int[vertices];
	arr = neighbors(node1);
	for (int i = 1; i <= arr[0];i++)
	{
		if(!close[arr[i]])
		{
			visited.push_back(arr[i]);
			close[arr[i]] = true;
			visit_neighbors(arr[i]);
		}
	}
}

int main()
{
	int width;
	cout << "Welcome to the Game of Hex!" << endl;
	cout << "Please enter a width to size hex board: ";
	while (1) {
	cin >> width; //Width is for board width.
	if (width >= 3 && width <= 19) break;
	cout << "Please enter a valid integer between 3 and 19: ";
	}
	cout << endl;
	Graph board = width; //Creates a board with "width" width.
	board.game(); //Starts game.
	return 0;
}
   