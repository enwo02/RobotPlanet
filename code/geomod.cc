/*
* File-Name: geomod.cc
* Authors: Sara Conti and Elio Wanner
* Created on: 7. March 2021
* Last edited on: 22. May 2021
* Version: 7.2
* Changelog: finalised code for rendu3
*/

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include "geomod.h"
#include "graphic.h"
#include "settings.h"


using namespace std;

namespace {
    double epsilon_zero;
    double maximum(0);
}

bool equal_zero(double value){
    if((abs(value)) < epsilon_zero){
        return true;
    }
    return false;
}

void setMax (double init_max){
    maximum = init_max;
    epsilon_zero = pow(10,-10)*maximum;
}

double getMax(){
    return maximum;
}

double getEpsilon(){
    return epsilon_zero;
}

double normalise (double point){
    if(point < (-maximum)){
        point += 2*maximum;
    } else if(point > maximum){
        point -= 2*maximum;
    }
    
    return point;
}

point2D normalise (point2D point){	
	point.x = normalise(point.x);
	point.y = normalise(point.y);
    return point;
}

double distance(point2D departurePoint, point2D arrivalPoint, 
				point2D& smallestVector){
    vector<point2D> equivalentPoints;
    point2D tmp;

    for (int k_x(-1); k_x <= 1; ++k_x){
        for (int k_y(-1); k_y <= 1; ++k_y){
            tmp.y = arrivalPoint.y + k_y*2*maximum;
            tmp.x = arrivalPoint.x + k_x*2*maximum;
            equivalentPoints.push_back(tmp);
        }
    }

    //just initialised with something that definitely is too big
    double minDistance(maximum*2);
    for(auto equivalentPoint : equivalentPoints){
		double tmpDistance;
		double xDifference(equivalentPoint.x - departurePoint.x);
		double yDifference(equivalentPoint.y - departurePoint.y);

		tmpDistance = sqrt( pow(xDifference, 2) + pow(yDifference, 2) );

		if(tmpDistance < minDistance){
			minDistance = tmpDistance;
			smallestVector.x = xDifference;
			smallestVector.y = yDifference;
		}
	}

	return minDistance;
}

bool isEqual(point2D p1, point2D p2){
    double dist;
    point2D vect; //here not really necessary,but distance() requires it
    dist = distance(p1, p2, vect);
    return equal_zero(dist);
}

bool isContained (circle c1, point2D p1){
    double dist;
    point2D pointCenter;
    point2D vect; //here not really necessary,but distance() requires it
    pointCenter.x = c1.x;
    pointCenter.y = c1.y;
    dist = distance(pointCenter, p1, vect);
    if(dist < c1.ray - epsilon_zero){
        return true;
    }
    else{
        return false;
    }
}

bool isIntersecting(circle c1, circle c2){
    double dist;
    point2D pointCenter1 {c1.x, c1.y};
    point2D pointCenter2 {c2.x, c2.y};
    
    point2D vect; //here not really necessary,but distance() requires it
    dist = distance(pointCenter1, pointCenter2, vect);
    if(dist < c1.ray + c2.ray - epsilon_zero){
        return true;
    }
    else{
        return false;
    }
}

//Drawing functions-----------------

void drawCircle(double x, double y, double ray, bool filled, int color){
	drawCircle(maximum, x, y, ray, filled, color);
	
	//overboards on the right
	if(x+ray > maximum){
		drawCircle(maximum, x-2*maximum, y, ray, filled, color);
		//top
		if(y+ray > maximum){
			drawCircle(maximum, x-2*maximum, y-2*maximum, ray, filled, color);
		}
		//bottom
		if(y-ray < -maximum){
			drawCircle(maximum, x-2*maximum, y+2*maximum, ray, filled, color);
		}
	}
	
	//overboards on the left
	if(x-ray < -maximum){
		drawCircle(maximum, x+2*maximum, y, ray, filled, color);
		//top
		if(y+ray > maximum){
			drawCircle(maximum, x+2*maximum, y-2*maximum, ray, filled, color);
		}
		//bottom
		if(y-ray < -maximum){
			drawCircle(maximum, x+2*maximum, y+2*maximum, ray, filled, color);
		}
	}
	
	//overboards on the top
	if(y+ray > maximum){
		drawCircle(maximum, x, y-2*maximum, ray, filled, color);
	}
	//overboards on the bottom
	if(y-ray < -maximum){
		drawCircle(maximum, x, y+2*maximum, ray, filled, color);
	}
	
	
}

void drawPoint(double x, double y, int color){
	drawPoint(maximum, x, y, color);
}

void drawLine(double x1, double y1, double x2, double y2, int color){
	point2D direction({0,0});
	distance({x1,y1}, {x2,y2}, direction);
	drawLine(maximum, x1, y1, x1 + direction.x, y1 + direction.y, color);
}


void drawCross(){
	if(showAxis){
		drawLine(maximum, -maximum, 0, maximum, 0, -3);
		drawLine(maximum, 0, -maximum, 0, maximum, -3);	
	}
	
	drawLine(maximum, -maximum, -maximum, maximum, -maximum, -3);
	drawLine(maximum, maximum,  -maximum, maximum, maximum, -3);
	drawLine(maximum, -maximum, -maximum, -maximum, maximum, -3);
	drawLine(maximum, -maximum,  maximum, maximum, maximum, -3);
}
