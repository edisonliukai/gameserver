#include "GameRole.h"
#include "GameMsg.h"
#include "msg.pb.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <ctime>
#include "RandomName.h"
using namespace std;

static std::default_random_engine  g_random_creator(time(NULL));

static int gPlayerCount = 0;
GameRole::GameRole()
{
	mPlayerId = ++gPlayerCount;
	//������� player_1 player_2 ....
	//mPlayerName = string("Player_") + std::to_string(gPlayerCount);
	//ÿ�δ��������ӻ�ȡ����
	mPlayerName = RandomName::getInstance().getName();

	//�����ص�ָ��,������ �� 100,100
	x = g_random_creator()%20+160;
	y = 0;
	z = g_random_creator()%20+134;
	v = 0;

}


GameRole::~GameRole()
{
	RandomName::getInstance().releaseName(this->mPlayerName);
}

bool GameRole::Init()
{
	//��������ߵ�ʱ��,��ʱ�ͻᴥ��role init,������ʵ��һЩ��ʼ����ҵ��

	//1 ������ҵ�id�ź����������
	auto msg = MakeLogonSyncPid();
	//����Ϣ�ӵ�protocolЭ���
	ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)this->mProtocol);

	//2 ������ҳ�����
	msg = MakeInitPosBroadcast();
	ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)this->mProtocol);


	//����ǰ�����ӵ�AOI����
	AOI_world::GetWorld()->AddPlayer(this);

	
	//��ȡ�ܱ������Ϣ���һ��������protobuf��Ϣ,���͸��Լ�
	msg = MakeSurPlays();
	ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)this->mProtocol);

	//3 ��ǰ�������
	//��AOI�����ȡ�ܱ����
	auto plist = AOI_world::GetWorld()->GetSurPlayers(this);
	for (auto r : plist)
	{
		if (r == this)
			continue;
		auto role = dynamic_cast<GameRole*>(r);
		//3.1 �����Ѿ�����������,��������
		msg = MakeInitPosBroadcast();//������ǰ��ҵĳ�����Ϣ
		ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)role->mProtocol);

		//3.2 ��������������,������������xxx���
		//msg = role->MakeInitPosBroadcast();//������ǰ��ҵĳ�����Ϣ
		//ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)this->mProtocol);
	}


	return true;
}

UserData * GameRole::ProcMsg(UserData & _poUserData)
{
	//�ֽ�userdata ת�ص� gamemsg
	auto msg = dynamic_cast<GameMsg*>(&_poUserData);
	//ʵ�ֶԿͻ��˴��������protobuf���ݽ���ҵ����
	for (auto &single : msg->mMsgList)
	{
		switch(single->m_MsgType)
		{
		//����ƶ�����Ϣ
		case GameSingleTLV::GAME_MSG_NEW_POSTION:
		{
			auto pbmsg = dynamic_cast<pb::Position*>(single->mPbMsg);
			//cout << pbmsg->x() << " " << pbmsg->y() << " " << pbmsg->z() << endl;
			this->ProcNewPosition(
				pbmsg->x(),
				pbmsg->y(),
				pbmsg->z(),
				pbmsg->v()
			);
			break;
		}
		case GameSingleTLV::GAME_MSG_TALK_CONTENT:
		{
			pb::Talk* talkMsg = dynamic_cast<pb::Talk*>(single->mPbMsg);
			string content = talkMsg->content();
			this->ProcTalkContent(content);
			break;
		}
		}
	}
	return nullptr;
}

void GameRole::Fini()
{
	//��������ߵ�ʱ��ͻ����������
	//���������������������,��������
	auto plist = AOI_world::GetWorld()->GetSurPlayers(this);
	for (auto r : plist)
	{
		if (r == this)
			continue;
		auto role = dynamic_cast<GameRole*>(r);
		auto msg = MakeLogoffSyncPid();//������ǰ��ҵ�������Ϣ
		ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)role->mProtocol);

	}

	//������ߵ�ʱ��ҲҪ��AOI�������
	AOI_world::GetWorld()->DelPlayer(this);

}

void GameRole::ProcNewPosition(float _x, float _y, float _z, float _v)
{
	//ÿ������ƶ���ʱ�򴥷��������,������������ʵ��AOI�仯

	//���ж��������ƶ���û�иı����
	if (AOI_world::GetWorld()->GridChanged(this, _x, _z))
	{
		//������ӱ䶯��,��ʱ��������ʵ��AOI�ı任
		AOI_world::GetWorld()->DelPlayer(this);//����ǰ��ҴӾɵĸ��������
		//1 �Ȼ�ȡ�ɵ���Χ��ҵ��б�
		auto oldList = AOI_world::GetWorld()->GetSurPlayers(this);

		//��ֵ������ĳ�Ա�����µ�����
		this->x = _x;
		this->y = _y;
		this->z = _z;
		this->v = _v;

		//2 �ٻ�ȡ�µ���Χ��ҵ��б�
		auto newList = AOI_world::GetWorld()->GetSurPlayers(this);

		//������Ұ��ʧ
		this->ViewDisappear(oldList, newList);
		//������Ұ����
		this->ViewAppear(oldList, newList);
		
		AOI_world::GetWorld()->AddPlayer(this);//����ǰ�����ӵ��µĸ���
	}

	//�ȸ�ֵ������ĳ�Ա����
	this->x = _x;
	this->y = _y;
	this->z = _z;
	this->v = _v;


	//ͬ����ǰ��λ�ø�AOI�������ܱ����
	auto plist = AOI_world::GetWorld()->GetSurPlayers(this);
	for (auto r : plist)
	{
		if (r == this)
			continue;
		auto role = dynamic_cast<GameRole*>(r);
		auto msg = MakeNewPosBroadcast();
		ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)role->mProtocol);

	}
}

void GameRole::ProcTalkContent(std::string content)
{
	//��ȡ�����������,���͹㲥��Ϣ
	auto roleList = ZinxKernel::Zinx_GetAllRole();
	for (auto r : roleList)
	{
		auto role = dynamic_cast<GameRole*>(r);
		auto msg = MakeTalkBroadcast(content);
		ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)role->mProtocol);
	}
}

void GameRole::ViewDisappear(std::list<AOI_Player*>& oldList, std::list<AOI_Player*>& newList)
{
	//����������,����Ҫ����,ʹ��stl�������㷨,����ע�����������ǿ����������
	vector<AOI_Player*> vOld(oldList.begin(), oldList.end());
	vector<AOI_Player*> vNew(newList.begin(), newList.end());
	std::sort(vOld.begin(), vOld.end());
	std::sort(vNew.begin(), vNew.end());
	//�
	vector<AOI_Player*> diff;
	set_difference(vOld.begin(), vOld.end(), vNew.begin(), vNew.end(),
		std::inserter(diff, diff.begin()));
	for (auto r : diff)
	{
		auto role = dynamic_cast<GameRole*>(r);
		//��Ұ��ʧ,˫����Ҫ����һ�����ߵ���Ϣ
		auto msg = role->MakeLogoffSyncPid();
		ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)this->mProtocol);

		msg = this->MakeLogoffSyncPid();
		ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)role->mProtocol);
	}
}

void GameRole::ViewAppear(std::list<AOI_Player*>& oldList, std::list<AOI_Player*>& newList)
{
	//����������,����Ҫ����,ʹ��stl�������㷨,����ע�����������ǿ����������
	vector<AOI_Player*> vOld(oldList.begin(), oldList.end());
	vector<AOI_Player*> vNew(newList.begin(), newList.end());
	std::sort(vOld.begin(), vOld.end());
	std::sort(vNew.begin(), vNew.end());
	//�
	vector<AOI_Player*> diff;
	set_difference(vNew.begin(), vNew.end(), vOld.begin(), vOld.end(),
		std::inserter(diff, diff.begin()));
	for (auto r : diff)
	{
		auto role = dynamic_cast<GameRole*>(r);
		//��Ұ����,˫����Ҫ����һ�����ߵ���Ϣ
		auto msg = role->MakeInitPosBroadcast();
		ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)this->mProtocol);

		msg = this->MakeInitPosBroadcast();
		ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)role->mProtocol);
	}
}

GameMsg * GameRole::MakeLogonSyncPid()
{
	//�����ﹹ��һ���������ͬ�� id����Ϣ
	auto msg = new GameMsg;
	auto pbMsg = new pb::SyncPid;
	pbMsg->set_pid(this->mPlayerId);
	pbMsg->set_username(this->mPlayerName);
	auto single = new GameSingleTLV(GameSingleTLV::GAME_MSG_LOGON_SYNCPID, pbMsg);
	msg->mMsgList.push_back(single);
	return msg;
}

GameMsg * GameRole::MakeTalkBroadcast(std::string _talkContent)
{
	//������ʼλ�õĹ㲥��Ϣ
	auto msg = new GameMsg;
	auto pbMsg = new pb::BroadCast;
	pbMsg->set_pid(this->mPlayerId);
	pbMsg->set_username(this->mPlayerName);
	pbMsg->set_tp(1);
	//����һ���Ӷ���,�Զ���ӵ�pbMsg��protobuf�������
	pbMsg->set_content(_talkContent);
	auto single = new GameSingleTLV(GameSingleTLV::GAME_MSG_BROADCAST, pbMsg);
	msg->mMsgList.push_back(single);
	return msg;
}

GameMsg * GameRole::MakeInitPosBroadcast()
{
	//������ʼλ�õĹ㲥��Ϣ
	auto msg = new GameMsg;
	auto pbMsg = new pb::BroadCast;
	pbMsg->set_pid(this->mPlayerId);
	pbMsg->set_username(this->mPlayerName);
	pbMsg->set_tp(2);
	//����һ���Ӷ���,�Զ���ӵ�pbMsg��protobuf�������
	auto pos = pbMsg->mutable_p();
	pos->set_x(x);
	pos->set_y(y);
	pos->set_z(z);
	pos->set_v(v);
	auto single = new GameSingleTLV(GameSingleTLV::GAME_MSG_BROADCAST, pbMsg);
	msg->mMsgList.push_back(single);
	return msg;
}

//����һ���µ�λ����Ϣ
GameMsg * GameRole::MakeNewPosBroadcast()
{
	//������ʼλ�õĹ㲥��Ϣ
	auto msg = new GameMsg;
	auto pbMsg = new pb::BroadCast;
	pbMsg->set_pid(this->mPlayerId);
	pbMsg->set_username(this->mPlayerName);
	pbMsg->set_tp(4);
	//����һ���Ӷ���,�Զ���ӵ�pbMsg��protobuf�������
	auto pos = pbMsg->mutable_p();
	pos->set_x(this->x);
	pos->set_y(this->y);
	pos->set_z(this->z);
	pos->set_v(this->v);
	auto single = new GameSingleTLV(GameSingleTLV::GAME_MSG_BROADCAST, pbMsg);
	msg->mMsgList.push_back(single);
	return msg;
}

//��������Ǵ���һ�����ߵ���Ϣ
GameMsg * GameRole::MakeLogoffSyncPid()
{
	auto msg = new GameMsg;
	auto pbMsg = new pb::SyncPid;
	pbMsg->set_pid(this->mPlayerId);
	pbMsg->set_username(this->mPlayerName);
	auto single = new GameSingleTLV(GameSingleTLV::GAME_MSG_LOGOFF_SYNCPID, pbMsg);
	msg->mMsgList.push_back(single);
	return msg;
}

GameMsg * GameRole::MakeSurPlays()
{
	auto msg = new GameMsg;
	auto pbMsg = new pb::SyncPlayers;
	auto surPlayerList = AOI_world::GetWorld()->GetSurPlayers(this);
	for (auto r : surPlayerList)
	{
		auto role = dynamic_cast<GameRole*>(r);
		//����������һ��playerͬʱ���ظö���ָ��
		auto singPlayer = pbMsg->add_ps();
		singPlayer->set_pid(role->mPlayerId);
		singPlayer->set_username(role->mPlayerName);
		//����һ��position����,ͬʱ���ظ�ָ��
		auto pos = singPlayer->mutable_p();
		pos->set_x(role->x);
		pos->set_y(role->y);
		pos->set_z(role->z);
		pos->set_v(role->v);
	}
	auto single = new GameSingleTLV(GameSingleTLV::GAME_MSG_SUR_PLAYER, pbMsg);
	msg->mMsgList.push_back(single);
	return msg;
}
