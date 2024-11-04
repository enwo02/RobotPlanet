/*
* File-Name: gisement.h
* Authors: Sara Conti and Elio Wanner
* Created on: 20. March 2021
* Last edited on: 16. May 2021
* Version: 2.4
* Changelog: fail to initialise
*/

#ifndef GISEMENT_H
#define GISEMENT_H

#include <string>
#include "geomod.h"

class Gisement {
protected:
	circle CircleG;
	double ressource;

public:
	Gisement(std::string lineReading);
	bool checkOverlap(Gisement currentGisement);
	circle getCircle();
	double getRes();
	void changeRes(double change);
	void checkRay();
	void show();
	void display();
	void save(std::ofstream& saveFile);
};

#endif
