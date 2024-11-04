/*
* File-Name: gisement.cc
* Authors: Sara Conti and Elio Wanner
* Created on: 20. March 2021
* Last edited on: 16. May 2021
* Version: 3.6
* Changelog: fail to init
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "constantes.h"
#include "gisement.h"
#include "geomod.h"
#include "utilities.h"
#include "message.h"
using namespace std;

//Constructor to create from string (initializing)
Gisement::Gisement(string lineReading){
	vector<double> input;
	input = decodeString(lineReading);
	CircleG.x = input[0];
	CircleG.y = input[1];
	CircleG.ray = input[2];
	ressource = input[3];
	checkRay();
}

void Gisement::checkRay(){
	if(CircleG.ray<rayon_min || CircleG.ray>rayon_max){
		cout << "Error: Size of ray invalid: "<< CircleG.ray << endl;
	}
}

circle Gisement::getCircle(){
	return CircleG;
}

double Gisement::getRes(){
	return ressource;
}

void Gisement::changeRes(double change){
	ressource += change;
}


bool Gisement::checkOverlap(Gisement currentGisement){
    if(isIntersecting(currentGisement.getCircle(), CircleG)){
		double x1( CircleG.x );
		double y1( CircleG.y );
		double x2( currentGisement.getCircle().x );
		double y2( currentGisement.getCircle().y );
		cout<< message::field_superposition(x1, y1, x2, y2);
		return false;
	}
	return true;
}

void Gisement::show(){
	cout << "Gis: (" << CircleG.x << "," << CircleG.y << ") ";
	cout << "ray: " << CircleG.ray << " res: " << ressource << endl;
}

void Gisement::display(){
	drawCircle(CircleG.x, CircleG.y, CircleG.ray, false, -1);
	double ressourceLeft;
	//get percentage of used resource
	ressourceLeft = ressource / (1000 * pow(CircleG.ray / rayon_min, 2) );
	if(ressourceLeft > 1){ressourceLeft = 1;}
	drawCircle(CircleG.x, CircleG.y, CircleG.ray*ressourceLeft, true, -1);
}

void Gisement::save(ofstream& saveFile){
	saveFile << CircleG.x << " " << CircleG.y << " " << CircleG.ray << " ";
	saveFile << ressource << endl;
}
