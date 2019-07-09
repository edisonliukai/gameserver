#include "ZinxTimer.h"
#include <sys/timerfd.h>
#include <iostream>
using namespace std;

ZinxTimer::ZinxTimer()
	:m_fd(-1)
{
}


ZinxTimer::~ZinxTimer()
{
}

bool ZinxTimer::Init()
{
	//��ʼ����ʱ�򴴽�timer
	m_fd = timerfd_create(CLOCK_MONOTONIC, 0);

	itimerspec spec = { {1,0},{1,0} };
	int ret = timerfd_settime(m_fd, 0, &spec, nullptr);
	return ret == 0;
}

bool ZinxTimer::ReadFd(std::string & _input)
{
	uint64_t overtimes=0;
	int len = read(m_fd, (char*)&overtimes, sizeof(overtimes));
	if (len == sizeof(overtimes))
	{
		//string���Ե���һ������,�洢�����Ƶ�����
		_input.append((char*)&overtimes, sizeof(overtimes));
		return true;
	}
	return false;
}

bool ZinxTimer::WriteFd(std::string & _output)
{
	return false;
}

void ZinxTimer::Fini()
{
	if (m_fd >= 0)
	{
		close(m_fd);
	}
}

int ZinxTimer::GetFd()
{
	return m_fd;
}

std::string ZinxTimer::GetChannelInfo()
{
	return "ZinxTimer";
}


AZinxHandler * ZinxTimer::GetInputNextStage(BytesMsg & _oInput)
{
	//ZinxTimer����һ������ڵ�,����ָ��dilever
	return &ZinxTimerDeliver::GetInstance();
}

ZinxTimerDeliver ZinxTimerDeliver::sm_Instance;
ZinxTimerDeliver::ZinxTimerDeliver()
	:m_TimerWheel(4) //�趨����Ĭ�ϳ�����10
	,m_curr_index(0)
{
}

IZinxMsg * ZinxTimerDeliver::InternalHandle(IZinxMsg & _oInput)
{
	BytesMsg *msg = dynamic_cast<BytesMsg*>(&_oInput);
	//�õ�overtimes
	uint64_t overtimes = *(uint64_t*)msg->szData.data();
	//������ʵ��ʱ�����ӵ���ת

	//Ҫ���ݳ�ʱ�Ĵ�����������Ҫ�������ٸ�
	for (int i = 0; i < overtimes; ++i)
	{
		//��ע���б�
		list<TimerOutProc*> unregisterList;

		//������ǰ�̶��µ���������
		for (auto it = m_TimerWheel[m_curr_index].begin(); it != m_TimerWheel[m_curr_index].end(); ++it)
		{
			it->LastCount--;
			if (it->LastCount < 0)
			{
				//ִ������
				it->pProc->Proc();
				unregisterList.push_back(it->pProc);
			}
		}

		for (auto &i : unregisterList) //C++11 ��������
		{
			//��ע��
			UnregisterProcObject(*i);
			//����ע��
			RegisterProcObject(*i);
		}

		//�̶�����Ҫ����,������������ӵĳ���,��Ӧ��ѭ��
		m_curr_index = (m_curr_index + 1) % m_TimerWheel.size();

	}


	return nullptr;
}

AZinxHandler * ZinxTimerDeliver::GetNextHandler(IZinxMsg & _oNextMsg)
{
	return nullptr;
}

bool ZinxTimerDeliver::RegisterProcObject(TimerOutProc & _proc)
{
	//ʱ����Ӧ��Ҫ����0
	if (_proc.GetTimerSec() <= 0)
	{
		return false;
	}
	//������ע�����
	int spec = _proc.GetTimerSec(); //��ȡ�����������ִ��һ��
	//����Ȧ��
	int round = spec / m_TimerWheel.size();
	//��������±�
	int idx = (m_curr_index + spec) % m_TimerWheel.size();

	if (idx == m_curr_index)
	{
		round -= 1;
	}

	WheelNode node;
	node.pProc = &_proc;
	node.LastCount = round;


	//���ڵ�ŵ��������
	m_TimerWheel[idx].push_back(node);
	return true;
}

void ZinxTimerDeliver::UnregisterProcObject(TimerOutProc & _proc)
{
	for (auto listIt = m_TimerWheel.begin(); listIt != m_TimerWheel.end(); ++listIt)
	{
		for (auto it = listIt->begin(); it != listIt->end();)
		{
			if (it->pProc == &_proc)
			{
				it = listIt->erase(it);
			}
			else
			{
				++it;
			}

		}
	}
}
