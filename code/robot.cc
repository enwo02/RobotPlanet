/*
* File-Name: robot.cc
* Authors: Elio Wanner and Sara Conti
* Created on: 2. April 2021
* Last edited on: 22. May 2021
* Version: 4.2
* Changelog: finalised code for rendu3
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "robot.h"
#include "geomod.h"
#include "utilities.h"
#include "message.h"
#include "constantes.h"
using namespace std;

//-----------General Robot---------------

//Default Constructor
Robot::Robot():uid(0), distanceTravelled(0), position({0,0}), goal({0,0}),achieved(0){}

//Standard Constructor
Robot::Robot(int _uid, double _distance, point2D _position, point2D _goal, 
			 point2D _home, bool _achieved)
	:uid(_uid), distanceTravelled(_distance), position(_position), goal(_goal), 
	 home(_home), achieved(_achieved){}

//Constructor to create from string (initializing)
Robot::Robot(string lineReading){
	vector<double> input;
	input = decodeString(lineReading);
	uid = input[0];
	distanceTravelled = input[1];
	position = {input[2],input[3]};
	goal = {input[4],input[5]};
	achieved = input[6];
}

void Robot::show(){
	cout << "uid: " << uid << " " << "dp: " << distanceTravelled << " ";
	cout << "pos: " << "(" << position.x << ","<< position.y << ")" << " ";
	cout << "goal: " <<"(" << goal.x << ","<< goal.y << ")" << " ";
	cout << "achieved: " << achieved << endl;
}

void Robot::showConnectedRobots(){
	cout << "I AM CONNECTED TO "<<connectedRobots.size()<<" ROBOTS-----------" << endl;
	for(auto robo : connectedRobots){
		robo->show();
	}
	cout << "FINISHED SHOWING CONNECTIONS------------------------------------" << endl;
}

void Robot::display(bool showComRay, bool showConnections, int color){
	drawPoint(position.x, position.y, color);

	//Display communication ray
	if(showComRay){
		drawCircle(position.x, position.y, rayon_comm, false, -3);
	}
	//Display lines between robots
	if(showConnections){
		for(auto neighbour : neighbourRobots){
			double neighbourX( neighbour->getPosition().x );
			double neighbourY( neighbour->getPosition().y );
			drawLine(position.x, position.y, neighbourX, neighbourY, -2);
		}
	}
}

point2D Robot::getPosition(){
	return position;
}

point2D Robot::getGoal(){
	return goal;
}

bool Robot::getAchieved(){
	return achieved;
}

void Robot::setGoal(point2D newGoal){
	goal = newGoal;
	achieved = false;
}

void Robot::setAchieved(bool newAchieved){
	achieved = newAchieved;
}

void Robot::setRemote(bool newRemote){
	remote = newRemote;
}

void Robot::setHome(point2D newHome){
	home = newHome;
}

void Robot::fuelUp(){
	distanceTravelled = 0;
}

int Robot::getUid(){
	return uid;
}

bool Robot::isRemote(){
	return remote;
}

string Robot::getType(){
	return type;
}

void Robot::doStep(){
	point2D departurePoint, arrivalPoint, travelDirection;
	departurePoint = position;
	arrivalPoint = goal;
	
	double leftOverFuel(0);
	if(type == "prosp"){leftOverFuel = maxD_prosp - distanceTravelled;}
	if(type == "forage"){leftOverFuel = maxD_forage - distanceTravelled;}
	if(type == "transp"){leftOverFuel = maxD_transp - distanceTravelled;}
	if(type == "com"){leftOverFuel = maxD_com - distanceTravelled;}

	double dist;
	dist = distance(departurePoint, arrivalPoint, travelDirection);
	if(dist >= deltaD and leftOverFuel >= deltaD){
		double tDX(travelDirection.x);
		double tDY(travelDirection.y);
		if(tDX > 0){
			position.x += deltaD/sqrt(1 + (tDY * tDY) / (tDX * tDX) );
		} else{
			position.x -= deltaD/sqrt(1 + (tDY * tDY) / (tDX * tDX) );
		}
		if(tDY > 0){
			position.y += deltaD/sqrt(1 + (tDX * tDX) / (tDY * tDY) );
		} else{
			position.y -= deltaD/sqrt(1 + (tDX * tDX) / (tDY * tDY) );
		}
		position = normalise(position);
		distanceTravelled += deltaD;
	}
	//If the robot is nearer than deltaD he goes precisely to his goal
	if(dist < deltaD and leftOverFuel >= dist){
		position = goal;
		distanceTravelled += dist;
		achieved = true;
	}
	if(isEqual(position, goal)){
		achieved = true;
	} else{
		achieved = false;
	}
}

void Robot::addNeighbour(shared_ptr<Robot> nearbyRobot){
	neighbourRobots.push_back(nearbyRobot);
}

void Robot::addConnected(std::shared_ptr<Robot> newConnectedRobot){
	connectedRobots.push_back(newConnectedRobot);
}

vector<shared_ptr<Robot>> Robot::getNeighbourRobots(){
	return neighbourRobots;
}

vector<shared_ptr<Robot>> Robot::getConnectedRobots(){
	return connectedRobots;
}

void Robot::resetNeighbours(){
	neighbourRobots.clear();
}

void Robot::resetConnectedRobots(){
	connectedRobots.clear();
}

bool Robot::isConnected(std::shared_ptr<Robot> otherRobo){
	for(unsigned int i(0); i < connectedRobots.size(); ++i){
		if(connectedRobots[i] == otherRobo){
			return true;
		}
	}
	return false;
}

void Robot::save(ofstream& saveFile){
	saveFile << "   " << uid << " " << distanceTravelled << " ";
	saveFile << position.x << " " << position.y << " ";
	saveFile << goal.x << " " << goal.y << " ";
	saveFile << achieved << " ";
}

//-----------Prospection Robot---------------

//Default Constructor
RoboProsp::RoboProsp()
	:Robot(){type = "prosp";}

//Standard Constructor
RoboProsp::RoboProsp(int _uid, double _distance, point2D _position, point2D _goal,
					 point2D _home, bool _achieved, bool _found)
	:Robot(_uid, _distance, _position, _goal, _home, _achieved), found(_found), 
		   retour(false){type = "prosp";}

//Constructor to create from string (initializing)
RoboProsp::RoboProsp(string lineReading)
	:Robot(lineReading){
	type = "prosp";
	vector<double> input;
	input = decodeString(lineReading);
	//If there also is a gisement found
	if(input.size() == 13){
		retour = input[7];
		found = input[8];
		gisement = {input[9],input[10],input[11]};
		capacityG = input[12];
	}
}

void RoboProsp::show(){
	cout << "P:\t";
	Robot::show();
	cout << "\tReturn: " << retour << " Found: " << found;
	if(found == true){
		cout << " Gisement: (" << gisement.x << "," << gisement.y << ") gRay: ";
		cout << gisement.ray << " gCapacity: " << capacityG << endl;
	} else{
		cout << endl;
	}
	
}

void RoboProsp::save(ofstream& saveFile){
	Robot::save(saveFile);
	saveFile << retour << " " << found << " ";
	if(found){
		saveFile << gisement.x << " " << gisement.y << " " << gisement.ray << " ";
		saveFile << capacityG;
	}
	saveFile << "\t#P"<< endl;
}

void RoboProsp::doStep(){
	bool beforeAchieved(achieved);
	Robot::doStep();
	//check if achieved status has changed
	//if it changed we should let the robot rest for 1 turn to let him look for a
	//gisement
	if(beforeAchieved == achieved and achieved){
		goal = home;
		achieved = false;
		retour = false;
		found = false;
	}
}

void RoboProsp::newFoundGisement(circle location, double capacity){
	retour = true;
	found = true;
	gisement = location;
	capacityG = capacity;
	goal = home;		//go home
}

bool RoboProsp::isGisementFound(){
	return found;
}

circle RoboProsp::getFoundGisement(){
	return gisement;
}

double RoboProsp::getFoundGisementRes(){
	return capacityG;
}

double RoboProsp::getFuelLeft(){
	return maxD_prosp - distanceTravelled;
}

//-----------Forage Robot---------------

//Default Constructor
RoboForage::RoboForage()
	:Robot(), full(false) {type = "forage";}

//Standard Constructor
RoboForage::RoboForage(int _uid, double _distance, point2D _position, point2D _goal,
					   point2D _home, bool _achieved) 
	:Robot(_uid, _distance, _position, _goal, _home, _achieved), full(false){
	type = "forage";
}

//Constructor to create from string (initializing)
RoboForage::RoboForage(string lineReading)
	:Robot(lineReading), full(false){type = "forage";}

void RoboForage::show(){
	cout << "F:\t";
	Robot::show();
	if(full == true){
		cout << "Full: true" << endl;
	} else{
		cout << "Full: false" << endl;
	}
}

void RoboForage::save(ofstream& saveFile){
	Robot::save(saveFile);
	saveFile << "\t#F" << endl;
}

void RoboForage::doStep(){
	Robot::doStep();
}

bool RoboForage::doForage(){
	if(achieved and not(full)){
		for(auto robo : neighbourRobots){
			if(robo->getType() == "transp" and robo->isEmpty()
			   and isEqual(robo->getPosition(), position)){
				//otherRobo is a roboTransp AND this roboTransp is empty 
				//AND this roboTransp is at the same position
				robo->exchangeRessource(true);
				return true;
			}
		}
	}
	return false;
}

void RoboForage::setGoalGisement(circle newGoalGisement){
	goalGisement = newGoalGisement;
}

circle RoboForage::getGoalGisement(){
	return goalGisement;
}

//-----------Transport Robot---------------

//Default Constructor
RoboTransp::RoboTransp()
	:Robot() {type = "transp";}

//Standard Constructor
RoboTransp::RoboTransp(int _uid, double _distance, point2D _position, point2D _goal,
					   point2D _home, bool _achieved, bool _retour)
	:Robot(_uid, _distance, _position, _goal, _home, _achieved), retour(_retour),
		   initialGoal(_goal) {type = "transp";}

//Constructor to create from string (initializing)
RoboTransp::RoboTransp(string lineReading)
	:Robot(lineReading) {type = "transp";}

void RoboTransp::show(){
	cout << "T:\t";
	Robot::show();
	cout << "\tReturn: "<< retour << endl;
}

void RoboTransp::save(ofstream& saveFile){
	Robot::save(saveFile);
	saveFile << retour << "\t#T"<< endl;
}

void RoboTransp::exchangeRessource(bool receiving){//true if loading, false if emptying
	if(receiving){
		retour = true;
		goal = home;
		home = position;
	} else{
		retour = false;
		goal = initialGoal;
		home = position;
	}
}
bool RoboTransp::isEmpty(){
	return not(retour);
}

double RoboTransp::getFuelLeft(){
	return maxD_transp - distanceTravelled;
}

//-----------Communication Robot---------------

//Default Constructor
RoboCom::RoboCom()
	:Robot() {type = "com";}

//Standard Constructor
RoboCom::RoboCom(int _uid, double _distance, point2D _position, point2D _goal,
				 point2D _home, bool _achieved)
	:Robot(_uid, _distance, _position, _goal, _home, _achieved) {type = "com";}

//Constructor to create from string (initializing)
RoboCom::RoboCom(string lineReading)
	:Robot(lineReading) {type = "com";}

void RoboCom::show(){
	cout << "C:\t";
	Robot::show();
}

void RoboCom::save(ofstream& saveFile){
	Robot::save(saveFile);
	saveFile << "\t#C"<< endl;
}
