#pragma once
#include "zinx.h"
#include <vector>
#include <list>

//����ǻص��ķ�װ
class TimerOutProc {
public:
    virtual void Proc() = 0;
    virtual int GetTimerSec() = 0;
	virtual ~TimerOutProc() {};
};
struct WheelNode{
    int LastCount = -1;
    TimerOutProc *pProc = NULL;
};
class ZinxTimerDeliver : public AZinxHandler
{
public:    
	ZinxTimerDeliver();
	/*��Ϣ����������������д�ú���ʵ����Ϣ�Ĵ���������Ҫһ�����ڼ�������ʱ��Ӧ�ô����µ���Ϣ���󣨶Ѷ��󣩲�����ָ��*/
	virtual IZinxMsg *InternalHandle(IZinxMsg &_oInput);

	/*��ȡ��һ�������ں�������������д�ú���������ָ����ǰ�����ڽ�������һ��������ʲô��ͨ��������Ϣ���󣬿�����Բ�ͬ������зֱ���*/
	virtual AZinxHandler *GetNextHandler(IZinxMsg &_oNextMsg);

	//ע������
	bool RegisterProcObject(TimerOutProc &_proc);
	//ע������
	void UnregisterProcObject(TimerOutProc &_proc);

	static ZinxTimerDeliver &GetInstance() { return sm_Instance; }

private:
	//����ʱ������
	std::vector<std::list<WheelNode>> m_TimerWheel;
	//���嵱ǰ�̶�
	int m_curr_index;
	//��������
	static ZinxTimerDeliver sm_Instance;
};
class ZinxTimer : public Ichannel
{
public:
	ZinxTimer();
	~ZinxTimer();
	/*ͨ����ʼ��������һ��أ�������Ӧ����д�������ʵ�ִ��ļ���һ���Ĳ�������
	�ú�������ͨ��������ӵ�zinxkernelʱ������*/
	virtual bool Init();

	/*��ȡ���ݣ� ������Ӧ�ø���Ŀ���ļ���ͬ����д�����������ʵ�ֽ�fd�е����ݶ�ȡ������_string��
	�ú�����������Դ����Ӧ���ļ������ݵ���ʱ������*/
	virtual bool ReadFd(std::string &_input);

	/*д�����ݣ� ������Ӧ�ý�����д���Ĳ���ͨ����д�ú���ʵ��
	�ú������ڵ���zinxkernel::sendout�����󱻵��ã�ͨ������ֱ�ӵ��ö���ͨ����·���õķ�ѹ���Ƶ��ã�*/
	virtual bool WriteFd(std::string &_output);

	/*ͨ��ȥ��ʼ����������Ӧ���ڸú������������Դ
	�ú�������ͨ�������zinxkernel��ժ��ʱ����*/
	virtual void Fini();

	/*��ȡ�ļ������������� ������Ӧ���ڸú������ص�ǰ���ĵ��ļ���
	һ��أ�������Ӧ�����������ж�������������¼��������¼��ǰIO���õ��ļ����ڴ˺�����ֻ�Ƿ��ظ�����*/
	virtual int GetFd();

    /*��ȡͨ����Ϣ�����������߿����ڸú����з��ظ�ͨ����ص�һЩ�����ַ���������������Һ͹���*/
	virtual std::string GetChannelInfo();
protected:
	/*��ȡ��һ�������ڣ�������Ӧ����д�ú�����ָ����һ��������
	һ��أ�������Ӧ���ڸú�������һ��Э��������������ȡ�����ֽ���*/
	virtual AZinxHandler *GetInputNextStage(BytesMsg &_oInput);


	//timerfd
	int m_fd;
};

