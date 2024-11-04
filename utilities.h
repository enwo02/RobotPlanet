/*
* File-Name: utilities.h
* Authors: Elio Wanner and Sara Conti
* Created on: 23. April 2021
* Last edited on: 23. April 2021
* Version: 1.0
* Changelog: moving functions from geomod to here
*/

#ifndef UTILITIES_H
#define UTILITIES_H
#include <vector>

std::vector<double> decodeString(std::string& lineReading);
void removeComment(std::string& line);
void trim(std::string& line);
#endif
