#include "AOI_world.h"
#include <iostream>
using namespace std;


AOI_world::AOI_world(int _minx, int _maxx, int _miny, int _maxy, int _xcnt, int _ycnt)
	:minX(_minx)
	,maxX(_maxx)
	,minY(_miny)
	,maxY(_maxy)
	,Xcnt(_xcnt)
	,Ycnt(_ycnt)
{
	//һ��ʼ���ݲ�������n*m������
	for (int i = 0; i < _xcnt*_ycnt; ++i)
	{
		m_grids.push_back(new AOI_Grid(i));
	}
}

int AOI_world::Xwidth()
{
	return 0;
}

int AOI_world::Ywidth()
{
	return 0;
}

AOI_world::~AOI_world()
{
	//�ͷŸ���
	for (auto g: m_grids)
	{
		delete g;
	}
}

std::list<AOI_Player*> AOI_world::GetSurPlayers(AOI_Player * _player)
{
	//�ȼ��㵱ǰ������ĸ�����
	int idx = this->calculate_grid_idx(_player->GetX(), _player->GetY());

	int indexes[] =
	{
		idx,
		idx+1,
		idx-1,
		idx-Xcnt,
		idx-Xcnt+1,
		idx-Xcnt-1,
		idx+Xcnt,
		idx+Xcnt+1,
		idx+Xcnt-1
	};
	std::list<AOI_Player*> l;
	for (auto i : indexes)
	{
		if (i >= 0 && i < m_grids.size())
		{
			for (auto player : m_grids[i]->m_players)
			{
				l.push_back(player);
			}
		}
	}
	return l;
}

void AOI_world::AddPlayer(AOI_Player * _player)
{
	int x = _player->GetX();
	int y = _player->GetY();
	int idx = this->calculate_grid_idx(x, y);

	//������
	if (idx >= 0 && idx < this->m_grids.size())
	{
		//���������ӵ���Ӧ����
		m_grids[idx]->m_players.push_back(_player);
		cout << "�����ҵ�����:" << idx << endl;
	}

}

void AOI_world::DelPlayer(AOI_Player * _player)
{
	//�ȼ������ұ������ĸ�����,Ȼ���������ɾ�������
	int x = _player->GetX();
	int y = _player->GetY();
	int idx = this->calculate_grid_idx(x, y);
	m_grids[idx]->m_players.remove(_player);
}

AOI_world *AOI_world::pxWorld;
//���ص�������ָ��
AOI_world * AOI_world::GetWorld()
{
	if (pxWorld == nullptr)
	{
		//AOI����,�ĵ����Ĳ�����
		pxWorld = new AOI_world(85, 410, 75, 400, 10, 20);
	}
	return pxWorld;
}

bool AOI_world::GridChanged(AOI_Player * _player, int _newX, int _newY)
{
	//�ж���Ҹ�����û�б仯,�б仯�ͷ���true
	int idxOld = calculate_grid_idx(_player->GetX(), _player->GetY());
	int idxNew = calculate_grid_idx(_newX,_newY);

	return idxOld!=idxNew;
}

int AOI_world::calculate_grid_idx(int x, int y)
{
	//����ÿ������x��Ŀ��
	int xWidth = (maxX - minX) / Xcnt;
	//����ÿ������y��Ŀ��
	int yWidth = (maxY - minY) / Ycnt;

	//��������
	int col = (x - minX) / xWidth;
	//��������
	int row = (y - minY) / yWidth;

	//��������=  ����*ÿһ�и�����+����
	int idx = row * Xcnt + col;
	return idx;
}
