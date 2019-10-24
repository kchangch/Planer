/** Student: Kidjou Chang
  * SID: 100281360
  * Course: CSPC1160
  * Program description: program that reads from file different cities and the path
  * that they can be visited with the price. The program determines the best route
  * that an user can take in order to visit the most amount of cities with the lowest
  * cost rate. The program utilizes the shortest path algorithm created by Dijkstra
  */

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include <cstdlib>
#include <iomanip>

using namespace std;

// Global variables
const int SIZE = 50;
int numOfCities = 0;

// Global arrays
string cityVertex[SIZE];
double adjacencyMatrix[SIZE][SIZE];
int dist[SIZE];
string pathTracker[SIZE];

// Function prototypes
void getCities(string fileName);
void fillAdjacenyMatrix(ifstream &input);
void extractCityNames(string firstLine);
void initializeArray();
void indexExtraction(string cityX1, string cityX2, int &indexRow, int &indexColumn);
void displayMessage(string &fileName, string &startLocation);
void dijkstra(double adjacencyMatrix[SIZE][SIZE], int sourceIndex);
int normalizeInput(string &startLocation);
int minDistance(int dist[], bool sptSet[]);
void printPath(int parent[], int j);
void printSolution(int dist[], int n, int parent[], int sourceIndex);
int bestRoute();
void printBestRoute(int bestRouteIndex);
void extractPath(int pathArray[], int bestRouteIndex, int &amountOfCities);
void printHeader(int amountOfCities, int pathArray[], int bestRouteIndex);
void printResult(int amountOfCities, int pathArray[], int bestRouteIndex);
int pathValidation (int sourceIndex);

int main() {
    
    system("cls");
    string fileName, startLocation;
    displayMessage(fileName, startLocation);
    getCities(fileName);
    dijkstra(adjacencyMatrix, pathValidation(normalizeInput(startLocation)));
    printBestRoute(bestRoute());
    system("PAUSE");
    
}

/** Function that initializes the path tracker array where the path to
  * each city will be stored
  */
void initializePathTracker() {
    
    for (int i = 0; i < SIZE; i++) {
        
        pathTracker[i] = to_string(i);
        
    }
    
}

/** Function that displays on terminal the start up message, asking the user to input
  * the name of the file to be read from and the starting location
  * param1: Name of the time
  * param2: Starting location
  */
void displayMessage(string &fileName, string &startLocation) {
    
    cout << "Please enter input file: ";
    cin >> fileName;
    cout << "Please enter your start location: ";
    cin >> startLocation;
    initializeArray();
    initializePathTracker();
    
}

/** Function that extracts the information from the input file
  * This function extract the vertex of the graph (cities) and stores it in an single 
  * dimensional array. It also extracts the edges of the graph (directions and costs)
  * and stores it in a 2D array (adjacency matrix)
  * param1: Name of the file to be read from
  */
void getCities(string fileName) {
    
    ifstream input(fileName);
    string city;
    
    if (input.is_open()) {
        
        getline(input,city);
        extractCityNames(city);
        fillAdjacenyMatrix(input);
        input.close();
        
    }
    
    else {
        
        cout << "ERROR - Failed reading the file. Please try again!" << endl << endl;
        exit(0);
        
    }
        
}

/** Function that fills the adjacency matrix while reading from the file
  * The adjacency matrix contains the costs from the starting point to the
  * destination
  * param1: Input file
  */
void fillAdjacenyMatrix(ifstream &input) {

    int row = 0;
    string cityX1, cityX2, price;
    
    while (!input.eof()) {

        int indexRow, indexColumn;
        
        input >> cityX1 >> cityX2 >> price;
        indexExtraction(cityX1, cityX2, indexRow, indexColumn);
        adjacencyMatrix[indexRow][indexColumn] = stod(price.substr(1,price.length()).c_str());
        adjacencyMatrix[indexColumn][indexRow] = stod(price.substr(1,price.length()).c_str());
        
    }

}

/** Function that extracts the indexes for columns and rows that will be used
  * in order to access the adjacency matrix
  * param1: Starting city
  * param2: Destination city
  * param3: Index of the row
  * param4: Index of the column
  */
void indexExtraction(string cityX1, string cityX2, int &indexRow, int &indexColumn) {
    
    bool statusRow = false;
    bool statusColumn = false;
    
    for (int i = 0; i < numOfCities; i++) {
        
        if (cityVertex[i] == cityX1) {
            indexRow = i;
            statusRow = true;
        }
        else if (cityVertex[i] == cityX2) {
            indexColumn = i;
            statusColumn = true;
        }
        else if (statusRow && statusColumn) {
            break;
        }
        
    }
    
}

/** Function that reads the first line of the file where all the cities are (vertex)
  * It goes through every name to make sure that it is valid name and then add it
  * into the array. It counts also the number of vertex that the graph will have
  * param1: First line from the input file
  */
void extractCityNames(string firstLine) {

    int index = 0;

    for (int i = 0; i < firstLine.length(); i++) {

        if (isalpha(firstLine[i])) {
        
            while (isalpha(firstLine[i])) {
            
                cityVertex[index] += firstLine[i++];
            
            }

            index++;
            numOfCities++;
            
        }

    }

}

/** Function that initializes the adjacency matrix at infinity values
  * This matrix will be used to store the cost of the edges of the graph
  */
void initializeArray() {
    
    for (int i = 0; i < SIZE; i++) {
        
        for (int j = 0; j < SIZE; j++) {
            
            adjacencyMatrix[i][j] = INT_MAX;
            
        }
        
    }
    
}

/** Function that normalizes the string received from the user. It makes sure
  * that the first letter is capital and the rest is lower case. This function
  * also verifies that the city received from the user is an existing vertex (valid city).
  * Otherwise it will display an error and finish the program
  * param1: Start location (input by user)
  */
int normalizeInput(string &startLocation) {
    
    int sourceIndex;
    
    startLocation[0] = toupper(startLocation[0]);
    
    for (int i = 1; i < startLocation.length(); i++) {
        
        startLocation[i] = tolower(startLocation[i]);
        
    }
    
    for (int i = 0; i < numOfCities; i++) {
        
        if (cityVertex[i] == startLocation) {
            
            sourceIndex = i;
            return sourceIndex;
            
        }
        
    }
    
    cout << "ERROR - The starting location is not part of the city list! Please try again!" << endl;
    exit(0);
    
}

/** Function that verifies that the city is not an isolated vertex, meaning that
  * it is reachable from other nodes (checks if the node is isolated or not)
  * param1: Index where the city in the vertex array
  * returns: Index if node is not isolated, otherwise terminates the program
  */
int pathValidation (int sourceIndex) {
    
    for (int i = 0; i < SIZE; i++) {
        
        if (adjacencyMatrix[sourceIndex][i] != INT_MAX) {
            return sourceIndex;
        }
    }
    
    cout << "ERROR - Other cities cannot be reached from this starting location. Please try another location!" << endl;
    exit(0);
    
}

/** Function to calculate the shortest path (Dijkstra's Algorithm)
  * param1: Adjacency matrix
  * param2: Starting node
  */
void dijkstra(double adjacencyMatrix[SIZE][SIZE], int sourceIndex) {

    // sptSet[i] will true if vertex i is included / in shortest
    // path tree or shortest distance from src to i is finalized
    bool sptSet[SIZE];

    // Initialize all distances as INFINITE and stpSet[] as false
    for (int i = 0; i < SIZE; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = false;
    }
    
    // Distance of source vertex from itself is always 0
    dist[sourceIndex] = 0;
    
    // Find shortest path for all vertices
    for (int count = 0; count < SIZE-1; count++) {
        // Pick the minimum distance vertex from the set of
        // vertices not yet processed. u is always equal to src
        // in first iteration.
        int u = minDistance(dist, sptSet);
        
        // Mark the picked vertex as processed
        sptSet[u] = true;
        
        // Update dist value of the adjacent vertices of the
        // picked vertex.
        for (int v = 0; v < SIZE; v++) {
            
            // Update dist[v] only if is not in sptSet, there is
            // an edge from u to v, and total weight of path from
            // source to v through u is smaller than current value of
            // dist[v]
            if (!sptSet[v] && adjacencyMatrix[u][v] && dist[u] + adjacencyMatrix[u][v] < dist[v]) {
                
                dist[v] = dist[u] + adjacencyMatrix[u][v];
                pathTracker[v] = (pathTracker[u] + '-' + pathTracker[v]);
                
            }
            
        }
        
    }

}

/** Function that calculates the minimum distance from one node to another.
  * It checks if the node has been visited or not from the sptSet[] array
  * param1: Distance array
  * param2: Visited array
  */
int minDistance(int dist[], bool sptSet[]) {
    
    // Initialize min value
    int min = INT_MAX, min_index;
    
    for (int v = 0; v < SIZE; v++) {
    
        if (sptSet[v] == false && dist[v] <= min) {
            
            min = dist[v], min_index = v;
            
        }
        
    }
    
    return min_index;
}

/** Function that checks the path tracker array and look for the best path.
  * It counts the amount of cities visited in the best path
  * return: The index where the best path is stored in the path tracker array
  */
int bestRoute() {
    
    int bestPath = 0;
    int bestRouteIndex;
    
    for (int i = 0; i < numOfCities; i++) {
        
        int amountOfCities = 0;
        
        for (int j = 0; j < pathTracker[i].length(); j++) {
            
            if (pathTracker[i][j] == '-') {
                amountOfCities++;
            }
            
        }

        if (amountOfCities > bestPath) {
            bestPath = amountOfCities;
            bestRouteIndex = i;
        }
        
    }
    
    return bestRouteIndex;
    
}

/** Function that prints out the result of the best path
  * param1: The index of the best path stored in the path tracker array
  */
void printBestRoute(int bestRouteIndex) {
    
    int pathArray[SIZE];
    int amountOfCities = 0;

    extractPath(pathArray, bestRouteIndex, amountOfCities);
    printHeader(amountOfCities, pathArray, bestRouteIndex);
    printResult(amountOfCities, pathArray, bestRouteIndex);
    
}

/** Function that extracts the best path from the tracker array and stores
  * the values in a new array only with that path containing the cities that
  * it visited
  * param1: Path array that contains only the best path
  * param2: The index of the best path stored in the path tracker array
  * param3: Amount of cities visited in this path
  */
void extractPath(int pathArray[], int bestRouteIndex, int &amountOfCities) {
    
    int pathIndex = 0;
    
    for (int i = 0; i < pathTracker[bestRouteIndex].length(); i++) {
        
        if (isdigit(pathTracker[bestRouteIndex][i])) {
            
            pathArray[pathIndex++] = pathTracker[bestRouteIndex][i] - '0';
            amountOfCities++;
            
        }
        
    }
    
}

/** Function that prints the header of the result that will be displayed in the terminal
  * param1: Amount of cities visited in this path
  * param2: Path array that contains only the best path
  * param3: The index of the best path stored in the path tracker array
  */
void printHeader(int amountOfCities, int pathArray[], int bestRouteIndex) {
    
    cout << "Well..." << endl;
    cout << "Suggesting plan to buy ticket with total cost $" << dist[bestRouteIndex] << "." << endl;
    
    for (int i = 0; i < 50; i++) {
        cout << '=';
    }
    
    cout << endl;
    
    for (int i = 0; i < amountOfCities; i++) {
        
        if (i+1 < amountOfCities)
            cout << cityVertex[pathArray[i]] << "->";
        else {
            cout << cityVertex[pathArray[i]];
        }
        
    }
    
    cout << endl;
    
    for (int i = 0; i < 50; i++) {
        cout << '=';
    }
    
    cout << endl;
    
}

/** Function that prints the result of the best path, displaying the cities in order of visit
  * param1: Amount of cities visited
  * param2: Path array that contains only the best path
  * param3: The index of the best path stored in the path tracker array
  */
void printResult(int amountOfCities, int pathArray[], int bestRouteIndex) {
    
    cout << "Detailed Plan #Visiting city: " << amountOfCities-1 << " Cost Rate: $" << dist[bestRouteIndex]/(amountOfCities-1) << endl;
    
    for (int i = 0; i < 50; i++) {
        cout << '-';
    }
    
    cout << endl;
    
    for (int i = 0; i < amountOfCities-1; i++) {
        
        cout << cityVertex[pathArray[i]] << "->" << cityVertex[pathArray[i+1]] << " $" << adjacencyMatrix[pathArray[i]][pathArray[i+1]] << endl;
        
    }
    
    for (int i = 0; i < 50; i++) {
        cout << '-';
    }
    
    cout << endl;
    
    cout << "Total cost: $" << dist[bestRouteIndex] << endl;
    
}


