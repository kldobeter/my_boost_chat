#include "server.h"
#include "client.h"
#include "chat_protocal.pb.h"
//log output
#include <boost/log/trivial.hpp>
//set the severity level
#include <boost/log/expressions.hpp>
//put the output log into the file
#include <boost/log/utility/setup/file.hpp>

#include <vector>
//
//
static std::function<void()> SafeHandler;
void signalHandler(int signo){
    BOOST_LOG_TRIVIAL(info)<<"handle system signal:"<<signo;
    if(SafeHandler){
        SafeHandler();
        SafeHandler = nullptr;
    }
}

//using chat_message_queue = std::deque<chat_message>;
//
int test_server(int argc, char* argv[]) {
	try {
		
		if (argc < 2) {
			std::cerr << "Usage:chat_server <port> [<port> ...]" << std::endl;
			return 0;
		}

		boost::asio::io_service io_service;

        //catch signal
        SafeHandler = [&io_service]{io_service.stop();};
        signal(SIGINT,signalHandler);

		std::list<chat_server> servers;
		for (int i = 1;i < argc;++i) {
			boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(),atoi(argv[i]));//ip和port
			servers.emplace_back(io_service, endpoint);
		}

		std::vector<std::thread> thread_pool;
		int threadnum = std::thread::hardware_concurrency();
		for (int i = 0;i < threadnum + 1;++i) {
			thread_pool.emplace_back([&io_service]() {io_service.run();});
		}
		io_service.run();

        for(auto& thread : thread_pool){
            thread.join();
        }

	}
	catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error)<<"Exception:"<<e.what()<<std::endl;
		std::cerr << e.what() << std::endl;
	}

	return 1;
}
void pro_input_message(char* line,chat_client& client) {
	chat_message chat;
	chat.body_length(std::strlen(line));
	std::memcpy(chat.body(), line, chat.body_length());
	chat.encode_header();
	client.write(chat);
}
void pro_input_message1(char* line, chat_client& client) {
	auto type = 0;
	chat_message chat;
	
	std::string input(line, line + std::strlen(line));
	std::string output;
	//if (parseMessage(input, &type, output, MFT_C_TRADITIONAL)) {
	//if (parseMessage(input, &type, output, MFT_SERIALIZATION)) {
	//if (parseMessage(input, &type, output, MFT_JSON)) {
	if (parseMessage(input, &type, output, MFT_PROTOBUF)) {
		chat.encode_message(type, output.data(), output.size());
		client.write(chat);
		std::cout << "Client send :[" << output.size() << "]" <<"type:"<<type<< std::endl;
	}
}
void test_client(int argc, char* argv[]) {
	try {
		if (argc != 3) {
			std::cerr << "Usage:chat_client <host> <port>" << std::endl;
			return;
		}
		boost::asio::io_service io_service;
		boost::asio::ip::tcp::resolver resolver(io_service);
		auto endpoint_iter = resolver.resolve({ argv[1],argv[2] });
		chat_client client(io_service, endpoint_iter);
		
		//该线程主要处理io
		std::thread t([&io_service](){io_service.run();});

		//主线程处理输入：
		char line[chat_message::max_body_length + 1];
		while (std::cin.getline(line, chat_message::max_body_length + 1)) {
			//将client在命令行输入消息发送给server
			/*pro_input_message(line,client);*/
			pro_input_message1(line, client);

		}
		////////////////////////////////////////////////////////////////////////////////////////
		client.close();
		t.join();
	}
	catch (const std::exception& e) {
		std::cerr <<"Exception: " <<e.what() << std::endl;
	}

}
//设置日志输出到文件
void setlogintofile(){
    boost::log::add_file_log("output.log");
}

//设置日志打印级别
void setseveritylevel(){
    //level >=info ,it can output
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::info        
    );
}
int main(int argc, char* argv[]) {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

    //setlogintofile();
    setseveritylevel();
    //log
    BOOST_LOG_TRIVIAL(trace)<<"A trace severity message!";
    BOOST_LOG_TRIVIAL(debug)<<"A debug severity message!";
    BOOST_LOG_TRIVIAL(info)<<"A info severity message!";
    BOOST_LOG_TRIVIAL(warning)<<"A warning severity message!";
    BOOST_LOG_TRIVIAL(error)<<"A error severity message!";
    BOOST_LOG_TRIVIAL(fatal)<<"A fatal severity message!";

    //
	test_server(argc, argv);
	//test_client(argc, argv);

    //
	//释放protobuf内存
	google::protobuf::ShutdownProtobufLibrary();
    BOOST_LOG_TRIVIAL(info)<<"safe release all resources!";
	
    return 0;
}
