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
	//玩家名字 player_1 player_2 ....
	//mPlayerName = string("Player_") + std::to_string(gPlayerCount);
	//每次从姓名池子获取名字
	mPlayerName = RandomName::getInstance().getName();

	//出生地的指定,比如是 在 100,100
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
	//当玩家上线的时候,此时就会触发role init,在这里实现一些初始化的业务

	//1 发送玩家的id号和姓名给玩家
	auto msg = MakeLogonSyncPid();
	//将消息扔到protocol协议层
	ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)this->mProtocol);

	//2 告诉玩家出生地
	msg = MakeInitPosBroadcast();
	ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)this->mProtocol);


	//将当前玩家添加到AOI世界
	AOI_world::GetWorld()->AddPlayer(this);

	
	//获取周边玩家信息组成一个单独的protobuf消息,发送给自己
	msg = MakeSurPlays();
	ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)this->mProtocol);

	//3 当前玩家上线
	//从AOI世界获取周边玩家
	auto plist = AOI_world::GetWorld()->GetSurPlayers(this);
	for (auto r : plist)
	{
		if (r == this)
			continue;
		auto role = dynamic_cast<GameRole*>(r);
		//3.1 告诉已经在世界的玩家,我上线了
		msg = MakeInitPosBroadcast();//产生当前玩家的出生信息
		ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)role->mProtocol);

		//3.2 告诉这个上线玩家,本来世界上有xxx玩家
		//msg = role->MakeInitPosBroadcast();//产生当前玩家的出生信息
		//ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)this->mProtocol);
	}


	return true;
}

UserData * GameRole::ProcMsg(UserData & _poUserData)
{
	//现将userdata 转回到 gamemsg
	auto msg = dynamic_cast<GameMsg*>(&_poUserData);
	//实现对客户端传输过来的protobuf数据进行业务处理
	for (auto &single : msg->mMsgList)
	{
		switch(single->m_MsgType)
		{
		//玩家移动的消息
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
	//当玩家下线的时候就会在这里调用
	//告诉这个世界里边所有玩家,我下线了
	auto plist = AOI_world::GetWorld()->GetSurPlayers(this);
	for (auto r : plist)
	{
		if (r == this)
			continue;
		auto role = dynamic_cast<GameRole*>(r);
		auto msg = MakeLogoffSyncPid();//产生当前玩家的下线消息
		ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)role->mProtocol);

	}

	//玩家下线的时候也要从AOI世界清除
	AOI_world::GetWorld()->DelPlayer(this);

}

void GameRole::ProcNewPosition(float _x, float _y, float _z, float _v)
{
	//每次玩家移动的时候触发这个函数,在这个函数里边实现AOI变化

	//先判断玩家这次移动有没有改变格子
	if (AOI_world::GetWorld()->GridChanged(this, _x, _z))
	{
		//如果格子变动了,此时就在这里实现AOI的变换
		AOI_world::GetWorld()->DelPlayer(this);//将当前玩家从旧的格子清理掉
		//1 先获取旧的周围玩家的列表
		auto oldList = AOI_world::GetWorld()->GetSurPlayers(this);

		//赋值给自身的成员变量新的坐标
		this->x = _x;
		this->y = _y;
		this->z = _z;
		this->v = _v;

		//2 再获取新的周围玩家的列表
		auto newList = AOI_world::GetWorld()->GetSurPlayers(this);

		//处理视野消失
		this->ViewDisappear(oldList, newList);
		//处理视野出现
		this->ViewAppear(oldList, newList);
		
		AOI_world::GetWorld()->AddPlayer(this);//将当前玩家添加到新的格子
	}

	//先赋值给自身的成员变量
	this->x = _x;
	this->y = _y;
	this->z = _z;
	this->v = _v;


	//同步当前的位置给AOI世界上周边玩家
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
	//获取世界所有玩家,发送广播消息
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
	//做集合运算,首先要排序,使用stl的排序算法,首先注意容器必须是可以随机访问
	vector<AOI_Player*> vOld(oldList.begin(), oldList.end());
	vector<AOI_Player*> vNew(newList.begin(), newList.end());
	std::sort(vOld.begin(), vOld.end());
	std::sort(vNew.begin(), vNew.end());
	//差集
	vector<AOI_Player*> diff;
	set_difference(vOld.begin(), vOld.end(), vNew.begin(), vNew.end(),
		std::inserter(diff, diff.begin()));
	for (auto r : diff)
	{
		auto role = dynamic_cast<GameRole*>(r);
		//视野消失,双方都要发送一个下线的消息
		auto msg = role->MakeLogoffSyncPid();
		ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)this->mProtocol);

		msg = this->MakeLogoffSyncPid();
		ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)role->mProtocol);
	}
}

void GameRole::ViewAppear(std::list<AOI_Player*>& oldList, std::list<AOI_Player*>& newList)
{
	//做集合运算,首先要排序,使用stl的排序算法,首先注意容器必须是可以随机访问
	vector<AOI_Player*> vOld(oldList.begin(), oldList.end());
	vector<AOI_Player*> vNew(newList.begin(), newList.end());
	std::sort(vOld.begin(), vOld.end());
	std::sort(vNew.begin(), vNew.end());
	//差集
	vector<AOI_Player*> diff;
	set_difference(vNew.begin(), vNew.end(), vOld.begin(), vOld.end(),
		std::inserter(diff, diff.begin()));
	for (auto r : diff)
	{
		auto role = dynamic_cast<GameRole*>(r);
		//视野出现,双方都要发送一个上线的消息
		auto msg = role->MakeInitPosBroadcast();
		ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)this->mProtocol);

		msg = this->MakeInitPosBroadcast();
		ZinxKernel::Zinx_SendOut(*msg, *(Iprotocol*)role->mProtocol);
	}
}

GameMsg * GameRole::MakeLogonSyncPid()
{
	//在这里构造一个玩家上线同步 id的消息
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
	//创建初始位置的广播消息
	auto msg = new GameMsg;
	auto pbMsg = new pb::BroadCast;
	pbMsg->set_pid(this->mPlayerId);
	pbMsg->set_username(this->mPlayerName);
	pbMsg->set_tp(1);
	//创建一个子对象,自动添加到pbMsg的protobuf对象里边
	pbMsg->set_content(_talkContent);
	auto single = new GameSingleTLV(GameSingleTLV::GAME_MSG_BROADCAST, pbMsg);
	msg->mMsgList.push_back(single);
	return msg;
}

GameMsg * GameRole::MakeInitPosBroadcast()
{
	//创建初始位置的广播消息
	auto msg = new GameMsg;
	auto pbMsg = new pb::BroadCast;
	pbMsg->set_pid(this->mPlayerId);
	pbMsg->set_username(this->mPlayerName);
	pbMsg->set_tp(2);
	//创建一个子对象,自动添加到pbMsg的protobuf对象里边
	auto pos = pbMsg->mutable_p();
	pos->set_x(x);
	pos->set_y(y);
	pos->set_z(z);
	pos->set_v(v);
	auto single = new GameSingleTLV(GameSingleTLV::GAME_MSG_BROADCAST, pbMsg);
	msg->mMsgList.push_back(single);
	return msg;
}

//构造一个新的位置消息
GameMsg * GameRole::MakeNewPosBroadcast()
{
	//创建初始位置的广播消息
	auto msg = new GameMsg;
	auto pbMsg = new pb::BroadCast;
	pbMsg->set_pid(this->mPlayerId);
	pbMsg->set_username(this->mPlayerName);
	pbMsg->set_tp(4);
	//创建一个子对象,自动添加到pbMsg的protobuf对象里边
	auto pos = pbMsg->mutable_p();
	pos->set_x(this->x);
	pos->set_y(this->y);
	pos->set_z(this->z);
	pos->set_v(this->v);
	auto single = new GameSingleTLV(GameSingleTLV::GAME_MSG_BROADCAST, pbMsg);
	msg->mMsgList.push_back(single);
	return msg;
}

//这个函数是创建一个下线的消息
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
		//数组里边添加一个player同时返回该对象指针
		auto singPlayer = pbMsg->add_ps();
		singPlayer->set_pid(role->mPlayerId);
		singPlayer->set_username(role->mPlayerName);
		//加入一个position对象,同时返回该指针
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
