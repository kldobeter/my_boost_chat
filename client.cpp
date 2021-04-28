#include "client.h"
#include "struct_header.h"

chat_client::chat_client(boost::asio::io_service& io, boost::asio::ip::tcp::resolver::iterator endpoint_iter)
	:m_io_service(io),m_socket(io)
{
	do_connect(endpoint_iter);
}

void chat_client::write(const chat_message& msg){
	//post操作是表示向io_service注册一个事件，让该事件在io_service的控制下去运行
	//因为在client测试代码中，io_service.run是在另一个线程去运行，而write操作却在main线程运行，
	//所以write操作使用了post操作
	m_io_service.post([this,msg]() {
		//first write
		auto b_is_write = !m_writemsgs.empty();
		m_writemsgs.emplace_back(msg);
		if (!b_is_write) {
			do_write();
		}
	});
}
//close操作也是在main线程中调用的，也需要向io_service注册。因此也调用了post操作
void chat_client::close() {
	m_io_service.post([this]() {
		m_socket.close();
	});
}

void chat_client::do_connect(boost::asio::ip::tcp::resolver::iterator endpoint_iter) {
	boost::asio::async_connect(m_socket, endpoint_iter,
		[this](const boost::system::error_code& ec,boost::asio::ip::tcp::resolver::iterator iter) {
			if(!ec)
				do_read_header();
		});
}

void chat_client::do_read_header() {
	boost::asio::async_read(m_socket,boost::asio::buffer(m_readmsg.data(),m_readmsg.header_length),
		[this](const boost::system::error_code& ec,std::size_t length) {
			if (!ec && m_readmsg.decode_header()) {
				do_read_body();
			}
			else {
				m_socket.close();
			}
		});
}
//void chat_client::print_readdata() {
//	std::cout.write(m_readmsg.body(), m_readmsg.body_length());
//	std::cout << std::endl;
//}
void chat_client::print_readdata() {
	if (m_readmsg.type() == MT_ROOM_INFO && m_readmsg.body_length() == sizeof(RoomInformation)) {
		const RoomInformation* info = reinterpret_cast<const RoomInformation*>(m_readmsg.body());
		//std::cout << "client: " << info->name.nameLen << "["<<info->name.name<<"]"<<std::endl;
        //std::cout << "["<<info->chat.infoLen<<"]"<<info->chat.information<<std::endl;
        std::cout<<"Client:['";
        std::cout.write(info->name.name, info->name.nameLen);
		std::cout << "']\n'";
		std::cout.write(info->chat.information,info->chat.infoLen);
		std::cout << "'\n";
	}else{
        std::cout<<"client not support this type:"<<m_readmsg.type()<<std::endl;
    }
}
void chat_client::do_read_body() {
	boost::asio::async_read(m_socket,boost::asio::buffer(m_readmsg.body(),m_readmsg.body_length()),
		[this](const boost::system::error_code& ec,std::size_t length) {
			if (!ec) {
				print_readdata();//输出读到的数据
				//
				do_read_header();
			}
			else {
				m_socket.close();
			}
		});
}
void chat_client::do_write() {
	boost::asio::async_write(m_socket,boost::asio::buffer(m_writemsgs.front().data(),m_writemsgs.front().length()),
		[this](const boost::system::error_code& ec,std::size_t length) {
			if (!ec) {//写成功后执行的操作
				m_writemsgs.pop_front();
				if (!m_writemsgs.empty()) {
					do_write();
				}
			}
			else {
				m_socket.close();
			}
		});
}
