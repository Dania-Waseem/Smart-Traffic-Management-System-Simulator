# Smart Traffic Management System Simulator

## Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [System Modules](#system-modules)
    
    - [Simulation Dashboard](#1-simulation-dashboard)
    - [City Traffic Network](#2-city-traffic-network-graph-representation)
    - [Vehicle Routing System](#3-vehicle-routing-system)
    - [Traffic Signal Management](#4-traffic-signal-management)
    - [Congestion Monitoring](#5-congestion-monitoring)
    - [Emergency Vehicle Handling](#6-emergency-vehicle-handling)
    - [Accident and Road Closure Simulation](#7-accident-and-road-closure-simulation)
    - 
- [System Workflow](#system-workflow)
- [Data Structures and Algorithms](#data-structures-and-algorithms)
- [Implementation Details](#implementation-details)
- [How to Run](#how-to-run)
  
## Overview

The **Smart Traffic Management System Simulator** is a practical project designed to simulate real-world traffic management in a city. This system aims to optimize urban traffic flow using data structures like graphs, heaps, and priority queues. The focus is on real-time vehicle routing, traffic signal control, congestion management, and emergency handling.

## Key Features

- **City Traffic Network (Graph Representation)**: Visualizes intersections and roads as a weighted directed graph.
- **Vehicle Routing System**: Implements Dijkstra’s algorithm for dynamic route calculation.
- **Traffic Signal Management**: Controls traffic lights using priority queues.
- **Congestion Monitoring**: Monitors real-time congestion and reroutes traffic as needed.
- **Emergency Vehicle Handling**: Ensures special routing for emergency vehicles with priority.
- **Accident and Road Closure Simulation**: Simulates disruptions like accidents or road closures.
- **Simulation Dashboard**: Interactive interface to control and visualize the simulation.


## System Modules

### 1. Simulation Dashboard
- Offers an interactive interface to visualize and control the simulation.
- Displays traffic flow, congestion levels, and signal statuses.
- Allows manual addition/removal of vehicles.
<br>![image](https://github.com/user-attachments/assets/09f20705-ab25-4c30-88f2-b095e1839951)<br>

### 2. City Traffic Network (Graph Representation)
- Represents the city’s traffic infrastructure as a directed, weighted graph.
  <br>
![image](https://github.com/user-attachments/assets/16c06f03-c25c-46aa-8d78-cb7a348e1c4b)
<br>
- Supports dynamic addition or removal of roads and intersections.
<br>

![mod1image2](https://github.com/user-attachments/assets/75ae38e0-d7ff-4843-832e-bcae49aee87f)

<br>
- Visualizes the graph structure in a text-based or graphical form.

### 3. Vehicle Routing System
- Uses **Dijkstra's Algorithm** to calculate the shortest or fastest route for vehicles.
  <br>
  
![image](https://github.com/user-attachments/assets/49b13efb-ee6d-4d0a-9d6d-5ab9422221a4)


<br>
- Recalculates routes dynamically when traffic conditions change.
- Supports vehicle tracking to monitor their movement across the network.
<br>

![vehm2Image2](https://github.com/user-attachments/assets/250baaea-4d52-42f9-b2bd-8448760e42d1)

<br>

### 4. Traffic Signal Management
- Manages traffic signals at intersections to optimize traffic flow.
- Uses a priority queue to adjust traffic signal timings based on vehicle density.
- Includes an emergency override system for critical traffic situations.
  <br>
![image](https://github.com/user-attachments/assets/dfba1ed4-980e-4e49-8993-47b3e1d87184)

<br>

<br>

![traffic2](https://github.com/user-attachments/assets/26781cd6-fa95-4c18-8daa-d83dc0fff0d0)

<br>

![traffic3](https://github.com/user-attachments/assets/4e5ee36f-c4c0-4637-bcdd-b47152ba31d2)

<br>

### 5. Congestion Monitoring

- Monitors real-time vehicle counts on each road segment.
- Identifies congested roads and reroutes traffic using BFS or DFS.
<br>

![congestion1](https://github.com/user-attachments/assets/3d7e1775-daf1-4b8c-ad68-cb6dd942c7f3)

<br>

- Displays congestion levels for analysis.
<br>![image](https://github.com/user-attachments/assets/fa7dee6d-3501-4dc0-a66c-f9d289a7ee7e)<br>


### 6. Emergency Vehicle Handling
- Routes emergency vehicles to minimize delays, overriding normal traffic signal operations.
- Uses A* Search Algorithm for the fastest route.
- Restores normal traffic flow after the emergency vehicle passes.
<br>![image](https://github.com/user-attachments/assets/b9005bdf-a339-46ec-af0e-7ab762460362)<br>

### 7. Accident and Road Closure Simulation
- Simulates disruptions like road closures or accidents.
- Dynamically blocks specific roads or intersections.
- Recalculates affected routes and updates the traffic network.
<br>![image](https://github.com/user-attachments/assets/6a6f1773-41c1-4359-ab3e-2bcecfd82dae)<br>
<br>![image](https://github.com/user-attachments/assets/b43c4990-3e53-4637-a5b9-06c246427d4c)<br>



## System Workflow

1. **Initialization**:
    - Define the road network as a graph.
    - Assign initial weights to roads (travel times or congestion levels).
    
2. **Traffic Simulation**:
    - Add vehicles to the network with defined start and end points.
    - Calculate and assign routes using Dijkstra's algorithm.

3. **Signal and Flow Management**:
    - Monitor vehicle density at intersections.
    - Dynamically adjust traffic signals to prioritize heavily congested roads.

4. **Emergency Handling**:
    - Adjust traffic flow for emergency vehicles.
    - Re-optimize traffic flow after the emergency vehicle has passed.

5. **Congestion Resolution**:
    - Reroute vehicles from congested roads to alternative paths.

## Data Structures and Algorithms

- **Graph**: Adjacency lists represent the road network.
- **Priority Queue**: Used for managing road signals based on vehicle density.
- **Min-Heap**: Finds the road with the highest congestion.
- **Hash Table**: Tracks vehicle counts for each road.
- **Dijkstra’s Algorithm**: Calculates the shortest paths for vehicles.
- **A* Search Algorithm**: Handles emergency vehicle routing.
- **BFS/DFS**: Detects congestion or inaccessible paths.

## Implementation Details

- **Programming Language**: C++
- **Data Input**:
    - **Road Network**: `road-network.csv` (Defines the road network).
    - **Vehicles**: `vehicles.csv` (Defines vehicle routes).
    - **Traffic Signals**: `traffic-signals.csv` (Defines signal timings).
    - **Emergency Vehicles**: `emergency-vehicles.csv` (Defines emergency routes).
    - **Accidents/Closures**: `accidents.csv` (Simulates road disruptions).
  
- **Visualization**: Console-based interface displays:
    - Graph structure (intersections and roads)
    - Vehicle positions and routes
    - Congestion levels
    - Traffic signal statuses

## How to Run

1. **Download Required Files**:
    - Download the following files to your project folder:
      - `road-network.csv`
      - `vehicles.csv`
      - `traffic-signals.csv`
      - `emergency-vehicles.csv`
      - `accidents.csv`

2. **Install Dependencies**:
    - No external libraries required. Standard C++ compiler and IDE like **VS Code** or **Code::Blocks** will work.

3. **Run the Program**:
    - Open the project folder in your C++ IDE (VS Code recommended).
    - Build and run the project.
    - Follow the on-screen instructions to interact with the simulation.
