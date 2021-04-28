#include "server.h"
#include "client.h"

//
//using chat_message_queue = std::deque<chat_message>;
//
int test_server(int argc, char* argv[]) {
	try {
		if (argc < 2) {
			std::cerr << "Usage:chat_server <port> [<port> ...]" << std::endl;
			return 0;
		}

		boost::asio::io_service io_service;
		std::list<chat_server> servers;
		for (int i = 1;i < argc;++i) {
			boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(),atoi(argv[i]));//ip和port
			servers.emplace_back(io_service, endpoint);
		}
		io_service.run();
	}
	catch (const std::exception& e) {
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
	if (parseMessage1(input, &type, output)) {
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
int main(int argc, char* argv[]) {
//	test_server(argc, argv);
	test_client(argc, argv);

	return 0;
}
