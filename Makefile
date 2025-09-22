CXX = g++
CXXFLAGS = -Wall -std=c++17 -pthread

TARGET = chatroom
SRC = chatroom.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)


