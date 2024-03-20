//#include <iostream>
//#include <vector>
//
//using namespace std;
//
////Circular Buffer
//template <typename T> class Buffer 
//{
//protected:
//	T *start, *end, *head, *tail;
//	int size, capacity = 0;
//public:
//	template <typename T> class Iterator
//	{
//		T *ptr;
//	public:
//		Iterator()
//		{
//			ptr = NULL;
//		}
//		void operator=(T *_ptr)
//		{
//			ptr = _ptr;
//		}
//		bool operator!=(T *_ptr)
//		{
//			if (ptr != _ptr)
//				return true;
//			else
//				return false;
//		}
//		Iterator& operator++()
//		{
//			ptr++;
//			return *this;
//		}
//		T& operator*()
//		{
//			return *ptr;
//		}
//	};
//	Buffer()
//	{
//		start = end = head = NULL;
//		size = capacity = 0;
//	}
//	Buffer(int _capacity)
//	{
//		size = 0;
//		capacity = _capacity;
//		start = (T*)malloc(_capacity * sizeof(T));
//		end = start + _capacity;
//		head = tail = start;
//	}
//	void push_back(T data)
//	{
//		if (tail == (head - 1))
//			head++;
//		else if (head == start && tail == end - 1)
//			tail = end - 1;
//		*tail = data;
//		if (tail == start)
//			tail = end - 1;
//		else
//			tail--;
//		if (size < capacity)
//			size++;
//	}
//	void push_front(T data)
//	{
//		if (head == (tail + 1))
//			tail++;
//		else if (head == start && tail == end - 1)
//			tail = start;
//		*head = data;
//		if (head == (end - 1))
//			head = start;
//		else
//			head++;
//
//		if (size < capacity)
//			size++;
//	}
//	T pop_back()
//	{
//		T temp;
//		if (tail == (end - 1))
//			tail = start;
//		else
//			tail++;
//		temp = *tail;
//		*tail = NULL;
//		size--;
//		return temp;
//	}
//	T pop_front()
//	{
//		T temp;
//		if (head == start)
//			head = end - 1;
//		else
//			head--;
//		temp = *head;
//		*head = NULL;
//		size--;
//		return temp;
//	}
//	void printData()
//	{
//		T *tempPtr = head;
//		for (int i = 1; i <= size; i++)
//		{
//			if (tempPtr == start)
//				tempPtr = end - 1;
//			else
//				tempPtr--;
//		}
//		for (int i = 1; i <= size; i++)
//		{
//			cout << *tempPtr << endl;
//			if (tempPtr == (end - 1))
//				tempPtr = start;
//			else
//				tempPtr++;
//		}
//	}
//	void printBuffer()
//	{
//		T *temp = start;
//		while (temp != end)
//		{
//			cout << *temp << endl;
//			temp++;
//		}
//	}
//	T* getStart()
//	{
//		return start;
//	}
//	T* getEnd()
//	{
//		return end;
//	}
//	T* getCurrent()
//	{
//		return head;
//	}
//	T* getLast()
//	{
//		if (head == start)
//			return end - 1;
//		else
//			return head - 1;
//	}
//};
//
//int main()
//{
//	Buffer<int> temp(10);
//	Buffer<int>::Iterator<int> it;
//
//	for (int i = 1; i <= 21; i++)
//		temp.push_front(i);
//	
//	temp.printBuffer();
//	cout << endl;
//	for (int i = 1; i <= 3; i++)
//	{
//		cout << "pop_front(): " << temp.pop_front() << endl;
//		cout << "pop_back(): " << temp.pop_back() << endl;
//
//	}
//	temp.push_back(100);
//	temp.push_front(200);
//	temp.printBuffer();
//	cout << endl;
//	temp.printBuffer();
//	cout << endl;
//	for (it = temp.getStart(); it != temp.getEnd(); ++it)
//	{
//		cout << *it << endl;
//	}
//
//	cout << endl << endl;
//
//	system("pause");
//	return 0;
//}
