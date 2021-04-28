#pragma once
//�Զ���ͷ�ļ�
#include "chat_message.h"
#include "struct_header.h"
//��������ͷ�ļ�
#include <boost/asio.hpp>
//c++��ͷ�ļ�
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
//c��ͷ�ļ�
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
	std::set<chat_participant_ptr> m_participants;//�������ҵ�session����
	enum { max_recent_msgs = 1024 };
	std::deque<chat_message> m_recent_msgs;//�����ʷ��Ϣ
};
class chat_session :public std::enable_shared_from_this<chat_session> {
public:
	chat_session(boost::asio::ip::tcp::socket socket, chat_room& room);
	//��ʼһ���Ự��
	/*1.�������һ�����session;
	   2.׼����ȡ���������е���ʷ��Ϣ
	*/
	void start();
	//�������ҷ�����Ϣ
	void deliver(const chat_message& msg);
private:
	//�����д����
	void do_read_header();
	void do_read_body();
	void do_write();

	//���������Ϣ
	void handle_message();
	RoomInformation buildRoomInfo() const;

	boost::asio::ip::tcp::socket m_socket;
	chat_room& m_room;//�õط�ʹ�����ã�����chat_session��chat_room������������ڶ�
	chat_message m_readmsg;
	std::deque<chat_message> m_writemsg;
	//ʹ����ϢЭ�飬����m_bindname,m_chatinfo�ֶ�
	std::string m_bindname;
	std::string m_chatinfo;
};

//������տͻ���
class chat_server {
public:
	chat_server(boost::asio::io_service& io,const boost::asio::ip::tcp::endpoint& endpoint);

private:
	void do_accept();

	boost::asio::ip::tcp::socket m_socket;
	boost::asio::ip::tcp::acceptor m_accept;
	chat_room m_room;
};