//1
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
//		Buffer<T>& myBuffer;
//		T *ptr;
//	public:
//		//Iterator()
//		//{
//		//	myBuffer = NULL;
//		//	ptr = NULL;
//		//}
//		Iterator(Buffer<T>& _Buffer) :myBuffer(_Buffer) {}
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
//			if (ptr != myBuffer.getEnd() - 1)
//				ptr++;
//			else
//				ptr = myBuffer.getStart();
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
//		if (tail == start && head == (end - 1))
//		{
//			tail = end - 1;
//			head--;
//		}
//		else if (tail == (head + 1))
//		{
//			if (head == start)
//			{
//				tail--;
//				head = end - 1;
//			}
//			else
//			{
//				tail--;
//				head--;
//			}
//		}
//		else
//			tail--;
//		*tail = data;
//		if (size < capacity)
//			size++;
//	}
//	void push_front(T data)
//	{
//		if (head == (end - 1) && tail == start)
//		{
//			head = start;
//			tail++;
//		}
//		else if (head == (tail - 1))
//		{
//			if (tail == (end - 1))
//			{
//				head++;
//				tail = start;
//			}
//			else
//			{
//				head++;
//				tail++;
//			}
//		}
//		else
//			head++;
//		*head = data;
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
//	T* getHead()
//	{
//		return head;
//	}
//	T* getTail()
//	{
//		return tail;
//	}
//};
//
//int main()
//{
//	Buffer<int> temp(10);
//	Buffer<int>::Iterator<int> it(temp);
//
//	for (int i = 1; i <= 21; i++)
//		temp.push_front(i);
//	 
//	temp.printBuffer();
//	cout << endl;
//	for (int i = 1; i <= 0; i++)
//	{
//		cout << "pop_front(): " << temp.pop_front() << endl;
//		cout << "pop_back(): " << temp.pop_back() << endl;
//
//	}
//	temp.push_back(100);
//	temp.push_front(200);
//	temp.printBuffer();
//	cout << endl;
//	//for (it = temp.getStart(); it != temp.getEnd(); ++it)
//	//{
//	//	cout << *it << endl;
//	//}
//	it = temp.getTail(); 
//	do
//	{
//		cout << *it << endl;
//		++it;
//	} while (it != temp.getHead());
//
//	cout << endl << endl;
//
//	system("pause");
//	return 0;
//}










//2
//#include <iostream>
//#include <thread>
//#include <ctime>
//#include <windows.h>
//
//using namespace std;
//
//int main()
//{
//	time_t result = time(nullptr);
//	while (1)
//	{
//		system("cls");
//		result = time(NULL);
//		cout << asctime(localtime(&result)) << result << " seconds since the Epoch\n";
//		cout << localtime(&result) << "\n";
//		this_thread::sleep_for(std::milliseconds(2000));
//		Sleep(250);
//	}
//	system("pause");
//	return 0;
//}










////3
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
//void performOperation(uint64_t *_a, uint64_t *_b, uint64_t *_sum)
//{
//	for (int i = 0; i < 10000000; i++)
//	{
//		*_sum += pow(_a[i], 2) + pow(_b[i], 2);
//	}
//}
//
//int main()
//{
//	uint64_t *a = new uint64_t[10000000], *b = new uint64_t[10000000], sum = 0;
//	time_point<system_clock> start, end;
//
//	for (int i = 0; i < 10000000; i++)
//	{
//		a[i] = 2;//rand() % 3;
//		b[i] = 2;//3 + rand() % 3;
//	}
//	//cout << *(&a + 1) - a;
//	start = system_clock::now();
//	performOperation(a, b, &sum);
//	end = system_clock::now();
//
//	duration<double> elapsed_seconds = end - start;
//	//time_t end_time = system_clock::to_time_t(end);
//
//	cout << "Elapsed time: " << elapsed_seconds.count() * 1 << "s\n";
//	cout << "Sum = " << sum << endl;
//	
//	delete[] a;
//	delete[] b;
//
//	system("pause");
//	return 0;
//}










////4
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
//void performOperation1(uint64_t *_a, uint64_t *_b, uint64_t *_sum, int _size)
//{
//	for (int i = 0; i < _size / 2; i++)
//	{
//		*_sum += pow(*_a, 2) + pow(*_b, 2);
//		_a++; _b++;
//	}
//}
//void performOperation2(uint64_t *_a, uint64_t *_b, uint64_t *_sum, int _size)
//{
//	for (int i = _size / 2; i < _size; i++)
//	{
//		*_sum += pow(*_a, 2) + pow(*_b, 2);
//		_a++; _b++;
//	}
//}
//
//
//int main()
//{
//	int size = 10000000;
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
//	thread thread_obj1(performOperation1, a, b, &sum1, size);
//	thread thread_obj2(performOperation2, &a[size / 2], &b[size / 2], &sum2, size);
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