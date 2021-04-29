#pragma once
#include "chat_message.h"
#include <boost/asio.hpp>
#include <deque>
#include <iostream>
#include <thread>
#include <cstdlib>

class chat_client {
public:
	chat_client(boost::asio::io_service& io, boost::asio::ip::tcp::resolver::iterator endpoint_iter);

	void write(const chat_message& msg);
	void close();
private:
	void do_connect(boost::asio::ip::tcp::resolver::iterator endpoint_iter);
	void do_write();

	void do_read_header();
	void do_read_body();
	void print_readdata(int flag);

	boost::asio::io_service& m_io_service;
	boost::asio::ip::tcp::socket m_socket;
	chat_message m_readmsg;
	std::deque<chat_message> m_writemsgs;
};
