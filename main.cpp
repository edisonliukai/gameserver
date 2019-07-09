#include "zinx.h"
#include "ZinxTCP.h"
#include "ZinxTimer.h"
#include "GameChannel.h"
#include <iostream>
#include "RandomName.h"
using namespace std;


//class MyProc : public TimerOutProc
//{
//public:
//	virtual void Proc()
//	{
//		//每次超时的时候就执行输出hello world
//		cout << "Hello World" << endl;
//	}
//	virtual int GetTimerSec()
//	{
//		//执行每秒超时一次
//		return 4;
//	}
//};

int main()
{
	//先初始化姓名池子
	RandomName::getInstance().init();

	ZinxKernel::ZinxKernelInit();
	//auto timerChannel = new ZinxTimer();

//	MyProc proc;
//	//注册任务
//	ZinxTimerDelever::GetInstance().RegisterProcObject(proc);


	auto listen = new ZinxTCPListen(8899, new GameChannelFact);

	//ZinxKernel::Zinx_Add_Channel(*timerChannel);
	ZinxKernel::Zinx_Add_Channel(*listen);
	ZinxKernel::Zinx_Run();
	ZinxKernel::ZinxKernelFini();

	return 0;
}

