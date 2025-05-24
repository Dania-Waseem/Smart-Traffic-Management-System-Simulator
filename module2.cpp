#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include <cstring>
#include <cmath> 
// For heuristic calculations

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

        // Mark this road as blocked
        for (int i = 0; i < roadClosureCount; ++i) {
            if ((roadClosures[i].intersection1 == roadStart && roadClosures[i].intersection2 == roadEnd) ||
                (roadClosures[i].intersection1 == roadEnd && roadClosures[i].intersection2 == roadStart)) {
                roadClosures[i].status = "Blocked";
                cout << "Road " << roadStart << " to " << roadEnd << " is now blocked." << endl;
                return;
            }
        }

        // If the road doesn't exist yet in the closure list, add it
        roadClosures[roadClosureCount].intersection1 = roadStart;
        roadClosures[roadClosureCount].intersection2 = roadEnd;
        roadClosures[roadClosureCount].status = "Blocked";
        roadClosureCount++;
        cout << "Road " << roadStart << " to " << roadEnd << " is now blocked." << endl;
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

int main() {
    RoadNetwork network;

    // Read data from CSV files
    network.readRoadNetwork();
    network.readTrafficSignals();
    network.readRoadClosures();

    int choice;
    do {
        cout << "\nMenu:\n";
        cout << "1. Block road\n";
        cout << "2. Check blocked roads\n";
        cout << "3. Route emergency vehicle\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                network.blockRoad();
                break;
            case 2:
                network.displayBlockedRoads();
                break;
            case 3:
                network.routeEmergencyVehicle();
                break;
            case 4:
                cout << "Exiting the system.\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);

    return 0;
}
