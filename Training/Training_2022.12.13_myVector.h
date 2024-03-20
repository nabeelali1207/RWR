////2022.12.13
//
//#pragma once
//
//#include <iostream>
//#include <cmath>
////#include <vector>
////#include <iterator>
//
//using namespace std;
//
//template <typename T> class myVector
//{
//	T *DATA;
//	int SIZE;
//	int CAPACITY;
//
//public:
//	template <typename T> class myIterator
//	{
//	public:
//		T *PTR;
//
//		myIterator()
//		{
//			PTR = NULL;
//		}
//
//		void operator=(T* ptr)
//		{
//			PTR = ptr;
//		}
//		T& operator*()
//		{
//			return *PTR;
//		}
//		myIterator& operator++()
//		{
//			PTR++;
//			return *this;
//		}
//		bool operator!=(myIterator ptr)
//		{
//			if (PTR != ptr.PTR)
//				return true;
//			else
//				return false;
//		}
//		bool operator<(myIterator ptr)
//		{
//			if (PTR < ptr.PTR)
//				return true;
//			else
//				return false;
//		}
//		bool operator<=(myIterator ptr)
//		{
//			if (PTR <= ptr.PTR)
//				return true;
//			else
//				return false;
//		}
//	};
//	myVector()
//	{
//		DATA = NULL;
//		SIZE = 0;
//		CAPACITY = 0;
//	}
//	myVector(myVector& other) //Assignemnt overloading alternative
//	{
//		CAPACITY = other.CAPACITY;
//		SIZE = other.SIZE;
//		//memcpy(DATA, other.DATA, other.SIZE);
//		//DATA = new T[CAPACITY];
//		DATA = (T*)malloc(sizeof(T) * CAPACITY);
//		for (int i = 0; i < SIZE; i++)
//		{
//			//DATA[i]=other.DATA[i];
//			DATA[i] = move(other.DATA[i]);
//		}
//	}
//
//	T operator[](int address)
//	{
//		return DATA[address];
//	}
//	template <typename T> myVector<T> operator+(myVector<T> other)
//	{
//		T TEMP;
//		myVector<T> temp;
//		temp.DATA = NULL;
//		temp.SIZE = 0;
//		temp.CAPACITY = temp.optimalCapacity(SIZE);
//		if (SIZE == other.SIZE)
//		{
//			temp.DATA = (T*)malloc(sizeof(T) * CAPACITY);
//			for (int i = 0; i < SIZE; i++)
//			{
//				TEMP = DATA[i] + other.DATA[i];
//				temp.push_back(TEMP);
//			}
//		}
//		return temp;
//
//	}
//	void operator=(myVector<T> rightSide)
//	{
//		//if (SIZE == rightSide.SIZE)
//		//{
//			delete[] DATA;
//			DATA = NULL;
//			SIZE = 0;
//			CAPACITY = 0;
//			for (int i = 0; i < rightSide.SIZE; i++)
//			{
//				push_back(rightSide.DATA[i]);
//			}
//		//}
//		
//	}
//	
//	float optimalCapacity(float newSIZE)
//	{
//		return pow(2, ceil(log2(float(newSIZE))));
//	}
//	int size()
//	{
//		return SIZE;
//	}
//	int capacity()
//	{
//		return CAPACITY;
//	}
//	myIterator<T> begin()
//	{
//		myIterator<T> temp;
//		if (SIZE > 0)
//			temp = DATA;
//		else
//			temp = NULL;
//		return temp;
//	}
//	myIterator<T> end()
//	{
//		myIterator<T> temp;
//		if (SIZE > 0)
//			temp = &DATA[SIZE];
//		else
//			temp = NULL;
//		return temp;
//	}
//	void push_back(T& val)
//	{
//		SIZE++;
//		if (SIZE > CAPACITY)
//		{
//			CAPACITY = optimalCapacity(float(SIZE));
//
//			T *temp = DATA;
//			DATA = (T*)malloc(sizeof(T) * CAPACITY);
//			for (int i = 0; i < (SIZE - 1); i++)
//			{
//				//DATA[i] = T(temp[i]);
//				//DATA[i].T(temp[i]);
//				//DATA[i]=temp[i];
//				DATA[i] = std::move(temp[i]);
//			}
//			//free(temp);
//		}
//		DATA[SIZE - 1] = val;
//	}
//	T pop_back()
//	{
//		if (SIZE >= 1)
//			SIZE--;
//		return DATA[SIZE];
//	}
//	void reserve(int val)
//	{
//		if (val > CAPACITY)
//		{
//			CAPACITY = optimalCapacity(float(val));
//
//			T *temp = DATA;
//			DATA = new T[CAPACITY];
//			for (int i = 0; i < SIZE; i++)
//			{
//				if (temp == NULL)
//					break;
//				DATA[i] = temp[i];
//			}
//			delete[] temp;
//		}
//	}
//	void shrink_to_fit()
//	{
//		if (CAPACITY > SIZE)
//		{
//			CAPACITY = SIZE;
//
//			T *temp = DATA;
//			DATA = new T[SIZE];
//			for (int i = 0; i < SIZE; i++)
//			{
//				DATA[i] = temp[i];
//			}
//			delete[] temp;
//		}
//	}
//	void resize(int val)
//	{
//		if (val < SIZE)
//			SIZE = val;
//	}
//	bool empty()
//	{
//		if (SIZE > 0)
//			return 0;
//		else
//			return 1;
//	}
//	void printData()
//	{
//			for (int i = 0; i < SIZE; i++)
//			{
//				cout << "Data Element # " << i + 1 << ": " << DATA[i] << endl;
//			}
//	}
//};
//
//template <typename T> void advance(myVector<T>::myIterator<T>& ptr, T val)
//{
//	for (int i = 1; i <= val; i++)
//	{
//		++ptr;
//	}
//};