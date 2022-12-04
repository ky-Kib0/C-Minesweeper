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
	//��map����
	memset(map, 0, ROW * COL * sizeof(int));
	DisableMineCount = 0;

	//����
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

	//��������Χ�ĸ�
	for (int i = 0; i < MINECOUNT; i++)
	{
		for (int a = MinePos[i].x - 1; a <= MinePos[i].x + 1; a++)
		{
			for (int b = MinePos[i].y - 1; b <= MinePos[i].y + 1; b++)
			{
				if (a < ROW && a >= 0 && b < COL && b >= 0)
				{
					//ȷ����Եû��ȥ
					if (map[a][b] != -1)
					{
						//�׸�����ֲ��ܸ�
						map[a][b]++;
					}
				}
			}
		}
	}

	//�����и��Ӹ�ס
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
	//����ͼƬ
	for (int i = 0; i < 12; i++)
	{
		char fileName[50] = { 0 };
		sprintf_s(fileName, "./images/%d.jpg", i);
		loadimage(img + i, fileName, IMGW, IMGW);
		//putimage(i * IMGW, 0, img + i);
	}

	//��������������ݣ�ȫ��ת���ɶ�Ӧ��ͼƬ������ͼ�δ�������
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
							Accessible.push(temp2);           //��ˮ�㷨�������������Ŀո���

						}
						if (map[i][j] >= 20 && map[i][j] <= 28)
						{
							map[i][j] -= 20;                  //����Щ�ո�����Χ�ĸ��Ӷ���								
						}												
					}
				}
			}		
		}
	}
}

void JudgeGameOver(int map[ROW][COL], int row, int col, Vector2 MinePos[])
{
	//����㵽�׾ͽ���
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
	//������Ϣ�ṹ��
	ExMessage msg;
	if (peekmessage(&msg, EX_MOUSE))
	{
		//���������ת���������±�
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
	//����ʤ��ʱ�����еĵ�����Ҳ�����ж�ʤ������
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
	//��������
	initgraph(COL * IMGW, ROW * IMGW, EX_SHOWCONSOLE);
	//�������������
	srand((unsigned)time(NULL));
	int map[ROW][COL] = {0};	
	queue<Vector2> AccessibleBlock;
	//�����ף�������vector2��
	Vector2 MinePos[MINECOUNT] = { 0 };
	IMAGE img[12]; //����ͼƬ����
	Initmap(map,MinePos);
	Showmap(map);

	while (true)
	{
		mouseEvent(map, AccessibleBlock, MinePos);
		Drawmap(map, img);
		if (Nowstate == GameOver)
		{
			int ret = MessageBox(GetHWnd(), "����!", "GG", MB_OKCANCEL);    //TEXT����title����box����
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
				int ret = MessageBox(GetHWnd(), "ţ��!", "����", MB_OKCANCEL);    //TEXT����title����box����
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