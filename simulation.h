/*
* File-Name: simulation.h
* Authors: Sara Conti and Elio Wanner
* Created on: 20. March 2021
* Last edited on: 22. May 2021
* Version: 3.4
* Changelog: finalised code for rendu3
*/

#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include "base.h"
#include "gisement.h"

class Simulation{
private:
	std::vector<Gisement> gisements;
	std::vector<Base> bases;

	//Methods used in the Constructor
	void destroy();										//deletes content
	std::vector<std::string> readFile(std::string fileName);
	void initGisements(std::vector<std::string> readings);
	void initBases(std::vector<std::string> readings);

	std::vector<std::shared_ptr<Robot>> getAllRobots();
	bool checkOverlapping();
	void checkRobotOnGis();
	void exchangeRessource();
	void updateConnectionsOfRobot(std::vector<std::shared_ptr<Robot>> allRobots, 
								  std::vector<bool> &visited,int current, int initial);
public:
	//Default Constructor
	Simulation(){};
	//Constructor with file
	Simulation(std::string fileName);

	void init(std::string fileName);					//initialises from file
	void save(std::string fileName);

	void show();										//show in terminal
	void display(bool showComRay, bool showConnections);//display graphically

	void doStep();
	void updateNb();
	
	void updateConnections();

	std::vector<BaseData> getSimData();

};

#endif
