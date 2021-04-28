#include "struct_header.h"
#include "serialize_object.h"
#include "json_object.h"
#include "chat_protocal.pb.h"
#include <cstring>
#include <cstdlib>
#include <iostream>

bool parseMessage1(const std::string& input, int* type, std::string& output) {
	auto pos = input.find_first_of(" ");//�����ַ����е�һ���ո�
	if (pos == std::string::npos)//û�ҵ�
		return false;
	if (pos == 0)
		return false;
	//"BindName ok"->substr ->BindName
	//"Chat hello"->substr->Chat
	auto cmd = input.substr(0, pos);
	if (cmd == "BindName") {
		//we try to bind name 
		std::string name = input.substr(pos + 1);
		if (name.size() > 32)
			return false;
		if (type)
			*type = MT_BIND_NAME;
		//ʹ��C struct BindName
		BindName bind_name;
		bind_name.nameLen = name.size();
		std::memcpy(&(bind_name.name), name.data(), name.size());
		auto buffer = reinterpret_cast<const char*>(&bind_name);
		output.assign(buffer, buffer + sizeof(bind_name));
        return true;

	}
	else if (cmd == "Chat") {
		//we try to chat
		std::string info = input.substr(pos + 1);
		if (info.size() > 256)
			return false;
		if (type)
			*type = MT_CHAT_INFO;
		//ʹ��C struct ChatInformation
		ChatInformation chat;
		chat.infoLen = info.size();
		std::memcpy(&(chat.information), info.data(),info.size());
		auto buffer = reinterpret_cast<const char*> (&chat);
		output.assign(buffer, buffer + sizeof(chat));
		return true;
	}

	return false;
}

bool parseMessage2(const std::string& input, int* type, std::string& output) {
    return true;
}
template <typename T>
std::string serialize(const T& obj) {
	std::stringstream ss;
	boost::archive::text_oarchive oa(ss);
	oa & obj;
	return ss.str();
}

bool parseMessageToProto(const std::string& input, int* type, std::string& output) {
	auto pos = input.find_first_of(" ");//�����ַ����е�һ���ո�
	if (pos == std::string::npos)//û�ҵ�
		return false;
	if (pos == 0)
		return false;
	//"BindName ok"->substr ->BindName
	//"Chat hello"->substr->Chat
	auto cmd = input.substr(0, pos);
	if (cmd == "BindName") {
		//we try to bind name 
		std::string name = input.substr(pos + 1);
		if (name.size() > 32)
			return false;
		if (type)
			*type = MT_BIND_NAME;
		//ʹ��protobuf���������л�
		//ʹ��.proto�ļ��ж����message
		PBindName bindname;
		bindname.set_name(name);
		auto flag = bindname.SerializeToString(&output);
		return flag;
	}
	else if (cmd == "Chat") {
		//we try to chat
		std::string info = input.substr(pos + 1);
		if (info.size() > 256)
			return false;
		if (type)
			*type = MT_CHAT_INFO;
		//ʹ��protobuf���������л�
		//ʹ��.proto�ļ��ж����message
		PChat chat;
		chat.set_information(info);
		auto flag = chat.SerializeToString(&output);
		return flag;
	}

	return false;
}

/*input�ĸ�ʽ��
BindName dudu
Chat hello
*/
/*type��ȡֵ:MT_BIND_NAME,MT_CHAT_INFO,MT_ROOM_INFO*/
/*output�ĸ�ʽ��json string
{��name�� : "dudu"}
{"information" : "hello"}
{"name" : "dudu" , "information" : "hello"}
*/
bool parseMessageToJson(const std::string& input, int* type, std::string& output) {
	auto pos = input.find_first_of(" ");//�����ַ����е�һ���ո�
	if (pos == std::string::npos)//û�ҵ�
		return false;
	if (pos == 0)
		return false;
	//"BindName ok"->substr ->BindName
	//"Chat hello"->substr->Chat
	auto cmd = input.substr(0, pos);
	if (cmd == "BindName") {
		//we try to bind name 
		std::string name = input.substr(pos + 1);
		if (name.size() > 32) 
			return false;
		if(type)
			*type = MT_BIND_NAME;
		//ʹ��ptree ��name ת��Ϊjson ��ʽ{"name" : "ok"}
		ptree tree;
		tree.put("name", name);
		output = ptreeToJsonString(tree);
		return true;
	}
	else if (cmd == "Chat") {
		//we try to chat
		std::string info = input.substr(pos + 1);
		if (info.size() > 256) 
			return false;
		if(type)
			*type = MT_CHAT_INFO;
		//ʹ��ptree ��chat ת��Ϊjson ��ʽ{"information" : "hello"}
		ptree tree;
		tree.put("information", info);
		output = ptreeToJsonString(tree);
		return true;
	}

	return false;
}
