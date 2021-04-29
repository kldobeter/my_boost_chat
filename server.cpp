#include "server.h"
#include "struct_header.h"
#include "serialize_object.h"

void chat_room::join(chat_participant_ptr participant) 
{//有session加入该room
    std::cout<<"one session join the room,recent_msg size="<<m_recent_msgs.size()<<std::endl;
	m_participants.insert(participant);
	for (const auto& msg : m_recent_msgs)
		participant->deliver(msg);

}
void chat_room::leave(chat_participant_ptr participant) 
{//有session离开该room
    std::cout<<"one session leave the room"<<std::endl;
	m_participants.erase(participant);
}
void chat_room::deliver(const chat_message& msg) 
{
	m_recent_msgs.emplace_back(msg);
	while (m_recent_msgs.size() > max_recent_msgs) 
	{
		m_recent_msgs.pop_front();
	}
	//若是在room中有一个人发消息，则room中的其他人都能够收到消息
	for (auto& participant : m_participants)
		participant->deliver(msg);
}

chat_session::chat_session(boost::asio::ip::tcp::socket socket, chat_room& room)
	:m_socket(std::move(socket)), m_room(room) {}

void chat_session::start() {
	m_room.join(shared_from_this());
	do_read_header();
}

void chat_session::deliver(const chat_message& msg) {
	bool b_is_write = !m_writemsg.empty();
	m_writemsg.emplace_back(msg);
	if (!b_is_write) {
		do_write();
	}
}
void chat_session::do_read_header() {
	auto self(shared_from_this());
	boost::asio::async_read(m_socket, boost::asio::buffer(m_readmsg.data(), m_readmsg.header_length),
		[this, self](const boost::system::error_code& ec, std::size_t length) {
			if (!ec && m_readmsg.decode_header()) {//m_readmsg需要使用this
				do_read_body();
			}
			else {
				m_room.leave(self);
			}
		});
}
void chat_session::do_read_body() {
	auto self(shared_from_this());
	boost::asio::async_read(m_socket, boost::asio::buffer(m_readmsg.body(), m_readmsg.body_length()),
		[this, self](const boost::system::error_code& ec, std::size_t length) {
			if (!ec) {//读取一个完整的消息header+body
				//m_room.deliver(m_readmsg);
				//处理收到的数据包，经过解析处理，再发送给server
				//handle_message(MFT_C_TRADITIONAL);
				handle_message(MFT_SERIALIZATION);
				do_read_header();
			}
			else {
				m_room.leave(self);
			}
		}
	);
}
RoomInformation chat_session::buildRoomInfo() const {
	RoomInformation info{0};
	info.name.nameLen = m_bindname.size();
	std::memcpy(info.name.name, m_bindname.data(), m_bindname.size());
	info.chat.infoLen = m_chatinfo.size();
	std::memcpy(info.chat.information, m_chatinfo.data(), m_chatinfo.size());
	return info;
}
std::string chat_session::buildRoomInfoStr() const {
	return oserialize(SRoomInfo(std::move(m_bindname),std::move(m_chatinfo)));
}

void chat_session::handle_message(int flag) {
	auto type = m_readmsg.type();
	if (type == MT_BIND_NAME)
	{
		if (flag == MFT_C_TRADITIONAL) {
			const BindName* name = reinterpret_cast<const BindName*>(m_readmsg.body());
			m_bindname.assign(name->name, name->name + name->nameLen);
		}
		else if (flag == MFT_SERIALIZATION) {
			SBindName bindname;
			iserialize(bindname, std::string(m_readmsg.body(), m_readmsg.body()+m_readmsg.body_length()));
			m_bindname = bindname.get_bindname();
		}
		else if (flag == MFT_JSON) {

		}
		else if (flag == MFT_PROTOBUF) {

		}
		else {}

	}
	else if (type == MT_CHAT_INFO) {
		//只有chat info才需要发送给服务器，绑定name的时候只需要绑定就好
		if (flag == MFT_C_TRADITIONAL) {
			const ChatInformation* info = reinterpret_cast<const ChatInformation*>(m_readmsg.body());
			m_chatinfo.assign(info->information, info->information + info->infoLen);
			auto room = buildRoomInfo();
			chat_message msg;
			msg.encode_message(MT_ROOM_INFO, &room, sizeof(room));
			m_room.deliver(msg);//broadcast
		}
		else if (flag == MFT_SERIALIZATION) {
			SChatInfo chatinfo;
			iserialize(chatinfo, std::string(m_readmsg.body(), m_readmsg.body() + m_readmsg.body_length()));
			m_chatinfo = chatinfo.get_chatinfo();
			auto room = buildRoomInfoStr();
			chat_message msg;
			msg.encode_message(MT_ROOM_INFO, room);
			m_room.deliver(msg);
		}
		else if (flag == MFT_JSON) {

		}
		else if (flag == MFT_PROTOBUF) {

		}
		else {}
		//
		

	}
	else {
        std::cout<<"server receive not vaild msg:[type="<<type<<"]"<<std::endl;
		//not valid msg ,do nothing for now
	}
	//
}

void chat_session::do_write() {
	auto self(shared_from_this());
	boost::asio::async_write(m_socket,
		boost::asio::buffer(m_writemsg.front().data(), m_writemsg.front().length()),
		[this, self](const boost::system::error_code& ec, std::size_t length) {
			if (!ec) {
				m_writemsg.pop_front();
				if (!m_writemsg.empty()) {
					do_write();
				}
				
			}else {
					m_room.leave(self);
				}
		});
}

chat_server::chat_server(boost::asio::io_service& io,const boost::asio::ip::tcp::endpoint& endpoint)
	:m_socket(io), m_accept(io, endpoint) {
	do_accept();
}

void chat_server::do_accept() {
	m_accept.async_accept(m_socket,
		[this](const boost::system::error_code& ec) {
			if (!ec) {
				auto session = std::make_shared<chat_session>(std::move(m_socket), m_room);
				session->start();
			}
			do_accept();
		});
}
