/*
* File-Name: base.h
* Authors: Sara Conti and Elio Wanner
* Created on: 20. March 2021
* Last edited on: 22. May 2021
* Version: 4.3
* Changelog: finalised code for rendu3
*/

#ifndef BASE_H
#define BASE_H

#include <memory>
#include "geomod.h"
#include "gisement.h"
#include "robot.h"

struct BaseData
{
  unsigned uid;
  unsigned nbP;
  unsigned nbF;
  unsigned nbT;
  unsigned nbC;
  double ressource;
  double ressource_p;
};

class Base {
private:
	double ressources;
	bool state;
	circle CircleB;
	std::vector<std::shared_ptr<Robot>> robots;
	int baseIndex;
	std::vector<int> getNbRoboType();
	
	//Methods for strategies
	void createComGrid(int& nbNewRobots);
	void createRoboProsp(int& nbNewRobots);
	void changeRoboProspGoal(std::shared_ptr<Robot> robo);
	void createRoboForage(int& nbNewRobots);

public:
	//Constructor
	Base(std::string lineReading, int baseIndex);
	void destroy();
	//Getters
	point2D getCenter();
	circle getCircle();
	std::vector<std::shared_ptr<Robot>> getRobots();
	std::shared_ptr<Robot> getCenterRobot();
	
	void show();												//show in terminal
	void display(bool showComRay, bool showConnections);		//display graphically
	
	void addRobot(Robot* newRobot);
	
	bool checkOverlap(Gisement currentGisement);
	bool checkOverlap(Base currentBase);
	bool checkUid(Robot* otherRobot);
	bool checkCenterRobot();
	
	void doStep();
	void createRobots(point2D goal, std::string type);
	
	void save(std::ofstream& saveFile);
	
	BaseData getBaseData();
	void changeRes(double change);
};

#endif
