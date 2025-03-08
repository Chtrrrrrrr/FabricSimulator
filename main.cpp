/*****************************************************************//**
 * \file   main.cpp
 * \brief  模拟三部曲——布料模拟
 *		   采用每个节点于其周围四个节点以假像弹簧形式链接的模拟方法
 * \author Chtrrrrrrr
 * \date   January 2025
 *********************************************************************/
//#include "stdafx"
#include <iostream>
#include <graphics.h>
#include <easyx.h>
#include "windows.system.h"
#include <conio.h>
#include <stdio.h>
#include <objidl.h>
#include <gdiplus.h>
#include <stdlib.h>

using namespace std;
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#pragma GCC optimize(3,"Ofast") //吸氧

const float g = 9.8; //重力加速度
const float timespeed = 0.3; //每一帧假设是多长时间
const float k = 5; //弹力系数k
const float defaultx = 40; //F = kx中的x标准长度
const float maxx = 300; //最大长度，大于断掉
const float airconstant = 1000; //空气阻力系数 = C * ruo * S
ExMessage MouseEvent;
bool IsDrugging = false; //是否正在拖拽
const int Width = 20; //布料
const int Height = 20;
const int Interval = 40; //间隔长度
const int r = 4; //半径

struct Vector
{
	float x;
	float y;
	//运算符重载
	Vector operator+(const Vector& v)
	{
		Vector temp;
		temp.x = x + v.x;
		temp.y = y + v.y;
		return temp;
	}
	Vector operator*(double k)
	{
		Vector temp;
		temp.x = x * k;
		temp.y = y * k;
		return temp;
	}
	//构造函数
	Vector(float X, float Y)
	{
		x = X;
		y = Y;
	}
	Vector()
	{
		x = 0;
		y = 0;
	}
	//矢量长
	float length()
	{
		return sqrt(x * x + y * y);
	}
};
Vector Wind; //风

//假设节点间作用力为弹力
class Particle
{
private: 
	float mass = 0.5; //质量
	Vector gravity; //重力
	Vector deltax; //delta x
	Vector velocity; //速度，为了方便这里不采用经典力学的动量冲量，使用运动学公式
	bool isdrugged = false; //是否被拖拽
public:
	bool isconnect[4] = { 1,1,1,1 }; //与其链接的四个是否都链接了，懒得用链表
	bool isfixed = false; //是否被固定住
	Vector* otherlocation[4]; //与其链接的四个的位置
	Vector location; //位置

	Particle()
	{
		location.x = 0;
		location.y = 0;
		gravity.y = mass * g;
	}
	void SetLocation(float x, float y) //设置位置
	{
		location.x = x;
		location.y = y;
	}
	void Calculate() //计算和画图
	{
		Vector summedforce; summedforce = summedforce + gravity + Wind; //合力，合力+重力
		if (MouseEvent.x > location.x - r && MouseEvent.x < location.x + r && MouseEvent.y > location.y - r && MouseEvent.y < location.y + r && MouseEvent.rbutton == true)
		{
			cout << "IsFixed!：" << isdrugged << endl;
			isfixed = true;
		}
		//鼠标拉力
		if (MouseEvent.x > location.x - r && MouseEvent.x < location.x + r && MouseEvent.y > location.y - r && MouseEvent.y < location.y + r && MouseEvent.lbutton == true && IsDrugging == false)
		{
			cout << "IsDrugged!：" << isdrugged << endl;
			isdrugged = true;
			IsDrugging = true;
		}
		if (MouseEvent.lbutton == false)
		{
			isdrugged = false;
			IsDrugging = false;
		}
		if (isdrugged == true)
		{
			cout << "IsCauculatingMouseForce!：" << isdrugged << endl;
			Vector elasticity(MouseEvent.x - location.x, MouseEvent.y - location.y); //弹力
			double len = elasticity.length();
			float elasticityforce = k * (len); //F = kx
			if (len > 1)
			{
				elasticity.x = elasticity.x * (elasticityforce / len);
				elasticity.y = elasticity.y * (elasticityforce / len);
			}
			else
			{
				elasticity.x = elasticity.x / len;
				elasticity.y = elasticity.y / len;
			}
			//cerr << "Node (" << location.x << ", " << location.y << ") elasticity: "<< elasticity.x << ", " << elasticity.y << " " << elasticityforce/len<<endl;
			summedforce = summedforce + elasticity; //加上弹力
			//setlinecolor(RGB(255, 225 * tanh(-0.001 * abs(elasticityforce))+225, 225 * tanh(-0.001 * abs(elasticityforce))+225)); //上色
			//line(location.x, location.y, MouseEvent.x, MouseEvent.y);
		}
		//计算 delta x，为了防止循环时之前的节点的新更新给之后的节点带来影响，应统一用旧状态计算完后再统一变成新状态
		for (int i = 0; i < 4; i++) //剩下四个相连的
			if (otherlocation[i] != nullptr && isconnect[i] == true) //如果还连着
			{
				Vector elasticity(otherlocation[i]->x - location.x, otherlocation[i]->y - location.y); //弹力
				double len = elasticity.length();
				float elasticityforce = k * (len - defaultx); //F = kx
				if (len > 2)
				{
					elasticity.x = elasticity.x * (elasticityforce / len);
					elasticity.y = elasticity.y * (elasticityforce / len);
				}
				else
				{
					elasticity.x = elasticity.x / len;
					elasticity.y = elasticity.y / len;
				}
				//cerr << "Node (" << location.x << ", " << location.y << ") elasticity: "<< elasticity.x << ", " << elasticity.y << " " << elasticityforce/len<<endl;
				summedforce = summedforce + elasticity; //加上弹力
				setlinecolor(RGB(255, 225 * tanh(-0.001 * abs(elasticityforce)) + 225, 225 * tanh(-0.001 * abs(elasticityforce)) + 225)); //上色
				line(location.x, location.y, otherlocation[i]->x, otherlocation[i]->y);
				//两种断裂情况
				if (len > maxx) isconnect[i] = false;
				if (MouseEvent.x > min(otherlocation[i]->x, location.x) && MouseEvent.x < max(otherlocation[i]->x, location.x) && MouseEvent.y > min(otherlocation[i]->y, location.y) && MouseEvent.y < max(otherlocation[i]->y, location.y) && MouseEvent.ctrl == true)
				{
					isconnect[i] = false;
					cout << "Is cutted!" << endl;
				}

			}
		//cerr << "Node (" << location.x << ", " << location.y << ") summed force: " << summedforce.x << ", " << summedforce.y << " " << endl;
		summedforce.x += -0.5 * airconstant * velocity.x * abs(velocity.x);
		summedforce.y += -0.5 * airconstant * velocity.y * abs(velocity.y);
		Vector velocity2 = velocity + Vector(summedforce.x / mass * timespeed, summedforce.y / mass * timespeed); //delta v = F / m * delta t
			//化曲为直
		/*setlinecolor(GREEN);
		setlinestyle(SOLID_FILL, 4);
		line(location.x, location.y, location.x + velocity2.x, location.y + velocity2.y); //速度
		setlinestyle(SOLID_FILL, 1);*/
		deltax.x = (velocity.x + velocity2.x) / 2.0 * timespeed; //运动学公式
		deltax.y = (velocity.y + velocity2.y) / 2.0 * timespeed; //同理
		//画图
		if (isfixed) setfillcolor(YELLOW);
		else setfillcolor(WHITE);
		solidcircle(location.x, location.y, r);
	}
	void Refresh() //更新位置
	{
		if (isfixed == true) return;
		location = location + deltax;
	}
};

int main()
{
	initgraph(1500, 1300,1);
	Particle fabric[Width][Height];
	//SetWorkingDirectory("D:\\EasyX\\");
	//initgraph(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), INIT_RENDERMANUAL | INIT_NOFORCEEXIT);
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	//initialization
	for (int i = 0; i < Width; i++)
		for (int j = 0; j < Height; j++)
		{
			fabric[i][j].SetLocation(100 + Interval * i, 100 + Interval * j);
			if (i==0||j==0) fabric[i][j].isfixed = true;
		}
	for (int i = 0; i < Width; i++)
		for (int j = 0; j < Height; j++)
		{
			if (i > 0) fabric[i][j].otherlocation[0] = &fabric[i - 1][j].location;
			else       fabric[i][j].isconnect[0] = false;
			if (j > 0) fabric[i][j].otherlocation[1] = &fabric[i][j - 1].location;
			else       fabric[i][j].isconnect[1] = false;
			if (i < Width - 1) fabric[i][j].otherlocation[2] = &fabric[i + 1][j].location;
			else               fabric[i][j].isconnect[2] = false;
			if (j < Height - 1) fabric[i][j].otherlocation[3] = &fabric[i][j + 1].location;
			else                fabric[i][j].isconnect[3] = false;

		}
	//body
	//cerr << fabric[1][1].otherlocation[1]->x;
	for (int i = 0; i < Width; i++) {
		for (int j = 0; j < Height; j++) {
			for (int k = 0; k < 4; k++) {
				if (fabric[i][j].isconnect[k]) {
					if (fabric[i][j].otherlocation[k] != nullptr) {
						cerr << "Node (" << i << ", " << j << ") is connected to ("
							<< (fabric[i][j].otherlocation[k]->x-100)/Interval << ", "
							<< (fabric[i][j].otherlocation[k]->y-100)/Interval << ")" << endl;
					}
					else {
						cerr << "Node (" << i << ", " << j << ") has isconnect["
							<< k << "] = true but otherlocation[" << k << "] is nullptr" << endl;
					}
				}
				else {
					cerr << "Node (" << i << ", " << j << ") has isconnect["
						<< k << "] = false" << endl;
				}
			}
		}
	}
	int Windforce = 0;
	cout << "————————————————————————————————————————————————" << endl
		<< "操作：左键拖拽，右键固定，滚轮上下为风力大小增减（可以为负），按住中键为风（只有水平），按住ctrl会将鼠标经过的连接断开（连接的长度大于300也会断）" << endl
		<< "如果想要更改参数，请去程序里修改，PS：鼠标移动慢点，因为帧率低可能会识别失败" << endl
		<< "Author:Chtrrrrrrr" << endl
		<< "————————————————————————————————————————————————" << endl;
	system("pause");
	for (;;)
	{
		peekmessage(&MouseEvent, EX_MOUSE);
		if (MouseEvent.mbutton == true)
			Wind.x = Windforce;
		else if (MouseEvent.mbutton == false)
			Wind.x = 0;
		Windforce += MouseEvent.wheel /120;
		cout << "风力" << Windforce << endl;
		MouseEvent.wheel = 0;
		BeginBatchDraw();
		cleardevice();
		for (int i = 0; i < Width; i++)
			for (int j = 0; j < Height; j++)
			{
				fabric[i][j].Calculate();
				//fabric[i][j].Display();
				fabric[i][j].Refresh();
			}
		FlushBatchDraw();
		Sleep(0);
		//_getch();
		/*
		* 	cout << peekmessage(&MouseEvent, EX_MOUSE) << endl;
		peekmessage(&KeyEvent, EX_KEY);
		if (KeyEvent.vkcode == 0x44) //D
			Wind.x = 10;
		else if (KeyEvent.vkcode == 0x41) //A
			Wind.x = -10;
		else if (KeyEvent.vkcode == 0x57) //W
			Wind.y = -10;
		else if (KeyEvent.vkcode == 0x53) //S
			Wind.y = 10;
		else
		{
			Wind.x = 0;
			Wind.y = 0;
		}
		if (KeyEvent.vkcode == VK_SPACE)
			_getch();

		*/
	}
	return 0;
}
