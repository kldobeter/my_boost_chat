#pragma once
//自定义头文件
#include "chat_message.h"
#include "struct_header.h"
//第三方库头文件
#include <boost/asio.hpp>
//c++库头文件
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
//c库头文件
#include <cstdlib>

//typedef std::shared_ptr<chat_session> chat_participant_ptr;
class chat_session;

class chat_room {
	using chat_participant_ptr = std::shared_ptr<chat_session>;
public:
	void join(chat_participant_ptr participant);
	void leave(chat_participant_ptr participant);
	void deliver(const chat_message& msg);

private:
	std::set<chat_participant_ptr> m_participants;//该聊天室的session集合
	enum { max_recent_msgs = 1024 };
	std::deque<chat_message> m_recent_msgs;//存放历史消息
};
class chat_session :public std::enable_shared_from_this<chat_session> {
public:
	chat_session(boost::asio::ip::tcp::socket socket, chat_room& room);
	//开始一个会话：
	/*1.该聊天室会加入该session;
	   2.准备读取该聊天室中的历史消息
	*/
	void start();
	//向聊天室发送消息
	void deliver(const chat_message& msg);
private:
	//处理读写操作
	void do_read_header();
	void do_read_body();
	void do_write();

	//处理接收消息
	void handle_message();
	RoomInformation buildRoomInfo() const;

	boost::asio::ip::tcp::socket m_socket;
	chat_room& m_room;//该地方使用引用，表明chat_session比chat_room对象的生命周期短
	chat_message m_readmsg;
	std::deque<chat_message> m_writemsg;
	//使用消息协议，增加m_bindname,m_chatinfo字段
	std::string m_bindname;
	std::string m_chatinfo;
};

//负责接收客户端
class chat_server {
public:
	chat_server(boost::asio::io_service& io,const boost::asio::ip::tcp::endpoint& endpoint);

private:
	void do_accept();

	boost::asio::ip::tcp::socket m_socket;
	boost::asio::ip::tcp::acceptor m_accept;
	chat_room m_room;
};