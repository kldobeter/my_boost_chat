client:
	g++ -O2 -std=c++14 -o $@ *.cpp *.cc -lboost_system -lboost_serialization -lpthread -lprotobuf
server:
	g++ -DBOOST_LOG_DYN_LINK -O2 -std=c++14 -o $@ *.cpp *.cc -lboost_system -lboost_log -lboost_log_setup -lboost_thread -lboost_serialization -lpthread -lprotobuf
server-debug:
	g++ -DBOOST_LOG_DYN_LINK -g -ggdb -std=c++14 -o $@ *.cpp *.cc -lboost_system -lboost_log -lboost_log_setup -lboost_thread -lboost_serialization -lpthread -lprotobuf


clean:
	rm -rf client server server-debug

