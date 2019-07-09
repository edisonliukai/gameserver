#include "GameMsg.h"
#include "msg.pb.h"


GameMsg::GameMsg()
{
}


GameMsg::~GameMsg()
{
	//�ͷŵ�ǰlist��ߵ����ж���
	for (auto &i : mMsgList)
	{
		delete i;
	}
}

std::string GameSingleTLV::serialize()
{
	std::string out;
	if (mPbMsg != nullptr)
	{
		out = mPbMsg->SerializeAsString();
	}
	return out;
}

GameSingleTLV::~GameSingleTLV()
{
	//�����Զ�����ͷ�
	if (mPbMsg != nullptr)
	{
		delete mPbMsg;
	}
}

GameSingleTLV::GameSingleTLV(GameMsgType type, std::string content)
	:m_MsgType(type)
{

	//���ݲ�ͬ����Ϣid,Ȼ�����л�Ϊ��ͬ��protobuf����
	switch (m_MsgType)
	{
	case GAME_MSG_LOGON_SYNCPID:
	{
		mPbMsg = new pb::SyncPid;
		break;
	}
	case GAME_MSG_TALK_CONTENT:
	{
		mPbMsg = new pb::Talk;
		break;
	}
	case GAME_MSG_NEW_POSTION:
	{
		mPbMsg = new pb::Position;
		break;
	}
	case GAME_MSG_BROADCAST:
	{
		mPbMsg = new pb::BroadCast;
		break;
	}
	case GAME_MSG_LOGOFF_SYNCPID:
	{
		mPbMsg = new pb::SyncPid;
		break;
	}
	case GAME_MSG_SUR_PLAYER:
	{
		mPbMsg = new pb::SyncPlayers;
		break;
	}
	default:
		break;
	}
	//�����л�
	mPbMsg->ParseFromString(content);
}
