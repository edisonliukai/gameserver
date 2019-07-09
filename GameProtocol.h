#pragma once
#include "zinx.h"
#include <string>
class GameChannel;
class GameRole;
class GameProtocol :
	public Iprotocol
{
public:
	GameProtocol();
	virtual ~GameProtocol();

	/*ԭʼ���ݺ�ҵ�������໥��������������д�ú�����ʵ��Э��*/
	virtual UserData *raw2request(std::string _szInput);
	
	/*ԭʼ���ݺ�ҵ�������໥��������������д�ú�����ʵ��Э��*/
	virtual std::string *response2raw(UserData &_oUserData);


	GameRole *mRole;
	GameChannel *mChannel;

protected:
    /*��ȡ�����ɫ��������������Ӧ����д�ú���������ָ����ǰ�������û�������ϢӦ�ô��ݸ��ĸ���ɫ����*/
	virtual Irole *GetMsgProcessor(UserDataMsg &_oUserDataMsg);

	/*��ȡ����ͨ��������������Ӧ����д�ú���������ָ����ǰ�ֽ���Ӧ�����ĸ�ͨ�����󷢳�*/
	virtual Ichannel *GetMsgSender(BytesMsg &_oBytes);

	//�洢��һ�ζ�ȡ���ǻ�û�д���������
	std::string mLastBuf;
};

