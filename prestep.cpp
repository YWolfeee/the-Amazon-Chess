#include"pch.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include<queue>
#include<time.h>
#include<iomanip>
#include<cmath>
#include<algorithm>

#define GRIDSIZE 8
#define OBSTACLE 2
#define judge_black 0
#define judge_white 1
#define grid_black 1
#define grid_white -1 
#define c_arrival 1
#define debugging 1
#define assail_kill 4
#define assail_checkmate 2
using namespace std;
int limitationopen = 6;//表示接受的每层最少打开数目
int maxopenpoint = limitationopen * (limitationopen + 1); //第二层最大打开的解法数量
int currBotColor; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[GRIDSIZE][GRIDSIZE] = { 0 }; // 先x后y，记录棋盘状态
int me, en;
int person[2];//表示两个人 person[0] = me;person[1] = en;
int times = 0;//表示打开次数
int openlayer = 2;//表示打开层数
int cutting = 0;

//参数部分 由于每次是Layer0的时候更新，Layer++的时候就不动了 所以就放外面
double c_assail = 0, c_mobival = 1;
double  fir_c_arrival;

//走法记录组织
struct behavior
{
	int stx;
	int sty;
	int x_chess;
	int y_chess;
	int x_jian;
	int y_jian;
	double beah_value;
	double darrival = 0;
	double dmobival = 0;
	double assail = 0;
	behavior *next;
};
//主干信息存贮组织
struct Layerstruct
{
	int num_canarr = 0;//表示目前还需要考虑的位置数量
	int num_canmobival = 0;//表示目前还需要考虑的位置的基础价值和
	double stadarrival;//表示标准到达状态
	double stadmobi_val = 0;//表示chess.mobival的标准值
	int version;
	int standardmodel[2][4];
	int standardliantong[2][4];
	int zongqingkuang = 0;
	behavior next_behavior, store_behavior[90],result,present;
}layer[56];
//回传组织
struct returnstruct
{
	int lose;
	int stx;
	int sty;
	int x_chess;
	int y_chess;
	int x_jian;
	int y_jian;
	int advfoot;
	double stepvalue;//表达的是这部走法的返回价值
	int layer;
};
//传递组织
struct interstruct
{
	int style;
	int chessbox[GRIDSIZE][GRIDSIZE];
	int color;
	int layer;
};
//棋盘单元组织
struct placeevaluation//这是对一个位置价值进行评估的结构体 其中(curbotcolor+1)/2表示我方（要么0 要么1）
{
	int oriimportance;
	int arrival[2][5] = { 0 };//（大体完成）在gridInfo为0的时候才有意义 表示这个位置是否还可接触（不可接触的位置抛弃）  1表示可以 0表示不行 2表示假死【被对方堵死】
	int mobility = 0; //表示这个位置的活动力 表示可到达的方向数量
}place[8][8];
//棋子单元组织
struct chessevaluation//这是一个对每个棋子的价值的判断 这是限制对方棋子价值、增加我方棋子价值的重要评估手段{尤其在后期？}
{
	int x; int y;//表示位置
	int liantong = 0;//表示这个棋子本身的联通格数量
	double arr_value = 0;//表示能到达的格子的总价值的总和
	int danger = 8;//表示棋子的危机情况 即还有几个方向可以前往（数值越大越不危险）
	int model = 0;//表示棋子的状态【具体原理没想好 0正常 1濒死（《=2） 2没救】
	double totalvalue = 0;//表示棋子的总评估价值
	bool warring;
}chess[2][4];
//参数写入组织
struct versionstruct
{
	int version;
	int ifsecond;
	double totalcontrol, halfcontrol;
	double first_c_arrival;
	double c_assail = 0;
	double c_mobival = 1;
	int limitationopen;
	//double alass_stad;
	//double almob_stad;
	//double alhal_stad;
	//double k_ass_stad;
	//double k_mob_stad;
	//double k_hal_stad;
}v;
//移动性组织
struct direction_move
{
	int x; int y;
}run[8];

//正负判断结构
inline int signal(double x)
{
	if (x > 0)
		return 1;
	else if (x == 0)
		return 0;
	else
		return -1;
}
//初始化结构（初始盘面、方向写入、等级划分）  PS：数值待定
inline int class_confirm(int x, int y)
{
	if (x == 0 || x == GRIDSIZE - 1 || y == 0 || y == GRIDSIZE - 1)
		return 1;//边界
	if (x == 1 || (x == GRIDSIZE - 2) || y == 1 || (y == GRIDSIZE - 2))
		return 3;//次边界
	return 5;//内部
}
void initalization_movement(interstruct inter)
{
	// 初始化棋盘
	if (inter.style == 0)
	{
		for (int i = 0; i < GRIDSIZE; i++)
		{
			for (int j = 0; j < GRIDSIZE; j++)
				gridInfo[i][j] = 0;
		}
		gridInfo[0][(GRIDSIZE - 1) / 3] = gridInfo[(GRIDSIZE - 1) / 3][0]
			= gridInfo[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][0]
			= gridInfo[GRIDSIZE - 1][(GRIDSIZE - 1) / 3] = grid_black;
		gridInfo[0][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = gridInfo[(GRIDSIZE - 1) / 3][GRIDSIZE - 1]
			= gridInfo[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][GRIDSIZE - 1]
			= gridInfo[GRIDSIZE - 1][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = grid_white;
	}
	//参数归零
	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{
			place[i][j].arrival[0][0] = -1; place[i][j].arrival[1][0] = -1;
		}
	}
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			chess[i][j].arr_value = 0;
			chess[i][j].danger = 0;
			chess[i][j].liantong = 0;
			chess[i][j].model = 0;
			chess[i][j].totalvalue = 0;
			chess[i][j].warring = 0;
		}
	}

	layer[inter.layer].num_canarr = 0;//表示目前还需要考虑的位置数量
	layer[inter.layer].num_canmobival = 0;//表示目前还需要考虑的位置的基础价值和
	layer[inter.layer].stadarrival = 0;//表示标准到达状态
	layer[inter.layer].stadmobi_val = 0;//表示chess.mobival的标准值

	//初始化数值
	if (inter.layer == 0)
	{
		for (int i = 0; i < GRIDSIZE; i++)
		{
			for (int j = 0; j < GRIDSIZE; j++)
			{
				place[i][j].oriimportance = class_confirm(i, j);
			}
		}
		run[0].x = 1; run[0].y = 0;
		run[1].x = -1; run[1].y = 0;
		run[2].x = 0; run[2].y = 1;
		run[3].x = 0; run[3].y = -1;
		run[4].x = 1; run[4].y = 1;
		run[5].x = -1; run[5].y = -1;
		run[6].x = -1; run[6].y = 1;
		run[7].x = 1; run[7].y = -1;
	}
}
// 位置判断结构
inline bool inMap(int x, int y)//内联函数 减少栈空间的打开
{
	if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
		return false;
	return true;
}
inline bool emptyplace(int x, int y) //防止调用超定义域的grid
{
	if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
		return false;
	if (gridInfo[x][y] != 0)
		return false;
	return true;
}
//相邻位置判断结构
inline bool adjoin_check(int x, int y)
{
	if (x > y)
	{
		int temp = y;
		y = x;
		x = temp;

	}
	if ((x == 0 && y == 2) ||
		(x == 0 && y == 3) ||
		(x == 0 && y == 4) ||
		(x == 0 && y == 7) ||
		(x == 1 && y == 2) ||
		(x == 1 && y == 3) ||
		(x == 1 && y == 5) ||
		(x == 1 && y == 6) ||
		(x == 2 && y == 4) ||
		(x == 2 && y == 6) ||
		(x == 3 && y == 5) ||
		(x == 3 && y == 7))
		return true;
	return false;
}
//棋盘显示结构（待优化）
void print_chess()
{
	//if (debugging == 1)
		//cout << "————————————————————————————————" << endl;
	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{
			cout << right << setw(2) << gridInfo[j][i];
		}
		cout << endl;
	}
	cout << endl;

	/*
	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{
			cout << right << setw(2) << place[j][i].arrival[0][0];
		}
		cout << endl;
	}
	cout << endl;

	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{
			cout << right << setw(2) << place[j][i].arrival[1][0];
		}
		cout << endl;
	}
	cout << endl;
	*/
}
// 模拟落子结构
bool ProcStep(int x0, int y0, int x1, int y1, int x2, int y2, int color, bool check_only)
{
	if ((!inMap(x0, y0)) || (!inMap(x1, y1)) || (!inMap(x2, y2)))
		return false;
	if (gridInfo[x0][y0] != color || gridInfo[x1][y1] != 0)
		return false;
	if ((gridInfo[x2][y2] != 0) && !(x2 == x0 && y2 == y0))
		return false;
	if (!check_only)
	{
		gridInfo[x0][y0] = 0;
		gridInfo[x1][y1] = color;
		gridInfo[x2][y2] = OBSTACLE;
	}
	return true;
}
//到达位置差值计算结构
inline double darrival()
{
	double d = 0; int temp;
	if (v.version == 0)
	{
		for (int i = 0; i < GRIDSIZE; i++)
		{
			for (int j = 0; j < GRIDSIZE; j++)
			{
				if (place[i][j].arrival[me][0] == -1 && place[i][j].arrival[en][0] == -1)
					continue;
				else if (place[i][j].arrival[me][0] == -1)
					d -= v.totalcontrol;
				else if (place[i][j].arrival[en][0] == -1)
					d += v.totalcontrol;
				else
				{
					temp = -(place[i][j].arrival[me][0] - place[i][j].arrival[en][0]);
					switch (temp)
					{
						//版本1
						/*
					case 0:break;
					case 1:d += v.halfcontrol; break;
					case -1:d -= v.halfcontrol; break;
					default:
						d += signal(temp) * v.totalcontrol;
						*/
						//版本2
					case 2: d += v.totalcontrol; break;
					case 1: d += v.halfcontrol; break;
					case 0: d -= v.halfcontrol; break;
					default: d += signal(temp) * v.totalcontrol;
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < GRIDSIZE; i++)
		{
			for (int j = 0; j < GRIDSIZE; j++)
				d += (place[i][j].arrival[me][0] != 0) - (place[i][j].arrival[en][0] != 0);
		}
	}
	return d;
}
//领先步数计算结构
int dadvfoot()
{
	double d = 0; int temp;
	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
			d += (place[i][j].arrival[me][0] != -1) - (place[i][j].arrival[en][0] != -1);
	}
	return d;
}

//动态权参中枢系统(根据当前的盘面对参数动态赋值的综合评估系统)
void dymanicgeneration(int lay,versionstruct v)//输入层数
{
	//c_assail = v.alass_stad * layer[lay].stadarrival + v.k_ass_stad;
	//c_mobival = v.almob_stad * layer[lay].stadarrival + v.k_mob_stad;
	//v.halfcontrol = v.alhal_stad * layer[lay].stadarrival + v.k_mob_stad; //矩阵运算
	//项目重载
	//layer[lay].stadarrival = darrival();
}
//进攻中枢系统(根据对方的4枚棋子的danger情况完成)——————未能实现根据liantong改变的判断，原因是耗时的急剧增加
int cal_movedirection(int x, int y)
{
	int ans_movedirection = 0;
	for (int j = 0; j < 8; j++)
	{
		int xx = x + run[j].x, yy = y + run[j].y;
		int foot = 1;
		if (emptyplace(xx, yy))//表示这是其中一个出逃方向
		{
			ans_movedirection++;
		}
	}
	return ans_movedirection;
}
double secondass(int order)//表示是对第order枚棋子进行一个二层assailment估计 返回刺杀的可能性 如果可能返回需要牺牲的格子数量
{
	//目前的盘面局势很确定 但是要注意回溯
	int controlplace = 0;
	for (int j = 0; j < 8; j++)//先确定8个方向
	{
		int x = chess[en][order].x + run[j].x, y = chess[en][order].y + run[j].y;
		int foot = 1;
		while (emptyplace(x, y))//表示这是其中一个出逃方向
		{
			int r = cal_movedirection(x, y) + (foot == 1);//cal_move表示x,y这个点周围的空格的个数
			if (r >= 4)//表示基本上认为这是可以跑出去的走法
				return 0;
			if (r == 3)//这个比较独特 除去来路 另外还有两个走法 如果这俩不相邻 就认为凉凉了（前进方向为n） 否则就继续判断
			{
				int xiangling = 0;
				int fangxiang[2] = { -1,-1 };
				for (int k = 0; k < 8; k++)
				{
					if ((run[k].x == -run[j].x) && (run[k].y == -run[j].y))continue;
					if (emptyplace(x + run[k].x, y + run[k].y))
					{
						if (fangxiang[0] == -1) fangxiang[0] = k;
						else { fangxiang[1] = k; break; }
					}
				}//寻找出两个方向
				if (adjoin_check(fangxiang[0], fangxiang[1]))//表示相邻
				{
					int flag = 0;
					for (int i = 0; i < 2; i++)
					{
						int xx = x + run[fangxiang[i]].x, yy = y + run[fangxiang[i]].y;//来到这个格子
						for (int k = 0; k < 8; k++)
						{
							if (k == fangxiang[i]) continue;//因为有可能会被堵上
							if (run[k].x == -run[fangxiang[i]].x && run[k].y == -run[fangxiang[i]].y)continue;
							int foot_move = 1;
							while (emptyplace(xx + foot_move * run[k].x, yy + foot_move * run[k].y))
								foot_move++;
							if (inMap(xx + foot_move * run[k].x, yy + foot_move * run[k].y))
							{
								if (gridInfo[xx + foot_move * run[k].x][yy + foot_move * run[k].y] == currBotColor)
								{
									flag = 1;
									break;
								}
							}
						}
						if (flag)break;
					}
					if (flag == 0)
						return 0;
					else
					{
						foot++;
						x += run[j].x; y += run[j].y;
						continue;
					}
				}
				else
					return 0;
			}

			//r==2的具有一定风险的判断 就是因为来的方向肯定是一个 也就是另外只有一个方向可以走动 那就认为他会死（大概率？）
			if (r == 2 && !emptyplace(x + run[j].x, y + run[j].y))//也就是另一条路的方向不是这边 认为另外那个方向可以堵死？
			{
				controlplace += foot;
				break;
			}
			if (r == 1)//死胡同了
			{
				controlplace += foot;
				break;
			}
			foot++;
			x += run[j].x; y += run[j].y;
		}
	}
	return controlplace;
}
double assailmentevaluation(int lay)//注释以下 model:0表示正常 1表示将军(direction =1||2) 2表示凉凉
{
	double assail_value = 0;
	for (int i = 0; i < 4; i++)
	{
		if (chess[en][i].model != layer[lay].standardmodel[en][i])//表示这枚棋子的状态发生了改变 注意增加表示有利改变 减少表示不利改变
		{
			if (chess[en][i].model == 2)//这里表示某种意义上的杀死这枚棋子了
				assail_value += min(assail_kill, layer[lay].standardliantong[en][i]);
			else if (chess[en][i].model == 0)//这里表示释放了这枚棋子
			{
				//从濒死释放
				assail_value -= assail_checkmate;
			}
			else//这是使得目前状态为濒死
			{
				if (layer[lay].standardmodel[en][i] == 0)//有利增幅 但是要进行二层模拟
				{
					double receive = secondass(i);
					if (receive)
						assail_value += assail_kill - secondass(i);//二层模拟的评估结果					
				}
				else//从真死释放 相当于让开给了对方跑出来的机会（相当吃亏）
					assail_value -= min(assail_kill, chess[en][i].liantong);
			}
		}
		/*
		else
		{
			if (chess[en][i].liantong < standardchess[en][i].liantong - 1)//相当于减少了联通分支(注意 这里只分析的是 改变后使得对方被困住的 我方围地那种不考虑)
			{
				if (chess[en][i].liantong <= 3 || ((double)chess[en][i].liantong / num_canarr) <= 0.2)//这里两个参数都应该还可以调整
					assail_value += 5 - chess[en][i].liantong;
			}
		}
		*/
	}
	return assail_value;
}
//活动中枢系统
void chess_arrplaceevaluation()
{
	for (int per = 0; per < 2; per++)
	{
		for (int i = 0; i < 4; i++)
		{
			chess[person[per]][i].danger = 0;
			chess[person[per]][i].arr_value = 0;
			for (int j = 0; j < 8; j++)
			{
				int x = chess[person[per]][i].x + run[j].x, y = chess[person[per]][i].y + run[j].y;
				if (inMap(x, y)) {
					//if (gridInfo[x][y] == 0 || gridInfo[x][y] == currBotColor * (1 - 2 * per)) 不对！！！如果是有他的棋子的时候其实也应该认为挡住了
					if (gridInfo[x][y] == 0)
						chess[person[per]][i].danger++;//己方用于危机性判断 地方用于assailment
				}
				while (inMap(x, y))
				{
					if (gridInfo[x][y] != 0)
						break;
					chess[person[per]][i].arr_value += place[x][y].oriimportance;
					x += run[j].x; y += run[j].y;
				}
			}
			if (chess[person[per]][i].danger == 0)//没救了
				chess[person[per]][i].model = 2;
			else if (chess[person[per]][i].danger == 1 || chess[person[per]][i].danger == 2)//濒死
				chess[person[per]][i].model = 1;
			else//正常
				chess[person[per]][i].model = 0;
		}
	}
}
void chess_personnalliantong()
{
	for (int per = 0; per < 2; per++)
	{
		for (int i = 0; i < GRIDSIZE; i++)
		{
			for (int j = 0; j < GRIDSIZE; j++)
			{
				for (int r = 1; r < 5; r++)
				{
					place[i][j].arrival[person[per]][r] = -1;
				}
			}
		}
		for (int r = 0; r < 4; r++)
		{
			struct bfs
			{
				int x;
				int y;
			};
			bfs q[64]; bfs temp = { 0,0 }, insert = { 0,0 };
			int fir = 0, las = 0;
			temp.x = chess[person[per]][r].x; temp.y = chess[person[per]][r].y;
			q[++las] = temp;
			place[temp.x][temp.y].arrival[person[per]][r + 1] = 1;
			chess[person[per]][r].liantong++;
			while (fir < las)
			{
				temp = q[++fir];
				int x = temp.x, y = temp.y;
				for (int j = 0; j < 8; j++)
				{
					insert.x = x + run[j].x; insert.y = y + run[j].y;
					while (inMap(insert.x, insert.y))
					{
						if (gridInfo[insert.x][insert.y] == 0)
						{
							if (place[insert.x][insert.y].arrival[person[per]][r + 1] == -1)//表示这个之前还没有访问过
							{
								q[++las] = insert;
								place[insert.x][insert.y].arrival[person[per]][r + 1] = 1;
								chess[person[per]][r].liantong++;
							}
							insert.x += run[j].x; insert.y += run[j].y;
						}
						else if (gridInfo[insert.x][insert.y] == -currBotColor)
						{
							chess[person[per]][r].warring = 1;
							break;
						}
						else break;
					}
				}
			}
		}
	}

}
//模样判定 改(核心中枢)
void place_controlevaluation(int ver)
{
	while (ver != 2)
	{
		int r = 0;
		for (int i = 0; i < GRIDSIZE; i++)
		{
			for (int j = 0; j < GRIDSIZE; j++)
			{
				//			for (int r = 0; r < 5; r++)
				//			{

				place[i][j].arrival[person[ver]][r] = -1;//表示到不了
				//place[i][j].arrival[1][r] = 0;
//			}
			}
		}
		int foot = 0;
		struct bfs
		{
			int x;
			int y;
			int foot;
		};
		bfs q[64]; bfs temp = { 0,0,0 }; bfs insert = { 0,0,0 };
		int fir = 0, las = 0;
		for (int i = 0; i < 4; i++)
		{
			temp.x = chess[person[ver]][i].x; temp.y = chess[person[ver]][i].y;
			q[++las] = temp;
			place[temp.x][temp.y].arrival[person[ver]][r] = foot;
		}
		while (fir < las)
		{
			temp = q[++fir];
			int x = temp.x, y = temp.y;
			insert.foot = temp.foot + 1;
			for (int j = 0; j < 8; j++)
			{
				insert.x = x + run[j].x; insert.y = y + run[j].y;
				while (emptyplace(insert.x, insert.y))
				{
					if (place[insert.x][insert.y].arrival[person[ver]][r] == -1)//表示这个之前还没有访问过
					{
						q[++las] = insert;
						place[insert.x][insert.y].arrival[person[ver]][r] = insert.foot;
					}
					insert.x += run[j].x; insert.y += run[j].y;
				}
			}
		}
		ver++;
	}
}
//行为判断中枢系统(单次操作的终端)
double judge(behavior next, int order, int lay)//最后的参数表示层数
{
	double d = layer[lay].stadarrival;
	double assail_value = 0;
	double dmobi_val = 0;
	//中间是估价操作

	//先评估每个格子的arrive 用广搜 首先判断连通性是否被破坏 如果没有 就跳过
	/*
	for (int i = 0; i < 3; i++)
	{
		int x, y;
		if (i == 0) { x = next.x_jian; y = next.y_jian; }
		else if (i == 1) { x = next.stx; y = next.sty; }
		else { x = next.y_chess; y = next.y_chess; }
		int barrier = 0;
		if ((x == 0 && gridInfo[1][y] != 0) || (x == GRIDSIZE && gridInfo[GRIDSIZE - 1][y] != 0)
			|| (y == 0 && gridInfo[x][1] != 0) || (y == GRIDSIZE && gridInfo[x][GRIDSIZE - 1] != 0))
			barrier = 2;
		else if (1 <= x && x <= GRIDSIZE - 1 && 1 <= y && y <= GRIDSIZE - 1)
		{
			for (int j = 0; j < 4; j++)
			{
				int xx = x + run[j].x, yy = y + run[j].y;
				if (inMap(xx, yy))
				{
					if (gridInfo[xx][yy] != 0)
						barrier++;
				}
			}
		}
		//操作一 我方的“我方棋子阻挡导致的联通性变化 无法考虑

		if (barrier >= 2 && i == 0)
		{
			place_arriveevaluation(0);
			d = darrival();
			break;
		}
		else if (barrier >= 2 && i != 0)
		{
			place_arriveevaluation(1);
			d = darrival() - 1;
			break;
		}

		//操作二 统一重算 这样子第一步要365ms

		if (barrier >= 2)
		{
			if(v.version==0)
				place_controlevaluation(0);
			else
				place_arriveevaluation(0);
			d = darrival();
			break;
		}
	}
	*/
	place_controlevaluation(0);
	d = darrival();

	//接下来评估每个chess的arr_value  注意这里评估的是一步到达 这是一个判断(能否同时进行danger的计算？)

	chess_arrplaceevaluation();
	for (int i = 0; i < 4; i++)
		dmobi_val += chess[me][i].arr_value - chess[en][i].arr_value;


	//接下来是assailment评估
	assail_value = assailmentevaluation(lay);
	//注意这个玩意应该是要和阶段有关的 后阶段不能老是干这个了


	if (debugging == 1)
	{
		//if (next.stx == 0 && next.sty == 4 && next.x_chess == 2 && next.y_chess == 4 && next.x_jian == 0 && next.y_jian == 4)
		//{
		//	cout << "SB" << endl;
		//	print_chess();
		//}
	}
	//return dmobi_val - stadmobi_val;

	layer[lay].next_behavior.dmobival = dmobi_val - layer[lay].stadmobi_val;
	layer[lay].next_behavior.darrival = d - layer[lay].stadarrival;
	layer[lay].next_behavior.assail = assail_value * c_assail;
	double k;
	if (layer[lay].num_canarr != 0)
	{
		k = (double)((double)pow(d - layer[lay].stadarrival, 2) / layer[lay].num_canarr) * signal(layer[lay].next_behavior.darrival)
			+ (double)((double)layer[lay].next_behavior.dmobival / layer[lay].num_canmobival) * c_mobival
			+ (double)c_assail * assail_value;
	}
	else//已经是非战斗态势了 那么只是单纯的不需要进行任何的多余判断的走法
		k = (double)(pow(d - layer[lay].stadarrival, 2)) * signal(layer[lay].next_behavior.darrival);
	return k;
}

/*____________________________________________________________________________________________________________________*/
/*____________________________________________________________________________________________________________________*/
//int main()

returnstruct littlemain(interstruct inter, versionstruct vv)
{

	//进行初始化设定
	if (inter.layer==0)
		cutting = 0;
		//当不是第一层时 来到这里的时候满足currBotcolor 棋盘info me person en 这些都已经是当今层 
		//读取出条件参数
		if (inter.layer == 0)
		{
			v = vv;
			limitationopen = 6;//表示接受的每层最少打开数目
			maxopenpoint = limitationopen * (limitationopen + 1); //第二层最大打开的解法数量
			c_assail = v.c_assail;
			c_mobival = v.c_mobival;
		}
		returnstruct returns;
		returns.layer = inter.layer;//偶数自己 奇数别人；
		returns.lose = 0;
		srand(time(0));
		initalization_movement(inter);//初始化
		//这里是预存储可能打开多层的操作 大概是42个 相当可观了
		for (int i = 0; i < maxopenpoint; i++)
			layer[inter.layer].store_behavior[i].beah_value = -100000;
		double minvalue = -100000;
		int numstore = 0;
		int stillwarring = 1;
		layer[inter.layer].result.beah_value = -100000;//最终的结果
	

	//传递棋盘的操作
	if (inter.style == 0)
	{
		int x0, y0, x1, y1, x2, y2;
		int turnID;
		cin >> turnID;
		// 读入到当前回合为止，自己和对手的所有行动，从而把局面恢复到当前回合
		currBotColor = grid_white; // 先假设自己是白方
		for (int i = 0; i < turnID; i++)
		{
			// 根据这些输入输出逐渐恢复状态到当前回合
			// 首先是对手行动
			cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
			if (x0 == -1)
				currBotColor = grid_black; // 第一回合收到坐标是-1, -1，说明我是黑方
			else
				ProcStep(x0, y0, x1, y1, x2, y2, -currBotColor, false); // 模拟对方落子
																		// 然后是自己当时的行动
																		// 对手行动总比自己行动多一个
			if (i < turnID - 1)
			{
				cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
				if (x0 >= 0)
					ProcStep(x0, y0, x1, y1, x2, y2, currBotColor, false); // 模拟己方落子
			}
		}
		me = (1 + currBotColor) / 2;
		en = 1 - me;
		person[0] = me; person[1] = en;   //这是所有数组的2维与我、敌的对应（这个在下一层肯定要修改）
	}
	else
	{
		if (inter.layer == 0)
		{
			int tiaoshi = 0;
			for (int i = 0; i < GRIDSIZE; i++)
			{
				for (int j = 0; j < GRIDSIZE; j++)
					gridInfo[i][j] = inter.chessbox[i][j];
			}
			currBotColor = inter.color;
			me = (1 + currBotColor) / 2; en = 1 - me;
			person[0] = me; person[1] = en;   //这是所有数组的2维与我、敌的对应（这个在下一层肯定要修改）
		}
		//如果这不是第一层 那么应该在外部完成回溯操作.
	}

	clock_t starttime = clock();

	//初始化当前棋子位置
	int location[2] = { 0,0 };
	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{

			if (gridInfo[i][j] == currBotColor)
			{
				chess[me][location[me]].x = i;
				chess[me][location[me]++].y = j;
			}
			else if (gridInfo[i][j] == -currBotColor)
			{
				chess[en][location[en]].x = i;
				chess[en][location[en]++].y = j;
			}
		}
	}//记录双方棋子位置

	//刻画原始状态------------------------------------------//
	
		chess_arrplaceevaluation();//刻画初状态的到达情况以及danger的情况
		chess_personnalliantong();
		for (int i = 0; i < 4; i++)
		{
			layer[inter.layer].stadmobi_val += chess[me][i].arr_value - chess[en][i].arr_value;
			layer[inter.layer].standardmodel[me][i] = chess[me][i].model;
			layer[inter.layer].standardmodel[en][i] = chess[en][i].model;
			layer[inter.layer].standardliantong[me][i] = chess[me][i].liantong;
			layer[inter.layer].standardliantong[en][i] = chess[en][i].liantong;
		}
		//进行place.arrival的原始状态刻画操作
		place_controlevaluation(0);
		for (int i = 0; i < GRIDSIZE; i++)
		{
			for (int j = 0; j < GRIDSIZE; j++)
			{
				if ((place[i][j].arrival[me][0] != -1) && (place[i][j].arrival[en][0] != -1) && (gridInfo[i][j] != -currBotColor))
				{
					layer[inter.layer].num_canarr++;
					layer[inter.layer].num_canmobival += class_confirm(i, j);
				}
			}
		}
		layer[inter.layer].stadarrival = darrival();
		layer[inter.layer].zongqingkuang = 0;
		double p = 0.1, temptime = 0;
	


	//情况搜索开始 

	//分为战争态势和非战争态势
	stillwarring = 0;
	for (int i = 0; i < 4; i++)
	{
		if (chess[me][i].warring == 1 && chess[me][i].liantong >= 2)
		{
			stillwarring = 1;
			break;
		}
	}

	//大搜索
	for (int i = 0; i < 4; i++)
	{
		if ((chess[me][i].warring == 1 && chess[me][i].liantong >= 2) || (stillwarring == 0))//这枚棋子处于战斗状态
		{
			layer[inter.layer].next_behavior.stx = chess[me][i].x;
			layer[inter.layer].next_behavior.sty = chess[me][i].y;
			for (int j = 0; j < 8; j++)//表示考虑移动的8个方向
			{
				int foot = 1;
				while (emptyplace(layer[inter.layer].next_behavior.stx + foot * run[j].x, layer[inter.layer].next_behavior.sty + foot * run[j].y))//是空的
				{
					chess[me][i].x = layer[inter.layer].next_behavior.stx + foot * run[j].x;
					chess[me][i].y = layer[inter.layer].next_behavior.sty + foot * run[j].y;
					layer[inter.layer].next_behavior.x_chess = chess[me][i].x;
					layer[inter.layer].next_behavior.y_chess = chess[me][i].y;
					gridInfo[layer[inter.layer].next_behavior.stx][layer[inter.layer].next_behavior.sty] = 0;
					gridInfo[layer[inter.layer].next_behavior.x_chess][layer[inter.layer].next_behavior.y_chess] = currBotColor;
					//接下来考虑这么走的 jian的摆放;
					for (int r = 0; r < 8; r++)
					{
						int foot_jian = 1;
						while (emptyplace(chess[me][i].x + foot_jian * run[r].x, chess[me][i].y + foot_jian * run[r].y))
						{
							layer[inter.layer].zongqingkuang++;
							//						temptime = (rand() % 100) * 0.01;
							//						if (temptime < p){
							layer[inter.layer].next_behavior.x_jian = chess[me][i].x + foot_jian * run[r].x;
							layer[inter.layer].next_behavior.y_jian = chess[me][i].y + foot_jian * run[r].y;
							gridInfo[layer[inter.layer].next_behavior.x_jian][layer[inter.layer].next_behavior.y_jian] = 2;//这是修改操作 然后回溯
							//if (layer[inter.layer].next_behavior.stx == 4 && layer[inter.layer].next_behavior.sty == 5 && layer[inter.layer].next_behavior.x_chess == 4 && layer[inter.layer].next_behavior.y_chess == 4 && layer[inter.layer].next_behavior.x_jian == 4 && layer[inter.layer].next_behavior.y_jian == 5)
							//	cout << "SB" <<endl;

							layer[inter.layer].next_behavior.beah_value = 0;
							double temp_value = judge(layer[inter.layer].next_behavior, i, inter.layer);
							if (temp_value >= minvalue)
							{
								layer[inter.layer].next_behavior.beah_value = temp_value;//这是需要插入的结构体;
								if (inter.layer == openlayer - 1)
								{
									if (layer[inter.layer - 1].present.beah_value - temp_value < layer[inter.layer - 1].result.beah_value)//
									{
										gridInfo[layer[inter.layer].next_behavior.x_jian][layer[inter.layer].next_behavior.y_jian] = 0;
										gridInfo[layer[inter.layer].next_behavior.stx][layer[inter.layer].next_behavior.sty] = currBotColor;
										gridInfo[layer[inter.layer].next_behavior.x_chess][layer[inter.layer].next_behavior.y_chess] = 0;//内部回溯
										returns.stepvalue = temp_value;
										cutting++;
										return returns;
									}
								}
								for (int ins = min(numstore, maxopenpoint - 2); ins >= -1; ins--)
								{
									if (ins == -1)
									{
										layer[inter.layer].store_behavior[ins + 1] = layer[inter.layer].next_behavior;
										numstore = min(maxopenpoint - 1, numstore + 1);
										minvalue = layer[inter.layer].store_behavior[maxopenpoint - 1].beah_value;
										break;
									}
									if (layer[inter.layer].store_behavior[ins].beah_value <= temp_value)
									{
										layer[inter.layer].store_behavior[ins + 1] = layer[inter.layer].store_behavior[ins];
									}
									else
									{
										layer[inter.layer].store_behavior[ins + 1] = layer[inter.layer].next_behavior;
										numstore = min(maxopenpoint - 1, numstore + 1);
										minvalue = layer[inter.layer].store_behavior[maxopenpoint - 1].beah_value;
										break;
									}
								}
							}
							gridInfo[layer[inter.layer].next_behavior.x_jian][layer[inter.layer].next_behavior.y_jian] = 0;
							foot_jian++;
						}
					}
					gridInfo[layer[inter.layer].next_behavior.stx][layer[inter.layer].next_behavior.sty] = currBotColor;
					gridInfo[layer[inter.layer].next_behavior.x_chess][layer[inter.layer].next_behavior.y_chess] = 0;
					chess[me][i].x = layer[inter.layer].next_behavior.stx;
					chess[me][i].y = layer[inter.layer].next_behavior.sty;
					foot++;
				}
			}
		}
	}
	
	//二层操作数量的决策
	if (inter.layer == 0 && v.ifsecond != 0)
	{
		clock_t endtime = clock();
		clock_t usetime = endtime - starttime;
		if (usetime != 0)
			times = min(1200,(int)(2 * max(9,990/(int)usetime - 1)));
		else
			times = 1200;
	}

	//游戏结束判断(用于外接 && 多层的返回)
	if (layer[inter.layer].zongqingkuang == 0)
	{	
		if (inter.layer == 0)
		{
			if (debugging != 0)
			{
				print_chess();
				cout << "无棋可走！";
				returns.advfoot = -dadvfoot();
				if (currBotColor == 1)
					cout << "后手（白色）获得胜利，优势为" << returns.advfoot << "步" << endl;
				else
					cout << "先手（黑色）获得胜利，优势为" << returns.advfoot << "步" << endl;
				cout << "分界线——————————————————————————————————————————" << endl;
			}
			returns.lose = 1;
			return returns;
		}
		else
		{
				returns.stepvalue = -10000;
				return returns;
		}
	}

	//多层搜索引擎
	if (v.ifsecond != 0 && stillwarring != 0)
	{
		//if (debugging)
		//{
		//	for (int i = 0; i < times; i++)
		//		cout << layer[inter.layer].store_behavior[i].stx << " " << layer[inter.layer].store_behavior[i].sty << " " << layer[inter.layer].store_behavior[i].x_chess << " " << layer[inter.layer].store_behavior[i].y_chess 
		//			<< " " << layer[inter.layer].store_behavior[i].x_jian << " " << layer[inter.layer].store_behavior[i].y_jian << " " << layer[inter.layer].store_behavior[i].beah_value << endl;
		//}
		interstruct internext;
		for (int i = 0; i < GRIDSIZE; i++)
		{
			for (int j = 0; j < GRIDSIZE; j++)
				internext.chessbox[i][j] = inter.chessbox[i][j];
		}
		internext.style = 3; internext.layer = inter.layer + 1;
		internext.color = -inter.color;
		en = 1 - en;
		me = 1 - me;
		person[0] = me; person[1] = en;
		currBotColor *= -1;
		//此时完成的参数修改
		if (v.ifsecond == 2)
		{	//由于times是固定的 所以每一个程序都可以知道自己的归属 但是或许 需要增加一个层级之间的元素传递
			openlayer = 2; int s = limitationopen;
			if (limitationopen * (limitationopen + 1) <= times && (limitationopen *(limitationopen *(limitationopen + 1) + 1) > times))
			{
				openlayer = 3;
				s = limitationopen;
				while (s * (s + 1) <= times)s++;
				s--;//每层打开s个

			}
			else if (limitationopen *(limitationopen *(limitationopen + 1) + 1) <= times)
			{
				openlayer = 4;
				s = limitationopen;
				while (s * (s * (s + 1) + 1) <= times)s++;
				s--;//每层打开every个
			}
			//含义limitation表示最少检测数量 s表示实际数量 openlayer表示层数
			if (inter.layer < openlayer - 1)//非根节点
			{
				if (inter.layer == 0)
				{
					for (int i = 0; i < min(layer[inter.layer].zongqingkuang, (openlayer == 2) ? times : s); i++)
					{
						layer[inter.layer].present = layer[inter.layer].store_behavior[i];
						//cout << layer[inter.layer].store_behavior[i].stx << " " << layer[inter.layer].store_behavior[i].sty << " " << layer[inter.layer].store_behavior[i].x_chess << " " << layer[inter.layer].store_behavior[i].y_chess << " " << layer[inter.layer].store_behavior[i].x_jian << " " << layer[inter.layer].store_behavior[i].y_jian << "第" << inter.layer << "层的第" << i << "个store的价值为" << " " << layer[inter.layer].store_behavior[i].beah_value << endl;
						gridInfo[layer[inter.layer].store_behavior[i].stx][layer[inter.layer].store_behavior[i].sty] = 0;
						gridInfo[layer[inter.layer].store_behavior[i].x_chess][layer[inter.layer].store_behavior[i].y_chess] = -currBotColor;
						gridInfo[layer[inter.layer].store_behavior[i].x_jian][layer[inter.layer].store_behavior[i].y_jian] = 2;//这是修改操作 然后回溯
						layer[inter.layer].store_behavior[i].beah_value -= littlemain(internext, vv).stepvalue;
						if (layer[inter.layer].result.beah_value < layer[inter.layer].store_behavior[i].beah_value)
							layer[inter.layer].result = layer[inter.layer].store_behavior[i];
						gridInfo[layer[inter.layer].store_behavior[i].x_jian][layer[inter.layer].store_behavior[i].y_jian] = 0;
						gridInfo[layer[inter.layer].store_behavior[i].stx][layer[inter.layer].store_behavior[i].sty] = -currBotColor;
						gridInfo[layer[inter.layer].store_behavior[i].x_chess][layer[inter.layer].store_behavior[i].y_chess] = 0;
					}
				}
				else
				{
					for (int i = 0; i < min(layer[inter.layer].zongqingkuang, s); i++)
					{
						layer[inter.layer].present = layer[inter.layer].store_behavior[i];
						//cout << layer[inter.layer].store_behavior[i].stx << " " << layer[inter.layer].store_behavior[i].sty << " " << layer[inter.layer].store_behavior[i].x_chess << " " << layer[inter.layer].store_behavior[i].y_chess << " " << layer[inter.layer].store_behavior[i].x_jian << " " << layer[inter.layer].store_behavior[i].y_jian << "第" << inter.layer << "层的第" << i << "个store的价值为" << " " << layer[inter.layer].store_behavior[i].beah_value << endl;
						gridInfo[layer[inter.layer].store_behavior[i].stx][layer[inter.layer].store_behavior[i].sty] = 0;
						gridInfo[layer[inter.layer].store_behavior[i].x_chess][layer[inter.layer].store_behavior[i].y_chess] = -currBotColor;
						gridInfo[layer[inter.layer].store_behavior[i].x_jian][layer[inter.layer].store_behavior[i].y_jian] = 2;//这是修改操作 然后回溯
						layer[inter.layer].store_behavior[i].beah_value -= littlemain(internext, vv).stepvalue;
						if (layer[inter.layer].result.beah_value < layer[inter.layer].store_behavior[i].beah_value)
							layer[inter.layer].result = layer[inter.layer].store_behavior[i];
						gridInfo[layer[inter.layer].store_behavior[i].x_jian][layer[inter.layer].store_behavior[i].y_jian] = 0;
						gridInfo[layer[inter.layer].store_behavior[i].stx][layer[inter.layer].store_behavior[i].sty] = -currBotColor;
						gridInfo[layer[inter.layer].store_behavior[i].x_chess][layer[inter.layer].store_behavior[i].y_chess] = 0;
						if (layer[inter.layer - 1].present.beah_value - layer[inter.layer].store_behavior[i].beah_value < layer[inter.layer - 1].result.beah_value)
							break;
					}
				}
			}
			else
				layer[inter.layer].result = layer[inter.layer].store_behavior[0];//最终层 直接返回参数
		}
		if (v.ifsecond == 1)
		{
			if (times < limitationopen * (limitationopen + 1))//二层逻辑展开
			{
				if (inter.layer == 0)
				{
					for (int i = 0; i < min(times, layer[inter.layer].zongqingkuang); i++)//写入二层的关键 此时 t层的读取&存储已经全部完成 因此可以进行修改参数进入t+1层
					{
						//cout << layer[inter.layer].store_behavior[i].stx << " " << layer[inter.layer].store_behavior[i].sty << " " << layer[inter.layer].store_behavior[i].x_chess << " " << layer[inter.layer].store_behavior[i].y_chess << " " << layer[inter.layer].store_behavior[i].x_jian << " " << layer[inter.layer].store_behavior[i].y_jian << "第" << inter.layer << "层的第" << i << "个store的价值为" << " " << layer[inter.layer].store_behavior[i].beah_value << endl;
						gridInfo[layer[inter.layer].store_behavior[i].stx][layer[inter.layer].store_behavior[i].sty] = 0;
						gridInfo[layer[inter.layer].store_behavior[i].x_chess][layer[inter.layer].store_behavior[i].y_chess] = -currBotColor;
						gridInfo[layer[inter.layer].store_behavior[i].x_jian][layer[inter.layer].store_behavior[i].y_jian] = 2;//这是修改操作 然后回溯
						layer[inter.layer].store_behavior[i].beah_value -= littlemain(internext, vv).stepvalue;
						gridInfo[layer[inter.layer].store_behavior[i].x_jian][layer[inter.layer].store_behavior[i].y_jian] = 0;
						gridInfo[layer[inter.layer].store_behavior[i].stx][layer[inter.layer].store_behavior[i].sty] = -currBotColor;
						gridInfo[layer[inter.layer].store_behavior[i].x_chess][layer[inter.layer].store_behavior[i].y_chess] = 0;
					}
					int maxpoint = 0; double maxvalue = -100000;
					for (int i = 0; i < min(times, layer[inter.layer].zongqingkuang); i++)
					{
						if (layer[inter.layer].store_behavior[i].beah_value > maxvalue)
						{
							maxpoint = i;
							maxvalue = layer[inter.layer].store_behavior[i].beah_value;
						}
					}
					layer[inter.layer].result = layer[inter.layer].store_behavior[maxpoint];
				}
				else
				{
					//cout << layer[inter.layer].store_behavior[0].stx << " " << layer[inter.layer].store_behavior[0].sty << " " << layer[inter.layer].store_behavior[0].x_chess << " " << layer[inter.layer].store_behavior[0].y_chess << " " << layer[inter.layer].store_behavior[0].x_jian << " " << layer[inter.layer].store_behavior[0].y_jian << "第" << inter.layer << "层的第" << 0 << "个store的价值为" << " " << layer[inter.layer].store_behavior[0].beah_value << endl;
					layer[inter.layer].result = layer[inter.layer].store_behavior[0];
				}
			}
			else if (42 <= times && times < 258)//三层逻辑展开
			{
				if (inter.layer <= 1)
				{
					int every = 4;
					while (every * (every + 1) <= times)every++;
					every--;//每层打开every个
					for (int i = 0; i < min(every, layer[inter.layer].zongqingkuang); i++)
					{
						//cout << layer[inter.layer].store_behavior[i].stx << " " << layer[inter.layer].store_behavior[i].sty << " " << layer[inter.layer].store_behavior[i].x_chess << " " << layer[inter.layer].store_behavior[i].y_chess << " " << layer[inter.layer].store_behavior[i].x_jian << " " << layer[inter.layer].store_behavior[i].y_jian << "第" << inter.layer << "层的第" << i << "个store的价值为" << " " << layer[inter.layer].store_behavior[i].beah_value << endl;
						gridInfo[layer[inter.layer].store_behavior[i].stx][layer[inter.layer].store_behavior[i].sty] = 0;
						gridInfo[layer[inter.layer].store_behavior[i].x_chess][layer[inter.layer].store_behavior[i].y_chess] = -currBotColor;
						gridInfo[layer[inter.layer].store_behavior[i].x_jian][layer[inter.layer].store_behavior[i].y_jian] = 2;//这是修改操作 然后回溯
						layer[inter.layer].store_behavior[i].beah_value -= littlemain(internext, vv).stepvalue;
						gridInfo[layer[inter.layer].store_behavior[i].x_jian][layer[inter.layer].store_behavior[i].y_jian] = 0;
						gridInfo[layer[inter.layer].store_behavior[i].stx][layer[inter.layer].store_behavior[i].sty] = -currBotColor;
						gridInfo[layer[inter.layer].store_behavior[i].x_chess][layer[inter.layer].store_behavior[i].y_chess] = 0;
					}
					int maxpoint = 0; double maxvalue = -100000;
					for (int i = 0; i < min(every, layer[inter.layer].zongqingkuang); i++)
					{
						if (layer[inter.layer].store_behavior[i].beah_value >= maxvalue)
						{
							maxpoint = i;
							maxvalue = layer[inter.layer].store_behavior[i].beah_value;
						}
					}
					layer[inter.layer].result = layer[inter.layer].store_behavior[maxpoint];
				}
				else
				{
					layer[inter.layer].result = layer[inter.layer].store_behavior[0];
					//cout << layer[inter.layer].store_behavior[0].stx << " " << layer[inter.layer].store_behavior[0].sty << " " << layer[inter.layer].store_behavior[0].x_chess << " " << layer[inter.layer].store_behavior[0].y_chess << " " << layer[inter.layer].store_behavior[0].x_jian << " " << layer[inter.layer].store_behavior[0].y_jian << "第" << inter.layer << "层的第" << 0 << "个store的价值为" << " " << layer[inter.layer].store_behavior[0].beah_value << endl;
				}
			}
			else if (times >= 258)//四层逻辑展开
			{
				if (inter.layer <= 2)
				{
					int every = 4;
					while (every*(1 + every * (1 + every)) <= times)every++;
					every--;
					for (int i = 0; i < min(every, layer[inter.layer].zongqingkuang); i++)
					{
						//cout << layer[inter.layer].store_behavior[i].stx << " " << layer[inter.layer].store_behavior[i].sty << " " << layer[inter.layer].store_behavior[i].x_chess << " " << layer[inter.layer].store_behavior[i].y_chess << " " << layer[inter.layer].store_behavior[i].x_jian << " " << layer[inter.layer].store_behavior[i].y_jian << "第" << inter.layer << "层的第" << i << "个store的价值为" << " " << layer[inter.layer].store_behavior[i].beah_value << endl;
						gridInfo[layer[inter.layer].store_behavior[i].stx][layer[inter.layer].store_behavior[i].sty] = 0;
						gridInfo[layer[inter.layer].store_behavior[i].x_chess][layer[inter.layer].store_behavior[i].y_chess] = -currBotColor;
						gridInfo[layer[inter.layer].store_behavior[i].x_jian][layer[inter.layer].store_behavior[i].y_jian] = 2;//这是修改操作 然后回溯
						layer[inter.layer].store_behavior[i].beah_value -= littlemain(internext, vv).stepvalue;
						gridInfo[layer[inter.layer].store_behavior[i].x_jian][layer[inter.layer].store_behavior[i].y_jian] = 0;
						gridInfo[layer[inter.layer].store_behavior[i].stx][layer[inter.layer].store_behavior[i].sty] = -currBotColor;
						gridInfo[layer[inter.layer].store_behavior[i].x_chess][layer[inter.layer].store_behavior[i].y_chess] = 0;
					}
					int maxpoint = 0; double maxvalue = -100000;
					for (int i = 0; i < min(every, layer[inter.layer].zongqingkuang); i++)
					{
						if (layer[inter.layer].store_behavior[i].beah_value >= maxvalue)
						{
							maxpoint = i;
							maxvalue = layer[inter.layer].store_behavior[i].beah_value;
						}
					}
					layer[inter.layer].result = layer[inter.layer].store_behavior[maxpoint];
				}
				else
				{
					layer[inter.layer].result = layer[inter.layer].store_behavior[0];
					//cout << layer[inter.layer].store_behavior[0].stx << " " << layer[inter.layer].store_behavior[0].sty << " " << layer[inter.layer].store_behavior[0].x_chess << " " << layer[inter.layer].store_behavior[0].y_chess << " " << layer[inter.layer].store_behavior[0].x_jian << " " << layer[inter.layer].store_behavior[0].y_jian << "第" << inter.layer << "层的第" << 0 << "个store的价值为" << " " << layer[inter.layer].store_behavior[0].beah_value << endl;
				}
			}
		}
		//回溯
		en = 1 - en;
		me = 1 - me;
		person[0] = me; person[1] = en;
		currBotColor *= -1;
	}
	else
	{
		layer[inter.layer].result = layer[inter.layer].store_behavior[0];
		//cout << layer[inter.layer].store_behavior[0].stx << " " << layer[inter.layer].store_behavior[0].sty << " " << layer[inter.layer].store_behavior[0].x_chess << " " << layer[inter.layer].store_behavior[0].y_chess << " " << layer[inter.layer].store_behavior[0].x_jian << " " << layer[inter.layer].store_behavior[0].y_jian << "第" << inter.layer << "层的第" << 0 << "个store的价值为" << " " << layer[inter.layer].store_behavior[0].beah_value << endl;
	}
	//无论如何 走到这里的时候应该是已经完成了这个程序 存储在了layer[inter.layer].result里面
	
	//参数输出结构
	if (inter.layer == 0)
	{
		clock_t endtime = clock();
		clock_t usetime = endtime - starttime;
		if (debugging == 1)
		{
			cout << "程序执行的时间为：" << usetime << endl;
			cout << "可以多执行的次数为" << times << endl;

			//	cout << "初始还可能到达的格子数量为" << num_canarr << endl;
			//	cout << "初始的stadarrival为" << stadarrival << endl;
			//	cout << "收获的格子数量为" << layer[inter.layer].result.darrival << endl;
			cout << "格子加权价值为" << (double)((double)(pow(layer[inter.layer].result.darrival, 2) + v.first_c_arrival * layer[inter.layer].result.darrival) / layer[inter.layer].num_canarr)* c_arrival * signal(layer[inter.layer].result.darrival) << endl;

			//	cout << "初始还可能到达的格子价值和为" << num_canmobival << endl;
			//	cout << "初始的dmobi_val为" << stadmobi_val << endl;
			//	cout << "收获的格子价值为" << layer[inter.layer].result.dmobival << endl;
			double temp = (double)((double)layer[inter.layer].result.dmobival / layer[inter.layer].num_canmobival) * c_mobival;
			cout << "价值部分的价值为" << temp << endl;

			cout << "刺杀部分的价值为" << layer[inter.layer].result.assail << endl;
			cout << endl;
			print_chess();

			cout << "总共走法数为" << layer[inter.layer].zongqingkuang << "步" << endl;
			cout << "决策的最终价值为：" << layer[inter.layer].result.beah_value << endl;
		}
		//if (layer[inter.layer].result.stx == 5 && layer[inter.layer].result.sty == 3 && layer[inter.layer].result.x_chess == 1 && layer[inter.layer].result.y_chess == 2 && layer[inter.layer].result.x_jian == 0 && layer[inter.layer].result.y_jian == 3)
		//cout << layer[inter.layer].result.stx << ' ' << layer[inter.layer].result.sty << ' ' << layer[inter.layer].result.x_chess << ' ' << layer[inter.layer].result.y_chess << ' ' << layer[inter.layer].result.x_jian << ' ' << layer[inter.layer].result.y_jian << endl;
		if (debugging == 1)
		{
			cout << "优化步数" << cutting << endl;
			cout << "分界线——————————————————————————————————————————" << endl;
		}
	}

	//返回结构
	returns.stepvalue = layer[inter.layer].result.beah_value;
	returns.stx = layer[inter.layer].result.stx;
	returns.sty = layer[inter.layer].result.sty;
	returns.x_chess = layer[inter.layer].result.x_chess;
	returns.y_chess = layer[inter.layer].result.y_chess;
	returns.x_jian = layer[inter.layer].result.x_jian;
	returns.y_jian = layer[inter.layer].result.y_jian;
	return returns;
	//return 0;
}

