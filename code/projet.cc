/*
* File-Name: projet.cc
* Authors: Elio Wanner and Sara Conti
* Created on: 7. March 2021
* Last edited on: 22. May 2021
* Version: 10.2
* Changelog: finalised code for rendu3
*/

#include <iostream>
#include <string>
#include "constantes.h"
#include "gui.h"
#include <gtkmm/application.h>
#include <gtkmm/window.h>
using namespace std;

int main(int argc, char** argv){

	setMax(dim_max);
	auto app = Gtk::Application::create("org.gtkmm.example");

	Gui g1(argc, argv);
	g1.set_resizable(true);

	return app->run(g1);

}
