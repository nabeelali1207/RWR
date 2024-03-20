//2022.12.09

//1
//Templated Linked List
#include <iostream>
#include <vector>
#include <random>
#include <time.h>
#include <set>
#include "Training_2022.12.12_myVector.h"
//#include "Training_temp.h"

using namespace std;

template <typename T> struct Node
{
	Node *NEXT;
	T DATA;
};

template <typename T> class LinkedList
{
	Node<T> *HEAD, *TAIL;
	int SIZE;

public:
	template <typename T> class myIterator
	{
	public:
		Node<T> *PTR;

		myIterator()
		{
			PTR = NULL;
		}
		void operator=(Node<T> *ptr)
		{
			PTR = ptr;
		}
		T& operator*()
		{
			return PTR->DATA;
		}
		myIterator& operator++()
		{
			PTR = PTR->NEXT;
			return *this;
		}
		bool operator!=(Node<T> *ptr)
		{
			if (PTR != ptr)
				return 1;
			else
				return 0;
		}
		bool operator<(Node<T>* ptr)
		{
			if ((ptr == PTR) && (PTR->NEXT == NULL))
				return false;
			else
				return true;
		}
	};

	LinkedList()
	{
		HEAD = NULL;
		TAIL = NULL;
		SIZE = 0;
	}
	Node<T>* begin()
	{
		return HEAD;
	}
	Node<T>* end()
	{
		return TAIL->NEXT;
	}
	void push_back(T data)
	{
		Node<T> *tempNode = new Node<T>;
		tempNode->DATA = data;
		tempNode->NEXT = NULL;
		if (TAIL != NULL)
			TAIL->NEXT = tempNode;
		TAIL = tempNode;
		if (HEAD == NULL)
			HEAD = tempNode;
		SIZE++;
	}
	void push_front(T data)
	{
		Node<T> *tempNode = new Node<T>;
		tempNode->DATA = data;
		tempNode->NEXT = HEAD;
		HEAD = tempNode;
		if (TAIL == NULL)
			TAIL = tempNode;
		SIZE++;
	}
	void insertNode_afterOffset(T data, int offset)
	{
		if (offset == (SIZE - 1))
		{
			push_back(data);
		}
		else
		{
			Node *tempNode = new Node, *traversalPtr = HEAD;
			tempNode->DATA = data;
			for (int i = 1; i <= offset; i++)
			{
				traversalPtr = traversalPtr->NEXT;
			}
			tempNode->NEXT = traversalPtr->NEXT;
			traversalPtr->NEXT = tempNode;
			SIZE++;
		}
	}
	void insertNode_offset(T data, int offset)
	{
		if (offset == 0)
		{
			push_front(data);
		}
		else
		{
			Node *tempNode = new Node, *traversalPtr = HEAD;
			tempNode->DATA = data;
			for (int i = 1; i < offset; i++)
			{
				traversalPtr = traversalPtr->NEXT;
			}
			tempNode->NEXT = traversalPtr->NEXT;
			traversalPtr->NEXT = tempNode;
			SIZE++;
		}
	}
	void delete_back()
	{
		Node<T> *tempPtr = HEAD;
		while (tempPtr->NEXT != TAIL)
			tempPtr = tempPtr->NEXT;
		delete[] TAIL;
		TAIL = tempPtr;
		TAIL->NEXT = NULL;
		SIZE--;
	}
	void delete_front()
	{
		Node<T> *tempPtr = HEAD->NEXT;
		delete[] HEAD;
		HEAD = tempPtr;
		SIZE--;
	}
	void deleteNode_address(Node<T> *address)
	{
		Node<T> *tempPtr = HEAD;
		while (tempPtr->NEXT != address)
			tempPtr = tempPtr->NEXT;
		tempPtr->NEXT = address->NEXT;
		delete[] address;
		SIZE--;
	}
	void deleteNode_offset(int offset)
	{
		if (offset == 0)
		{
			delete_front();
		}
		else if (offset == (SIZE - 1))
		{
			delete_back();
		}
		else
		{
			Node<T> *traversalPtr1 = HEAD, *traversalPtr2 = HEAD->NEXT;
			for (int i = 1; i < offset; i++)
			{
				traversalPtr1 = traversalPtr1->NEXT;
				traversalPtr2 = traversalPtr2->NEXT;
			}
			traversalPtr1->NEXT = traversalPtr2->NEXT;
			delete[] traversalPtr2;
			SIZE--;
		}
	}
	void deleteNode_data(T data)
	{
		Node<T> *traversalPtr1 = HEAD, *traversalPtr2 = HEAD->NEXT;
		while (HEAD->DATA == data)
		{
			delete_front();
			traversalPtr1 = HEAD;
			traversalPtr2 = HEAD->NEXT;
		}
		while (traversalPtr2 != NULL)
		{
			if (traversalPtr2->DATA == data)
			{
				traversalPtr1->NEXT = traversalPtr2->NEXT;
				delete[] traversalPtr2;
				traversalPtr2 = traversalPtr1->NEXT;
				SIZE--;
				continue;
			}
			traversalPtr1 = traversalPtr1->NEXT;
			traversalPtr2 = traversalPtr2->NEXT;
		}
	}
	int searchNode(T data)
	{
		Node<T> *traversalPtr = HEAD;
		int offset = 0, found = 0;
		while (traversalPtr != NULL)
		{
			if (traversalPtr->DATA == data)
				return offset;
			offset++;
			traversalPtr = traversalPtr->NEXT;
		}
		return -1;
	}
	void bubbleSort()
	{
		Node<T> *traversalPtr1 = HEAD, *traversalPtr2 = HEAD->NEXT;
		while (traversalPtr2->NEXT != NULL)
		{
			if ((traversalPtr1 == HEAD) && (traversalPtr1->DATA > traversalPtr2->DATA))
			{
				traversalPtr1->NEXT = traversalPtr2->NEXT;
				traversalPtr2->NEXT = traversalPtr1;
				HEAD = traversalPtr2;
				traversalPtr1 = HEAD;
				traversalPtr2 = HEAD->NEXT;
				continue;
			}
			if (traversalPtr2->DATA > traversalPtr2->NEXT->DATA)
			{
				traversalPtr1->NEXT = traversalPtr2->NEXT;
				traversalPtr2->NEXT = traversalPtr1->NEXT->NEXT;
				traversalPtr1->NEXT->NEXT = traversalPtr2;
				if (traversalPtr1->NEXT == TAIL)
				{
					TAIL = traversalPtr2;

				}
				traversalPtr1 = HEAD;
				traversalPtr2 = HEAD->NEXT;
				continue;
			}
			traversalPtr1 = traversalPtr1->NEXT;
			traversalPtr2 = traversalPtr2->NEXT;
		}
	}
	void printNode(int offset)
	{
		if (HEAD == NULL)
		{
			cout << "List is empty\n";
		}
		else if (offset >= 0)
		{
			Node<T> *traversalPtr = HEAD;
			for (int i = 1; i <= offset; i++)
			{
				traversalPtr = traversalPtr->NEXT;
			}
			cout << "Node Data = " << traversalPtr->DATA;
			cout << ",\nNode Address = " << traversalPtr->NEXT << endl;
		}
	}
	void printNodes(int offsetStart, int offsetEnd)
	{
		if (HEAD == NULL)
		{
			cout << "List is empty\n";
		}
		else  if ((offsetStart >= 0) && (offsetEnd < SIZE))
		{
			Node<T> *traversalPtr = HEAD;
			for (int i = 1; i <= offsetStart; i++)
			{
				traversalPtr = traversalPtr->NEXT;
			}
			for (int i = offsetStart; i <= offsetEnd; i++)
			{
				cout << "Node Data = " << traversalPtr->DATA;
				cout << ",\nNode Address = " << traversalPtr->NEXT << endl;
				traversalPtr = traversalPtr->NEXT;
			}
		}
	}
	void printList()
	{
		if (HEAD == NULL)
		{
			cout << "List is empty\n";
		}
		else
		{
			Node<T> *tempPtr = HEAD;
			cout << "LinkedList:\n";
			while (tempPtr != NULL)
			{
				cout << "data = " << tempPtr->DATA << endl;
				tempPtr = tempPtr->NEXT;
			}
			cout << endl;
		}
	}
	int size()
	{
		return SIZE;
	}
};

int COUNT = 0;
class myClass
{
	
public:
	int val;
	myClass()
	{
		cout << "Constructor called";
		cout << "Val = " << val << endl;

		COUNT++;
	}
	~myClass()
	{
		cout << "Destructor called ";
		cout << "Val = " << val << endl;
	}
	myClass(const myClass& ptr)
	{
		val = ptr.val;
		cout << "Copy-Constructor called";
		cout << "Val = " << val << endl;

	}
	myClass(myClass&& ptr)noexcept:val(ptr.val)
	{
		cout << "Move-Constructor called";
		cout << "Val = " << val << endl;

	}
	myClass& operator=(myClass& other)
	{
		//*this=std::move(other);

		val = other.val;

		return *this;
	}
	void testing()
	{
		
	}
};



int main()
{
	myVector<myClass> v;
	//myVector<myClass> v;
	myClass temp;
	

	for (int i = 0; i < 8; i++)
	{
		temp.val = i;
		cout << v.size() << "    " << v.capacity() << endl;

		v.push_back(temp);
	}

	myVector<myClass> v2 = v;
	v.push_back(temp);
	
	v2.printData();
	cout << "Count = " << COUNT << endl;
	system("pause");
	return 0;
}
