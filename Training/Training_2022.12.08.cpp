//2022.12.08

////1
////Continued - 3.3 Custom Vector Class
//#include <iostream>
//#include <cmath>
////#include <vector>
////#include <iterator>
//using namespace std;
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
//void advance(myVector::myIterator& ptr, int val);
//
//
//
//
//
//int main()
//{
//	myVector v;
//	myVector::myIterator ptr;
//	//advance();
//	int range = 10;
//
//	for (int i = 0; i < range; i++)
//	{
//		v.push_back(i + 10);
//		cout << "v[" << i << "] = " << v[i] << endl;
//	}
//	cout << "\nSize = " << v.size() << endl;
//	cout << "Capacity = " << v.capacity() << endl;
//	for (int i = 0; i < (range / 2); i++)
//	{
//		v.pop_back();
//	}
//	cout << "\nSize = " << v.size() << endl;
//	cout << "Capacity = " << v.capacity() << endl;
//	for (int i = 0; i < range * 2; i++)
//	{
//		v.push_back(i + 100);
//	}
//	cout << "\nSize = " << v.size() << endl;
//	cout << "Capacity = " << v.capacity() << endl;
//
//	v.reserve(100);
//
//	cout << "After reserve(100):\nSize = " << v.size() << endl;
//	cout << "Capacity = " << v.capacity() << endl;
//
//	v.shrink_to_fit();
//	v.shrink_to_fit();
//
//	cout << "After shrink_to_fit():\nSize = " << v.size() << endl;
//	cout << "Capacity = " << v.capacity() << endl;
//
//	v.resize(400);
//
//	cout << endl << endl;
//
//	for (int i = 0; i < v.size(); i++)
//	{
//		cout << "v[" << i << "] = "  << v[i] << endl;
//	}
//	cout << endl << endl;
//	for (ptr = v.begin(); ptr <= v.end(); ++ptr)
//	{
//		cout << "*ptr =  " << *ptr << endl;
//	}
//
//	cout << endl << endl;
//
//	ptr = v.begin();
//	advance(ptr, 24);
//	cout << "Advanced *ptr = " << *ptr << endl;
//
//	system("pause");
//
//	return 0;
//}
//
//
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
//		temp = &DATA[SIZE - 1];
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










////2
////Linked List
//#include <iostream>
//#include <random>
//#include <time.h>
//#include "myVector.h"
//
//using namespace std;
//
//struct Node
//{
//	Node *NEXT;
//	int DATA;
//};
//
//class LinkedList
//{
//	Node *HEAD, *TAIL;
//	int SIZE;
//
//public:
//	class myIterator
//	{
//	public:
//		Node *PTR;
//
//		myIterator();
//		void operator=(Node *ptr);
//		int& operator*();
//		myIterator& operator++();
//		bool operator!=(Node *ptr);
//	};
//
//	LinkedList();
//	Node* begin();
//	Node* end();
//	void push_back(int data);
//	void push_front(int data);
//	void insertNode_afterOffset(int data, int offset);
//	void insertNode_offset(int data, int offset);
//	void delete_back();
//	void delete_front();
//	void deleteNode_address(Node *address);
//	void deleteNode_offset(int offset);
//	void deleteNode_data(int data);
//	int searchNode(int data);
//	void bubbleSort();
//	void printNode(int offset);
//	void printNodes(int offsetStart, int offsetEnd);
//	void printList();
//	int size(); 
//};
//
//
//
//
//
//int main()
//{
//	LinkedList test;
//	LinkedList::myIterator it;
//	
//	int range = 100;
//
//	srand(time(0));
//
//	for (int i = 1; i <= range; i++)
//	{
//		test.push_front(rand() % 1000);
//		test.push_back(2000 + (rand() % 1000));
//	}
//	test.printList();
//
//	test.bubbleSort();
//	it = test.begin();
//	cout << "*it method: \n";
//	for (it = test.begin(); it != test.end(); ++it)
//	{
//		cout << "val = " << *it << endl;
//	}
//
//	system("pause");
//}
//
//
//
//
//
//LinkedList::LinkedList()
//{
//	HEAD = NULL;
//	TAIL = NULL;
//	SIZE = 0;
//}
//Node* LinkedList::begin()
//{
//	return HEAD;
//}
//Node* LinkedList::end()
//{
//	return TAIL->NEXT;
//}
//void LinkedList::push_back(int data)
//{
//	Node *tempNode = new Node;
//	tempNode->DATA = data;
//	tempNode->NEXT = NULL;
//	if (TAIL != NULL)
//		TAIL->NEXT = tempNode;
//	TAIL = tempNode;
//	if (HEAD == NULL)
//		HEAD = tempNode;
//	SIZE++;
//}
//void LinkedList::push_front(int data)
//{
//	Node *tempNode = new Node;
//	tempNode->DATA = data;
//	tempNode->NEXT = HEAD;
//	HEAD = tempNode;
//	if (TAIL == NULL)
//		TAIL = tempNode;
//	SIZE++;
//}
//void LinkedList::insertNode_afterOffset(int data, int offset)
//{
//	if (offset == (SIZE - 1))
//	{
//		push_back(data);
//	}
//	else
//	{
//		Node *tempNode = new Node, *traversalPtr = HEAD;
//		tempNode->DATA = data;
//		for (int i = 1; i <= offset; i++)
//		{
//			traversalPtr = traversalPtr->NEXT;
//		}
//		tempNode->NEXT = traversalPtr->NEXT;
//		traversalPtr->NEXT = tempNode;
//		SIZE++;
//	}
//}
//void LinkedList::insertNode_offset(int data, int offset)
//{
//	if (offset == 0)
//	{
//		push_front(data);
//	}
//	else
//	{
//		Node *tempNode = new Node, *traversalPtr = HEAD;
//		tempNode->DATA = data;
//		for (int i = 1; i < offset; i++)
//		{
//			traversalPtr = traversalPtr->NEXT;
//		}
//		tempNode->NEXT = traversalPtr->NEXT;
//		traversalPtr->NEXT = tempNode;
//		SIZE++;
//	}
//}
//void LinkedList::delete_back()
//{
//	Node *tempPtr = HEAD;
//	while (tempPtr->NEXT != TAIL)
//		tempPtr = tempPtr->NEXT;
//	delete[] TAIL;
//	TAIL = tempPtr;
//	TAIL->NEXT = NULL;
//	SIZE--;
//}
//void LinkedList::delete_front()
//{
//	Node *tempPtr = HEAD->NEXT;
//	delete[] HEAD;
//	HEAD = tempPtr;
//	SIZE--;
//}
//void LinkedList::deleteNode_address(Node *address)
//{
//	Node *tempPtr = HEAD;
//	while (tempPtr->NEXT != address)
//		tempPtr = tempPtr->NEXT;
//	tempPtr->NEXT = address->NEXT;
//	delete[] address;
//	SIZE--;
//}
//void LinkedList::deleteNode_offset(int offset)
//{
//	if (offset == 0)
//	{
//		delete_front();
//	}
//	else if (offset == (SIZE - 1))
//	{
//		delete_back();
//	}
//	else
//	{
//		Node *traversalPtr1 = HEAD, *traversalPtr2 = HEAD->NEXT;
//		for (int i = 1; i < offset; i++)
//		{
//			traversalPtr1 = traversalPtr1->NEXT;
//			traversalPtr2 = traversalPtr2->NEXT;
//		}
//		traversalPtr1->NEXT = traversalPtr2->NEXT;
//		delete[] traversalPtr2;
//		SIZE--;
//	}
//}
//void LinkedList::deleteNode_data(int data)
//{
//	Node *traversalPtr1 = HEAD, *traversalPtr2 = HEAD->NEXT;
//	while (HEAD->DATA == data)
//	{
//		delete_front();
//		traversalPtr1 = HEAD;
//		traversalPtr2 = HEAD->NEXT;
//	}
//	while (traversalPtr2 != NULL)
//	{
//		if (traversalPtr2->DATA == data)
//		{
//			traversalPtr1->NEXT = traversalPtr2->NEXT;
//			delete[] traversalPtr2;
//			traversalPtr2 = traversalPtr1->NEXT;
//			SIZE--;
//			continue;
//		}
//		traversalPtr1 = traversalPtr1->NEXT;
//		traversalPtr2 = traversalPtr2->NEXT;
//	}
//}
//int LinkedList::searchNode(int data)
//{
//	Node *traversalPtr = HEAD;
//	int offset = 0, found = 0;
//	while (traversalPtr != NULL)
//	{
//		if (traversalPtr->DATA == data)
//			return offset;
//		offset++;
//		traversalPtr = traversalPtr->NEXT;
//	}
//	return -1;
//}
//void LinkedList::bubbleSort()
//{
//	Node *traversalPtr1 = HEAD, *traversalPtr2 = HEAD->NEXT;
//	while (traversalPtr2->NEXT != NULL)
//	{
//		if ((traversalPtr1 == HEAD) && (traversalPtr1->DATA > traversalPtr2->DATA))
//		{
//			traversalPtr1->NEXT = traversalPtr2->NEXT;
//			traversalPtr2->NEXT = traversalPtr1;
//			HEAD = traversalPtr2;
//			traversalPtr1 = HEAD;
//			traversalPtr2 = HEAD->NEXT;
//			continue;
//		}
//		if (traversalPtr2->DATA > traversalPtr2->NEXT->DATA)
//		{
//			traversalPtr1->NEXT = traversalPtr2->NEXT;
//			traversalPtr2->NEXT = traversalPtr1->NEXT->NEXT;
//			traversalPtr1->NEXT->NEXT = traversalPtr2;
//			if (traversalPtr1->NEXT == TAIL)
//			{
//				TAIL = traversalPtr2;
//
//			}
//			traversalPtr1 = HEAD;
//			traversalPtr2 = HEAD->NEXT;
//			continue;
//		}
//		traversalPtr1 = traversalPtr1->NEXT;
//		traversalPtr2 = traversalPtr2->NEXT;
//	}
//}
//void LinkedList::printNode(int offset)
//{
//	if (HEAD == NULL)
//	{
//		cout << "List is empty\n";
//	}
//	else if (offset >= 0)
//	{
//		Node *traversalPtr = HEAD;
//		for (int i = 1; i <= offset; i++)
//		{
//			traversalPtr = traversalPtr->NEXT;
//		}
//		cout << "Node Data = " << traversalPtr->DATA;
//		cout << ",\nNode Address = " << traversalPtr->NEXT << endl;
//	}
//}
//void LinkedList::printNodes(int offsetStart, int offsetEnd)
//{
//	if (HEAD == NULL)
//	{
//		cout << "List is empty\n";
//	}
//	else  if ((offsetStart >= 0) && (offsetEnd <= SIZE))
//	{
//		Node *traversalPtr = HEAD;
//		for (int i = 1; i <= offsetStart; i++)
//		{
//			traversalPtr = traversalPtr->NEXT;
//		}
//		for (int i = offsetStart; i < offsetEnd; i++)
//		{
//			cout << "Node Data = " << traversalPtr->DATA;
//			cout << ",\nNode Address = " << traversalPtr->NEXT << endl;
//			traversalPtr = traversalPtr->NEXT;
//		}
//	}
//}
//void LinkedList::printList()
//{
//	if (HEAD == NULL)
//	{
//		cout << "List is empty\n";
//	}
//	else
//	{
//		Node *tempPtr = HEAD;
//		cout << "LinkedList:\n";
//		while (tempPtr != NULL)
//		{
//			cout << "data = " << tempPtr->DATA << endl;
//			tempPtr = tempPtr->NEXT;
//		}
//		cout << endl;
//	}
//}
//int LinkedList::size()
//{
//	return SIZE;
//}
//
//
//
//
//LinkedList::myIterator::myIterator()
//{
//	PTR = NULL;
//}
//void LinkedList::myIterator::operator=(Node *ptr)
//{
//	PTR = ptr;
//}
//int& LinkedList::myIterator::operator*()
//{
//	return PTR->DATA;
//}
//LinkedList::myIterator& LinkedList::myIterator::operator++()
//{
//	PTR = PTR->NEXT;
//	return *this;
//}
//bool LinkedList::myIterator::operator!=(Node *ptr)
//{
//	if (PTR != ptr)
//		return 1;
//	else
//		return 0;
//}