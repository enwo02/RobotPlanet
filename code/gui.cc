/*
* File-Name: gui.cc
* Authors: Sara Conti and Elio Wanner
* Created on: 26. April 2021
* Last edited on: 14. May 2021
* Version: 4.1
* Changelog: cleaned up code
*/
#include <iostream>
#include <cairomm/context.h>
#include <cstdlib>
#include "gui.h"
#include "settings.h"
#include "graphic.h"
#include "simulation.h"

using namespace std;

#define GTK_COMPATIBILITY_MODE
#ifdef GTK_COMPATIBILITY_MODE

namespace Gtk{
	template<class T, class T_Args>
	auto make_managed(T_Args&& args) -> T*{
		return manage(new T(std::forward<T_Args>(args)));
	}
}
#endif

MyArea::MyArea(): showComRay(0), showConnections(1){}

void MyArea::toggle_comRay(){
    showComRay = !showComRay;
}

void MyArea::toggle_Connections(){
    showConnections = !showConnections;
}

void MyArea::save_file(string filepath){
    sim.save(filepath);
}

void MyArea::open_file(string filepath){
    sim.init(filepath);
}

vector<BaseData> MyArea::getSimData(){
    return sim.getSimData();
}

void MyArea::step(){
    sim.doStep();
}

bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	graphic_set_size(get_allocation().get_height(), get_allocation().get_width());
    graphic_set_context(cr);

	drawCross();
	sim.updateNb();
	sim.updateConnections();
	sim.display(showComRay, showConnections);

	return true;
}

Gui::Gui() :
	started(0),
	m_Box(Gtk::ORIENTATION_VERTICAL,10),
	m_Box_Top(Gtk::ORIENTATION_HORIZONTAL,10),
	m_Box_Bottom(Gtk::ORIENTATION_HORIZONTAL,10),
	m_Box_Left(Gtk::ORIENTATION_VERTICAL, 10),
	m_Box_Right(Gtk::ORIENTATION_VERTICAL, 10),
	Box_General(Gtk::ORIENTATION_VERTICAL),
	Box_Toggle(Gtk::ORIENTATION_VERTICAL),
	Box_Instructions(Gtk::ORIENTATION_VERTICAL),
	_v_box(Gtk::ORIENTATION_VERTICAL, 10),
	General("General"), Toggle("Toggle display"), Instructions("Instructions"),
	Label_Instructions(" press s to start the simulation "
					   "\n press 1 to do one step "
					   "\n press r to activate the toogle range button \n"
					   " press c to activate the toogle link button "
					   "\n press q to exit"),
	exit_button("exit"),open_button("open"), save_button("save"),
	start_button("start"), 
	step_button("step"),
	link_button("toggle link"), range_button("toggle range")
{
	set_title("Project Planet Donut - Sara Conti and Elio Wanner");
	set_border_width(5);

	exit_button.signal_clicked().connect(sigc::mem_fun(*this,
			  &Gui::on_button_clicked_exit));
	open_button.signal_clicked().connect(sigc::mem_fun(*this,
			  &Gui::on_button_clicked_open));
	save_button.signal_clicked().connect(sigc::mem_fun(*this,
			  &Gui::on_button_clicked_save));
	start_button.signal_clicked().connect(sigc::mem_fun(*this,
			  &Gui::on_button_clicked_start));
	step_button.signal_clicked().connect(sigc::mem_fun(*this,
			  &Gui::on_button_clicked_step));
	link_button.signal_clicked().connect(sigc::mem_fun(*this,
			  &Gui::on_button_clicked_link));
	range_button.signal_clicked().connect(sigc::mem_fun(*this,
			  &Gui::on_button_clicked_range));

	//Boxes + Frame around buttons
	m_Box_Left.add(General);
	General.add(Box_General);
	Box_General.pack_start(exit_button, false, false);
	Box_General.pack_start(open_button, false, false);
	Box_General.pack_start(save_button, false, false);
	Box_General.pack_start(start_button, false, false);
	Box_General.pack_start(step_button, false, false);

	m_Box_Left.add(Toggle);
	Toggle.add(Box_Toggle);
	Box_Toggle.pack_start(link_button, false, false);
	Box_Toggle.pack_start(range_button, false, false);
	
	m_Box_Left.add(Instructions);
	Instructions.add(Box_Instructions);
	Box_Instructions.pack_start(Label_Instructions,Gtk::PACK_SHRINK);	

	// This is the part for the scrolled_window
	_v_box.add(_scrolled_window);

	_scrolled_window.set_size_request(-1, 200);
	_scrolled_window.add(_tree_view);
	_scrolled_window.set_policy(Gtk::PolicyType::POLICY_AUTOMATIC,
							  Gtk::PolicyType::POLICY_AUTOMATIC);
	_scrolled_window.set_hexpand();

	_tree_view.append_column("uid", _columns._col_id);
	_tree_view.append_column("nbP", _columns._col_nbP);
	_tree_view.append_column("nbF", _columns._col_nbF);
	_tree_view.append_column("nbT", _columns._col_nbT);
	_tree_view.append_column("nbC", _columns._col_nbC);
	_tree_view.append_column_numeric("Amount resource",_columns._col_resource, "%.2f");

	auto cell = Gtk::make_managed<Gtk::CellRendererProgress>();
	int cols_count = _tree_view.append_column("Mission completeness", *cell);

	auto progress_col = _tree_view.get_column(cols_count - 1);
	if(progress_col){
		progress_col->add_attribute(cell->property_value(),
									_columns._col_resource_percentage);
	}
	
	add(m_Box);
	m_Box.pack_start(m_Box_Top);
	m_Box.pack_start(m_Box_Bottom);
	m_Box_Top.pack_start(m_Box_Left);
	m_Box_Top.pack_start(m_Box_Right);
	m_Box_Bottom.pack_start(_v_box);

	m_Area.set_size_request(drawingAreaSize,drawingAreaSize);
	m_Box_Right.pack_start(m_Area);

	Glib::signal_idle().connect( sigc::mem_fun(*this, &Gui::on_idle) );
	show_all_children();
}

Gui::Gui(int argc, char** argv):Gui(){
	if (argc>1){
		m_Area.open_file(argv[1]);
	}
}

void Gui::on_button_clicked_exit(){
  exit(0);
}

void Gui::on_button_clicked_open(){
	Gtk::FileChooserDialog dialog("Please choose a file",
		  Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Open", Gtk::RESPONSE_OK);

	Gtk::Label m_Label_Info;

	m_Label_Info.set_text("choosing a file");

	//Show the dialog and wait for a user response:
	int result = dialog.run();

	m_Label_Info.set_text("Done choosing a file");

	//Handle the response:
	switch(result){
		case(Gtk::RESPONSE_OK):{
			std::cout << "Open clicked." << std::endl;

			//Notice that this is a std::string, not a Glib::ustring.
			std::string filename = dialog.get_filename();
			std::cout << "File selected: " <<  filename << std::endl;
			m_Area.open_file(filename);
			m_Area.queue_draw();
			break;
		}
		case(Gtk::RESPONSE_CANCEL):{
			std::cout << "Cancel clicked." << std::endl;
			break;
		}
		default:{
			std::cout << "Unexpected button clicked." << std::endl;
			break;
		}
	}
}

void Gui::on_button_clicked_save(){
	Gtk::FileChooserDialog dialog("Please choose a file",
								  Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Save", Gtk::RESPONSE_OK);

	Gtk::Label m_Label_Info;

	m_Label_Info.set_text("choosing a file");

	//Show the dialog and wait for a user response:
	int result = dialog.run();

	m_Label_Info.set_text("Done choosing a file");

	//Handle the response:
	switch(result){
		case(Gtk::RESPONSE_OK):{
			std::string filename = dialog.get_filename();
			std::cout << "File selected: " <<  filename << std::endl;
			m_Area.save_file(filename);
			m_Area.queue_draw();
			break;
		}
		case(Gtk::RESPONSE_CANCEL):{
			std::cout << "Cancel clicked." << std::endl;
			break;
		}
		default:{
			std::cout << "Unexpected button clicked." << std::endl;
			break;
		}
	}
}
void Gui::on_button_clicked_start(){
    if(started==0){
		cout<< "start" <<endl;
		started=!started;
		start_button.set_label("stop");
		step_button.hide();
	}
	else if (started==1){
		cout<< "stop" << endl;
		started=! started;
		start_button.set_label("start");
		step_button.show();
	}
}

bool Gui::on_idle(){
	if(started){
		m_Area.step();
		m_Area.queue_draw();
	}
	tree_view_update();
	return true;
}

void Gui::on_button_clicked_step(){
    m_Area.step();
    m_Area.queue_draw();
}

void Gui::on_button_clicked_range(){
	m_Area.toggle_comRay();
	m_Area.queue_draw();
}

void Gui::on_button_clicked_link(){
	m_Area.toggle_Connections();
	m_Area.queue_draw();
}

void Gui::tree_view_update(){
	Glib::RefPtr<Gtk::ListStore> ref_tree_model = Gtk::ListStore::create(_columns);
	_tree_view.set_model(ref_tree_model);

	vector<BaseData> report;
	report = m_Area.getSimData();

	for(size_t i = 0 ; i <report.size() ; ++i){
		auto row = *(ref_tree_model->append());
		row[_columns._col_id] = report[i].uid;
		row[_columns._col_nbP] = report[i].nbP;
		row[_columns._col_nbF] = report[i].nbF;
		row[_columns._col_nbT] = report[i].nbT;
		row[_columns._col_nbC] = report[i].nbC;
		row[_columns._col_resource] = report[i].ressource;
		row[_columns._col_resource_percentage] = report[i].ressource_p;
	}
}

/*Keyboard signal handler
 * s -> Start into continous mode
 * 1 -> 1 Step
 * 2 -> 10 Steps
 * r -> Toggle comRay
 * c -> Toggle connections
 * q -> Quit the program
 */

bool Gui::on_key_press_event(GdkEventKey * key_event){
	if(key_event->type == GDK_KEY_PRESS){
		switch(gdk_keyval_to_unicode(key_event->keyval)){
			case 's':
                on_button_clicked_start();
				break;
			case '1':
				if(started==0){
					cout << "1 Step" << endl;
					on_button_clicked_step();
				}
				break;
			case '2':
				if (buttonMultipleSteps==true && started==0){
					cout << "10 Step" << endl;
					for(int i(0); i < 10; ++i){
						on_button_clicked_step();
					}
				}
				break;
			case 'r':
				cout << "toggle range" << endl;
				m_Area.toggle_comRay();
				m_Area.queue_draw();
				break;
			case 'c':
				cout << "toggle link" << endl;
				m_Area.toggle_Connections();
				m_Area.queue_draw();
				break;
			case 'q':
				cout << "Exit" << endl;
				exit(0);
				break;
		}
	}

	return Gtk::Window::on_key_press_event(key_event);
}
