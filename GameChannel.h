#pragma once
#include "ZinxTCP.h"


class GameProtocol;
class GameRole;
class GameChannel :
	public ZinxTcpData
{
public:
	GameChannel(int _fd);
	virtual ~GameChannel();
	virtual AZinxHandler * GetInputNextStage(BytesMsg & _oInput);
	//�󶨵�Э����ҵ������
	GameProtocol *mProtocol;
	GameRole *mRole;
};

class GameChannelFact :
	public IZinxTcpConnFact
{
public:
	virtual ZinxTcpData *CreateTcpDataChannel(int _fd);
};
