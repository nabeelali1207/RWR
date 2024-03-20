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
//	void printQueueDF_inOrder()
//	{
//		typename list<Node<T>*>::iterator it;
//		//it = queue.begin();
//		int position = 1;
//		Node<T> *currentNode;
//		queue.push_back(top);
//		cout << queue.size() << endl;
//		while (1)
//		{
//			//for (int i = 1; i <= position; i++)
//			//	it++;
//
//			currentNode = queue.front();
//			//cout << currentNode->data << " ";
//			queue.pop_front();
//			if (currentNode->right != NULL)
//				queue.push_front(currentNode->right);
//			queue.push_front(currentNode);
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
//	void printRecursiveDF_inOrder(Node<T> *currentNode)
//	{
//		if (currentNode != NULL)
//		{
//			printRecursiveDF_inOrder(currentNode->left);
//			cout << currentNode->data << " ";
//			printRecursiveDF_inOrder(currentNode->right);
//		}
//		else
//			return;
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
//			printQueueDF_inOrder();
//			break;
//		case 4:
//			h = maxDepth(top);
//			for (int i = 1; i <= h; i++)
//				printRecursiveBF(top, i);
//			break;
//		case 5:
//			printRecursiveDF(top);
//			break;
//		case 6:
//			printRecursiveDF_inOrder(top);
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
//
//	test.push(6);
//	test.push(8);
//	test.push(9);
//	test.push(7);
//	test.push(3);
//	test.push(1);
//	test.push(4);
//	test.push(2);
//	test.push(5);
//	test.push(10);
//
//	test.print(3);
//	cout << endl;
//
//	system("pause");
//	return 0;
//}


















////3
//#include <iostream>
//#include <list>
//#include <vector>
//#include <iterator>
//#include <string>
//#include <random>
//
//using namespace std;
//
//template <typename T> struct Node
//{
//	Node<T> *left, *right;
//	T data;
//	int state = 0;
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
//		tempNode->state = 0;
//		tempNode->left = NULL;
//		tempNode->right = NULL;
//		if (top != NULL)
//		{
//			while (1)
//			{
//				if (_data == tempPtr->data)
//					break;
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
//	
//	//BST DF in-order traversal
//	void print()
//	{
//		Node<T> *currentNode;
//		queue.push_back(top);
//		cout << queue.size() << endl;
//		while (1)
//		{
//			if (queue.size() == 0)
//				break;
//			currentNode = queue.back();
//			queue.pop_back();
//			if (((currentNode->left == NULL) && (currentNode->right == NULL)) || (currentNode->state == 1))
//			{
//				cout << currentNode->data << " ";
//				currentNode->state = 2;
//				continue;
//			}
//			if (currentNode->right != NULL)
//				queue.push_back(currentNode->right);
//			if (currentNode->state == 0)
//				queue.push_back(currentNode);
//			if (currentNode->left != NULL)
//				queue.push_back(currentNode->left);
//			currentNode->state = 1;
//		}
//	}
//};
//
//
//int main()
//{
//	BST<int> test;
//
//	test.push(6);
//	test.push(8);
//	test.push(9);
//	test.push(7);
//	test.push(3);
//	test.push(1);
//	test.push(4);
//	test.push(2);
//	test.push(5);
//	test.push(10);
//
//	for (int i = 1; i <= 10; i++)
//	{
//		test.push(10 + rand() % 40);
//	}
//
//	test.print();
//	cout << endl;
//
//	system("pause");
//	return 0;
//}










//4
#include <iostream>
#include <map>

using namespace std;






