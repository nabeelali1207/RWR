////#include <iostream>
////#include <cmath>
////#include <vector>
////#include <windows.h>
////
////
////using namespace std;
////
////
////
////struct Position2D
////{
////	int x, y;
////	Position2D()
////	{
////		x = 0; y = 0;
////		cout << "Position2D Default Constructor called\n";
////	}
////	Position2D(int posX, int posY)
////	{
////		x = posX;
////		y = posY;
////		cout << "Position2D Constructor called\n";
////	}
////};
////
////class Shape
////{
////protected:
////	Position2D startPosition;
////	Shape()
////	{
////		startPosition.x = 0;
////		startPosition.y = 0;
////		cout << "Shape Default-Constructor called\n";
////	}
////	Shape(int posX, int posY) :startPosition(posX, posY)
////	{
////		cout << "Shape Constructor called\n";
////	}
////public:
////	virtual void Draw() = 0;
////	virtual void updateData() = 0;
////};
////
////
////class Diamond :public Shape
////{
////public:
////	int scale;
////	//Scale >4 and odd number
////	Diamond(int posX, int posY, int _scale) :Shape(posX, posY)
////	{
////		scale = _scale;
////		cout << "Diamond Constructor called\n";
////	}
////	void Draw()
////	{
////		if (scale < 5)
////			return;
////		else if (scale % 2 == 0)
////			scale--;
////		int tempL = scale / 2, tempR = scale / 2 + 2;
////		for (int y = 1; y <= startPosition.y; y++)
////			cout << endl;
////		for (int i = 0; i <= scale / 2; i++)
////		{
////			for (int x = 1; x <= startPosition.x; x++)
////				cout << "  ";
////			for (int j = 1; j <= scale; j++)
////			{
////				if (j <= tempL)
////					cout << " ";
////				//if (i == 0 && j == 1 + tempL)
////				//	cout << "* ";
////				else if (j == tempL + 1 || j == tempR - 1)
////					cout << "*";
////				else if (j > tempL + 1 && j < tempR - 1)
////					cout << " ";
////				if (j >= tempR)
////					cout << " ";
////			}
////			cout << endl;
////			tempL--;
////			tempR++;
////		}
////		tempL += 2;
////		tempR -= 2;
////		for (int i = 0; i <= scale / 2 - 1; i++)
////		{
////			for (int x = 1; x <= startPosition.x; x++)
////				cout << "  ";
////			for (int j = 1; j <= scale; j++)
////			{
////				if (j <= tempL)
////					cout << " ";
////				//if (i == height / 2 && j == 1 + tempL)
////				//	cout << "* ";
////				else if (j == tempL + 1 || j == tempR - 1)
////					cout << "*";
////				else if (j > tempL + 1 && j < tempR - 1)
////					cout << " ";
////				if (j >= tempR)
////					cout << " ";
////			}
////			cout << endl;
////			tempL++;
////			tempR--;
////		}
////	}
////	void updateData() {}
////};
////
////class Triangle :public Shape
////{
////public:
////	int height;
////	Triangle(int posX, int posY, int _height) :Shape(posX, posY)
////	{
////		height = _height;
////		cout << "Triangle Constructor called \n";
////	}
////	void Draw()
////	{
////		int asterisks = 1, spaces = (2 * height - 1) / 2;
////		for (int y = 1; y <= startPosition.y; y++)
////			cout << endl;
////		for (int i = 1; i <= height; i++)
////		{
////			for (int x = 1; x <= startPosition.x; x++)
////				cout << "  ";
////			for (int a = 1; a <= spaces; a++)
////				cout << "  ";
////			for (int b = 1; b <= asterisks; b++)
////				cout << "* ";
////			for (int c = 1; c <= spaces; c++)
////				cout << "  ";
////			cout << endl;
////			asterisks += 2;
////			spaces--;
////		}
////	}
////	void updateData() {}
////};
////
////class Arrow :public Shape
////{
////public:
////	int length, loopLength = 10, posX, posY;
////	bool switchCase = 0;
////	Arrow(int _posX, int _posY, int _length) :Shape(_posX, _posY)
////	{
////		length = _length;
////		posX = _posX;
////		posY = _posY;
////		cout << "Arrow Constructor called \n";
////	}
////	void Draw()
////	{
////		do
////		{
////			for (int y = 1; y <= startPosition.y; y++)
////				cout << endl;
////			for (int i = 1; i <= length; i++)
////			{
////				for (int x = 1; x <= startPosition.x; x++)
////					cout << "  ";
////				cout << "--";
////			}
////			cout << ">";
////		} while (0);
////	}
////	void updateData()
////	{
////		if (switchCase == 0)
////		{
////			startPosition.y++;
////			length++;
////			if (startPosition.y >= posY + loopLength)
////				switchCase = 1;
////		}
////		else
////		{
////			startPosition.y--;
////			length--;
////			if (startPosition.y <= posY + 1)
////				switchCase = 0;
////		}
////	}
////	void foo()
////	{
////
////	}
////};
////
////class Rectangle :public Shape
////{
////public:
////	int width, height;
////	Rectangle(int posX, int posY, int _width, int _height) :Shape(posX, posY)
////	{
////		width = _width;
////		height = _height;
////		cout << "Rectangle Constructor called \n";
////	}
////	void Draw()
////	{
////		for (int y = 1; y <= startPosition.y; y++)
////			cout << endl;
////		for (int i = 1; i <= height; i++)
////		{
////			for (int x = 1; x <= startPosition.x; x++)
////				cout << "  ";
////			for (int j = 1; j <= width; j++)
////			{
////				if (i == 1 || i == height || j == 1 || j == width)
////					cout << "* ";
////				else
////					cout << "  ";
////			}
////			cout << endl;
////		}
////	}
////	void updateData() {}
////};
////class Square :public Rectangle
////{
////public:
////	Square(int posX, int posY, int width) :Rectangle(posX, posY, width, width) {}
////};
////
////
////class display
////{
////public:
////	vector<Shape*> v;
////	void addShape(Shape* temp)
////	{
////		v.push_back(temp);
////	}
////	void updateDisplay()
////	{
////		int myDelay = 0;
////		do
////		{
////			system("cls");
////			for (int i = 0; i < v.size(); i++)
////			{
////				v[i]->Draw();
////			}
////			Sleep(50);
////			if (myDelay % 2 == 0)
////				v[3]->updateData();
////			myDelay++;
////		} while (1);
////	}
////};
////
////struct test
////{
////	char c1;
////	int var2;
////};
////
////
////int main()
////{
////	//display D;
////
////	//D.addShape(new Diamond(8, 0, 5));
////	//D.addShape(new Square(7, 0, 5));
////	////D.addShape(new Rectangle(0, 0, 21, 5));
////	//D.addShape(new Triangle(1, 0, 5));
////	//D.addShape(new Arrow(0, 0, 10));
////
////	////D.updateDisplay();
////
////	////for (int i = 0; i < D.v.size(); i++)
////	////{
////	////	D.v[i]->Draw();
////	////}
////
////	////Serializing
////	//char arr[8] = { 1, 2, 3, 4, 5, 6, 7, 8};
////	//test *T = (test*)&arr[0];
////	//cout << hex << int(T->c1) << endl;
////	//cout << hex << int(T->var2) << endl;
////	//cout << sizeof(T);
////	
////	//De-Serializing
////	test T;
////	T.c1 = 1;
////	T.var2 = 16000;
////	//char* temp = new char[sizeof(test)];
////	char* temp = (char*)&T;
////
////	for (int i = 0; i < sizeof(test); i++)
////	{
////		cout << hex << uint16_t(temp[i]) << " " ;
////	}
////
////	system("pause");
////	return 0;
////}
//
//
//
//
//
//#include <iostream>
//#include <fstream>
//
//using namespace std;
//
//int main()
//{
//	ofstream outf("sample.bin", ios::binary | std::ios::trunc); //binary and trunc flags
//	if (!outf)
//	{
//		cerr << "invalid\n";
//		return 1;
//	}
//	int arr[128];
//	for (int i = 0; i < 128; i++)
//	{
//		arr[i] = i;
//	}
//	
//	outf.write((char *)arr,128*sizeof(int));
//	//for (int i = 0; i < 128; i++)
//	//{
//	//	outf << arr[i];
//	//}
//	
//
//
//	system("pause");
//	return 0;
//}