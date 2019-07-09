#pragma once
#include "zinx.h"
#include "GameMsg.h"
#include "AOI_world.h"
class GameChannel;
class GameProtocol;
class GameRole :
	public Irole,
	public AOI_Player
{
public:
	//AOI player��ʵ��
	virtual int GetX() { return x; }
	virtual int GetY() { return z; }  //AOI�����Y��Ӧ3d��Ϸ�����z��

	//��¼�������Ϸ��ߵ���ά״̬
	float x;
	float y;
	float z;
	//����������ϱ��ĸ�����
	float v;


	GameRole();
	virtual ~GameRole();

	/*��ʼ�������������߿�����д�÷���ʵ�ֶ�����صĳ�ʼ�����ú�������role������ӵ�zinxkernelʱ����*/
	virtual bool Init();

	/*������Ϣ��������д�÷������Զ�ҵ�����ݽ��д���������Ҫ�����������̣���Ӧ�������������Ϣ���Ѷ���*/
	//Э��㴦��������ݻᴫ�ݵ�����
	virtual UserData *ProcMsg(UserData &_poUserData);

	/*ȥ��ʼ�����������Ƴ�ʼ���������ú�������role����ժ����zinxkernelʱ����*/
	virtual void Fini();

	GameChannel *mChannel;
	GameProtocol *mProtocol;

	//��������ƶ�����Ϣ
	void ProcNewPosition(float _x , float _y , float _z, float _v);
	void ProcTalkContent(std::string content);
	//������Ұ��ʧ����Ұ���ֵĺ���
	void ViewDisappear(std::list<AOI_Player*> &oldList, std::list<AOI_Player*> &newList);
	void ViewAppear(std::list<AOI_Player*> &oldList, std::list<AOI_Player*> &newList);

    /*��������ʱ��id��������Ϣ*/
    GameMsg *MakeLogonSyncPid();
    /*�����㲥������Ϣ*/
    GameMsg *MakeTalkBroadcast(std::string _talkContent);
    /*�����㲥����λ����Ϣ*/
    GameMsg *MakeInitPosBroadcast();
    /*�����㲥�ƶ�����λ����Ϣ*/
    GameMsg *MakeNewPosBroadcast();
    /*��������ʱ��id��������Ϣ*/
    GameMsg *MakeLogoffSyncPid();
    /*������Χ���λ����Ϣ*/
    GameMsg *MakeSurPlays();



	int mPlayerId;
	std::string mPlayerName;


};

