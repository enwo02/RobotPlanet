/*
* File-Name: base.cc
* Authors: Elio Wanner and Sara Conti
* Created on: 2. April 2021
* Last edited on: 22. May 2021
* Version: 4.0
* Changelog: finalised code for rendu3
*/

#include <iostream>
#include <fstream>
#include <vector>
#include "base.h"
#include "geomod.h"
#include "utilities.h"
#include "message.h"
#include "robot.h"
#include "constantes.h"
#include "gisement.h"
#include "settings.h"
using namespace std;

Base::Base(string lineReading, int baseIndex):state(false), baseIndex(baseIndex){
	//Standard state for a base is non-auto = 0
	vector<double> input;
	input = decodeString(lineReading);

	CircleB.x = input[0];
	CircleB.y = input[1];
	CircleB.ray = rayon_base;
	ressources = input[2];
}

void Base::destroy(){
	ressources = 0;
	robots.clear();
}

vector<int> Base::getNbRoboType(){
	int nbP(0), nbF(0), nbT(0), nbC(0);
	for(auto robo : robots){
		if(robo->getType() == "prosp"){ ++nbP;}
		if(robo->getType() == "forage"){ ++nbF;}
		if(robo->getType() == "transp"){ ++nbT;}
		if(robo->getType() == "com"){ ++nbC;}
	}
	return {nbP, nbF, nbT, nbC};
}

point2D Base::getCenter(){
	return {CircleB.x, CircleB.y};
}

circle Base::getCircle(){
	return CircleB;
}

vector<shared_ptr<Robot>> Base::getRobots(){
	return robots;
}

std::shared_ptr<Robot> Base::getCenterRobot(){
	shared_ptr<Robot> centerRobot;
	for(unsigned int k(0); k < robots.size(); ++k){
		if(isEqual(robots[k]->getPosition(), robots[k]->getGoal()) &&
		   robots[k]->getGoal().x == CircleB.x &&
		   robots[k]->getGoal().y == CircleB.y){
			centerRobot = robots[k];
		}
	}
	return centerRobot;
}

void Base::show(){
	cout << "Base: res: " << ressources << " state: ";
	if(state){
		cout << "self-sustaining";
	}
	else{
		cout << "dependant";
	}
	cout << endl << "Robots of Base: "<< endl;
	for(auto robo : robots){
		robo->show();
	}
}

void Base::display(bool showComRay, bool showConnections){
	for(auto robo : robots){
		robo->display(showComRay, showConnections, baseIndex);
	}
	drawCircle(CircleB.x, CircleB.y, displayRadiusBase, false, baseIndex);
}

void Base::addRobot(Robot* newRobot){
	if(newRobot != nullptr){
		checkUid(newRobot);
		robots.push_back(shared_ptr<Robot>(newRobot));
	}
}

bool Base::checkOverlap(Gisement currentGisement){
	if(isIntersecting(CircleB, currentGisement.getCircle() )){
		double x_base(CircleB.x);
		double y_base(CircleB.y);
		double x_field(currentGisement.getCircle().x);
		double y_field(currentGisement.getCircle().y);
		cout<< message::base_field_superposition(x_base, y_base, x_field, y_field);
		return false;
	}
	return true;
}

bool Base::checkOverlap(Base currentBase){
	if(isIntersecting(currentBase.getCircle(), CircleB)){
		double x1(CircleB.x);
		double y1(CircleB.y);
		double x2(currentBase.getCircle().x);
		double y2(currentBase.getCircle().y);
		cout<< message::base_superposition(x1, y1, x2, y2);
		return false;
	}
	return true;
}

bool Base::checkUid(Robot* otherRobot){
	int otherUid( otherRobot->getUid() );
	for(auto const& robo : robots){
		if(robo->getUid() == otherUid){
			cout << message::identical_robot_uid(otherUid);
			return false;
		}
	}
	return true;
}

bool Base::checkCenterRobot(){
//checks for the existance of a Communication Robot at the center of a base
	bool found(false);
	for(unsigned int i(0); i < robots.size(); ++i){
		if(isEqual(robots[i]->getPosition(), robots[i]->getGoal()) &&
		   robots[i]->getGoal().x == CircleB.x &&
		   robots[i]->getGoal().y == CircleB.y){
			found = true;
		}
	}
	if(not(found)){
		cout << message::missing_robot_communication(CircleB.x, CircleB.y);
		return false;
	}
	return true;
}

void Base::doStep(){
	if(ressources/finR >= 1){
		ressources = finR;
		state = true;
	} else if(ressources <= 0){
		this->destroy();
	} else{
		
		//To not create more than max_robot(3) robots per step
		int nbNewRobots(0);

		//When to create Forage
		createRoboForage(nbNewRobots);

		//When to create Propspection
		createRoboProsp(nbNewRobots);

		//When to create communication grid
		createComGrid(nbNewRobots);
		
		for(auto robo : robots){
			//change goal of roboProsp if he is connected and returning home
			if(robo->getType() == "prosp" and isEqual(robo->getGoal(), getCenter())
			   and robo->isRemote() and not(robo->getAchieved()) ){
				changeRoboProspGoal(robo);
			}
			robo->doStep();
		}

	}
}

void Base::save(ofstream& saveFile){
	saveFile << CircleB.x << " " << CircleB.y << " " << ressources << " ";
	vector<int> nbOfRobots;
	nbOfRobots = this->getNbRoboType();

	saveFile << nbOfRobots[0] << " " << nbOfRobots[1] << " ";
	saveFile << nbOfRobots[2] << " " << nbOfRobots[3] << endl;
	for(int i(0); i < 4; ++i){
		for(auto robo : robots){
			if(i == 0 and robo->getType() == "prosp"){
				robo->save(saveFile);
			}
			if(i == 1 and robo->getType() == "forage"){
				robo->save(saveFile);
			}
			if(i == 2 and robo->getType() == "transp"){
				robo->save(saveFile);
			}
			if(i == 3 and robo->getType() == "com"){
				robo->save(saveFile);
			}
		}
	}
	
}

void Base::createRobots(point2D goal, string type){
	point2D center{CircleB.x, CircleB.y};

	//find an unused uid
	int uid(-1);
	int newUid(0);
	do{
		bool alreadyExists(false);
		for(auto robo : robots){
			if(robo->getUid() == newUid){alreadyExists = true;}
		}
		if(not(alreadyExists)){uid = newUid;}
		++newUid;
	} while (uid < 0);

	//actually create robot
	if(type == "prosp"){
		addRobot(new RoboProsp(uid, 0, center , goal, center, 0, 0) );
		ressources -= cost_prosp;
	}
	if(type == "forage"){
		addRobot(new RoboForage(uid, 0, center, goal, center, 0) );
		ressources -= cost_forage;
	}
	if(type == "transp"){ 
		addRobot(new RoboTransp(uid, 0, center , goal, center, 0, 0) );
		ressources -= cost_transp;
	}
	if(type == "com"){ 
		addRobot(new RoboCom(uid, 0, center , goal, center, 0) );
		ressources -= cost_com;
	}

}

BaseData Base::getBaseData(){
	BaseData data;
	data.uid = baseIndex;
	vector<int> nbOfRobots;
	nbOfRobots = this->getNbRoboType();

	data.nbP = nbOfRobots[0];
	data.nbF = nbOfRobots[1];
	data.nbT = nbOfRobots[2];
	data.nbC = nbOfRobots[3];
	data.ressource = ressources;
	data.ressource_p = 100*ressources/finR;

	return data;
}

//Methods for strategy (these are all private)

void Base::createComGrid(int& nbNewRobots){
	if(ressources/finR >= comGridLimit/100){
		point2D center{CircleB.x, CircleB.y};

		double safetyDistance(comGridDensity);
		int nbInOneDirection;
		//the +1 ensures that everything is covered
		nbInOneDirection = 2 * getMax() / (rayon_comm-safetyDistance) + 1;

		if(ressources >= (max_robots-nbNewRobots) * cost_com){
			point2D goal(center);
			for(int i(0); i < nbInOneDirection; ++i){
				for(int k(0); k < nbInOneDirection; ++k){
					bool foundRoboWithSameGoal(false);
					goal = normalise(goal);
					//check if there is already a rC with the same goal
					for(unsigned int m(0); m < robots.size(); ++m){
						if(robots[m]->getGoal().x == goal.x 
						   and robots[m]->getGoal().y == goal.y){
							foundRoboWithSameGoal = true;
						}
					}
					if(not(foundRoboWithSameGoal) and nbNewRobots < max_robots) {
						this->createRobots(goal, "com");
						++nbNewRobots;
					}
					goal.x += rayon_comm-safetyDistance;
				}
				goal.y += rayon_comm-safetyDistance;
				goal.x = center.x;
			}
		}
	}
}

void Base::createRoboProsp(int& nbNewRobots){
	int nbRoboProsp;
	nbRoboProsp = this->getNbRoboType()[0];

	if(ressources >= cost_prosp+2*cost_com and nbNewRobots < max_robots 
	   and nbRoboProsp < maxProsp){
		point2D goal({CircleB.x, CircleB.y});
		point2D goalCom1(goal);
		point2D goalCom2(goal);
		if(nbRoboProsp == 0){
			goal.y += 3*rayon_comm;	//go up
			goalCom1.y += rayon_comm;
			goalCom2.y += 2*rayon_comm;
		}
		if(nbRoboProsp == 1){
			goal.x += 3*rayon_comm;	//go right
			goalCom1.x += rayon_comm;
			goalCom2.x += 2*rayon_comm;
		}
		if(nbRoboProsp == 2){
			goal.y -= 3*rayon_comm;	//go down
			goalCom1.y -= rayon_comm;
			goalCom2.y -= 2*rayon_comm;
		}
		if(nbRoboProsp == 3){
			goal.x -= 3*rayon_comm;	//go left
			goalCom1.x -= rayon_comm;
			goalCom2.x -= 2*rayon_comm;
		}
		goal = normalise(goal);
		goalCom1 = normalise(goalCom1);
		goalCom2 = normalise(goalCom2);
		this->createRobots(goal, "prosp");
		this->createRobots(goalCom1, "com");
		this->createRobots(goalCom2, "com");
		++nbNewRobots;
	}
}

void Base::changeRoboProspGoal(shared_ptr<Robot> robo){
	point2D newGoal;
	//seeding the randomness algorithm with somthing that differs during the simulation
	//but is always the same if you redo a simulation
	srand(seed+ressources);
	//If robot is home set his start point random on the map
	if(isContained(CircleB, robo->getPosition() )){
		int dimension = 2 * dim_max;
		newGoal.x = rand() % dimension - dim_max;
		newGoal.y = rand() % dimension - dim_max;
	} else{
		newGoal = robo->getPosition();
	}
	newGoal.x += prospXdirection;
	newGoal.y += prospYdirection;
	
	for(int i(0); i < prospRandomness; ++i){
		if(rand() % 2  == 0){
			newGoal.y += prospYdirection;
		}
	}
	newGoal = normalise(newGoal);

	double distGoalHome(0);//Distance to the newGoal and then back home
	point2D randomPoint({0,0});
	distGoalHome += distance(robo->getPosition(), newGoal, randomPoint);
	distGoalHome += distance(newGoal, getCenter(), randomPoint);
	if(robo->getFuelLeft() > distGoalHome){
		robo->setGoal(newGoal);
	} else{
		robo->setGoal(getCenter());
	}
}

void Base::createRoboForage(int& nbNewRobots){
	if(ressources >= cost_forage+cost_transp and nbNewRobots < max_robots){
		shared_ptr<Robot> centerRobot;
		centerRobot = this->getCenterRobot();
		vector<shared_ptr<Robot>> connectedRobots;
		connectedRobots = centerRobot->getConnectedRobots();
		for(unsigned int i(0); i < connectedRobots.size(); ++i){
			if(connectedRobots[i]->getType() == "prosp" 
			   and connectedRobots[i]->isRemote() 
			   and connectedRobots[i]->isGisementFound() ){
				//if type Prosp AND connected to base AND foundGisment 
				//AND no other forage there
				double gisRes;
				gisRes = connectedRobots[i]->getFoundGisementRes();
				if(gisRes > profitability*(cost_forage + cost_transp)){
				//We can gain at least profitability of what it costed to create robots
					circle foundGisement;
					foundGisement = connectedRobots[i]->getFoundGisement();
					point2D gisLocation;
					gisLocation.x = foundGisement.x;
					gisLocation.y = foundGisement.y;
					bool found(false);
					for(auto robo : robots){
						point2D goalGisementCoords;
						goalGisementCoords.x = robo->getGoalGisement().x;
						goalGisementCoords.y = robo->getGoalGisement().y;
						if(robo->getType() == "forage" 
						   and isEqual(goalGisementCoords, gisLocation)){
							found = true;
						}
					}
					if(not(found)){
						this->createRobots(gisLocation, "forage");
						robots.back()->setGoalGisement(foundGisement);
						++nbNewRobots;
						this->createRobots(gisLocation, "transp");
						++nbNewRobots;
					}
				}
			}
		}
	}
}

void Base::changeRes(double change){
	ressources += change;
}






