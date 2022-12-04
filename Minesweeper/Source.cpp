#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <easyx.h>
#include <queue>
#include <algorithm>
using namespace std;
#define ROW 10
#define COL 20
#define MINECOUNT 20
#define IMGW 50

enum State
{
	Engaging,
	GameOver
};

State Nowstate = Engaging;

int DisableMineCount = 0;

class Vector2
{
public:
	int x;
	int y;	
};

void Showmap(int map[ROW][COL])
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			printf("%4d ", map[i][j]);
		}
		printf("\n");
	}
	printf("%d", DisableMineCount);
	printf("\n");
}

void Initmap(int map[ROW][COL], Vector2 MinePos[])
{	
	//将map清零
	memset(map, 0, ROW * COL * sizeof(int));
	DisableMineCount = 0;

	//埋雷
	for (int i = 0; i < MINECOUNT;)
	{
		int r = rand() % ROW;
		int c = rand() % COL;
		if (map[r][c] == 0)
		{
			map[r][c] = -1;
			MinePos[i].x = r;
			MinePos[i].y = c;
			i++;
		}
	}

	//遍历雷周围的格
	for (int i = 0; i < MINECOUNT; i++)
	{
		for (int a = MinePos[i].x - 1; a <= MinePos[i].x + 1; a++)
		{
			for (int b = MinePos[i].y - 1; b <= MinePos[i].y + 1; b++)
			{
				if (a < ROW && a >= 0 && b < COL && b >= 0)
				{
					//确保边缘没出去
					if (map[a][b] != -1)
					{
						//雷格的数字不能改
						map[a][b]++;
					}
				}
			}
		}
	}

	//把所有格子盖住
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			map[i][j] += 20;
		}
	}
}

void Drawmap(int map[ROW][COL], IMAGE img[])
{
	//加载图片
	for (int i = 0; i < 12; i++)
	{
		char fileName[50] = { 0 };
		sprintf_s(fileName, "./images/%d.jpg", i);
		loadimage(img + i, fileName, IMGW, IMGW);
		//putimage(i * IMGW, 0, img + i);
	}

	//把数组里面的数据，全都转换成对应的图片，贴到图形窗口上面
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (map[i][j] >= 0 && map[i][j] <= 8)
			{
				putimage(j * IMGW, i * IMGW, img + map[i][j]);
			}
			else if (map[i][j] == -1)
			{
				putimage(j * IMGW, i * IMGW, img + 9);
			}
			else if (map[i][j] >= 19 && map[i][j] <= 28)
			{
				putimage(j * IMGW, i * IMGW, img + 10);
			}
			else if (map[i][j] == -100)
			{
				putimage(j * IMGW, i * IMGW, img + 11);
			}
		}
	}
}

void OpenNull(int map[ROW][COL], int row, int col, queue<Vector2> Accessible)
{	
	if (map[row][col] == 0)
	{
		Vector2 temp;
		temp.x = row;
		temp.y = col;
		Accessible.push(temp);
		while (Accessible.size() > 0)
		{
			Vector2 check = Accessible.front();
			Accessible.pop();
			for (int i = check.x - 1; i <= check.x + 1; i++)
			{
				for (int j = check.y - 1; j <= check.y + 1; j++)
				{
					if (i < ROW && i >= 0 && j < COL && j >= 0)
					{
						if (map[i][j] == 20)
						{
							Vector2 temp2;
							temp2.x = i;
							temp2.y = j;
							Accessible.push(temp2);           //洪水算法遍历所有相连的空格子

						}
						if (map[i][j] >= 20 && map[i][j] <= 28)
						{
							map[i][j] -= 20;                  //将这些空格子周围的格子都打开								
						}												
					}
				}
			}		
		}
	}
}

void JudgeGameOver(int map[ROW][COL], int row, int col, Vector2 MinePos[])
{
	//如果点到雷就结束
	if (map[row][col] == 19)
	{
		for (int i = 0; i < MINECOUNT; i++)
		{
			map[MinePos[i].x][MinePos[i].y] -= 20;
		}
		Nowstate = GameOver;
	}
}

void mouseEvent(int map[ROW][COL], queue<Vector2> a,Vector2 MinePos[])
{
	//定义消息结构体
	ExMessage msg;
	if (peekmessage(&msg, EX_MOUSE))
	{
		//把鼠标坐标转换成数组下标
		int c = msg.x / IMGW;
		int r = msg.y / IMGW;
		if (msg.message == WM_LBUTTONDOWN)
		{
			if (map[r][c] > 19 && map[r][c] <= 28)
			{
				map[r][c] -= 20;
				OpenNull(map, r, c, a);
				JudgeGameOver(map, r, c, MinePos);
				Showmap(map);
			}	
			else if (map[r][c] == 19)
			{
				OpenNull(map, r, c, a);
				JudgeGameOver(map, r, c, MinePos);
				Showmap(map);
			}
		}
		else if (msg.message == WM_RBUTTONDOWN)
		{
			if (map[r][c] == 19)
			{
				map[r][c] -= 119;
				DisableMineCount++;
				Showmap(map);
			}
			else if (map[r][c] >= 0 && map[r][c] <= 8)
			{

			}
			else
			{
				Nowstate = GameOver;
			}
		}
	}
	
}

bool VictoryJudge(int map[ROW][COL])
{
	//计算胜利时场景中的点数，也就是判断胜利条件
	int x = 0;
	bool V = false;
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (map[i][j] > 10)
			{
				x++;
			}
		}
	}
	x == 0 ? V = true : false;
	return V;
}

int main()
{
	//创建窗口
	initgraph(COL * IMGW, ROW * IMGW, EX_SHOWCONSOLE);
	//设置随机数种子
	srand((unsigned)time(NULL));
	int map[ROW][COL] = {0};	
	queue<Vector2> AccessibleBlock;
	//生成雷，并存于vector2中
	Vector2 MinePos[MINECOUNT] = { 0 };
	IMAGE img[12]; //定义图片数组
	Initmap(map,MinePos);
	Showmap(map);

	while (true)
	{
		mouseEvent(map, AccessibleBlock, MinePos);
		Drawmap(map, img);
		if (Nowstate == GameOver)
		{
			int ret = MessageBox(GetHWnd(), "寄啦!", "GG", MB_OKCANCEL);    //TEXT名，title名，box类型
			if (ret == IDOK)
			{
				Initmap(map,MinePos);
				Showmap(map);
				Nowstate = Engaging;
			}
			else
			{
				exit(886);
			}
		}
		if (DisableMineCount == MINECOUNT)
		{
			if (VictoryJudge(map))
			{
				int ret = MessageBox(GetHWnd(), "牛蛙!", "过关", MB_OKCANCEL);    //TEXT名，title名，box类型
				if (ret == IDOK)
				{
					Initmap(map, MinePos);
					Showmap(map);
					Nowstate = Engaging;
				}
				else
				{
					exit(886);
				}
			}
		}
	}	
	system("pause");
	return 0;
}