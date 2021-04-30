client:
	g++ -O2 -std=c++14 -o client *.cpp *.cc -lboost_system -lboost_serialization -lpthread -lprotobuf
server:
	g++ -DBOOST_LOG_DYN_LINK -O2 -std=c++14 -o server *.cpp *.cc -lboost_system -lboost_serialization -lboost_log -lboost_log_setup -lpthread -lprotobuf
