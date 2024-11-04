# Robot Exploration Simulation

This project simulates the deployment, exploration, and resource gathering by autonomous robots. The robots are tasked with mapping an area, identifying resources, and establishing a communication network.
![Simulation Screenshot](example_1.png)

## Project Structure

- **main.cc**: Entry point for the simulation.
- **base.cc**: Handles the base station logic, including robot management and communication.
- **geomod.cc**: Geometric operations and calculations.
- **gisement.cc**: Resource management and extraction logic.
- **graphic.cc**: Handles visualization of the robot movements and interactions.
- **gui.cc**: Graphical User Interface for interaction with the simulation.
- **message.cc**: Manages inter-robot communication and message handling.
- **robot.cc**: Logic for individual robot actions and states.
- **simulation.cc**: Core simulation loop and execution logic.
- **utilities.cc**: Helper functions and utilities.

## Key Features

- **Robot Deployment**: Robots are deployed in orthogonal directions (N, E, S, W) from the base. Communication robots support prospecting robots by extending their range.
- **Resource Gathering**: Prospecting robots identify resource deposits, triggering the creation of specialized robots for extraction and transport.
- **Communication Network**: Communication robots establish a network to keep prospecting robots connected to the base.
- **Autonomy and Maintenance**: Robots perform autonomous actions with minimal base interaction. Maintenance routines ensure longevity and resource optimization.
- **Robustness**: Adaptive strategies allow robots to explore diverse directions to maximize coverage and efficiency.

## Compilation and Execution

To compile the project, use the provided `makefile`:

```bash
make
```

Run the simulation with:

```bash
./projet test_scenario.txt
```

## Authors

- Elio Wanner
- Sara Conti
