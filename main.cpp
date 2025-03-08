/*****************************************************************//**
 * \file   main.cpp
 * \brief  ģ����������������ģ��
 *		   ����ÿ���ڵ�������Χ�ĸ��ڵ��Լ��񵯻���ʽ���ӵ�ģ�ⷽ��
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
#pragma GCC optimize(3,"Ofast") //����

const float g = 9.8; //�������ٶ�
const float timespeed = 0.3; //ÿһ֡�����Ƕ೤ʱ��
const float k = 5; //����ϵ��k
const float defaultx = 40; //F = kx�е�x��׼����
const float maxx = 300; //��󳤶ȣ����ڶϵ�
const float airconstant = 1000; //��������ϵ�� = C * ruo * S
ExMessage MouseEvent;
bool IsDrugging = false; //�Ƿ�������ק
const int Width = 20; //����
const int Height = 20;
const int Interval = 40; //�������
const int r = 4; //�뾶

struct Vector
{
	float x;
	float y;
	//���������
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
	//���캯��
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
	//ʸ����
	float length()
	{
		return sqrt(x * x + y * y);
	}
};
Vector Wind; //��

//����ڵ��������Ϊ����
class Particle
{
private: 
	float mass = 0.5; //����
	Vector gravity; //����
	Vector deltax; //delta x
	Vector velocity; //�ٶȣ�Ϊ�˷������ﲻ���þ�����ѧ�Ķ���������ʹ���˶�ѧ��ʽ
	bool isdrugged = false; //�Ƿ���ק
public:
	bool isconnect[4] = { 1,1,1,1 }; //�������ӵ��ĸ��Ƿ������ˣ�����������
	bool isfixed = false; //�Ƿ񱻹̶�ס
	Vector* otherlocation[4]; //�������ӵ��ĸ���λ��
	Vector location; //λ��

	Particle()
	{
		location.x = 0;
		location.y = 0;
		gravity.y = mass * g;
	}
	void SetLocation(float x, float y) //����λ��
	{
		location.x = x;
		location.y = y;
	}
	void Calculate() //����ͻ�ͼ
	{
		Vector summedforce; summedforce = summedforce + gravity + Wind; //����������+����
		if (MouseEvent.x > location.x - r && MouseEvent.x < location.x + r && MouseEvent.y > location.y - r && MouseEvent.y < location.y + r && MouseEvent.rbutton == true)
		{
			cout << "IsFixed!��" << isdrugged << endl;
			isfixed = true;
		}
		//�������
		if (MouseEvent.x > location.x - r && MouseEvent.x < location.x + r && MouseEvent.y > location.y - r && MouseEvent.y < location.y + r && MouseEvent.lbutton == true && IsDrugging == false)
		{
			cout << "IsDrugged!��" << isdrugged << endl;
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
			cout << "IsCauculatingMouseForce!��" << isdrugged << endl;
			Vector elasticity(MouseEvent.x - location.x, MouseEvent.y - location.y); //����
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
			summedforce = summedforce + elasticity; //���ϵ���
			//setlinecolor(RGB(255, 225 * tanh(-0.001 * abs(elasticityforce))+225, 225 * tanh(-0.001 * abs(elasticityforce))+225)); //��ɫ
			//line(location.x, location.y, MouseEvent.x, MouseEvent.y);
		}
		//���� delta x��Ϊ�˷�ֹѭ��ʱ֮ǰ�Ľڵ���¸��¸�֮��Ľڵ����Ӱ�죬Ӧͳһ�þ�״̬���������ͳһ�����״̬
		for (int i = 0; i < 4; i++) //ʣ���ĸ�������
			if (otherlocation[i] != nullptr && isconnect[i] == true) //���������
			{
				Vector elasticity(otherlocation[i]->x - location.x, otherlocation[i]->y - location.y); //����
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
				summedforce = summedforce + elasticity; //���ϵ���
				setlinecolor(RGB(255, 225 * tanh(-0.001 * abs(elasticityforce)) + 225, 225 * tanh(-0.001 * abs(elasticityforce)) + 225)); //��ɫ
				line(location.x, location.y, otherlocation[i]->x, otherlocation[i]->y);
				//���ֶ������
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
			//����Ϊֱ
		/*setlinecolor(GREEN);
		setlinestyle(SOLID_FILL, 4);
		line(location.x, location.y, location.x + velocity2.x, location.y + velocity2.y); //�ٶ�
		setlinestyle(SOLID_FILL, 1);*/
		deltax.x = (velocity.x + velocity2.x) / 2.0 * timespeed; //�˶�ѧ��ʽ
		deltax.y = (velocity.y + velocity2.y) / 2.0 * timespeed; //ͬ��
		//��ͼ
		if (isfixed) setfillcolor(YELLOW);
		else setfillcolor(WHITE);
		solidcircle(location.x, location.y, r);
	}
	void Refresh() //����λ��
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
	cout << "������������������������������������������������������������������������������������������������" << endl
		<< "�����������ק���Ҽ��̶�����������Ϊ������С����������Ϊ��������ס�м�Ϊ�磨ֻ��ˮƽ������סctrl�Ὣ��꾭�������ӶϿ������ӵĳ��ȴ���300Ҳ��ϣ�" << endl
		<< "�����Ҫ���Ĳ�������ȥ�������޸ģ�PS������ƶ����㣬��Ϊ֡�ʵͿ��ܻ�ʶ��ʧ��" << endl
		<< "Author:Chtrrrrrrr" << endl
		<< "������������������������������������������������������������������������������������������������" << endl;
	system("pause");
	for (;;)
	{
		peekmessage(&MouseEvent, EX_MOUSE);
		if (MouseEvent.mbutton == true)
			Wind.x = Windforce;
		else if (MouseEvent.mbutton == false)
			Wind.x = 0;
		Windforce += MouseEvent.wheel /120;
		cout << "����" << Windforce << endl;
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
