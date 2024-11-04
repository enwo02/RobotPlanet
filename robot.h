/*
* File-Name: robot.h
* Authors: Sara Conti and Elio Wanner
* Created on: 20. March 2021
* Last edited on: 22. May 2021
* Version: 4.3
* Changelog: finalised code for rendu3
*/

#ifndef ROBOT_H
#define ROBOT_H

#include <vector>
#include <memory>
#include "geomod.h"
#include "gisement.h"

//-----------General Robot---------------
class Robot{
protected:
	int uid;
	double distanceTravelled;
	point2D position;
	point2D goal;
	point2D home;	//coordinates of home base
	bool achieved;
	std::vector<std::shared_ptr<Robot>> neighbourRobots;
	std::vector<std::shared_ptr<Robot>> connectedRobots;
	std::string type;
	bool remote;

public:
	//Default Constructor
	Robot();
	//Standard Constructor
	Robot(int _uid, double _distance, point2D _position, point2D _goal, 
		  point2D _home, bool _achieved);
	//Constructor to create from string (initializing)
	Robot(std::string lineReading);
	//Destructor
	virtual ~Robot(){}
	
	virtual void show();
	void showConnectedRobots();
	void display(bool showComRay, bool showConnections, int color);
	point2D getPosition();
	point2D getGoal();
	bool getAchieved();
	void setGoal(point2D newGoal);
	void setAchieved(bool newAchieved);
	void setRemote(bool newRemote);
	void setHome(point2D newHome);
	void fuelUp();
	std::string getType();
	int getUid();
	bool isRemote();
	virtual void doStep();
	void addNeighbour(std::shared_ptr<Robot> nearbyRobot);
	void addConnected(std::shared_ptr<Robot> newConnectedRobot);
	std::vector<std::shared_ptr<Robot>> getNeighbourRobots();
	std::vector<std::shared_ptr<Robot>> getConnectedRobots();
	void resetNeighbours();
	void resetConnectedRobots();
	bool isConnected(std::shared_ptr<Robot> otherRobo);
	virtual void save(std::ofstream& saveFile);
	
	//methods which are only used for specific robots
	virtual void newFoundGisement(circle location, double capacity){}
	virtual bool isGisementFound(){return false;}
	virtual circle getFoundGisement(){return {0,0,0};}
	virtual double getFoundGisementRes(){return 0;}
	virtual void exchangeRessource(bool receiving){}
	virtual bool isEmpty(){return false;}
	virtual bool doForage(){return false;}
	virtual double getFuelLeft(){return 0;}
	virtual void setGoalGisement(circle newGoalGisement){}
	virtual circle getGoalGisement(){return {0,0,0};}
};

//-----------Prospection Robot---------------
class RoboProsp : public Robot{
private:
	bool found;
	bool retour;
	circle gisement;
	double capacityG;
public:
	//Default Constructor
	RoboProsp();
	//Standard Constructor
	RoboProsp(int _uid, double _distance, point2D _position, point2D _goal, 
			  point2D _home, bool _achieved, bool _found);
	//Constructor to create from string (initializing)
	RoboProsp(std::string lineReading);
	//Destructor
	virtual ~RoboProsp() override {}
	
	virtual void show() override;
	virtual void save(std::ofstream& saveFile) override;
	virtual void doStep() override;

	virtual void newFoundGisement(circle location, double capacity) override;
	virtual bool isGisementFound() override;
	virtual circle getFoundGisement() override;
	virtual double getFoundGisementRes() override;
	virtual double getFuelLeft() override;
};

//-----------Forage Robot---------------
class RoboForage : public Robot{
private:
	bool full;
	circle goalGisement;
public:
	//Default Constructor
	RoboForage();
	//Standard Constructor
	RoboForage(int _uid, double _distance, point2D _position, point2D _goal,
			   point2D _home, bool _achieved);
	//Constructor to create from string (initializing)
	RoboForage(std::string lineReading);
	//Destructor
	virtual ~RoboForage() override {}
	
	virtual void show() override;
	virtual void save(std::ofstream& saveFile) override;
	virtual void doStep() override;
	virtual bool doForage() override;
	virtual void setGoalGisement(circle newGoalGisement) override;
	virtual circle getGoalGisement() override;
};

//-----------Transport Robot---------------
class RoboTransp : public Robot {
private:
	bool retour;
	point2D initialGoal;
public:
	//Default Constructor
	RoboTransp();
	//Standard Constructor
	RoboTransp(int _uid, double _distance, point2D _position, point2D _goal,
			   point2D _home, bool _achieved, bool _retour);
	//Constructor to create from string (initializing)
	RoboTransp(std::string lineReading);
	//Destructor
	virtual ~RoboTransp() override {}

	virtual void show() override;
	virtual void save(std::ofstream& saveFile) override;
	virtual void exchangeRessource(bool receiving) override;
	virtual bool isEmpty() override;
	virtual double getFuelLeft() override;
};

//-----------Communication Robot---------------
class RoboCom : public Robot{
private:
	std::vector<circle> gisement;		//nearby gisements
	std::vector<double> capacityG;		// -->their capacity

public:
	//Default Constructor
	RoboCom();
	//Standard Constructor
	RoboCom(int _uid, double _distance, point2D _position, point2D _goal,
			point2D _home, bool _achieved);
	//Constructor to create from string (initializing)
	RoboCom(std::string lineReading);
	//Destructor
	virtual ~RoboCom() override {}

	virtual void show() override;
	virtual void save(std::ofstream& saveFile) override;
};

#endif
