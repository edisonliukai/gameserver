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
	//一开始根据参数创建n*m个格子
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
	//释放格子
	for (auto g: m_grids)
	{
		delete g;
	}
}

std::list<AOI_Player*> AOI_world::GetSurPlayers(AOI_Player * _player)
{
	//先计算当前玩家在哪个格子
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

	//做保护
	if (idx >= 0 && idx < this->m_grids.size())
	{
		//将该玩家添加到对应格子
		m_grids[idx]->m_players.push_back(_player);
		cout << "添加玩家到格子:" << idx << endl;
	}

}

void AOI_world::DelPlayer(AOI_Player * _player)
{
	//先计算该玩家本来在哪个格子,然后到这个格子删除该玩家
	int x = _player->GetX();
	int y = _player->GetY();
	int idx = this->calculate_grid_idx(x, y);
	m_grids[idx]->m_players.remove(_player);
}

AOI_world *AOI_world::pxWorld;
//返回单例对象指针
AOI_world * AOI_world::GetWorld()
{
	if (pxWorld == nullptr)
	{
		//AOI世界,的单例的参数是
		pxWorld = new AOI_world(85, 410, 75, 400, 10, 20);
	}
	return pxWorld;
}

bool AOI_world::GridChanged(AOI_Player * _player, int _newX, int _newY)
{
	//判断玩家格子有没有变化,有变化就返回true
	int idxOld = calculate_grid_idx(_player->GetX(), _player->GetY());
	int idxNew = calculate_grid_idx(_newX,_newY);

	return idxOld!=idxNew;
}

int AOI_world::calculate_grid_idx(int x, int y)
{
	//计算每个格子x轴的宽度
	int xWidth = (maxX - minX) / Xcnt;
	//计算每个格子y轴的宽度
	int yWidth = (maxY - minY) / Ycnt;

	//计算列数
	int col = (x - minX) / xWidth;
	//计算行数
	int row = (y - minY) / yWidth;

	//格子坐标=  行数*每一行格子数+列数
	int idx = row * Xcnt + col;
	return idx;
}
