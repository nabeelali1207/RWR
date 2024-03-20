////2022.12.06
//
////1
//#include <iostream>
//#include <vector>
//
//using namespace std;
//
//int main()
//{
//	vector<int>v;
//	vector<int>::iterator it;
//
//	
//	cout << "Initial Size: " << v.SIZE() << endl;
//	cout << "Initial Capacity: " << v.capacity() << endl << endl;
//	for (int i = 0; i < 10; i++)
//	{
//		v.push_back(i);
//		cout << "push_back(" << i << ")\nSize: " << v.SIZE() << endl;
//		cout << "Capacity: " << v.capacity() << endl << endl;
//	}
//
//	cout << "Current Size: " << v.SIZE() << endl;
//	cout << "Current Capacity: " << v.capacity() << endl << endl;
//	v.reSIZE(0);
//	cout << "Size after reSIZE(0): " << v.SIZE() << endl;
//	cout << "Capacity after reSIZE(0): " << v.capacity() << endl << endl;
//
//	for (int i = 0; i < 5; i++)
//	{
//		v.push_back(i);
//		cout << "push_back(" << i << ")\nSize: " << v.SIZE() << endl;
//		cout << "Capacity: " << v.capacity() << endl << endl;
//	}
//
//	v.shrink_to_fit();
//	cout << "Current Size: " << v.SIZE() << endl;
//	cout << "Current Capacity: " << v.capacity() << endl << endl;
//	v.pop_back();
//	cout << "Size after pop_back(): " << v.SIZE() << endl;
//	cout << "Capacity after pop_back(): " << v.capacity() << endl << endl;
//
//	system("pause");
//	system("cls");
//
//	for (int i = 0; i < 10; i++)
//	{
//		v.push_back(i + 10);
//	}
//	for (int i = 0; i < v.SIZE(); i++)
//	{
//		cout << "Data " << i << ": " << v[i] << endl;
//	}
//	it = v.begin() + 4;
//	v.erase(it);
//	for (int i = 0; i < v.SIZE(); i++)
//	{
//		cout << "Data " << i << " after erase(): " << v[i] << endl;
//	}
//
//	system("pause");
//	return 0;
//}








////2
//#include <iostream>
//#include <vector>
//
//using namespace std;
//
//template<typename T>
//class Vector
//{
//public:
//	typedef vector<vector<T>> type;
//};
//
//int main()
//{
//	Vector<int>::type intSequences;
//	Vector<string>::type stringSequences;
//
//
//
//	return 0;
//}








////3.1 Custom Vector Class
//#include <iostream>
//#include <math.h>
////#include <vector>
//using namespace std;
//
//class myVector
//{
//private:
//	int *DATA;
//	int SIZE;
//	int CAPACITY;
//public:
//	class iterator
//	{
//
//	};
//	myVector();
//
//	int size();
//	int capacity();
//	void push_back(int val);
//	void pop_back();
//	int operator[](int address);
//	//int findOptimalCapacity(int newSIZE);
//	
//};
//
//int main()
//{
//	myVector v;
//
//	//cout << endl << endl;
//	for (int i = 0; i < 10; i++)
//	{
//		v.push_back(i + 10);
//		cout << "\nSize = " << v.size() << endl;
//		cout << "Capacity = " << v.capacity() << endl;
//		cout << "v[" << i << "] = " << v[i] << endl;
//	}
//	cout << endl << endl;
//	for (int i = 0; i < 10; i++)
//	{
//		v.pop_back();
//		cout << "\nSize = " << v.size() << endl;
//		cout << "Capacity = " << v.capacity() << endl;
//		cout << "v[" << v.size() - 1 << "] = " << v[v.size() - 1] << endl;
//	}
//
//	
//	
//	system("pause");
//	return 0;
//}
//
//myVector::myVector()
//{
//	DATA = NULL;
//	SIZE = 0;
//	CAPACITY = 0;
//}
//
//int myVector::size()
//{
//	return SIZE;
//}
//int myVector::capacity()
//{
//	return CAPACITY;
//}
//
//int myVector::operator[](int address)
//{
//	return DATA[address];
//}
//
//void myVector::push_back(int val)
//{
//	if (SIZE == 0)
//	{
//		if (SIZE >= CAPACITY)
//			CAPACITY++;
//		SIZE++;
//
//		//delete[] DATA;
//		DATA = new int[SIZE];
//		DATA[0] = val;
//	}
//	else
//	{
//		if (SIZE >= CAPACITY)
//			CAPACITY++;
//		SIZE++;
//		int num = 1;
//		while (num < CAPACITY)
//		{
//			num *= 2;
//		}
//		CAPACITY = num;
//
//		int *temp = new int[SIZE - 1];
//		for (int i = 0; i < (SIZE - 1); i++)
//		{
//			temp[i] = DATA[i];
//		}
//		delete[] DATA;
//
//		DATA = new int[CAPACITY];
//		for (int i = 0; i < (SIZE - 1); i++)
//		{
//			DATA[i] = temp[i];
//		}
//		DATA[SIZE - 1] = val;
//		delete[] temp;
//	}
//}
//
//void myVector::pop_back()
//{
//	if (SIZE == 1)
//	{
//		SIZE--;
//		delete[] DATA;
//	}
//	else if (SIZE > 1)
//	{
//		SIZE--;
//		
//		//int num = 1;
//		//while (num >= SIZE)
//		//{
//		//	num *= 2;
//		//}
//		//CAPACITY = num;
//
//		//int* temp = new int[CAPACITY];
//		//for (int i = 0; i < (SIZE - 1); i++)
//		//{
//		//	temp[i] = DATA[i];
//		//}
//		//delete[] DATA;
//
//		//DATA = new int[CAPACITY];
//		//for (int i = 0; i < (SIZE - 1); i++)
//		//{
//		//	DATA[i] = temp[i];
//		//};
//		//delete[] temp;
//	}
//}






////3.2 Custom Vector Class
//#include <iostream>
//#include <cmath>
////#include <vector>
//using namespace std;
//
//class myVector
//{
//private:
//	int *DATA;
//	int SIZE;
//	int CAPACITY;
//public:
//	class iterator
//	{
//
//	};
//	myVector();
//
//	int size();
//	int capacity();
//	void push_back(int val);
//	void pop_back();
//	int operator[](int address);
//	float optimalCapacity(float newSIZE);
//	
//};
//
//int main()
//{
//	myVector v;
//
//	float test = 2;
//	cout << log2(test); //log2 returning 0 on test = 1
//	//cout << endl << endl;
//	for (int i = 0; i < 10; i++)
//	{
//		v.push_back(i + 10);
//		cout << "\nSize = " << v.size() << endl;
//		cout << "Capacity = " << v.capacity() << endl;
//		cout << "v[" << i << "] = " << v[i] << endl;
//	}
//	cout << endl << endl;
//	for (int i = 0; i < 10; i++)
//	{
//		v.pop_back();
//		cout << "\nSize = " << v.size() << endl;
//		cout << "Capacity = " << v.capacity() << endl;
//		cout << "v[" << v.size() - 1 << "] = " << v[v.size() - 1] << endl;
//	}
//
//	
//	
//	system("pause");
//	return 0;
//}
//
//myVector::myVector()
//{
//	DATA = new int[1];
//	SIZE = 0;
//	CAPACITY = 0;
//}
//
//int myVector::size()
//{
//	return SIZE;
//}
//int myVector::capacity()
//{
//	return CAPACITY;
//}
//
//int myVector::operator[](int address)
//{
//	return DATA[address];
//}
//
//float myVector::optimalCapacity(float newSIZE)
//{
//	return pow(2, ceil(log2(float(newSIZE))));
//}
//
//void myVector::push_back(int val)
//{
//	SIZE++;
//	if (SIZE > 1)
//	{
//		CAPACITY = optimalCapacity(float(SIZE));
//		int *temp = new int[SIZE - 1];
//		for (int i = 0; i < (SIZE - 1); i++)
//		{
//			temp[i] = DATA[i];
//		}
//		delete[] DATA;
//		DATA = new int[CAPACITY];
//		for (int i = 0; i < (SIZE - 1); i++)
//		{
//			DATA[i] = temp[i];
//		}
//		delete[] temp;
//	}
//	else
//	{
//		CAPACITY++;
//		DATA = new int[CAPACITY];
//	}
//	DATA[SIZE - 1] = val;
//	
//}
//
//void myVector::pop_back()
//{
//	if (SIZE >= 1)
//		SIZE--;
//
//}












