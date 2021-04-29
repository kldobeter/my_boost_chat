#pragma once
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
/*ʹ��boost�⣬���л�*/

class SBindName {
public:
	SBindName() {}
	SBindName(std::string name):m_bindname(std::move(name)){}
	const std::string& get_bindname()const { return m_bindname; }

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& m_bindname;//�൱���������ֲ���
		//ar << m_bindname;
		//ar >> m_bindname;
	}
	std::string m_bindname;
};
class SChatInfo {
public:
	SChatInfo() {}
	SChatInfo(std::string info) :m_chatinfo(std::move(info)) {}
	const std::string& get_chatinfo()const { return m_chatinfo; }

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& m_chatinfo;//�൱���������ֲ���
		//ar << m_bindname;
		//ar >> m_bindname;
	}
	std::string m_chatinfo;
};
class SRoomInfo {
public:
	SRoomInfo() {}
	SRoomInfo(std::string name,std::string info) 
		:m_bind(std::move(name)),m_chat(std::move(info)) {}
	const std::string& get_name()const { return m_bind.get_bindname(); }
	const std::string& get_info()const { return m_chat.get_chatinfo(); }


private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& m_bind;//�൱���������ֲ���
		//ar << m_bind;
		//ar >> m_bind;
		ar& m_chat;
		//ar << m_chat;
		//ar >> m_chat;
	}

	SBindName m_bind;
	SChatInfo m_chat;
};