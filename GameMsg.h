#pragma once
#include "zinx.h"
#include <list>
#include "msg.pb.h"

//������������洢һ���߼�����,���ճ��֮�������ĵ���һ��ҵ����
class GameSingleTLV
{
public:
	int gInt;
	//�����߼���Ϣ������
	enum GameMsgType {
		GAME_MSG_LOGON_SYNCPID = 1,
		GAME_MSG_TALK_CONTENT = 2,
		GAME_MSG_NEW_POSTION = 3,
		GAME_MSG_BROADCAST = 200,
		GAME_MSG_LOGOFF_SYNCPID = 201,
		GAME_MSG_SUR_PLAYER = 202,
	} m_MsgType;

	//����һ����������ָ��,�����洢��ͬ���͵�����
	::google::protobuf::Message *mPbMsg;

	std::string serialize();

	~GameSingleTLV();
	GameSingleTLV(GameMsgType type, std::string content);
	GameSingleTLV(GameMsgType type, ::google::protobuf::Message *pbmsg) :m_MsgType(type), mPbMsg(pbmsg) {}

};

class GameMsg :
	public UserData
{
public:
	GameMsg();
	virtual ~GameMsg();

	std::list<GameSingleTLV*> mMsgList;
};

