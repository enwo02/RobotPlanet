/*
* File-Name: gui.cc
* Authors: Sara Conti and Elio Wanner
* Created on: 26. April 2021
* Last edited on: 14. May 2021
* Version: 4.1
* Changelog: cleaned up code
*/

#ifndef GTKMM_EXAMPLE_MYEVENTS_H
#define GTKMM_EXAMPLE_MYEVENTS_H

#include <gtkmm.h>
#include <string>
#include "simulation.h"

class MyArea : public Gtk::DrawingArea{
public:
	MyArea();
	MyArea(std::string filename);
	virtual ~MyArea(){}
	void step();
	void toggle_comRay();
	void toggle_Connections ();
	std::vector<BaseData> getSimData();
	void save_file(std::string filepath);
	void open_file(std::string filepath);

protected:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

private:
	Simulation sim;
	bool showComRay;
	bool showConnections;
	std::string filepath;
};


class Gui : public Gtk::Window{
public:
	Gui();
	Gui(int argc, char** argv);
	virtual ~Gui(){}
	void tree_view_update();
private:
	bool started;
protected:
	// Keyboard signal handler:
	bool on_key_press_event(GdkEventKey * key_event);

	// interface components
	Gtk::Box m_Box, m_Box_Top, m_Box_Bottom, m_Box_Left, m_Box_Right, 
		 Box_General, Box_Toggle, Box_Instructions, _v_box;
	MyArea m_Area;

	Gtk::Frame General;
	Gtk::Frame Toggle;
	Gtk::Frame Instructions;
	


	Gtk::Label Label_Instructions; 
	Gtk::Label Label_button2;
	Gtk::Label Label_Instructions_2;
	
		
	//Member widgets:
	Gtk::Button exit_button;
	Gtk::Button open_button;
	Gtk::Button save_button;
	Gtk::Button start_button;
	Gtk::Button step_button;
	Gtk::Button link_button;
	Gtk::Button range_button;

	bool on_idle();

	//Signal handlers:
	void on_button_clicked_exit();
	void on_button_clicked_open();
	void on_button_clicked_save();
	void on_button_clicked_start();
	void on_button_clicked_step();
	void on_button_clicked_link();
	void on_button_clicked_range();


    //internal class for private use of scrolledWindow
    class Model_columns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        Model_columns()
        {
            add(_col_id);
            add(_col_nbP);
            add(_col_nbF);
            add(_col_nbT);
            add(_col_nbC);
            add(_col_resource);
            add(_col_resource_percentage);
        }

        Gtk::TreeModelColumn<int> _col_id;
        Gtk::TreeModelColumn<int> _col_nbP;
        Gtk::TreeModelColumn<int> _col_nbF;
        Gtk::TreeModelColumn<int> _col_nbT;
        Gtk::TreeModelColumn<int> _col_nbC;
        Gtk::TreeModelColumn<double> _col_resource;
        Gtk::TreeModelColumn<int> _col_resource_percentage;
    };
    Model_columns _columns;

    Gtk::ScrolledWindow _scrolled_window;
    Gtk::TreeView _tree_view;

    unsigned count;
};

#endif // GTKMM_EXAMPLE_MYEVENTS_H

