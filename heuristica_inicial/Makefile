CXX = g++
CAMINHO_GVNS = ..
CXXFLAGS = -O3 -std=c++17 -Wall -Wextra -Wpedantic -MMD -MP \
	-I$(CAMINHO_GVNS)

ALVOS = grau_relativo grau_relativo_gatilho
OBJETOS_COMUNS = Graph.o HeuristicaInicial.o
DEPENDENCIAS = Graph.d HeuristicaInicial.d grau_relativo.d \
	grau_relativo_gatilho.d

all: $(ALVOS)

grau_relativo: grau_relativo.o $(OBJETOS_COMUNS)
	$(CXX) $(CXXFLAGS) -o $@ $^

grau_relativo_gatilho: grau_relativo_gatilho.o $(OBJETOS_COMUNS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

Graph.o: $(CAMINHO_GVNS)/Graph.cpp $(CAMINHO_GVNS)/Graph.h
	$(CXX) $(CXXFLAGS) -c $(CAMINHO_GVNS)/Graph.cpp -o $@

clean:
	rm -f $(ALVOS) *.o *.d

-include $(DEPENDENCIAS)
