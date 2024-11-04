/*
* File-Name: settings.h
* Authors: Elio Wanner and Sara Conti
* Created on: 15. May 2021
* Last edited on: 22. May 2021
* Version: 1.5
* Changelog: finalised code for rendu3
*/
#ifndef SETTINGS_H_INCLUDED
#define SETTINGS_H_INCLUDED

#include "constantes.h"

/* -------- VISUAL SETTINGS -------- */

constexpr double strokeWidth(1);				//used in graphic for stroke width
constexpr double pointSize(3);					//size to display robots
constexpr bool showAxis(false);
constexpr double displayRadiusBase(10 * rayon_base);
constexpr unsigned drawingAreaSize(500);


/* -------- STRATEGY SETTINGS -------- */

//Percentage of ressources when to create comGrid
constexpr double comGridLimit(1);

//To set new goal of roboProsp
constexpr double prospXdirection(dim_max/20);
constexpr double prospYdirection(dim_max/35);

//Increases chance of going more into y-direction (up)
constexpr double prospRandomness(5);

//If a robot has less than that he will get recharged
constexpr double rechargeLimit(4 * dim_max);

//when to send forage & transport:
//2 means if base gets back double the ressources it costed to create robots
constexpr double profitability(2);

//max number of prosp per base
constexpr double maxProsp(4);

//gives a saftey distance to ensure stable connection between roboComs
//should be smaller than comRadius, if negative -> denser than rayonCom
constexpr double comGridDensity(50);

//This randommly choosen seed ensures that a simulation can be reproduced
constexpr int seed(2021);

/* -------- DEBUG SETTINGS -------- */

constexpr bool debugShowSimulation(false);

//activate key (2) which allows to do 10 steps at a time
constexpr bool buttonMultipleSteps(false);


#endif // SETTINGS_H_INCLUDED
