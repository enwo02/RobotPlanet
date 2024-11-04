OUT = projet
CXX = g++
CXXFLAGS = -Wall -std=c++11 -O3 -w
LINKING = `pkg-config --cflags gtkmm-3.0`
LDLIBS = `pkg-config --libs gtkmm-3.0`
OFILES = projet.o geomod.o utilities.o base.o robot.o gisement.o message.o \
		 simulation.o graphic.o gui.o

all: $(OUT)

geomod.o: geomod.cc geomod.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

utilities.o: utilities.cc utilities.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

graphic.o: graphic.cc graphic.h settings.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

gui.o: gui.cc gui.h settings.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

base.o: base.cc base.h geomod.h gisement.h robot.h utilities.h message.h \
  constantes.h settings.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

robot.o: robot.cc robot.h geomod.h utilities.h message.h constantes.h settings.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

gisement.o: gisement.cc constantes.h settings.h gisement.h geomod.h utilities.h \
  message.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

message.o: message.cc message.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

simulation.o: simulation.cc simulation.h base.h geomod.h gisement.h \
  robot.h utilities.h message.h settings.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)


projet.o: projet.cc simulation.h constantes.h settings.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

$(OUT): $(OFILES)
	$(CXX) $(CXXFLAGS) $(LINKING) $(OFILES) -o $@ $(LDLIBS)

clean:
	@echo "Cleaning compilation files"
	@rm *.o $(OUT) *.cc~ *.h~
