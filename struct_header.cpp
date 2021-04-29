#include "struct_header.h"
#include "serialize_object.h"
#include "json_object.h"
#include "chat_protocal.pb.h"
#include <cstring>
#include <cstdlib>
#include <iostream>

/**
 * @brief Parse the input data into a format output data  
 * @param [in]   const std::string& input:input format:“BindName dudu”,"Chat hello"
 * @param [out] int* type:MT_BIND_NAME,MT_CHAT_INFO,MT_ROOM_INFO
 * @param [out] std::string& output:different flag decide to genarate  different output format string
 * @param [in]   int flag:MFT_C_TRADITIONAL,MFT_SERIALIZATION,MFT_JSON,MFT_PROTOBUF
 * @return          bool:return true/false
*/
bool parseMessage(const std::string& input, int* type, std::string& output, int flag) {
	bool ret = false;
	auto pos = input.find_first_of(" ");//find the first space of the input string 
	if (pos == std::string::npos)//not find 
		return ret;
	if (pos == 0)
		return ret;
    
    //MsgFormatType mft_flag = MsgFormatType(flag);    
	//"BindName ok"->substr ->BindName
	//"Chat hello"->substr->Chat
	auto cmd = input.substr(0, pos);
	if (cmd == "BindName") {
		//we try to bind name 
		std::string name = input.substr(pos + 1);
		if (name.size() > 32)
			return ret;
		if (type)
			*type = MT_BIND_NAME;

		switch (flag)
		{
		case MFT_C_TRADITIONAL:{//use C struct BindName
			BindName bind_name;
			bind_name.nameLen = name.size();
			std::memcpy(&(bind_name.name), name.data(), name.size());
			auto buffer = reinterpret_cast<const char*>(&bind_name);
			output.assign(buffer, buffer + sizeof(bind_name));
			ret = true;
			break;
        }
		case MFT_SERIALIZATION://use boost serialization
			output = oserialize(SBindName(std::move(name)));
			ret = true;
			break;
        case MFT_JSON:{//use ptree, name string parse into json format{"name" : "ok"}
			ptree tree_name;
			tree_name.put("name", name);
			output = ptreeToJsonString(tree_name);
			ret = true;
			break;
		}
		case MFT_PROTOBUF:{//use google protobuf, serialize to string 
			PBindName bindname;
			bindname.set_name(name);
			ret = bindname.SerializeToString(&output);
			break;
         }

		}
	}
	else if (cmd == "Chat") {
		//we try to chat
		std::string info = input.substr(pos + 1);
		if (info.size() > 256)
			return ret;
		if (type)
			*type = MT_CHAT_INFO;
		switch (flag)
		{
		case MFT_C_TRADITIONAL:{//C struct ChatInformation
			ChatInformation chat;
			chat.infoLen = info.size();
			std::memcpy(&(chat.information), info.data(), info.size());
			auto buffer = reinterpret_cast<const char*> (&chat);
			output.assign(buffer, buffer + sizeof(chat));
			ret = true;
			break;
        }
		case MFT_SERIALIZATION://boost serialize
			output = oserialize(SBindName(std::move(info)));
			ret = true;
			break;
		case MFT_JSON: {// to json string format{"information" : "hello"}
			ptree tree_info;
			tree_info.put("information", info);
			output = ptreeToJsonString(tree_info);
			ret = true;
			break;
		}
			
		case MFT_PROTOBUF:{//use protobuf to serialization
			PChat pchat;
			pchat.set_information(info);
			ret = pchat.SerializeToString(&output);
			break;
         }
		}
	}
	return ret;
}

bool parseMessage1(const std::string& input, int* type, std::string& output) {
	auto pos = input.find_first_of(" ");//查找字符串中第一个空格
	if (pos == std::string::npos)//没找到
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
		//使用C struct BindName
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
		//使用C struct ChatInformation
		ChatInformation chat;
		chat.infoLen = info.size();
		std::memcpy(&(chat.information), info.data(),info.size());
		auto buffer = reinterpret_cast<const char*> (&chat);
		output.assign(buffer, buffer + sizeof(chat));
		return true;
	}

	return false;
}
//2.第二种方式，使用boost库的序列化，将输入的字符串 序列化 发送数据
bool parseMessage2(const std::string& input, int* type, std::string& output) {
	auto pos = input.find_first_of(" ");//查找字符串中第一个空格
	if (pos == std::string::npos)//没找到
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
		
		/*BindName bind_name;
		bind_name.nameLen = name.size();
		std::memcpy(&(bind_name.name), name.data(), name.size());
		auto buffer = reinterpret_cast<const char*>(&bind_name);
		output.assign(buffer, buffer + sizeof(bind_name));*/
		//使用序列化
		output = oserialize(SBindName(std::move(name)));
		return true;

	}
	else if (cmd == "Chat") {
		//we try to chat
		std::string info = input.substr(pos + 1);
		if (info.size() > 256)
			return false;
		if (type)
			*type = MT_CHAT_INFO;
	
		/*ChatInformation chat;
		chat.infoLen = info.size();
		std::memcpy(&(chat.information), info.data(), info.size());
		auto buffer = reinterpret_cast<const char*> (&chat);
		output.assign(buffer, buffer + sizeof(chat));*/
		//使用序列化
		output = oserialize(SChatInfo(std::move(info)));
		return true;
	}

	return false;
}

bool parseMessageToProto(const std::string& input, int* type, std::string& output) {
	auto pos = input.find_first_of(" ");//查找字符串中第一个空格
	if (pos == std::string::npos)//没找到
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
		//使用protobuf，将其序列化
		//使用.proto文件中定义的message
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
		//使用protobuf，将其序列化
		//使用.proto文件中定义的message
		PChat chat;
		chat.set_information(info);
		auto flag = chat.SerializeToString(&output);
		return flag;
	}

	return false;
}

bool parseMessageToJson(const std::string& input, int* type, std::string& output) {
	auto pos = input.find_first_of(" ");//查找字符串中第一个空格
	if (pos == std::string::npos)//没找到
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
		//使用ptree 将name 转化为json 格式{"name" : "ok"}
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
		//使用ptree 将chat 转化为json 格式{"information" : "hello"}
		ptree tree;
		tree.put("information", info);
		output = ptreeToJsonString(tree);
		return true;
	}

	return false;
}
