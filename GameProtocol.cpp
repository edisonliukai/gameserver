#include "GameProtocol.h"
#include <iostream>
#include "GameMsg.h"
using namespace std;


GameProtocol::GameProtocol()
{
}


GameProtocol::~GameProtocol()
{
}

UserData * GameProtocol::raw2request(std::string _szInput)
{
	//ͨ����TCP��ȡ�����ݻᴫ�䵽����������Э�����
	//ÿ�ζ����¶���������׷�ӵ���һ�ε�����
	mLastBuf.append(_szInput);
	GameMsg *gameMsg = nullptr;
	//���ݳ�������Ҫ����8��������
	while (mLastBuf.size() >= 8)
	{
		// 0x01 0x02 0x03 0x04    ->   0x04030201
		int len =
			mLastBuf[0] |
			mLastBuf[1] << 8 |
			mLastBuf[2] << 16 |
			mLastBuf[3] << 24;
		int msgType =
			mLastBuf[4] |
			mLastBuf[5] << 8 |
			mLastBuf[6] << 16 |
			mLastBuf[7] << 24;
		cout << "len:" << len << " msgType:" << msgType << endl;
		//�ж���Ϣ���ݳ��ȹ�����  01000000010000000102000000020000000202
		if (mLastBuf.size() - 8 >= len)
		{
			//���ַ���ȡ�Ӵ��Ϳ���   ���Ӵ���protobuf��һ����Ϣ
			string msgContent = mLastBuf.substr(8, len);
			//bufҪ������������
			mLastBuf.erase(0, 8 + len);

			//����һ��SingletTLV
			auto msg = new GameSingleTLV((GameSingleTLV::GameMsgType)msgType, msgContent);

			if (gameMsg == nullptr)
			{
				gameMsg = new GameMsg;
			}
			//��singletlv�ӵ�GameMsg
			gameMsg->mMsgList.push_back(msg);
		}
		else
		{
			//ʣ�³��Ȳ���,�Ͳ�Ҫ����ѭ���ȴ���һ�����ݵ����ٳ���
			break;
		}
	}
	//����ֵ��������߼���Ϣ���б�
	return gameMsg;
}

std::string * GameProtocol::response2raw(UserData & _oUserData)
{
	//protocol Э���Ҫ��������� roleҵ��� �����������ݵ�Э���װ
	string *buf = new std::string;
	auto msg = dynamic_cast<GameMsg*>(&_oUserData);
	for (auto &single : msg->mMsgList)
	{
		//��ȡprotobuf��Ϣ����
		string content = single->serialize();
		//��Ϣ����
		//��С�˵��ֽ������
		int32_t len = content.size();
		buf->push_back((char)(len & 0xff));
		buf->push_back((char)(len>>8 & 0xff));
		buf->push_back((char)(len>>16 & 0xff));
		buf->push_back((char)(len>>24& 0xff));


		int32_t type = single->m_MsgType;
		buf->push_back((char)(type & 0xff));
		buf->push_back((char)(type>>8 & 0xff));
		buf->push_back((char)(type>>16 & 0xff));
		buf->push_back((char)(type>>24& 0xff));

		buf->append(content);
	}

	return buf;
}

Irole * GameProtocol::GetMsgProcessor(UserDataMsg & _oUserDataMsg)
{
	return (Irole*)this->mRole;
}

Ichannel * GameProtocol::GetMsgSender(BytesMsg & _oBytes)
{
	return (Ichannel*)mChannel;
}
