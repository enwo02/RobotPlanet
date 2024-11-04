/*
* File-Name: simulation.cc
* Authors: Sara Conti and Elio Wanner
* Created on: 2. April 2021
* Last edited on: 21. May 2021
* Version: 5.1
* Changelog: finalised code for rendu3
*/
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include "constantes.h"
#include "settings.h"
#include "simulation.h"
#include "geomod.h"
#include "utilities.h"
#include "base.h"
#include "gisement.h"
#include "robot.h"
#include "message.h"

using namespace std;

Simulation::Simulation(string fileName){
	this->init(fileName);
}

void Simulation::init(string fileName){
	this->destroy();				//erases existing contents in this simulation

	vector<string> readings;
	readings = readFile(fileName);

	initGisements(readings);

	initBases(readings);
	
	if( not( checkOverlapping() ) ){
		this->destroy();
	} else{
		cout << message::success();
	}
}

void Simulation::save(string fileName){
	ofstream saveFile(fileName);
	if(!saveFile.fail() ){
		saveFile << "#This file was automatically saved" << endl << endl;
		saveFile << "#--------------------GISEMENTS----------------------"<<endl;
		saveFile << gisements.size() << "\t\t\t\t#Number of gisements" << endl;
		for(auto gis : gisements){
			gis.save(saveFile);
		}

		saveFile << endl << "#--------------------BASES----------------------"<<endl;
		saveFile << bases.size() << "\t\t\t\t#Number of bases" << endl;
		for(auto base : bases){
			saveFile << endl;
			base.save(saveFile);
		}
	}
	saveFile.close();

}

void Simulation::destroy(){
	gisements.clear();
	bases.clear();
}

vector<string> Simulation::readFile(string fileName){
	string newReading;
	vector<string> readings;
	ifstream dataFile(fileName);
	if(dataFile.fail()){
		cout << "Error: Couldn't open the file " << fileName << endl;
		this->destroy();
	}else{
		while(getline(dataFile >> ws, newReading)){
			trim(newReading);
			if(newReading[0] == '#') continue;
			readings.push_back(newReading);
		}
	}
	return readings;
}

void Simulation::initGisements(vector<string> readings){
	int linePointer(0);
	int nbGisements(stod(readings[linePointer]));
	++linePointer;
	for(int i(0); i < nbGisements; ++i){
		Gisement newGisement(readings[linePointer]);
		++linePointer;
		gisements.push_back(newGisement);
	}
}

void Simulation::initBases(vector<string> readings){
	int linePointer(stod(readings[0])+1);		//Set linePointer to start of bases
	int nbBases(stod(readings[linePointer]));
	++linePointer;
	for(int i(0); i < nbBases; ++i){
		Base newBase(readings[linePointer], i);
		vector<double> input;
		input = decodeString(readings[linePointer]);
		++linePointer;

		//Adding Robots to base
		for(int i(0); i < input[3]; ++i){
			newBase.addRobot(new RoboProsp(readings[linePointer]));
			++linePointer;
		}
		for(int i(0); i < input[4]; ++i){
			newBase.addRobot(new RoboForage(readings[linePointer]));
			++linePointer;
		}
		for(int i(0); i < input[5]; ++i){
			newBase.addRobot(new RoboTransp(readings[linePointer]));
			++linePointer;
		}
		for(int i(0); i < input[6]; ++i){
			newBase.addRobot(new RoboCom(readings[linePointer]));
			++linePointer;
		}
		vector<shared_ptr<Robot>> newBaseRobots;
		newBaseRobots = newBase.getRobots();
		point2D baseCenter;
		baseCenter = newBase.getCenter();
		for(auto robo : newBaseRobots){
			robo->setHome(baseCenter);
		}
		
		//Check for the existence of the center Communication Robot
		newBase.checkCenterRobot();

		bases.push_back(newBase);
	}
}

void Simulation::checkRobotOnGis(){
	vector<shared_ptr<Robot>> allRobots;
	allRobots = this->getAllRobots();

	//compare all roboProsp and roboForage with gisements
	for(unsigned int i(0); i < allRobots.size(); ++i){
		for(auto gis : gisements){
			if( isContained(gis.getCircle(), allRobots[i]->getPosition() ) ){
				//Check if roboProsp has found a gisement
				if(allRobots[i]->getType() == "prosp" 
				   and allRobots[i]->getAchieved() == true){
					allRobots[i]->newFoundGisement(gis.getCircle(), gis.getRes() );
				}
				//stop roboForage as soon as he touches his gisement
				if(allRobots[i]->getType() == "forage"){
					circle goalGisement;
					goalGisement = allRobots[i]->getGoalGisement();
					point2D gisementCenter, currentRoboPos;
					gisementCenter.x = goalGisement.x;
					gisementCenter.y = goalGisement.y;
					currentRoboPos = allRobots[i]->getPosition();
					point2D randomPoint;
					double distUntilGoal;
					distUntilGoal =distance(currentRoboPos,gisementCenter,randomPoint);
					if(distUntilGoal <= goalGisement.ray){
						point2D newGoal;
						newGoal = allRobots[i]->getPosition();
						allRobots[i]->setGoal(newGoal);
						allRobots[i]->setAchieved(true);
					}
				}
			}
		}
	}
}

void Simulation::exchangeRessource(){
	vector<shared_ptr<Robot>> allRobots;
	allRobots = this->getAllRobots();
	for(unsigned int i(0); i < allRobots.size(); ++i){
		//Gisement->RoboForage exchange
		for(unsigned int k(0); k < gisements.size(); ++k){
			if( isContained(gisements[k].getCircle(), allRobots[i]->getPosition() )
				and allRobots[i]->getType() == "forage" 
				and gisements[k].getRes() >= deltaR){
				bool mined;
				mined = allRobots[i]->doForage();
				if(mined) gisements[k].changeRes(-deltaR);
			}
		}
		for(unsigned int k(0); k < bases.size(); ++k){
			//RoboTransport->Base exchange
			if( isEqual(bases[k].getCenter(), allRobots[i]->getPosition() )
				and allRobots[i]->getType() == "transp" 
				and not(allRobots[i]->isEmpty()) ){
				allRobots[i]->exchangeRessource(false);
				bases[k].changeRes(deltaR);
			}
			//Base->Robots exchange (Recharge robots)
			double leftOverFuel;
			leftOverFuel = allRobots[i]->getFuelLeft();
			if(isEqual( bases[k].getCenter(), allRobots[i]->getPosition() ) 
			   and leftOverFuel <= rechargeLimit){
				if(allRobots[i]->getType() == "transp"){//Fuel up roboTransp
					allRobots[i]->fuelUp();
					bases[k].changeRes(-(maxD_transp - leftOverFuel )* cost_repair);
				}
				if(allRobots[i]->getType() == "prosp"){//Fuel up roboProsp
					allRobots[i]->fuelUp();
					bases[k].changeRes(-(maxD_prosp - leftOverFuel )* cost_repair);
				}
			}
		}
	}
}

vector<shared_ptr<Robot>> Simulation::getAllRobots(){
	vector<shared_ptr<Robot>> allRobots;
	for(auto base : bases){
		vector<shared_ptr<Robot>> newRobots;
		newRobots = base.getRobots();
		for(auto newR : newRobots){
			allRobots.push_back(newR);
		}
	}
	return allRobots;
}

bool Simulation::checkOverlapping(){
	bool success(true);
	for (unsigned int i(0); i < bases.size(); ++i){
		//Base vs Gisement
		for (unsigned int j(0); j < gisements.size(); ++j){
			if( not( bases[i].checkOverlap(gisements[j]) ) ){
				success = false;
			}
		}
		//Base vs Base
		for (unsigned int j(0); j < bases.size() && j!=i; ++j){
			if( not( bases[j].checkOverlap(bases[i]) ) ){
				success = false;
			}
		}
	}
	//Gisement vs Gisement
	for (unsigned int i(0); i < gisements.size(); ++i){
		for (unsigned int j(0); j < gisements.size() && j!=i; ++j){
			if( not( gisements[j].checkOverlap(gisements[i]) ) ){
				success = false;
			}
		}
	}
	
	if(not(success)){
		return false;
	}
	return true;
}

void Simulation::show(){
	cout << endl << "Printing all gisements:" << endl;
	for(auto gis : gisements){
		gis.show();
	}
	cout << endl << "Printing all bases:" << endl;
	for(auto base : bases){
		cout << "----------------------------------------" << endl;
		base.show();
		cout << endl;
	}
}

void Simulation::display(bool showComRay, bool showConnections){
	for(auto gis : gisements){
		gis.display();
	}

	for(auto base : bases){
		base.display(showComRay, showConnections);
	}
}

void Simulation::doStep(){
	if(debugShowSimulation){
		cout << debugShowSimulation;
		this->show();
		cout << endl << endl;
	}
	
	
	for(unsigned int i(0); i < bases.size(); ++i){
		bases[i].doStep();
		//delete base if no ressources left
		if(bases[i].getBaseData().ressource <= 0){
			bases.erase(bases.begin() + i);
		}
	}
	this->checkRobotOnGis();
	this->exchangeRessource();
}

void Simulation::updateNb(){
	vector<shared_ptr<Robot>> allRobots;
	allRobots = this->getAllRobots();

	//compare all robots to all other robots
	for(unsigned int i(0); i < allRobots.size(); ++i){
		allRobots[i]->resetNeighbours();
		for(unsigned int k(0); k < allRobots.size(); ++k){
			double dist;
			point2D r1 (allRobots[i]->getPosition());
			point2D r2 (allRobots[k]->getPosition());
			point2D emptyPoint;
			dist = distance(r1, r2, emptyPoint);
			if(allRobots[i] != allRobots[k] and dist <= rayon_comm){
				allRobots[i]->addNeighbour(allRobots[k]);
			}
		}
	}
}

void Simulation::updateConnections(){
	vector<shared_ptr<Robot>> allRobots;
	allRobots = this->getAllRobots();
	vector<int> indexCenterRobots;
	
	//update the connection list of the center robot
	for(auto base : bases){
		shared_ptr<Robot> centerRobot;
		centerRobot = base.getCenterRobot();
		for(unsigned int i(0); i < allRobots.size(); ++i){
			if(allRobots[i] == centerRobot){
				vector<bool> visited(allRobots.size());
				allRobots[i]->resetConnectedRobots();
				updateConnectionsOfRobot(allRobots, visited, i, i);
			}
		}
	}
	
	//update the remote state of the robots
	for(auto base : bases){
		vector<shared_ptr<Robot>> robotsOfBase;
		shared_ptr<Robot> centerRobot;
		robotsOfBase = base.getRobots();
		centerRobot = base.getCenterRobot();
		for(unsigned int i(0); i < robotsOfBase.size(); ++i){
			if( centerRobot->isConnected(robotsOfBase[i]) ){
				robotsOfBase[i]->setRemote(true);
			} else{
				robotsOfBase[i]->setRemote(false);
			}
		}
	}
}

void Simulation::updateConnectionsOfRobot(vector<shared_ptr<Robot>> allRobots, 
										  vector<bool> &visited,
										  int current, int initial){
	vector<shared_ptr<Robot>> NeighbourRobots;
	NeighbourRobots = allRobots[current]->getNeighbourRobots();
	
	if(not(visited[current])){
		visited[current]=1;
		for(unsigned int k(0); k < NeighbourRobots.size(); ++k){
			vector<shared_ptr<Robot>> ConnectedRobots;
			ConnectedRobots = allRobots[initial]->getConnectedRobots();
			bool found(false);
			//check if not already in connected list
			for (unsigned int m(0); m < ConnectedRobots.size() ; ++m){
				if(NeighbourRobots[k] == ConnectedRobots[m]){
					found=true;
				}
			}
			
			if(not(found) && NeighbourRobots[k] != allRobots[initial]){
				allRobots[initial]->addConnected(NeighbourRobots[k]);
				
				//find index of k'th roboter of neighbours in allrobots list
				int indexInAllRobots(0);
				for(unsigned int h(0); h < allRobots.size(); ++h){
					if(NeighbourRobots[k] == allRobots[h]){
						indexInAllRobots = h;
					}
				}
				//recursive call
				updateConnectionsOfRobot(allRobots,visited,indexInAllRobots,initial);
			}
			
		}
	}
}

vector<BaseData> Simulation::getSimData(){
	vector<BaseData> data;
	for(unsigned int i(0); i < bases.size(); ++i){
		data.push_back( bases[i].getBaseData() );
	}
	return data;
}
