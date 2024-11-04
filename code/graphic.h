/*
* File-Name: graphic.h
* Authors: Elio Wanner and Sara Conti
* Created on: 26. April 2021
* Last edited on: 22. May 2021
* Version: 2.5
* Changelog: finalised code for rendu3
*/

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <gtkmm.h>
#include "geomod.h"

void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr);

void graphic_set_size(double size, double width);

void drawCircle(const double size, double x, double y, double ray, bool filled,
				int color);

void drawPoint(const double size, double x, double y, int color);

void drawLine(const double size, double x1, double y1, double x2, double y2,
			  int color);

void setColor(int color);

void changeCoords(double size, double& x, double& y);

#endif
