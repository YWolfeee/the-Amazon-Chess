#include"pch.h"
#include"标头.h"
#include<iostream>
#include <stdio.h>
#include<fstream>
#include<time.h>
constexpr auto GRIDSIZE = 8;
#define OBSTACLE 2
#define judge_black 0
#define judge_white 1
#define grid_black 1
#define grid_white -1 
//#define c_mobival 1
#define c_arrival 2
//#define c_assail 1
#define debugging 1
#define assail_kill 4
#define assail_checkmate 2

using namespace std;

//写参组织
extern struct versionstruct
{
	int version;
	int ifsecond;
	double totalcontrol = 1, halfcontrol = 0.5;
	double first_c_arrival = 0;
	double c_assail = 0;
	double c_mobival = 0;
	int limitationopen;
	//double alass_stad;
	//double almob_stad;
	//double alhal_stad;
	//double k_ass_stad;
	//double k_mob_stad;
	//double k_hal_stad;
};
extern struct interstruct
{
	int style;
	int chessbox[GRIDSIZE][GRIDSIZE];
	int color;
	int layer;
};
extern struct returnstruct
{
	int lose = 0;;
	int stx;
	int sty;
	int x_chess;
	int y_chess;
	int x_jian;
	int y_jian;
	int advfoot;
};
interstruct inter;
void chess_boxclear()
{
	for (int i = 0; i < GRIDSIZE; i++)
	{
		for (int j = 0; j < GRIDSIZE; j++)
		{
			inter.chessbox[i][j] = 0;
		}
	}
	inter.chessbox[0][(GRIDSIZE - 1) / 3] = inter.chessbox[(GRIDSIZE - 1) / 3][0]
		= inter.chessbox[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][0]
		= inter.chessbox[GRIDSIZE - 1][(GRIDSIZE - 1) / 3] = grid_black;
	inter.chessbox[0][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = inter.chessbox[(GRIDSIZE - 1) / 3][GRIDSIZE - 1]
		= inter.chessbox[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][GRIDSIZE - 1]
		= inter.chessbox[GRIDSIZE - 1][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = grid_white;
}

int main()
{
	inter.layer = 0;
	ofstream outfile;
	outfile.open("C:\\Users\\Alienware\\Desktop\\ML.txt");
	versionstruct v[2];
	v[0].version = 0; v[0].ifsecond = 2; v[0].c_mobival = 0; v[0].c_assail = 0; v[0].first_c_arrival = 0; v[0].halfcontrol = 1; v[0].totalcontrol = 3;
	//v[0].version = 0; v[0].c_mobival = 0; v[0].c_assail = 0; v[0].first_c_arrival = 0; v[0].halfcontrol = 2; v[0].totalcontrol = 3;
	v[1].version = 0; v[1].ifsecond = 2; v[1].c_mobival = 0; v[1].c_assail = 1; v[1].first_c_arrival = 0; v[1].halfcontrol = 1; v[1].totalcontrol = 3;
	chess_boxclear();
	if (debugging == 1)
	{
		cout << "请输入模式，0为一步，1为人机对决，2为机器之间,3为间断模式，4为ML模式";
		cin >> inter.style;
	}
	else if (debugging == 0)
		inter.style = 0;
	else if (debugging == 4)
		inter.style = 4;
	if (inter.style == 0)
	{
		littlemain(inter,v[0]);
	}
	else if (inter.style == 1)//未完成
	{
		cout << "欢迎进入人机模式，请输入先后手（0先1后）：";
		int temp; cin >> temp;
		cout << "请输入对手程序的参数值，0表示缺省：";
		cin >> v[0].version;
		if (v[0].version)
		{
			cout << "请依次输入版本号和一次项，total,half,刺杀值，活动性:";
			cin >> v[0].version >> v[0].first_c_arrival >> v[0].totalcontrol >> v[0].halfcontrol >> v[0].c_assail >> v[0].c_mobival;
		}
		cout << endl; "对决开始！";
		chess_boxclear();


	}
	else if (inter.style == 2)
	{
		cout << "这是一个用于对不同参数的操作进行评估的竞赛终端。" << endl;
		cout << "请输入先手程序的参数值，0表示缺省：";
		cin >> v[0].version;
		if (v[0].version)
		{
			cout << "请依次输入版本号和一次项，total,half,刺杀值，活动性:";
			cin >> v[0].version >> v[0].first_c_arrival >> v[0].totalcontrol >> v[0].halfcontrol >>  v[0].c_assail >> v[0].c_mobival;
		}
		cout << "请输入后手程序的参数值，0表示缺省：";
		cin >> v[1].version;
		if (v[1].version)
		{
			cout << "请依次输入版本号和一次项，total,half,刺杀值,活动性:";
			cin >> v[1].version >> v[1].first_c_arrival >> v[1].totalcontrol >> v[1].halfcontrol >> v[1].c_assail >> v[1].c_mobival;
		}
		clock_t starttime = clock();
		inter.color = 1;
		returnstruct returns;
		returns = littlemain(inter,v[(-inter.color+1)/2]);
		while (returns.lose == 0)
		{
			inter.chessbox[returns.stx][returns.sty] = 0;
			inter.chessbox[returns.x_chess][returns.y_chess] = inter.color;
			inter.chessbox[returns.x_jian][returns.y_jian] = OBSTACLE;
			
			inter.color *= -1;
			returns = littlemain(inter,v[(-inter.color + 1)/2]);
		}
		clock_t endtime = clock();
		cout << "对局用时" << -starttime+endtime << "ms" << endl;
		cout << "对决结束" << endl;
	}
	else if (inter.style == 3)
	{
		cin >> inter.color;
		for (int i = 0; i < GRIDSIZE; i++)
		{
			for (int j = 0; j < GRIDSIZE; j++)
			{
				cin >> inter.chessbox[j][i];
			}
		}
		littlemain(inter,v[0]);
	}
	else if (inter.style == 4)
	{
		cout << "这是一个用于自我对战实现参数优化的终端" << endl;
		v[0].version = 0; v[1].version = 0;
		v[0].ifsecond = 2; v[1].ifsecond = 2;
		v[0].c_mobival = 0; v[1].c_mobival = 0;
		v[0].first_c_arrival = 0; v[1].first_c_arrival = 0;
		v[0].totalcontrol = 3; v[1].totalcontrol = 3;
		//limitation

		for (int order = 0; order < 2; order++)//反应先后手 先看先手的情况
		{
			if (order == 0)
				outfile << "先手机器的胜率显示如下" << endl;
			else
				outfile << endl << endl << "后手机器的胜率如下" << endl;
			for (int li = 0; li < 4; li++)
			{
				v[0].limitationopen = li + 6;
				for (int i = 0; i < 4; i++)
				{
					v[0].c_assail = pow(2, i) * 0.5;
					for (int r = 0; r < 3; r++)
					{
						double times = (r + 1) * 0.25;
						v[0].halfcontrol = times * v[0].totalcontrol;
						int totalplace = 0, win = 0, totalfoot = 0;
						//参数调整部分//
						clock_t starttime = clock();
						for (int x = 0; x < 4; x++)
						{
							v[1].limitationopen = x + 6;
							for (int y = 0; y < 4; y++)
							{
								v[1].c_assail = pow(2, y) * 0.5;
								for (int z = 0; z < 3; z++)
								{
									double times2 = (z + 1) * 0.25;
									v[1].halfcontrol = times2 * v[1].totalcontrol;
									if (!(li == x && i == y && r == z))
									{
										totalplace++;
										inter.color = 1; inter.layer = 0;
										int tempturn = order;
										returnstruct returns;
										chess_boxclear();
										returns.lose = 0;
										returns = littlemain(inter, v[tempturn]);
										while (returns.lose == 0)
										{
											inter.chessbox[returns.stx][returns.sty] = 0;
											inter.chessbox[returns.x_chess][returns.y_chess] = inter.color;
											inter.chessbox[returns.x_jian][returns.y_jian] = OBSTACLE;
											inter.color *= -1;
											tempturn = 1 - tempturn;
											returns = littlemain(inter, v[tempturn]);
										}
										if (tempturn != 0)//v[0]永远是测试机器
										{
											win++;
											totalfoot += returns.advfoot;
										}
										else
											totalfoot -= returns.advfoot;
										//cout << "打完一场了！" << endl;
									}
								}
							}
						}
						//这里输出
						clock_t endtime = clock();
						cout << "用时约" << (endtime - starttime) / 1000 << "秒" << endl;
						outfile << "limit,half,assail的值分别为" << v[0].limitationopen << "," << v[0].halfcontrol << "," << v[0].c_assail << "时，BOT的胜率是" << (double)win / totalplace << "，平均步数为" << (double)totalfoot / totalplace << endl;
						//单一变量部分//
						/*
						//inter.color = 1 - 2 * order;
						inter.color = 1;
						int turn = order;
						returnstruct returns;
						chess_boxclear();
						returns.lose = 0;
						returns = littlemain(inter, v[turn]);
						while (returns.lose == 0)
						{
							inter.chessbox[returns.stx][returns.sty] = 0;
							inter.chessbox[returns.x_chess][returns.y_chess] = inter.color;
							inter.chessbox[returns.x_jian][returns.y_jian] = OBSTACLE;
							inter.color *= -1;
							turn = 1 - turn;
							returns = littlemain(inter, v[turn]);
						}
						if (turn != order)
						{
							//if (v[0].first_c_arrival == 0.5 && v[0].totalcontrol == 0.5 && r == 2)
							//	cout << "在0.5 0.5 0.333参数下，" << v[0].c_assail << ":" << v[1].c_assail << "中的先手获胜，步数为" << returns.advfoot << endl;
							sigwin++;
							sigtotalfoot += returns.advfoot;
						}
						else
						{
							//if (v[0].first_c_arrival == 0.5 && v[0].totalcontrol == 0.5 && r == 2)
							//	cout << "在0.5 0.5 0.333参数下，" << v[0].c_assail << ":" << v[1].c_assail << "中的后手获胜，步数为" << returns.advfoot << endl;

							sigtotalfoot -= returns.advfoot;
						}
						//outfile << "打完一场了！" << endl;

						//outfile << "参数组合（一次项，total，half）" <<v[0].first_c_arrival << "," << v[0].totalcontrol << "," << v[0].halfcontrol << "在" << order << "手（0先1后）" << "胜率为" << (double)win / totalplace  << ",平均获胜格子" << (double)totalfoot/totalplace << endl;

						//outfile << endl;
						*/
					}
				}
			}
		}
	}
	outfile.close();
	return 0;
}