/*
* File-Name: utilities.cc
* Authors: Elio Wanner and Sara Conti
* Created on: 23. April 2021
* Last edited on: 23. April 2021
* Version: 1.0
* Changelog: moving functions from geomod to here
*/

#include <vector>
#include <string>
#include "utilities.h"


using namespace std;

vector<double> decodeString(string& lineReading){
	//splits a string into a vector of doubles
	removeComment(lineReading);

	vector<double> inputs;
	string currentReading;
	int endOfFirstInput(0);
	while(lineReading.size() > 0){	//while not everything read
		trim(lineReading);
		endOfFirstInput = lineReading.find_first_of(" ");
		currentReading = lineReading.substr(0, endOfFirstInput);

		if(currentReading == "true"){
			inputs.push_back(1);
		} else if(currentReading == "false"){
			inputs.push_back(0);
		} else{
			inputs.push_back(stod(currentReading));
		}

		if(endOfFirstInput != -1){
			lineReading = lineReading.substr(endOfFirstInput, lineReading.size() );
		} else{
			lineReading = "";
		}
	}

	return inputs;
}

void removeComment(string& line){
	int firstOccurence;
	firstOccurence = line.find_first_of("#");
	if(firstOccurence >= 0){
		line = line.substr(0, firstOccurence);
	}
}
void trim(string& line){
	//removes white spaces at front & back
	while(line.at(0) == ' '){
		line.erase(0,1);
	}
	while(line.at(line.size()-1) == ' '){
		line.erase(line.size()-1);
	}
	while(line.at(0) == '\t'){
		line.erase(0,1);
	}
	while(line.at(line.size()-1) == '\t'){
		line.erase(line.size()-1);
	}
}


