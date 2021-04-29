#pragma once
#include "struct_header.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>


//server -> client / client ->server  message 格式 {header,body}
//使用header + body的格式传输数据
//header（存放的是消息的长度） + body（可变（最大512个字节），存放的是消息）
class chat_message {
public:
	enum { header_length =sizeof(Header)  };//4
	enum { max_body_length = 512 };
	chat_message(){}
	
	const char* data() const { return m_data; }
	char* data() { return m_data; }

	const char* body() const { return m_data + header_length; }
	char* body() { return m_data + header_length; }

	std::size_t length() const { return header_length + m_header.bodySize; };//m_body_length; }
	std::size_t body_length() const { return m_header.bodySize; }

	int type()const { return m_header.type; }

	void body_length(std::size_t new_len) {
		m_header.bodySize = (new_len > max_body_length ? max_body_length : new_len);
		//m_body_length = (new_len > max_body_length ? max_body_length : new_len);
	}
	void encode_message(int msg_type, const void* buf, size_t buflen) {
		assert(buflen < max_body_length);
		m_header.bodySize = buflen;
		m_header.type = msg_type;
		std::memcpy(body(), buf, buflen);
		std::memcpy(data(),&m_header,sizeof(m_header));
    }

    void encode_message(int msg_type,const std::string& str){
        encode_message(msg_type,str.data(),str.size());
    }

	//将数据的长度放在m_data
	void encode_header() {
		char header[header_length + 1]={ 0 };
		sprintf(m_data, "%4d", static_cast<int> (m_header.bodySize));
		std::memcpy(m_data, header, header_length);
	}
	//从m_data解析出数据长度,放在m_body_length
	bool decode_header() {
		std::memcpy(&m_header, data(), header_length);
		if (m_header.bodySize > max_body_length) {
			std::cerr << "decode header failed,body size:" << m_header.bodySize << ",type:" << m_header.type << std::endl;
			m_header.bodySize = 0;
			return false;
		}
		return true;
		/*char header[header_length + 1] = { 0 };
		strncat(header, m_data, header_length);
		m_body_length = atoi(header);
		if (m_body_length > max_body_length) {
			m_body_length = 0;
			return false;
		}
		return true;
		*/
	}

private:
	char m_data[header_length + max_body_length];
	Header m_header;
	//std::size_t m_body_length;
};
