#pragma once
#include "serialize_object.h"
#include <sstream>
#include <string>

enum MsgFormatType {
	MFT_C_TRADITIONAL = 1,//传统C struct格式传输消息
	MFT_SERIALIZATION = 2,//使用boost序列化格式传递消息
	MFT_JSON = 3,//json 格式
	MFT_PROTOBUF = 4,//google的protobuf格式序列化
};

//1.使用传统C结构 设计消息协议
struct Header {
	int bodySize;
	int type;
};
enum MessageType {
	MT_BIND_NAME = 1,//{"name" : "dudu"}
	MT_CHAT_INFO  = 2,//{"information" : "what i'm say"}
	MT_ROOM_INFO = 3,//{"name" : "dudu" , "information" : "what i'm say"}
};

//client send
struct BindName {
	char name[32];
	int nameLen;
};

//client send
struct ChatInformation {
	char information[256];
	int infoLen;
};

//server send
struct RoomInformation {
	BindName name;
	ChatInformation chat;
};

//parseMessage
bool parseMessage(const std::string& input, int* type, std::string& output, int flag);
//bool parseMessage1(const std::string& input, int* type,std::string& output);
//bool parseMessage2(const std::string& input, int* type, std::string& output);
//bool parseMessageToJson(const std::string& input, int* type, std::string& output);
//bool parseMessageToProto(const std::string& input, int* type, std::string& output);

template <typename T>
void iserialize(T& obj, const std::string& str) {
	std::stringstream ss(str);
	boost::archive::text_iarchive ia(ss);
	ia& obj;
}

template <typename T>
std::string oserialize(const T& obj) {
	std::stringstream ss;
	boost::archive::text_oarchive oa(ss);
	oa& obj;
	return ss.str();
}
