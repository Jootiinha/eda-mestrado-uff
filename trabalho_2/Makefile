CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2

TARGET = galpao
SRC = src/main.cpp src/Grafo.cpp
TIME_CSV = resultados/time_compare.csv
TIME_CSV_LARGE = resultados/time_compare_large.csv
TIME_REPETITIONS = 100000
TIME_REPETITIONS_LARGE = 20000

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) dados/vertices.txt dados/arestas.txt

run-time-compare: $(TARGET)
	mkdir -p resultados
	./$(TARGET) --benchmark dados/vertices.txt dados/arestas.txt $(TIME_CSV) $(TIME_REPETITIONS)

run-time-compare-large: $(TARGET)
	mkdir -p resultados
	./$(TARGET) --benchmark dados/vertices_maior.txt dados/arestas_maior.txt $(TIME_CSV_LARGE) $(TIME_REPETITIONS_LARGE)

clean:
	rm -f $(TARGET) $(TIME_CSV) $(TIME_CSV_LARGE)
