//#include <iostream>
//#include <cmath>
//#include <vector>
//
//
//using namespace std;
//
//
//
//struct Position2D
//{
//	int x, y;
//	Position2D()
//	{
//		x = 0; y = 0;
//		cout << "Position2D Default Constructor called\n";
//	}
//	Position2D(int posX, int posY)
//	{
//		x = posX;
//		y = posY;
//		cout << "Position2D Constructor called\n";
//	}
//};
//
//
//class Shape
//{
//protected:
//	Position2D startPosition;
//	Shape()
//	{
//		startPosition.x = 0;
//		startPosition.y = 0;
//		cout << "Shape Default-Constructor called\n";
//	}
//	Shape(int posX, int posY) :startPosition(posX, posY)
//	{
//		cout << "Shape Constructor called\n";
//	}
//public:
//	virtual void Draw() = 0;
//};
//
//
//class Diamond :public Shape
//{
//public:
//	int scale;
//	//Scale >4 and odd number
//	Diamond(int posX, int posY, int _scale) :Shape(posX, posY)
//	{
//		scale = _scale;
//		cout << "Diamond Constructor called\n";
//	}
//	void Draw()
//	{
//		if (scale < 5)
//			return;
//		else if (scale % 2 == 0)
//			scale--;
//		int tempL = scale / 2, tempR = scale / 2 + 2;
//		for (int y = 1; y <= startPosition.y; y++)
//			cout << endl;
//		for (int i = 0; i <= scale / 2; i++)
//		{
//			for (int x = 1; x <= startPosition.x; x++)
//				cout << "  ";
//			for (int j = 1; j <= scale; j++)
//			{
//				if (j <= tempL)
//					cout << " ";
//				//if (i == 0 && j == 1 + tempL)
//				//	cout << "* ";
//				else if (j == tempL + 1 || j == tempR - 1)
//					cout << "*";
//				else if (j > tempL + 1 && j < tempR - 1)
//					cout << " ";
//				if (j >= tempR)
//					cout << " ";
//			}
//			cout << endl;
//			tempL--;
//			tempR++;
//		}
//		tempL += 2;
//		tempR -= 2;
//		for (int i = 0; i <= scale / 2 - 1; i++)
//		{
//			for (int x = 1; x <= startPosition.x; x++)
//				cout << "  ";
//			for (int j = 1; j <= scale; j++)
//			{
//				if (j <= tempL)
//					cout << " ";
//				//if (i == height / 2 && j == 1 + tempL)
//				//	cout << "* ";
//				else if (j == tempL + 1 || j == tempR - 1)
//					cout << "*";
//				else if (j > tempL + 1 && j < tempR - 1)
//					cout << " ";
//				if (j >= tempR)
//					cout << " ";
//			}
//			cout << endl;
//			tempL++;
//			tempR--;
//		}
//	}
//};
//
//class Triangle :public Shape
//{
//public:
//	int height;
//	Triangle(int posX, int posY, int _height) :Shape(posX, posY)
//	{
//		height = _height;
//		cout << "Triangle Constructor called \n";
//	}
//	void Draw()
//	{
//		int asterisks = 1, spaces = (2 * height - 1) / 2;
//		for (int y = 1; y <= startPosition.y; y++)
//			cout << endl;
//		for (int i = 1; i <= height; i++)
//		{
//			for (int x = 1; x <= startPosition.x; x++)
//				cout << "  ";
//			for (int a = 1; a <= spaces; a++)
//				cout << "  ";
//			for (int b = 1; b <= asterisks; b++)
//				cout << "* ";
//			for (int c = 1; c <= spaces; c++)
//				cout << "  ";
//			cout << endl;
//			asterisks += 2;
//			spaces--;
//		}
//	}
//};
//
//class Rectangle :public Shape
//{
//public:
//	int width, height;
//	Rectangle(int posX, int posY, int _width, int _height) :Shape(posX, posY)
//	{
//		width = _width;
//		height = _height;
//		cout << "Rectangle Constructor called \n";
//	}
//	void Draw()
//	{
//		for (int y = 1; y <= startPosition.y; y++)
//			cout << endl;
//		for (int i = 1; i <= height; i++)
//		{
//			for (int x = 1; x <= startPosition.x; x++)
//				cout << "  ";
//			for (int j = 1; j <= width; j++)
//			{
//				if (i == 1 || i == height || j == 1 || j == width)
//					cout << "* ";
//				else
//					cout << "  ";
//			}
//			cout << endl;
//		}
//	}
//};
//
//class Square :public Rectangle
//{
//public:
//	Square(int posX, int posY, int width) :Rectangle(posX, posY, width, width) {}
//};
//
//
//class display
//{
//public:
//	vector<Shape*> v;
//	void addShape(Shape* temp)
//	{
//		v.push_back(temp);
//	}
//};
//
//
//
//int main()
//{
//	//vector<Shape*> v;
//	//Square testSquare(0, 0, 10);
//	//testSquare.Draw();
//	//v.push_back(new Diamond(0, 0, 10));
//	//v.push_back(new Square(0, 0, 20));
//	//v.push_back(new Rectangle(0, 0, 10, 20 ));
//	//v[0]->Draw();
//
//	display D;
//
//	D.addShape(new Diamond(8, 0, 10));
//	D.addShape(new Square(7, 0, 7));
//	D.addShape(new Rectangle(0, 0, 21, 5));
//	D.addShape(new Triangle(1, 0, 10));
//
//
//	for (int i = 0; i < D.v.size(); i++)
//	{
//		D.v[i]->Draw();
//	}
//
//
//	//NEXT TASK MOVING ARROW 
//	system("pause");
//	return 0;
//}