/*
* File-Name: geomod.h
* Authors: Sara Conti and Elio Wanner
* Created on: 7. March 2021
* Last edited on: 1. May 2021
* Version: 8.0
* Changelog: drawing functions
*/

#ifndef GEOMOD_H
#define GEOMOD_H
#include <vector>

void setMax(double init_max);
bool equal_zero(double value);
double getMax();
double getEpsilon();
struct point2D {
	double x;
	double y;
};
struct circle{
	double x;
	double y;
	double ray;
};
double normalise(double point);
point2D normalise(point2D point);
double distance(point2D departurePoint, point2D arrivalPoint, 
				point2D& smallestVector);
bool isEqual(point2D p1, point2D p2);
bool isContained (circle c1, point2D p1);
bool isIntersecting(circle c1, circle c2);

//Drawing functions

void drawCircle(double x, double y, double ray, bool filled, int color);

void drawPoint(double x, double y, int color);

void drawLine(double x1, double y1, double x2, double y2, int color);

void drawCross();

#endif
