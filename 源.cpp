#include"pch.h"
#include"��ͷ.h"
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

//д����֯
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
		cout << "������ģʽ��0Ϊһ����1Ϊ�˻��Ծ���2Ϊ����֮��,3Ϊ���ģʽ��4ΪMLģʽ";
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
	else if (inter.style == 1)//δ���
	{
		cout << "��ӭ�����˻�ģʽ���������Ⱥ��֣�0��1�󣩣�";
		int temp; cin >> temp;
		cout << "��������ֳ���Ĳ���ֵ��0��ʾȱʡ��";
		cin >> v[0].version;
		if (v[0].version)
		{
			cout << "����������汾�ź�һ���total,half,��ɱֵ�����:";
			cin >> v[0].version >> v[0].first_c_arrival >> v[0].totalcontrol >> v[0].halfcontrol >> v[0].c_assail >> v[0].c_mobival;
		}
		cout << endl; "�Ծ���ʼ��";
		chess_boxclear();


	}
	else if (inter.style == 2)
	{
		cout << "����һ�����ڶԲ�ͬ�����Ĳ������������ľ����նˡ�" << endl;
		cout << "���������ֳ���Ĳ���ֵ��0��ʾȱʡ��";
		cin >> v[0].version;
		if (v[0].version)
		{
			cout << "����������汾�ź�һ���total,half,��ɱֵ�����:";
			cin >> v[0].version >> v[0].first_c_arrival >> v[0].totalcontrol >> v[0].halfcontrol >>  v[0].c_assail >> v[0].c_mobival;
		}
		cout << "��������ֳ���Ĳ���ֵ��0��ʾȱʡ��";
		cin >> v[1].version;
		if (v[1].version)
		{
			cout << "����������汾�ź�һ���total,half,��ɱֵ,���:";
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
		cout << "�Ծ���ʱ" << -starttime+endtime << "ms" << endl;
		cout << "�Ծ�����" << endl;
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
		cout << "����һ���������Ҷ�սʵ�ֲ����Ż����ն�" << endl;
		v[0].version = 0; v[1].version = 0;
		v[0].ifsecond = 2; v[1].ifsecond = 2;
		v[0].c_mobival = 0; v[1].c_mobival = 0;
		v[0].first_c_arrival = 0; v[1].first_c_arrival = 0;
		v[0].totalcontrol = 3; v[1].totalcontrol = 3;
		//limitation

		for (int order = 0; order < 2; order++)//��Ӧ�Ⱥ��� �ȿ����ֵ����
		{
			if (order == 0)
				outfile << "���ֻ�����ʤ����ʾ����" << endl;
			else
				outfile << endl << endl << "���ֻ�����ʤ������" << endl;
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
						//������������//
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
										if (tempturn != 0)//v[0]��Զ�ǲ��Ի���
										{
											win++;
											totalfoot += returns.advfoot;
										}
										else
											totalfoot -= returns.advfoot;
										//cout << "����һ���ˣ�" << endl;
									}
								}
							}
						}
						//�������
						clock_t endtime = clock();
						cout << "��ʱԼ" << (endtime - starttime) / 1000 << "��" << endl;
						outfile << "limit,half,assail��ֵ�ֱ�Ϊ" << v[0].limitationopen << "," << v[0].halfcontrol << "," << v[0].c_assail << "ʱ��BOT��ʤ����" << (double)win / totalplace << "��ƽ������Ϊ" << (double)totalfoot / totalplace << endl;
						//��һ��������//
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
							//	cout << "��0.5 0.5 0.333�����£�" << v[0].c_assail << ":" << v[1].c_assail << "�е����ֻ�ʤ������Ϊ" << returns.advfoot << endl;
							sigwin++;
							sigtotalfoot += returns.advfoot;
						}
						else
						{
							//if (v[0].first_c_arrival == 0.5 && v[0].totalcontrol == 0.5 && r == 2)
							//	cout << "��0.5 0.5 0.333�����£�" << v[0].c_assail << ":" << v[1].c_assail << "�еĺ��ֻ�ʤ������Ϊ" << returns.advfoot << endl;

							sigtotalfoot -= returns.advfoot;
						}
						//outfile << "����һ���ˣ�" << endl;

						//outfile << "������ϣ�һ���total��half��" <<v[0].first_c_arrival << "," << v[0].totalcontrol << "," << v[0].halfcontrol << "��" << order << "�֣�0��1��" << "ʤ��Ϊ" << (double)win / totalplace  << ",ƽ����ʤ����" << (double)totalfoot/totalplace << endl;

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