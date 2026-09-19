CXX = g++
CXXFLAGS = -O3 -std=c++17 -march=native -mtune=native -Wall -Wextra \
	-Wpedantic -MMD -MP -I. -Iheuristica_inicial

ALVO = gvns-2queima
ALVO_TESTE = test_gvns

OBJETOS = gvns-2queima.o GVNS.o VND.o Vizinhancas.o Avaliador.o \
	Graph.o HeuristicaInicial.o
OBJETOS_SEM_PROGRAMA = GVNS.o VND.o Vizinhancas.o Avaliador.o \
	Graph.o HeuristicaInicial.o

DEPENDENCIAS = $(OBJETOS:.o=.d) tests/test_gvns.d

all: $(ALVO)

$(ALVO): $(OBJETOS)
	$(CXX) $(CXXFLAGS) -o $@ $^

teste: $(ALVO_TESTE)
	./$(ALVO_TESTE)

$(ALVO_TESTE): tests/test_gvns.o $(OBJETOS_SEM_PROGRAMA)
	$(CXX) $(CXXFLAGS) -o $@ $^

tests/test_gvns.o: tests/test_gvns.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

Graph.o: Graph.cpp Graph.h
	$(CXX) $(CXXFLAGS) -c Graph.cpp -o $@

HeuristicaInicial.o: heuristica_inicial/HeuristicaInicial.cpp \
		heuristica_inicial/HeuristicaInicial.h SimulacaoQueima.h
	$(CXX) $(CXXFLAGS) -c heuristica_inicial/HeuristicaInicial.cpp -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(ALVO) $(ALVO_TESTE) *.o *.d tests/*.o tests/*.d \
		resultados_gvns.csv

-include $(DEPENDENCIAS)
