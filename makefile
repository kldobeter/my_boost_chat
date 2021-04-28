client:
	g++ -O2 -std=c++14 -o client *.cpp *.cc -lboost_system -lboost_serialization -lpthread -lprotobuf
server:
	g++ -O2 -std=c++14 -o server *.cpp *.cc -lboost_system -lboost_serialization -lpthread -lprotobuf
