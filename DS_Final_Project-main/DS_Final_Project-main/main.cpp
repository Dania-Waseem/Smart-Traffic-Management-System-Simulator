#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include <cstring>
#include <cmath> 
#include <cstdlib>
#include <ctime>

using namespace std;

class Traffic
{
public:
    static const int MAX_NODES = 26; // Number of nodes (A to Z)
    static const int TABLE_SIZE = 100;
    int roads[MAX_NODES][MAX_NODES];

    struct Road
    {
        string roadName;
        int vehicleCount;
        Road *next;
    };

    Road *table[TABLE_SIZE];

    // hash function for road name
    int hashFunction(const string &name)
    {
        int hash = 0;
        for (char ch : name)
        {
            hash = (hash * 31 + ch) % TABLE_SIZE;
        }
        return hash;
    }

   //adding road in the hash table
    void addRoad(const string &name)
    {
        int index = hashFunction(name);
        Road *newRoad = new Road{name, 1, nullptr};
        if (table[index] == nullptr)
        {
            table[index] = newRoad;
        }
        else
        {
            Road *current = table[index];
            while (current->next != nullptr)
            {
                current = current->next;
            }
            current->next = newRoad;
        }
    }

    // Increasing the vehicle count for a road
    void updateRoadCount(const string &name)
    {
        int index = hashFunction(name);
        Road *current = table[index];
        while (current != nullptr)
        {
            if (current->roadName == name)
            {
                current->vehicleCount += 1;
                return;
            }
            current = current->next;
        }
        addRoad(name);
    }


    Traffic()
    {
        for (int i = 0; i < MAX_NODES; i++)
        {
            for (int j = 0; j < MAX_NODES; j++)
            {
                roads[i][j] = 0;
            }
        }
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            table[i] = nullptr;
        }
    }

    int getNodeIndex(char node)
    {
        return node - 'A';
    }

    
    void loadData(const string &filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cerr << "Error: Unable to open file!" << endl;
            return;
        }

        string line;
        getline(file, line); // Skip header

        while (getline(file, line))
        {
            stringstream ss(line);
            string vehicleID, start, end;
            getline(ss, vehicleID, ',');
            getline(ss, start, ',');
            getline(ss, end, ',');

            roads[getNodeIndex(start[0])][getNodeIndex(end[0])] = 1;

           
            for (char curNode = start[0]; curNode != end[0];)
            {
                char nextNode = curNode + 1;
                if (nextNode > end[0])
                    break;
                string road = string(1, curNode) + " to " + string(1, nextNode);
                updateRoadCount(road);
                curNode = nextNode;
            }
        }

        file.close();
    }

  
    void showCongestion()
    {
        bool congestionFound = false;
        cout << "------ Congestion Status ------" << endl;
        const int threshold = 3;

        for (int i = 0; i < TABLE_SIZE; i++)
        {
            Road *current = table[i];
            while (current != nullptr)
            {
                if (current->vehicleCount >= threshold)
                {
                    cout << current->roadName << " -> Vehicles: " << current->vehicleCount << " (Congested)" << endl;
                    congestionFound = true;
                }
                else if (current->vehicleCount > 0)
                {
                    cout << current->roadName << " -> Vehicles: " << current->vehicleCount << endl;
                }
                current = current->next;
            }
        }

        if (!congestionFound)
        {
            cout << "No congested roads detected." << endl;
        }
    }

    // Depth-First Search (DFS) for rerouting
    void dfs(int start, bool visited[], int parent[])
    {
        visited[start] = true;

        // Explore all the neighboring nodes
        for (int i = 0; i < MAX_NODES; i++)
        {
            if (roads[start][i] == 1 && !visited[i])
            {
                parent[i] = start;
                dfs(i, visited, parent);
            }
        }
    }

   
    void printPath(int parent[], int end)
    {
        if (end == -1)
        {
            return; // No path found
        }

        printPath(parent, parent[end]);
        cout << (char)(end + 'A') << " "; 
    }

    // Breadth-First Search (BFS) for rerouting
    void bfs(int start)
    {
        bool visited[MAX_NODES] = {false};
        int parent[MAX_NODES] = {-1};
        int queue[MAX_NODES];
        int front = 0, rear = 0;

        visited[start] = true;
        queue[rear++] = start;

        while (front != rear)
        {
            int node = queue[front++];
            for (int i = 0; i < MAX_NODES; i++)
            {
                if (roads[node][i] == 1 && !visited[i])
                {
                    visited[i] = true;
                    parent[i] = node;
                    queue[rear++] = i;
                }
            }
        }

       
        for (int i = 0; i < MAX_NODES; i++)
        {
            if (visited[i])
            {
                cout << "Node " << (char)(i + 'A') << " can be reached, rerouted path: ";
                printPath(parent, i);
                cout << endl;
            }
        }
    }
};

class PriorityQueue 
{
public:
    struct QueueNode 
    {
        char vertex;
        int distance;
    };

private:
    QueueNode *heap;
    int capacity;
    int size;

    void heapifyUp(int index) 
    {
        while (index > 0 && heap[index].distance < heap[(index - 1) / 2].distance) 
        {
            QueueNode temp = heap[index];
            heap[index] = heap[(index - 1) / 2];
            heap[(index - 1) / 2] = temp;
            index = (index - 1) / 2;
        }
    }

    void heapifyDown(int index) 
    {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < size && heap[left].distance < heap[smallest].distance)
            smallest = left;

        if (right < size && heap[right].distance < heap[smallest].distance)
            smallest = right;

        if (smallest != index) 
        {
            QueueNode temp = heap[index];
            heap[index] = heap[smallest];
            heap[smallest] = temp;
            heapifyDown(smallest);
        }
    }

public:
    PriorityQueue(int capacity) : capacity(capacity), size(0) 
    {
        heap = new QueueNode[capacity];
    }

    void insert(char vertex, int distance) 
    {
        heap[size] = {vertex, distance};
        heapifyUp(size);
        size++;
    }

    QueueNode extractMin() 
    {
        QueueNode minNode = heap[0];
        heap[0] = heap[size - 1];
        size--;
        heapifyDown(0);
        return minNode;
    }

    bool isEmpty() 
    {
        return size == 0;
    }
};

// representing the roads bw intersections as egdes
// travel time is weights

// next pointer:used to point to the next edge in adjacency list
class Edge
{
public:
    char destination;
    int weight;
    Edge *next;
};

// intersections are nodes
// has a pointer to 1st edge which originates from here
class Node
{
public:
    char name;
    Edge *head;
};

// Blocked road structure
struct BlockedRoad 
{
    char intersection1;
    char intersection2;
    
    // Check if two roads are blocked (either direction)
    bool isBlocked(char start, char end) const 
    {
        return (intersection1 == start && intersection2 == end) || 
               (intersection1 == end && intersection2 == start);
    }
};

class Graph
{
public:
    Node *nodes;
    int size;
    static const int INF = 10000;  // Use a large number as infinity
    static const int MAX_BLOCKED_ROADS = 100;  // Maximum number of blocked roads

    BlockedRoad blockedRoads[MAX_BLOCKED_ROADS];  // Array to store blocked roads
    int blockedRoadCount = 0;  // Counter to keep track of how many roads are blocked

    Node *findOrCreateNode(char name)
    {
        for (int i = 0; i < size; ++i)
        {
            if (nodes[i].name == name)
            {
                return &nodes[i];
            }
            if (nodes[i].name == '\0')
            {
                nodes[i].name = name;
                return &nodes[i];
            }
        }
        return nullptr;
    }

    int parseInt(const char *str, int &index)
    {
        int result = 0;
        while (str[index] >= '0' && str[index] <= '9')
        {
            result = result * 10 + (str[index] - '0');
            index++;
        }
        return result;
    }

    Edge *sortEdges(Edge *head)
    {
        if (!head || !head->next)
        {
            return head;
        }
        // Bubble sort
        for (Edge *current = head; current; current = current->next)
        {
            for (Edge *nextEdge = head; nextEdge->next; nextEdge = nextEdge->next)
            {
                if (nextEdge->destination > nextEdge->next->destination)
                {
                    char tempDestination = nextEdge->destination;
                    nextEdge->destination = nextEdge->next->destination;
                    nextEdge->next->destination = tempDestination;

                    int tempWeight = nextEdge->weight;
                    nextEdge->weight = nextEdge->next->weight;
                    nextEdge->next->weight = tempWeight;
                }
            }
        }
        return head;
    }

    Graph(int size)
    {
        this->size = size;
        nodes = new Node[size];
        for (int i = 0; i < size; ++i)
        {
            nodes[i].name = '\0';
            nodes[i].head = nullptr;
        }
    }

    ~Graph()
    {
        for (int i = 0; i < size; ++i)
        {
            Edge *current = nodes[i].head;
            while (current)
            {
                Edge *temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] nodes;
    }

    void addEdge(char source, char destination, int weight)
    {
        Node *sourceNode = findOrCreateNode(source);

        Edge *current = sourceNode->head;
        while (current)
        {
            if (current->destination == destination && current->weight == weight)
            {
                return;
                // inorder to avoid duplicate edges
            }
            current = current->next;
        }

        Edge *newEdge = new Edge{destination, weight, sourceNode->head};
        sourceNode->head = newEdge;
    }

    bool removeEdge(char source, char destination)
    {
        Node *sourceNode = findOrCreateNode(source);
        Edge *current = sourceNode->head;
        Edge *prev = nullptr;

        while (current)
        {
            if (current->destination == destination)
            {
                if (prev)
                    prev->next = current->next;
                else
                    sourceNode->head = current->next;

                delete current;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    void loadFromCSV(const char *filePath)
    {
        ifstream file(filePath);
        if (!file.is_open())
        {
            cout << "Error: Could not open file " << filePath << endl;
            return;
        }

        string line;
        getline(file, line); 

        while (getline(file, line))
        {
            int index = 0;
            char source = line[index];
            index += 2; // Skipping the comma
            char destination = line[index];
            index += 2;
            int travelTime = 0;

            while (index < line.size() && line[index] >= '0' && line[index] <= '9')
            {
                travelTime = travelTime * 10 + (line[index] - '0');
                index++;
            }

            addEdge(source, destination, travelTime);
            addEdge(destination, source, travelTime);
        }

        file.close();
    }

    void updateCSV(const char *filePath)
    {
        ofstream file(filePath);
        if (!file.is_open())
        {
            cout << "Error: Could not open file for updating.\n";
            return;
        }

        file << "Source,Destination,TravelTime\n"; 

        for (int i = 0; i < size; ++i)
        {
            if (nodes[i].name == '\0')
                continue;

            Edge *current = nodes[i].head;
            while (current)
            {
                if (nodes[i].name < current->destination) 
                {
                    file << nodes[i].name << "," << current->destination << "," << current->weight << "\n";
                }
                current = current->next;
            }
        }

        file.close();
    }

    void displayGraph()
    {
        cout << "----- City Traffic Network -----\n";
        for (int i = 0; i < size; ++i)
        {
            if (nodes[i].name == '\0')
            {
                continue;
            }
           
            nodes[i].head = sortEdges(nodes[i].head);

            cout << nodes[i].name << " -> ";
            Edge *current = nodes[i].head;

            while (current)
            {
                cout << "(" << current->destination << ", " << current->weight << ")";
                if (current->next)
                {
                    cout << " ";
                }
                current = current->next;
            }
            cout << endl;
        }
    }

    void addRoad()
    {
        char source, destination;
        int weight;

        cout << "Enter source intersection: ";
        cin >> source;
        cout << "Enter destination intersection: ";
        cin >> destination;
        cout << "Enter travel time (weight): ";
        cin >> weight;

        if (weight <= 0)
        {
            cout << "Error: Weight must be a positive integer.\n";
            return;
        }

        addEdge(source, destination, weight);
        addEdge(destination, source, weight);
        updateCSV("road_network.csv");
        cout << "Road added successfully.\n";
    }

    void removeRoad()
    {
        char source, destination;
        cout << "Enter source intersection: ";
        cin >> source;
        cout << "Enter destination intersection: ";
        cin >> destination;

        if (removeEdge(source, destination) && removeEdge(destination, source))
        {
            updateCSV("road_network.csv");
            cout << "Road removed successfully.\n";
        }
        else
        {
            cout << "Error: Road not found.\n";
        }
    }

    void manageRoads()
    {
        int choice;
        do
        {
            cout << "\n1. Add a road\n";
            cout << "2. Remove a road\n";
            cout << "3. Return to main menu\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == 1)
                addRoad();
            else if (choice == 2)
                removeRoad();
            else if (choice != 3)
                cout << "Invalid choice. Try again.\n";

        } while (choice != 3);
    }

    // Load blocked roads from a CSV file
    void loadBlockedRoads(const char* filePath) 
    {
        ifstream file(filePath);
        if (!file.is_open()) 
        {
            cout << "Error: Could not open blocked roads file " << filePath << endl;
            return;
        }

        string line;
        getline(file, line); // Skip the header line

        while (getline(file, line)) 
        {
            // Use stringstream to parse the CSV line
            stringstream ss(line);
            string intersection1, intersection2, status;

            // Extract the values separated by commas
            getline(ss, intersection1, ','); // First intersection
            getline(ss, intersection2, ','); // Second intersection
            getline(ss, status, ',');        // Status of the road

            // Manual trimming of leading whitespace
            int start = 0;
            while (start < status.length() && (status[start] == ' ' || status[start] == '\t')) 
            {
                start++;
            }

            // Manual trimming of trailing whitespace
            int end = status.length() - 1;
            while (end >= start && (status[end] == ' ' || status[end] == '\t')) 
            {
                end--;
            }

            // Extract the cleaned status string
            string trimmedStatus = status.substr(start, end - start + 1);

            // Only consider roads that are "Blocked"
            if (trimmedStatus == "Blocked" && blockedRoadCount < MAX_BLOCKED_ROADS) 
            {
                blockedRoads[blockedRoadCount++] = {intersection1[0], intersection2[0]};
            }
        }

        file.close();
    }

    // Function to check if a road is blocked
    bool isRoadBlocked(char start, char end) const
    {
        for (int i = 0; i < blockedRoadCount; ++i) 
        {
            if (blockedRoads[i].isBlocked(start, end)) 
            {
                return true;
            }
        }
        return false;
    }

    void dijkstra(char source, char destination, bool vehicle) const
    {
        const int INF = 999999;  // Use a large number
        int dist[100];  // Distance array
        char prev[100];  // Array to store the previous node in the path
        bool visited[100] = {false};  // Visited nodes to avoid reprocessing

        // Initialize distance and previous arrays
        for (int i = 0; i < size; ++i) 
        {
            dist[i] = INF;
            prev[i] = '\0';
        }

        dist[source - 'A'] = 0;  // Distance to source is 0

        PriorityQueue pq(size);
        pq.insert(source, 0);  // Insert source with distance 0

        while (!pq.isEmpty()) 
        {
            PriorityQueue::QueueNode minNode = pq.extractMin();
            char current = minNode.vertex;

            if (visited[current - 'A']) 
            {
                continue;  // Skip if already visited
            }

            visited[current - 'A'] = true;  // Mark the current node as visited

            Node *node = &nodes[current - 'A'];

            for (Edge *edge = node->head; edge != nullptr; edge = edge->next) 
            {
                // Skip the edge if it's blocked
                if (isRoadBlocked(current, edge->destination)) 
                {
                    continue;
                }

                if (!visited[edge->destination - 'A']) 
                {
                    int newDist = dist[current - 'A'] + edge->weight;

                    if (newDist < dist[edge->destination - 'A']) 
                    {
                        dist[edge->destination - 'A'] = newDist;
                        prev[edge->destination - 'A'] = current;
                        pq.insert(edge->destination, newDist);
                    }
                }
            }
        }

        // Check if a valid path exists
        if (dist[destination - 'A'] == INF) 
        {
            cout << "No route found from " << source << " to " << destination << endl;
            return;  // No route found, exit the function
        }

        if (!vehicle)
        {
            // If a valid path exists, print the path
            cout << "Path from " << source << " to " << destination << ": ";
        }

        // To trace the path, we start from destination and move backwards using prev
        char path[100];
        int pathIndex = 0;

        for (char at = destination; at != '\0'; at = prev[at - 'A']) 
        {
            path[pathIndex++] = at;
        }

        // Print path in reverse order with arrows
        for (int i = pathIndex - 1; i >= 0; --i) 
        {
            cout << path[i];
            if (i > 0) 
            {
                cout << " -> ";
            }
        }

        cout << "\nTotal distance: " << dist[destination - 'A'] << endl;
    }

    void dfs(char current, char end, char currentPath[], int &currentIndex,
            char paths[][100], int pathWeights[], int &pathCount, int currentWeight,
            bool visited[]) 
    {
        visited[current - 'A'] = true;
        currentPath[currentIndex++] = current;

        if (current == end) 
        {
            // Save the path
            if (pathCount < 100) 
            {
                for (int i = 0; i < currentIndex; ++i) 
                {
                    paths[pathCount][i] = currentPath[i];
                }
                paths[pathCount][currentIndex] = '\0'; // Null-terminate
                pathWeights[pathCount] = currentWeight;
                pathCount++;
            }
        } 
        else 
        {
            // Visit all neighbors sorted by destination
            Node *nodePtr = findOrCreateNode(current);
            
            // Sort edges by destination before traversing them
            Edge* sortedHead = sortEdges(nodePtr->head);
            
            for (Edge *edge = sortedHead; edge != nullptr; edge = edge->next) 
            {
                if (!visited[edge->destination - 'A']) 
                {
                    dfs(edge->destination, end, currentPath, currentIndex, paths, pathWeights, pathCount, currentWeight + edge->weight, visited);
                }
            }
        }

        // Backtrack
        visited[current - 'A'] = false;
        currentIndex--;
    }

    void findAllPaths(char start, char end) 
    {
        const int MAX_PATHS = 100;  // Maximum number of paths to store
        const int MAX_NODES = 100;  // Maximum nodes in a graph

        char paths[MAX_PATHS][MAX_NODES];  // To store all paths
        int pathWeights[MAX_PATHS];         // To store weights of each path
        int pathCount = 0;                  // Number of paths found

        bool visited[26] = {false};
        char currentPath[100];
        int currentIndex = 0;

        dfs(start, end, currentPath, currentIndex, paths, pathWeights, pathCount, 0, visited);

        // Display all paths and their weights
        cout << "All possible paths from " << start << " to " << end << " are:\n";
        for (int i = 0; i < pathCount; ++i) 
        {
            cout << "Path: ";
            for (int j = 0; paths[i][j] != '\0'; ++j) 
            {
                cout << paths[i][j];
                if (paths[i][j + 1] != '\0') 
                {
                    cout << " -> ";
                }
            }
            cout << " | Total Weight: " << pathWeights[i] << endl;
        }
    }

    
};

// Struct for Vehicles
struct Vehicle 
{
    string vehicleID;
    char startIntersection;
    char endIntersection;
    char currentIntersection; // Where the vehicle is currently
    char route[100]; // Fixed-size array for the route
    int routeLength; // Number of elements in the route
};

// Struct for Intersections
struct Intersection 
{
    char intersectionID;  // ID for the intersection (A, B, C, etc.)
    int greenTime;        // Time for the green light (in seconds)
    int density;          // Traffic density at this intersection (number of vehicles)

    // Constructor to initialize the intersection
    Intersection(char id, int green, int dens = 0)
        : intersectionID(id), greenTime(green), density(dens) {}
};

// TrafficSignal Class
class TrafficSignal 
{
private:
    Intersection intersectionInfo;  // The Intersection data encapsulating ID, green time, and density

public:
    
    TrafficSignal() 
        : intersectionInfo(',', 0, 0) {}

    TrafficSignal(char id, int greenTime)
        : intersectionInfo(id, greenTime) {}

    // Getter for intersection ID
    char getIntersectionID() const 
    {
        return intersectionInfo.intersectionID;
    }

    // Getter for green time
    int getGreenTime() const 
    {
        return intersectionInfo.greenTime;
    }

    // Setter for green time
    void setGreenTime(int newGreenTime) 
    {
        intersectionInfo.greenTime = newGreenTime;
    }

    // Getter for density
    int getDensity() const 
    {
        return intersectionInfo.density;
    }

    // Setter for density
    void setDensity(int newDensity) 
    {
        intersectionInfo.density = newDensity;
    }

    // Method to increment green time based on density
    void incrementGreenTime(int additionalTime)
    {
        intersectionInfo.greenTime += additionalTime;
    }

    // Method to increment density
    void incrementDensity() 
    {
        intersectionInfo.density++;
    }

    // Print function for traffic signal info
    void displaySignalInfo() const 
    {
        cout << "Intersection " << intersectionInfo.intersectionID
             << ", Green Time: " << intersectionInfo.greenTime
             << endl;
    }
};

// MaxHeap Class for managing Traffic Signals based on density
class MaxHeap 
{
private:
    TrafficSignal* heapArray;
    int capacity;
    int size;

    void swap(TrafficSignal& a, TrafficSignal& b) 
    {
        TrafficSignal temp = a;
        a = b;
        b = temp;
    }

    void heapifyUp(int index) 
    {
        while (index > 0 && heapArray[index].getDensity() > heapArray[(index - 1) / 2].getDensity()) 
        {
            swap(heapArray[index], heapArray[(index - 1) / 2]);
            index = (index - 1) / 2;
        }
    }

    void heapifyDown(int index) 
    {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        int largest = index;

        if (leftChild < size && heapArray[leftChild].getDensity() > heapArray[largest].getDensity()) 
        {
            largest = leftChild;
        }
        if (rightChild < size && heapArray[rightChild].getDensity() > heapArray[largest].getDensity()) 
        {
            largest = rightChild;
        }
        if (largest != index) 
        {
            swap(heapArray[index], heapArray[largest]);
            heapifyDown(largest);
        }
    }

public:
    MaxHeap(int cap) 
    {
        capacity = cap;
        size = 0;
        heapArray = new TrafficSignal[capacity];
    }

    void insert(TrafficSignal signal) 
    {
        if (size < capacity) 
        {
            heapArray[size] = signal;
            heapifyUp(size);
            size++;
        }
    }

    TrafficSignal pop() 
    {
        if (size == 0) 
        {
            cout << "Heap is empty!" << endl;
            return TrafficSignal(' ', 0);  // Return empty signal if heap is empty
        }
        TrafficSignal top = heapArray[0];
        heapArray[0] = heapArray[size - 1];
        size--;
        heapifyDown(0);
        return top;
    }

    TrafficSignal peek() 
    {
        if (size > 0) 
        {
            return heapArray[0];
        }
        cout << "Heap is empty!" << endl;
        return TrafficSignal(' ', 0);  // Return empty signal if heap is empty
    }

    bool isEmpty() 
    {
        return size == 0;
    }

    int getSize() 
    {
        return size;
    }

    void print() 
    {
        if (size == 0) 
        {
            cout << "The heap is empty." << endl;
            return;
        }

        cout << "Heap contents: " << endl;
        for (int i = 0; i < size; i++) 
        {
            cout << "Intersection " << heapArray[i].getIntersectionID()
                << " with density " << heapArray[i].getDensity() << endl;
        }
    }
};

// TrafficSimulator Class to handle the simulation
class TrafficSimulator 
{
private:
    static const int MAX_VEHICLES = 100;
    static const int MAX_SIGNALS = 100;
    Vehicle vehicles[MAX_VEHICLES];
    TrafficSignal signals[MAX_SIGNALS];
    int vehicleCount;
    int signalCount;
    MaxHeap signalQueue;

public:
    TrafficSimulator() : vehicleCount(0), signalCount(0), signalQueue(MAX_SIGNALS) {}

    // Load vehicles from CSV file
    void loadVehiclesFromCSV(const char* filePath) 
    {
        ifstream file(filePath);
        if (!file.is_open()) 
        {
            cout << "Error: Could not open vehicles file " << filePath << endl;
            return;
        }

        string line;
        getline(file, line); // Skip header

        while (getline(file, line) && vehicleCount < MAX_VEHICLES) 
        {
            int index = 0;
            string vehicleID;
            
            // Extract vehicleID
            while (line[index] != ',') 
            {
                vehicleID += line[index++];
            }
            index++; // Skip the comma

            // Extract start and end intersections
            char startIntersection = line[index++];
            index++; // Skip the comma
            char endIntersection = line[index++];

            // Add the vehicle to the vehicles array
            vehicles[vehicleCount] = {vehicleID, startIntersection, endIntersection, startIntersection};
            vehicleCount++;

            // Update the density of the destination intersection
            // Update the density of the destination intersection
            for (int i = 0; i < signalCount; i++) 
            {
                if (signals[i].getIntersectionID() == endIntersection) 
                {
                    signals[i].incrementDensity();  // Increase the density of the destination intersection
                    cout << "Density increased at intersection " << endIntersection << endl;  // Debugging line
                    break; // Stop once the corresponding intersection is found
                }
            }

        }

        updateVehicleDensity();
    }

    // Load traffic signals from CSV file
    void loadTrafficSignalsFromCSV(const char* filePath) 
    {
        ifstream file(filePath);
        if (!file.is_open()) 
        {
            cout << "Error: Could not open traffic signals file " << filePath << endl;
            return;
        }

        string line;
        getline(file, line); // Skip header

        while (getline(file, line) && signalCount < MAX_SIGNALS)
        {
            int index = 0;
            char intersection;
            int greenTime = 0;

            intersection = line[index++];  // Get intersection character
            index++; // Skip the comma

            while (index < line.length() && line[index] >= '0' && line[index] <= '9') 
            {
                greenTime = greenTime * 10 + (line[index++] - '0');
            }

            // Using the constructor with two arguments
            signals[signalCount] = TrafficSignal(intersection, greenTime);

            signalCount++;
        }
    }

    // Initialize signals with intersections
    void initializeTrafficSignals() 
    {
        for (int i = 0; i < signalCount; i++) 
        {
            signals[i].setDensity(0);  // Initialize with zero density
        }
    }

    // Update vehicle density at the destination intersections
    void updateVehicleDensity() 
    {
        // Reset all densities to 0 before recalculating
        for (int i = 0; i < signalCount; i++) 
        {
            signals[i].setDensity(0);
        }

        for (int i = 0; i < vehicleCount; i++) 
        {
            char destinationIntersection = vehicles[i].endIntersection;
            
            for (int j = 0; j < signalCount; j++) 
            {
                if (signals[j].getIntersectionID() == destinationIntersection) 
                {
                    signals[j].incrementDensity();
                    break;
                }
            }
        }
    }

    // Manage traffic signals based on vehicle density and sort intersections by density
    void manageTrafficSignals() 
    {
        // Insert all signals into the heap based on density
        for (int i = 0; i < signalCount; i++) 
        {
            signalQueue.insert(signals[i]);
        }

        // Now, we pop the elements from the heap to get intersections in order of density
        cout << "\nIntersections Sorted by Density (High to Low):" << endl;

        // Extract and print intersections sorted by density
        while (!signalQueue.isEmpty()) 
        {
            TrafficSignal topSignal = signalQueue.pop();
            cout << "Intersection " << topSignal.getIntersectionID()
                << " with density " << topSignal.getDensity() << endl;

        }
    }

    // Method to add a vehicle
    void addVehicle() 
    {
        if (vehicleCount < MAX_VEHICLES) 
        {
            string vehicleID;
            char startIntersection, endIntersection;

            cout << "Enter vehicle ID: ";
            cin >> vehicleID;

            cout << "Enter start intersection: ";
            cin >> startIntersection;

            cout << "Enter end intersection: ";
            cin >> endIntersection;

            // Add the new vehicle
            vehicles[vehicleCount] = {vehicleID, startIntersection, endIntersection, startIntersection};
            vehicleCount++;

            // Update the density of the destination intersection
            for (int i = 0; i < signalCount; i++) 
            {
                if (signals[i].getIntersectionID() == endIntersection) 
                {
                    signals[i].incrementDensity();  // Increase the density
                    cout << "Vehicle added. Density increased at intersection " << endIntersection << endl;
                    break;
                }
            }

            // Update green time based on density after adding the vehicle
            updateGreenTimeAfterVehicleAddition();
        } 
        else 
        {
            cout << "Error: Maximum number of vehicles reached!" << endl;
        }
    }

    // Update green time after a new vehicle is added based on the density of intersections
    void updateGreenTimeAfterVehicleAddition() 
    {
        for (int i = 0; i < signalCount; i++) 
        {
            adjustGreenTime(signals[i]);  // Adjust green time based on updated density
        }
    }

    // Function to adjust the green time based on the density of the intersection
    void adjustGreenTime(TrafficSignal& signal) 
    {
        int density = signal.getDensity();


        int adjustmentFactor = 2;  // Adjust this factor as per your needs
        int newGreenTime = signal.getGreenTime() - (density * adjustmentFactor);

        // Ensure green time doesn't go negative
        if (newGreenTime < 0) 
        {
            newGreenTime = 0;
        }

        signal.setGreenTime(newGreenTime);  // Update the green time of the traffic signal

        cout << "Adjusted Green Time for Intersection " << signal.getIntersectionID()
            << ": " << newGreenTime << endl;
    }

    // Display vehicle information
    // Display vehicle information with route
    void displayVehiclesInfo(Graph& graph) 
    {
        cout << "Vehicle Information:" << endl;
        for (int i = 0; i < vehicleCount; i++) 
        {
            // Display basic vehicle information
            cout << vehicles[i].vehicleID << " : "
                << vehicles[i].startIntersection
                << " to " << vehicles[i].endIntersection
                << endl;

            // Call the dijkstra function to print the route for each vehicle
            cout << "Route: ";
            graph.dijkstra(vehicles[i].startIntersection, vehicles[i].endIntersection, true);
        }
    }

    // Display traffic signal information
    void displayTrafficSignalsInfo() 
    {
        cout << "\nTraffic Signals Information:" << endl;
        for (int i = 0; i < signalCount; i++) 
        {
            signals[i].displaySignalInfo();
        }
    }
};


#define MAX_INTERSECTIONS 30
#define INF INT_MAX

using namespace std;

class RoadNetwork {
private:
    struct RoadClosure {
        string intersection1;
        string intersection2;
        string status;
    };

    string intersections[MAX_INTERSECTIONS];
    int adjMatrix[MAX_INTERSECTIONS][MAX_INTERSECTIONS]; // Adjacency matrix
    int heuristic[MAX_INTERSECTIONS];                   // Heuristic values for A*
    RoadClosure roadClosures[MAX_INTERSECTIONS * MAX_INTERSECTIONS];
    int intersectionCount;
    int roadClosureCount;

    // Get the index of an intersection
    int getIntersectionIndex(const string& intersection) {
        for (int i = 0; i < intersectionCount; ++i) {
            if (intersections[i] == intersection) {
                return i;
            }
        }
        return -1;
    }

    // A* algorithm to find the shortest path
    void aStar(int start, int end, int parent[]) {
        int openSet[MAX_INTERSECTIONS];
        bool closedSet[MAX_INTERSECTIONS] = {false};
        int gScore[MAX_INTERSECTIONS]; // Cost from start to each intersection
        int fScore[MAX_INTERSECTIONS]; // Estimated cost (gScore + heuristic)

        // Initialize scores and openSet
        for (int i = 0; i < intersectionCount; ++i) {
            gScore[i] = INF;
            fScore[i] = INF;
            openSet[i] = 0;
            parent[i] = -1;
        }

        gScore[start] = 0;
        fScore[start] = heuristic[start];
        openSet[start] = 1;

        while (true) {
            // Find the node in the openSet with the smallest fScore
            int current = -1, lowestFScore = INF;
            for (int i = 0; i < intersectionCount; ++i) {
                if (openSet[i] && fScore[i] < lowestFScore) {
                    lowestFScore = fScore[i];
                    current = i;
                }
            }

            if (current == -1) {
                // No valid path
                return;
            }

            // If the destination is reached
            if (current == end) {
                return;
            }

            // Move current from openSet to closedSet
            openSet[current] = 0;
            closedSet[current] = true;

            // Check neighbors of the current node
            for (int neighbor = 0; neighbor < intersectionCount; ++neighbor) {
                if (adjMatrix[current][neighbor] == INF || closedSet[neighbor]) {
                    continue; // Skip if no connection or already visited
                }

                if (isRoadBlocked(intersections[current], intersections[neighbor])) {
                    continue; // Skip if the road is blocked
                }

                int tentativeGScore = gScore[current] + adjMatrix[current][neighbor];
                if (tentativeGScore < gScore[neighbor]) {
                    gScore[neighbor] = tentativeGScore;
                    fScore[neighbor] = gScore[neighbor] + heuristic[neighbor];
                    parent[neighbor] = current;
                    openSet[neighbor] = 1; // Add neighbor to openSet
                }
            }
        }
    }

    // Check if a road is blocked
    bool isRoadBlocked(const string& intersection1, const string& intersection2) {
        for (int i = 0; i < roadClosureCount; ++i) {
            if ((roadClosures[i].intersection1 == intersection1 && roadClosures[i].intersection2 == intersection2) ||
                (roadClosures[i].intersection1 == intersection2 && roadClosures[i].intersection2 == intersection1)) {
                if (roadClosures[i].status == "Blocked") {
                    return true;
                }
            }
        }
        return false;
    }

public:
    RoadNetwork() {
        intersectionCount = 0;
        roadClosureCount = 0;
        // Initialize adjacency matrix to INF
        for (int i = 0; i < MAX_INTERSECTIONS; ++i) {
            for (int j = 0; j < MAX_INTERSECTIONS; ++j) {
                adjMatrix[i][j] = INF;
            }
        }
    }

    void readRoadNetwork() {
        ifstream file("road_network.csv");
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string intersection1, intersection2;
            int time;
            getline(ss, intersection1, ',');
            getline(ss, intersection2, ',');
            ss >> time;

            int idx1 = getIntersectionIndex(intersection1);
            int idx2 = getIntersectionIndex(intersection2);

            if (idx1 == -1) {
                intersections[intersectionCount++] = intersection1;
                idx1 = intersectionCount - 1;
            }
            if (idx2 == -1) {
                intersections[intersectionCount++] = intersection2;
                idx2 = intersectionCount - 1;
            }

            adjMatrix[idx1][idx2] = time;
            adjMatrix[idx2][idx1] = time; // Assume undirected graph
        }
    }

    void readTrafficSignals() {
        ifstream file("traffic_signals.csv");
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string intersection;
            int greenTimeVal;
            getline(ss, intersection, ',');
            ss >> greenTimeVal;

            int idx = getIntersectionIndex(intersection);
            heuristic[idx] = greenTimeVal; // Use greenTime as heuristic for simplicity
        }
    }

    void readRoadClosures() {
        ifstream file("road_closures.csv");
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string intersection1, intersection2, status;
            getline(ss, intersection1, ',');
            getline(ss, intersection2, ',');
            getline(ss, status);

            roadClosures[roadClosureCount].intersection1 = intersection1;
            roadClosures[roadClosureCount].intersection2 = intersection2;
            roadClosures[roadClosureCount].status = status;
            roadClosureCount++;
        }
    }

void blockRoad() {
    string roadStart, roadEnd;
    cout << "Enter road to block (start, end): ";
    cin >> roadStart >> roadEnd;

    bool roadFound = false;

    // Mark this road as blocked
    for (int i = 0; i < roadClosureCount; ++i) {
        if ((roadClosures[i].intersection1 == roadStart && roadClosures[i].intersection2 == roadEnd) ||
            (roadClosures[i].intersection1 == roadEnd && roadClosures[i].intersection2 == roadStart)) {
            roadClosures[i].status = "Blocked";
            roadFound = true;
            cout << "Road " << roadStart << " to " << roadEnd << " is now blocked." << endl;
            break;
        }
    }

    // If the road doesn't exist yet in the closure list, add it
    if (!roadFound) {
        roadClosures[roadClosureCount].intersection1 = roadStart;
        roadClosures[roadClosureCount].intersection2 = roadEnd;
        roadClosures[roadClosureCount].status = "Blocked";
        roadClosureCount++;
        cout << "Road " << roadStart << " to " << roadEnd << " is now blocked." << endl;
    }

    // Update the CSV file
    ofstream outFile("road_closures.csv");
    if (!outFile.is_open()) {
        cerr << "Error: Unable to open road_closures.csv for writing." << endl;
        return;
    }

    for (int i = 0; i < roadClosureCount; ++i) {
        outFile << roadClosures[i].intersection1 << ","
                << roadClosures[i].intersection2 << ","
                << roadClosures[i].status << endl;
    }

    outFile.close();
    cout << "Road closures updated in road_closures.csv." << endl;
}


    void routeEmergencyVehicle() {
        string start, end;
        cout << "Enter start and end intersections for emergency vehicle: ";
        cin >> start >> end;

        int startIdx = getIntersectionIndex(start);
        int endIdx = getIntersectionIndex(end);

        if (startIdx == -1 || endIdx == -1) {
            cout << "Invalid intersection!" << endl;
            return;
        }

        int parent[MAX_INTERSECTIONS];
        cout << "Emergency Vehicle is being routed..." << endl;

        // Find the shortest path using A*
        aStar(startIdx, endIdx, parent);

        // Trace the path
        int path[MAX_INTERSECTIONS];
        int pathIndex = 0;
        int current = endIdx;
        while (current != -1) {
            path[pathIndex++] = current;
            current = parent[current];
        }

        // If no valid path was found
        if (pathIndex == 0 || parent[endIdx] == -1) {
            cout << "No valid path available due to road closures!" << endl;
            return;
        }

        // Print the emergency vehicle path
        cout << "Emergency Vehicle path: ";
        for (int i = pathIndex - 1; i >= 0; --i) {
            cout << intersections[path[i]];
            if (i > 0) cout << " -> ";
        }
        cout << endl;

        // Override traffic signals for emergency vehicle
        cout << "Override Traffic Signals for Emergency Vehicle" << endl;
        for (int i = pathIndex - 1; i >= 0; --i) {
            cout << "Intersection " << intersections[path[i]] << " overridden." << endl;
        }

        cout << "Emergency Vehicle routed successfully." << endl;
    }

    void displayBlockedRoads() {
        cout << "Blocked Roads:" << endl;
        for (int i = 0; i < roadClosureCount; ++i) {
            if (roadClosures[i].status == "Blocked") {
                cout << roadClosures[i].intersection1 << " to " << roadClosures[i].intersection2 << " is blocked." << endl;
            }
        }
    }
};
class SmartRouteCalculator {
private:
    // Constants inside the class
    static const int MAX_INTERSECTIO = 26;
    static const int MAX_ROADS = 2;

    // Structure definitions for Road and Junction within the class
    struct Road {
        string destination;
        int baseTravelTime;
        int vehicleCount;
        string roadName;
    };

    struct Junction {
        string name;
        Road roads[MAX_ROADS];
        int roadCount;
    };

    Junction junctions[MAX_INTERSECTIONS];
    int junctionCount;

    int getJunctionIndex(const string& name) {
        for (int i = 0; i < junctionCount; ++i) {
            if (junctions[i].name == name) return i;
        }
        return -1;
    }

    int getRoadIndex(int fromIdx, const string& to) {
        for (int i = 0; i < junctions[fromIdx].roadCount; ++i) {
            if (junctions[fromIdx].roads[i].destination == to) return i;
        }
        return -1;
    }

    // Helper function to calculate minimum distance (used for dynamic programming)
    int minDistance(int dist[], bool sptSet[]) {
        int min = INT_MAX, min_index;
        for (int v = 0; v < junctionCount; v++) {
            if (!sptSet[v] && dist[v] <= min) {
                min = dist[v];
                min_index = v;
            }
        }
        return min_index;
    }

    void dynamicProgrammingRoute(const string& start, const string& end) {
        int startIdx = getJunctionIndex(start);
        int endIdx = getJunctionIndex(end);

        if (startIdx == -1 || endIdx == -1) {
            cout << "Invalid junctions!" << endl;
            return;
        }

        int dist[MAX_INTERSECTIONS];
        bool sptSet[MAX_INTERSECTIONS];
        int parent[MAX_INTERSECTIONS]; // For route reconstruction

        for (int i = 0; i < junctionCount; i++) {
            dist[i] = INT_MAX;
            sptSet[i] = false;
        }

        dist[startIdx] = 0;
        parent[startIdx] = -1;

        for (int count = 0; count < junctionCount - 1; count++) {
            int u = minDistance(dist, sptSet);
            sptSet[u] = true;

            for (int v = 0; v < junctionCount; v++) {
                int roadIdx = getRoadIndex(u, junctions[v].name);
                if (!sptSet[v] && roadIdx != -1 && dist[u] != INT_MAX && dist[u] + junctions[u].roads[roadIdx].baseTravelTime < dist[v]) {
                    dist[v] = dist[u] + junctions[u].roads[roadIdx].baseTravelTime;
                    parent[v] = u;
                }
            }
        }

        // Display the shortest path
        cout << "Shortest path from " << start << " to " << end << ": " << endl;
        int current = endIdx;
        while (current != -1) {
            cout << junctions[current].name << " ";
            current = parent[current];
        }
        cout << endl;
        cout << "Total travel time: " << dist[endIdx] << " minutes" << endl;
    }

public:
    SmartRouteCalculator() : junctionCount(0) {
        srand(time(0));
    }

    void generateRandomJunctions() {
        for (char i = 'A'; i <= 'Z'; ++i) {
            junctions[junctionCount].name = string(1, i);
            junctions[junctionCount].roadCount = 0;
            ++junctionCount;
        }

        for (int i = 0; i < junctionCount; ++i) {
            for (int j = i + 1; j < junctionCount; ++j) {
                if (rand() % 2 == 0 && junctions[i].roadCount < MAX_ROADS && junctions[j].roadCount < MAX_ROADS) {
                    int travelTime = rand() % 10 + 5;
                    int vehicles = rand() % 50 + 1;
                    string roadName = junctions[i].name + "->" + junctions[j].name;

                    junctions[i].roads[junctions[i].roadCount] = {junctions[j].name, travelTime, vehicles, roadName};
                    junctions[i].roadCount++;

                    junctions[j].roads[junctions[j].roadCount] = {junctions[i].name, travelTime, vehicles, roadName};
                    junctions[j].roadCount++;
                }
            }
        }
    }

    void displayState() {
        for (int i = 0; i < junctionCount; ++i) {
            cout << "Junction " << junctions[i].name << ":\n";
            for (int j = 0; j < junctions[i].roadCount; ++j) {
                cout << "  -> " << junctions[i].roads[j].destination
                     << " | Base Travel Time: " << junctions[i].roads[j].baseTravelTime
                     << " | Vehicles: " << junctions[i].roads[j].vehicleCount << endl;
            }
        }
    }

    void moveVehicle() {
        string from, to;
        int vehicleCount;
        string timeOfDay;

        cout << "Enter starting junction: ";
        cin >> from;
        cout << "Enter destination junction: ";
        cin >> to;
        cout << "Enter number of vehicles to move: ";
        cin >> vehicleCount;
        cout << "Enter time of day (morning, midday, night): ";
        cin >> timeOfDay;

        int fromIdx = getJunctionIndex(from);
        int toIdx = getJunctionIndex(to);

        if (fromIdx == -1 || toIdx == -1) {
            cout << "Invalid junctions!" << endl;
            return;
        }

        int fromRoadIdx = getRoadIndex(fromIdx, to);
        int toRoadIdx = getRoadIndex(toIdx, from);

        if (fromRoadIdx == -1 || toRoadIdx == -1) {
            cout << "No valid road exists between these junctions!" << endl;
            return;
        }

        // Adjust vehicle count and travel time based on time of day
        int timeAdjustment = 0;
        if (timeOfDay == "morning") {
            timeAdjustment = 1;
        } else if (timeOfDay == "midday") {
            timeAdjustment = 2;
        } else if (timeOfDay == "night") {
            timeAdjustment = 3;
        } else {
            cout << "Invalid time of day!" << endl;
            return;
        }

        cout << "Before moving: \n";
        cout << "Vehicles on " << from << " -> " << to << ": " << junctions[fromIdx].roads[fromRoadIdx].vehicleCount
             << " | Base Travel Time: " << junctions[fromIdx].roads[fromRoadIdx].baseTravelTime << endl;

        junctions[fromIdx].roads[fromRoadIdx].vehicleCount -= vehicleCount;
        int newTravelTime = junctions[fromIdx].roads[fromRoadIdx].baseTravelTime - (vehicleCount / 10) - timeAdjustment;
        if (newTravelTime < 1) newTravelTime = 1;
        junctions[fromIdx].roads[fromRoadIdx].baseTravelTime = newTravelTime;

        junctions[toIdx].roads[toRoadIdx].vehicleCount += vehicleCount;
        newTravelTime = junctions[toIdx].roads[toRoadIdx].baseTravelTime + (vehicleCount / 10) + timeAdjustment;
        junctions[toIdx].roads[toRoadIdx].baseTravelTime = newTravelTime;

        cout << "After moving: \n";
        displayState();
    }

    void findBestRoute() {
        string start, end;
        cout << "Enter start junction: ";
        cin >> start;
        cout << "Enter destination junction: ";
        cin >> end;
        dynamicProgrammingRoute(start, end);
    }
};
int main() 
{
    // Initialize the graph for the city's road network
    Graph graph(100);  // Assume 100 intersections for this example
    graph.loadFromCSV("road_network.csv");  // Load the road network from a CSV file
    graph.loadBlockedRoads("road_closures.csv");  // Load blocked roads (closures)

    // Initialize the traffic simulator
    TrafficSimulator simulator;

    // Load vehicles and traffic signals from CSV files
    simulator.loadVehiclesFromCSV("vehicles.csv");
    simulator.loadTrafficSignalsFromCSV("traffic_signals.csv");

    Traffic traffic;
    
    traffic.loadData("vehicles.csv");

    RoadNetwork network;

    // Read data from CSV files
    network.readRoadNetwork();
    network.readTrafficSignals();
    network.readRoadClosures();


    int choice;
    do 
    {
        cout << "-------- Simulation Dashboard --------\n";
        cout << "1. Display City Traffic Network\n";
        cout << "2. Find Shortest Route\n";
        cout << "3. Find All Paths\n";
        cout << "4. Display all Vehicle Routes\n";
        cout << "5. Traffic Signals Display\n";
        cout << "6. Simulate Traffic\n";
        cout << "7. Add a Vehicle\n";
        cout << "8. Congestion Status:\n";
        cout << "9. Block road\n";
        cout << "10. Check blocked roads\n";
        cout << "11. Route emergency vehicle\n";
        cout<<"12. Smart route\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) 
        {
            // Display the city traffic network graph
            graph.displayGraph();

            cout << "Do you want to manage roads/intersections? (y/n): ";
            char manageChoice;
            cin >> manageChoice;
            if (manageChoice == 'y' || manageChoice == 'Y') 
            {
                graph.manageRoads();  // Manage roads and intersections (add/remove roads)
            }
            else if (manageChoice != 'y' && manageChoice != 'Y' && manageChoice != 'n' && manageChoice != 'N') 
            {
                cout << "Invalid choice.\n";
            }
        }
        else if (choice == 2) 
        {
            // Find the shortest path between two intersections
            char start, end;

            cout << "Enter starting intersection: ";
            cin >> start;
            cout << "Enter ending intersection: ";
            cin >> end;
            graph.loadBlockedRoads("road_closures.csv");

            graph.dijkstra(start, end, false);  // Use Dijkstra's algorithm to find the shortest path
        }
        else if (choice == 3) 
        {
            // Find all paths between two intersections
            char start, end;

            cout << "Enter starting intersection: ";
            cin >> start;
            cout << "Enter ending intersection: ";
            cin >> end;

            graph.findAllPaths(start, end);  // Find all paths between the two intersections
        }
        else if (choice == 4) 
        {
            graph.loadBlockedRoads("road_closures.csv");

            // Display vehicle information
            simulator.displayVehiclesInfo(graph);
        }
        else if (choice == 5) 
        {
            simulator.initializeTrafficSignals();
            simulator.displayTrafficSignalsInfo();

        }
        else if (choice == 6) 
        {
            // Update vehicle density for intersections
            simulator.updateVehicleDensity();

            // Manage traffic signals based on vehicle density
            simulator.manageTrafficSignals();
        }
        else if (choice == 7) 
        {
            // Add a new vehicle
            simulator.addVehicle();
        }
        else if (choice == 8)
        {
            traffic.showCongestion();

            int rerouteChoice;
            cout << "Do you want to reroute traffic using BFS or DFS?" << endl << "1 for BFS" << endl << "2 for DFS. ";
            cin >> rerouteChoice;

            if (rerouteChoice == 1)
            {
                traffic.bfs(0); // Starting BFS from node 'A'         
            }
            else if (rerouteChoice == 2)
            {
                bool visited[26] = {false};
                int parent[26] = {-1};
                traffic.dfs(0, visited, parent); 
               
                for (int i = 0; i < Traffic::MAX_NODES; i++)
                {
                    if (visited[i])
                    {
                        cout << "Node " << (char)(i + 'A') << " can be reached, rerouted path: ";
                        traffic.printPath(parent, i);
                        cout << endl;
                    }
                }
            }
            else
            {
                cout << "Invalid choice. Try again.\n";
            }
        }
        else if(choice==9)
        {
            network.blockRoad();
        }
        else if(choice==10){
            network.displayBlockedRoads();
        }
        else if(choice==11){
            network.routeEmergencyVehicle();
        }
        else if(choice==12)
        {
                SmartRouteCalculator calculator;
    calculator.generateRandomJunctions();
    calculator.displayState();
    calculator.moveVehicle();
    calculator.findBestRoute();
        }
        else if (choice != 0) 
        {
            cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 0);

    return 0;
}
