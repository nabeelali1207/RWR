////2022.12.08
//
//#pragma once
//
//#include <iostream>
//#include <cmath>
////#include <vector>
////#include <iterator>
//
////using namespace std;
//
//
//
//class myVector
//{
//	int *DATA;
//	int SIZE;
//	int CAPACITY;
//
//public:
//	class myIterator
//	{
//	public:
//		int *PTR;
//
//		myIterator();
//		void operator=(int* ptr);
//		int& operator*();
//		myIterator& operator++();
//		bool operator!=(myIterator ptr);
//		bool operator<(myIterator ptr);
//		bool operator<=(myIterator ptr);
//	};
//	myVector();
//	int operator[](int address);
//	float optimalCapacity(float newSIZE);
//	int size();
//	int capacity();
//	myIterator begin();
//	myIterator end();
//	void push_back(int val);
//	void pop_back();
//	void reserve(int val);
//	void shrink_to_fit();
//	void resize(int val);
//	bool empty();
//};
//
//
//
//myVector::myVector()
//{
//	DATA = NULL;
//	SIZE = 0;
//	CAPACITY = 0;
//}
//int myVector::operator[](int address)
//{
//	return DATA[address];
//}
//float myVector::optimalCapacity(float newSIZE)
//{
//	return pow(2, ceil(log2(float(newSIZE))));
//}
//int myVector::size()
//{
//	return SIZE;
//}
//int myVector::capacity()
//{
//	return CAPACITY;
//}
//myVector::myIterator myVector::begin()
//{
//	myIterator temp;
//	if (SIZE > 0)
//		temp = DATA;
//	else
//		temp = NULL;
//	return temp;
//}
//myVector::myIterator myVector::end()
//{
//	myIterator temp;
//	if (SIZE > 0)
//		temp = &DATA[SIZE];
//	else
//		temp = NULL;
//	return temp;
//}
//void myVector::push_back(int val)
//{
//	SIZE++;
//	if (SIZE > CAPACITY)
//	{
//		CAPACITY = optimalCapacity(float(SIZE));
//
//		int *temp = DATA;
//		DATA = new int[CAPACITY];
//		for (int i = 0; i < (SIZE - 1); i++)
//		{
//			DATA[i] = temp[i];
//		}
//		delete[] temp;
//	}
//	DATA[SIZE - 1] = val;
//}
//void myVector::pop_back()
//{
//	if (SIZE >= 1)
//		SIZE--;
//}
//void myVector::reserve(int val)
//{
//	if (val > CAPACITY)
//	{
//		CAPACITY = optimalCapacity(float(val));
//
//		int *temp = DATA;
//		DATA = new int[CAPACITY];
//		for (int i = 0; i < SIZE; i++)
//		{
//			DATA[i] = temp[i];
//		}
//		delete[] temp;
//	}
//}
//void myVector::shrink_to_fit()
//{
//	if (CAPACITY > SIZE)
//	{
//		CAPACITY = SIZE;
//
//		int *temp = DATA;
//		DATA = new int[SIZE];
//		for (int i = 0; i < SIZE; i++)
//		{
//			DATA[i] = temp[i];
//		}
//		delete[] temp;
//	}
//}
//void myVector::resize(int val)
//{
//	if (val < SIZE)
//		SIZE = val;
//}
//bool myVector::empty()
//{
//	if (SIZE > 0)
//		return 0;
//	else
//		return 1;
//}
//
//myVector::myIterator::myIterator()
//{
//	PTR = NULL;
//}
//void myVector::myIterator::operator=(int* ptr)
//{
//	PTR = ptr;
//}
//int& myVector::myIterator::operator*()
//{
//	return *PTR;
//}
//myVector::myIterator& myVector::myIterator::operator++()
//{
//	PTR++;
//	return *this;
//}
//bool myVector::myIterator::operator!=(myIterator ptr)
//{
//	if (PTR != ptr.PTR)
//		return true;
//	else
//		return false;
//}
//bool myVector::myIterator::operator<(myIterator ptr)
//{
//	if (PTR < ptr.PTR)
//		return true;
//	else
//		return false;
//}
//bool myVector::myIterator::operator<=(myIterator ptr)
//{
//	if (PTR <= ptr.PTR)
//		return true;
//	else
//		return false;
//}
//
//
//
//void advance(myVector::myIterator& ptr, int val)
//{
//	for (int i = 1; i <= val; i++)
//	{
//		++ptr;
//	}
//}
//
//
