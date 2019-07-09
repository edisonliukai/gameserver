#pragma once
#include <string>
#include <deque>
class RandomName
{
public:
	~RandomName();
	
	//��ȡһ�����������
	std::string getName();
	//�黹һ������
	void releaseName(std::string name);

	static RandomName &getInstance() { return smInstance; }
	void init();
private:
	RandomName();
	static RandomName smInstance;
	//��������
	std::deque<std::string> mNamePool;
};

