/*
* File-Name: graphic.cc
* Authors: Elio Wanner and Sara Conti
* Color-Chooser: Rafael Miguel Correa
* Created on: 26. April 2021
* Last edited on: 22. May 2021
* Version: 2.6
* Changelog: finalised code for rendu3
*/

#include <iostream>
#include "graphic.h"
#include "settings.h"


using namespace std;

static const Cairo::RefPtr<Cairo::Context>* ptcr(nullptr);
static double windowSize;

void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr){
	ptcr = &cr;
	(*ptcr)->set_line_width(strokeWidth);
}

void graphic_set_size(double height, double width){
	windowSize = min(height, width)/2;
}

void drawCircle(const double size, double x, double y, double ray, bool filled,
				int color){
	setColor(color);
	
	changeCoords(size, x, y);
	ray *= windowSize/size;

	(*ptcr)->arc(x, y, ray, 0, 2 * M_PI);
	if(filled) (*ptcr)->fill_preserve();
	(*ptcr)->stroke();
}

void drawPoint(const double size, double x, double y, int color){
	drawCircle(size, x, y,pointSize , true, color);
}

void drawLine(const double size, double x1, double y1, double x2, double y2, 
			  int color){
	setColor(color);
	
	changeCoords(size, x1, y1);
	changeCoords(size, x2, y2);
	
	(*ptcr)->move_to(x1, y1);
	(*ptcr)->line_to(x2, y2);
	(*ptcr)->stroke();
}



void setColor(int color){
	int moduloColor;
	moduloColor = color%6;
	switch (moduloColor){
		case 0:
			(*ptcr)->set_source_rgb(0.6,0.18,0.18);		//red
			break;
		case 1:
			(*ptcr)->set_source_rgb(0.18,0.6,0.18);		//green
			break;
		case 2:
			(*ptcr)->set_source_rgb(0.18,0.2,0.6);		//blue
			break;
		case 3:
			(*ptcr)->set_source_rgb(0.83,0.78,0.08);	//yellow
			break;
		case 4:
			(*ptcr)->set_source_rgb(0.7,0.3,0.78);		//magenta
			break;
		case 5:
			(*ptcr)->set_source_rgb(0.3,0.67,0.78);		//cyan
			break;
		
		case -1:
			(*ptcr)->set_source_rgb(0, 0, 0);			//black
			break;
		case -2:
			(*ptcr)->set_source_rgb(0.87,0.56,1);		//violet
			break;
		case -3:
			(*ptcr)->set_source_rgb(0.5,0.5,0.5);		//grey
			break;
	}
}

void changeCoords(double size, double& x, double& y){
	x += size;
	y *= -1;
	y += size;

	x *= windowSize/size;
	y *= windowSize/size;
}
