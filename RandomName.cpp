#include "RandomName.h"
#include <fstream>
#include <algorithm>
using namespace std;
#define FIRST_NAME_FILE "./random_first.txt"
#define LAST_NAME_FILE "./random_last.txt"



RandomName RandomName::smInstance;
std::string RandomName::getName()
{
	//����,��ǰ���
	string name = mNamePool.front();
	mNamePool.pop_front();
	return name;
}

void RandomName::releaseName(string name)
{
	//�黹����,���
	mNamePool.push_back(name);
}

RandomName::RandomName()
{
}

void RandomName::init()
{

	srand(time(NULL));
	//��ȡ�����ļ��Լ������ļ�
	ifstream first, last;
	first.open(FIRST_NAME_FILE);
	last.open(LAST_NAME_FILE);

	if (first.is_open() && last.is_open())
	{
		string firstName;
		//ÿ�ζ�ȡһ������
		while (getline(first, firstName))
		{
			//ÿ�ζ�ȡһ������
			string lastName;
			while (getline(last, lastName))
			{
				string name = firstName + " " + lastName;
				this->mNamePool.push_back(name);
			}
			//����һ��last�ļ���ĩβ״̬,�����ͷ��ʼ��
			last.clear(ios::goodbit);
            last.seekg(ios::beg);
		}
	}
	first.close();
	last.close();

	//ϴ��
	random_shuffle(this->mNamePool.begin(), this->mNamePool.end());
}


RandomName::~RandomName()
{
}
