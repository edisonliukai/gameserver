#include "GameChannel.h"
#include "GameProtocol.h"
#include "GameRole.h"


GameChannel::GameChannel(int _fd)
	:ZinxTcpData(_fd)
{
}

GameChannel::~GameChannel()
{



	//ͨ���㱻�ͷŵ��ͷž�ҪЭ���ͷ�Э����ҵ���Ķ���
	if (mProtocol)
	{
		ZinxKernel::Zinx_Del_Proto(*mProtocol);
		delete mProtocol;
	}
	if (mRole)
	{
		ZinxKernel::Zinx_Del_Role(*mRole);
		delete mRole;
	}
}

AZinxHandler * GameChannel::GetInputNextStage(BytesMsg & _oInput)
{
	//ͨ�������һ����������Э���
	return mProtocol;
}

ZinxTcpData * GameChannelFact::CreateTcpDataChannel(int _fd)
{
	//�����µ�tcp���ӽ�����ʱ����˴���ͨ�������ҲҪͬʱ����
	//Э����ҵ���Ķ���
	auto ch = new GameChannel(_fd);
	auto protocol = new GameProtocol;
	auto role = new GameRole;

	ch->mProtocol = protocol;
	ch->mRole = role;

	protocol->mChannel = ch;
	protocol->mRole = role;

	role->mChannel = ch;
	role->mProtocol = protocol;

	//Ҫ����ZinxKernel ��add role �� add proto �Ż�ִ�� role �� protocol��init
	ZinxKernel::Zinx_Add_Role(*role);
	ZinxKernel::Zinx_Add_Proto(*protocol);

	return ch ;
}
