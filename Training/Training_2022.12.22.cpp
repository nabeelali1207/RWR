////1
//#include <iostream>
//#include <thread>
//#include <ctime>
//#include <chrono>
//#include <random>
//#include <cmath>
//#include <windows.h>
//
//using namespace std;
//using namespace std::chrono;
//
//void performOperation(uint64_t *_a, uint64_t *_b, uint64_t *_sum, int _size)
//{
//	for (int i = 0; i < _size; i++)
//	{
//		*_sum += pow(*_a, 2) + pow(*_b, 2);
//		_a++; _b++;
//	}
//}
//void arithmeticOperation(uint64_t *_a, uint64_t *_b, uint64_t *_sum, int _size)
//{
//	for (int i = 0; i < _size; i++)
//	{
//		*_sum += pow(*_a, 2) + pow(*_b, 2);
//		_a++; _b++;
//	}
//}
//
//int main()
//{
//	int size = 100000000;
//	uint64_t *a = new uint64_t[size], *b = new uint64_t[size], sum = 0, sum1 = 0, sum2 = 0;
//	time_point<system_clock> start, end;
//
//	for (int i = 0; i < size; i++)
//	{
//		a[i] = 1;
//		b[i] = 1;
//	}
//
//	start = system_clock::now();
//	//performOperation(a, b, &sum, size);
//	thread thread_obj1(arithmeticOperation, a, b, &sum1, size / 2);
//	thread thread_obj2(arithmeticOperation, &a[size / 2], &b[size / 2], &sum2, size / 2);
//	thread_obj1.join();
//	thread_obj2.join();
//	end = system_clock::now();
//
//	duration<double> elapsed_seconds = end - start;
//
//	if (sum1 > 0 || sum2 > 0)
//		sum = sum1 + sum2;
//	cout << "Elapsed time: " << elapsed_seconds.count() * 1000 << "ms\n";
//	cout << "Sum = " << sum << endl;
//
//	delete[] a;
//	delete[] b;
//
//	system("pause");
//	return 0;
//}









////2
//#include <iostream>
//#include <list>
//#include <vector>
//#include <iterator>
//#include <string>
//
//using namespace std;
//
//template <typename T> struct Node
//{
//	Node<T> *left, *right;
//	T data;
//};
//
//template <typename T> class BST
//{
//	Node<T> *top;
//public:
//	list<Node<T>*> queue;
//
//	BST()
//	{
//		top = NULL;
//	}
//	void push(T _data)
//	{
//		Node<T> *tempNode = new Node<T>, *tempPtr = top;
//		tempNode->data = _data;
//		tempNode->left = NULL;
//		tempNode->right = NULL;
//		if (top != NULL)
//		{
//			while (1)
//			{
//				if ((_data < tempPtr->data) && (tempPtr->left != NULL))
//					tempPtr = tempPtr->left;
//				else if ((_data > tempPtr->data) && (tempPtr->right != NULL))
//					tempPtr = tempPtr->right;
//				else if (_data < tempPtr->data)
//				{
//					tempPtr->left = tempNode;
//					break;
//				}
//				else if (_data > tempPtr->data)
//				{
//					tempPtr->right = tempNode;
//					break;
//				}
//			}
//		}
//		else
//		{
//			top = tempNode;
//		}
//	}
//	void printQueueBF()
//	{
//		Node<T> *currentNode;
//		queue.push_back(top);
//		while (1)
//		{
//			currentNode = queue[1];
//			if (currentNode->left != NULL)
//				queue.push_back(currentNode->left);
//			if (currentNode->right != NULL)
//				queue.push_back(currentNode->right);
//		}
//	}
//	void printRecursiveBF(Node<T> *currentNode, int level)
//	{
//		if (currentNode == NULL)
//			return;
//		if (level == 1)
//			cout << currentNode->data << " ";
//		else if (level > 1) 
//		{
//			printRecursiveBF(currentNode->left, level - 1);
//			printRecursiveBF(currentNode->right, level - 1);
//		}
//	}
//	void printRecursiveDF(Node<T> *currentNode)
//	{
//		if (currentNode != NULL)
//		{
//			cout << currentNode->data << " ";
//			printRecursiveDF(currentNode->left);
//			printRecursiveDF(currentNode->right);
//		}
//	}
//	void print(int mode)
//	{
//		int h;
//		switch (mode)
//		{
//		case 1:
//			printQueueBF();
//			break;
//		case 3:
//			h = maxDepth(top);
//			for (int i = 1; i <= h; i++)
//				printRecursiveBF(top, i);
//			break;
//		case 4:
//			printRecursiveDF(top);
//			break;
//		}
//	}
//	Node<T>* getTop()
//	{
//		return top;
//	}
//	int maxDepth(Node<T> *currentNode)
//	{
//		if (currentNode != NULL)
//		{
//			int leftDepth = maxDepth(currentNode->left);
//			int rightDepth = maxDepth(currentNode->right);
//			if (leftDepth > rightDepth)
//				return (leftDepth + 1);
//			else
//				return (rightDepth + 1);
//		}
//		else
//			return 0;
//	}
//
//	//void printBT(const string& prefix, const Node<T>* node, bool isLeft)
//	//{
//	//	if (node != nullptr)
//	//	{
//	//		cout << prefix;
//	//		cout << (isLeft ? "|--" : "|__");
//	//		// print the value of the node
//	//		cout << node->data << endl;
//	//		// enter the next tree level - left and right branch
//	//		printBT(prefix + (isLeft ? "|   " : "    "), node->left, true);
//	//		printBT(prefix + (isLeft ? "|   " : "    "), node->right, false);
//	//	}
//	//}
//	//void print()
//	//{
//	//	printBT("", top, false);
//	//}
//};
//
//int main()
//{
//	BST<int> test;
//	list<int>::iterator it;
//	test.push(5);
//	test.push(8);
//	test.push(9);
//	test.push(7);
//	test.push(3);
//	test.push(1);
//	test.push(4);
//	test.push(10);
//	test.print(1);
//	cout << endl;
//	//for (it = test.queue.begin(); it != test.queue.end(); it++)
//	//{
//	//	cout << " " << *it << "\n";
//	//}
//	system("pause");
//	return 0;
//}









////3
//#include <iostream>
//#include <list>
//#include <vector>
//#include <iterator>
//#include <string>
//
//using namespace std;
//
//template <typename T> struct Node
//{
//	Node<T> *left, *right;
//	T data;
//};
//
//template <typename T> class BST
//{
//	Node<T> *top;
//public:
//	list<Node<T>*> queue;
//
//	BST()
//	{
//		top = NULL;
//	}
//	void push(T _data)
//	{
//		Node<T> *tempNode = new Node<T>, *tempPtr = top;
//		tempNode->data = _data;
//		tempNode->left = NULL;
//		tempNode->right = NULL;
//		if (top != NULL)
//		{
//			while (1)
//			{
//				if ((_data < tempPtr->data) && (tempPtr->left != NULL))
//					tempPtr = tempPtr->left;
//				else if ((_data > tempPtr->data) && (tempPtr->right != NULL))
//					tempPtr = tempPtr->right;
//				else if (_data < tempPtr->data)
//				{
//					tempPtr->left = tempNode;
//					break;
//				}
//				else if (_data > tempPtr->data)
//				{
//					tempPtr->right = tempNode;
//					break;
//				}
//			}
//		}
//		else
//		{
//			top = tempNode;
//		}
//	}
//	void printQueueBF()
//	{
//		Node<T> *currentNode;
//		queue.push_front(top);
//		cout << queue.size() << endl;
//		while (1)
//		{
//			currentNode = queue.front();
//			cout << currentNode->data << " ";
//			queue.pop_front();
//			if (currentNode->left != NULL)
//				queue.push_back(currentNode->left);
//			if (currentNode->right != NULL)
//				queue.push_back(currentNode->right);
//			if ((currentNode->left == NULL) && (currentNode->right == NULL) && (queue.size() == 0))
//				break;
//		}
//	}
//	void printQueueDF()
//	{
//		Node<T> *currentNode;
//		queue.push_back(top);
//		cout << queue.size() << endl;
//		while (1)
//		{
//			currentNode = queue.front();
//			cout << currentNode->data << " ";
//			queue.pop_front();
//			if (currentNode->right != NULL)
//				queue.push_front(currentNode->right);
//			if (currentNode->left != NULL)
//				queue.push_front(currentNode->left);
//			if ((currentNode->left == NULL) && (currentNode->right == NULL) && (queue.size() == 0))
//				break;
//		}
//	}
//	void printRecursiveBF(Node<T> *currentNode, int level)
//	{
//		if (currentNode == NULL)
//			return;
//		if (level == 1)
//			cout << currentNode->data << " ";
//		else if (level > 1)
//		{
//			printRecursiveBF(currentNode->left, level - 1);
//			printRecursiveBF(currentNode->right, level - 1);
//		}
//	}
//	void printRecursiveDF(Node<T> *currentNode)
//	{
//		if (currentNode != NULL)
//		{
//			cout << currentNode->data << " ";
//			printRecursiveDF(currentNode->left);
//			printRecursiveDF(currentNode->right);
//		}
//	}
//	void print(int mode)
//	{
//		int h;
//		switch (mode)
//		{
//		case 1:
//			printQueueBF();
//			break;
//		case 2:
//			printQueueDF();
//			break;
//		case 3:
//			h = maxDepth(top);
//			for (int i = 1; i <= h; i++)
//				printRecursiveBF(top, i);
//			break;
//		case 4:
//			printRecursiveDF(top);
//			break;
//		default:
//			cout << "Invalid Input\n";
//		}
//	}
//	Node<T>* getTop()
//	{
//		return top;
//	}
//	int maxDepth(Node<T> *currentNode)
//	{
//		if (currentNode != NULL)
//		{
//			int leftDepth = maxDepth(currentNode->left);
//			int rightDepth = maxDepth(currentNode->right);
//			if (leftDepth > rightDepth)
//				return (leftDepth + 1);
//			else
//				return (rightDepth + 1);
//		}
//		else
//			return 0;
//	}
//
//	//void printBT(const string& prefix, const Node<T>* node, bool isLeft)
//	//{
//	//	if (node != nullptr)
//	//	{
//	//		cout << prefix;
//	//		cout << (isLeft ? "|--" : "|__");
//	//		// print the value of the node
//	//		cout << node->data << endl;
//	//		// enter the next tree level - left and right branch
//	//		printBT(prefix + (isLeft ? "|   " : "    "), node->left, true);
//	//		printBT(prefix + (isLeft ? "|   " : "    "), node->right, false);
//	//	}
//	//}
//	//void print()
//	//{
//	//	printBT("", top, false);
//	//}
//};
//
//int main()
//{
//	BST<int> test;
//	//list<int>::iterator it;
//	test.push(5);
//	test.push(8);
//	test.push(9);
//	test.push(7);
//	test.push(3);
//	test.push(1);
//	test.push(4);
//	test.push(10);
//	test.print(2);
//	cout << endl;
//	//for (it = test.queue.begin(); it != test.queue.end(); it++)
//	//{
//	//	cout << " " << *it << "\n";
//	//}
//	system("pause");
//	return 0;
//}