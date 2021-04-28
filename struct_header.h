#pragma once
#include <string>

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
bool parseMessage1(const std::string& input, int* type,std::string& output);
bool parseMessage2(const std::string& input, int* type, std::string& output);
bool parseMessageToJson(const std::string& input, int* type, std::string& output);
bool parseMessageToProto(const std::string& input, int* type, std::string& output);
