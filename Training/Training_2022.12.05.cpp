/*
//1.1
#include <sstream>
#include <vector>
#include <iostream>
using namespace std;

vector<int> parseInts(string str) 
{
	// Complete this function
    stringstream ss("23,4,56");
    char ch;
    int a, b, c;
    ss >> a >> ch >> b >> ch >> c;
    vector<int> temp;
    temp.push_back(a);
    temp.push_back(b);
    temp.push_back(c);
    return temp;
}

int main() {
    string str;
    cin >> str;
    vector<int> integers = parseInts(str);
    for(int i = 0; i < integers.size(); i++) {
        cout << integers[i] << "\n";
    }
    
    return 0;
}
*/





/*
//1.2
#include <sstream>
#include <vector>
#include <iostream>
using namespace std;

vector<int> parseInts(string string) 
{
	// Complete this function
    vector<int> temp;
    stringstream ss;
    ss << string;
    char ch;
    int a;
    int size = ss.str().size();
    //ss.seekg(0, ios::end);
    //int size = ss.tellg();
    //ss.seekg(0, ios::beg);
    while (!ss.eof())
    {
        ss >> a >> ch;
        temp.push_back(a);
    }
    return temp;
}

int main() {
    string str;
    cin >> str;
    vector<int> integers = parseInts(str);
    for(int i = 0; i < integers.size(); i++) {
        cout << integers[i] << "\n";
    }
    
    return 0;
}
*/










/*
//2
//Class
#include <iostream>
#include <sstream>
using namespace std;



//
//Enter code for class Student here.
//Read statement for specification.
//
class Student
{
  private:
    int age, standard;
    string first_name, last_name;
    
  public:
    void set_age(int age);
    void set_standard(int standard);
    void set_first_name(string first_name);
    void set_last_name(string last_name);
    
    int get_age();
    int get_standard();
    string get_first_name();
    string get_last_name();
    
    string to_string();
    
};



int main() {
    int age, standard;
    string first_name, last_name;
    
    cin >> age >> first_name >> last_name >> standard;
    
    Student st;
    st.set_age(age);
    st.set_standard(standard);
    st.set_first_name(first_name);
    st.set_last_name(last_name);
    
    cout << st.get_age() << "\n";
    cout << st.get_last_name() << ", " << st.get_first_name() << "\n";
    cout << st.get_standard() << "\n";
    cout << "\n";
    cout << st.to_string();
    
    return 0;
}



void Student::set_age(int age)
{
    this->age = age;
}
void Student::set_standard(int standard)
{
    this->standard = standard;
}
void Student::set_first_name(string first_name)
{
    this->first_name = first_name;
}
void Student::set_last_name(string last_name)
{
    this->last_name = last_name;
}

int Student::get_age()
{
    return age;
}
int Student::get_standard()
{
    return standard;
}
string Student::get_first_name()
{
    return first_name;
}
string Student::get_last_name()
{
    return last_name;
}

string Student::to_string()
{
    stringstream ss;
    ss << age << "," << first_name << "," << last_name << "," << standard;
    string  TempStr;
    ss >> TempStr;
    return TempStr;
}
*/










/*
//3
//Classes and Objects
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
using namespace std;

// Write your Student class here
class Student
{
  private:
    int scores[5];
  
  public:  
    void input()
    {
        for (int i = 0; i < 5; i++)
        {
            cin >> scores[i];
        }
    }
    int calculateTotalScore()
    {
        int sum = 0;
        for (int i = 0; i < 5; i++)
        {
            sum += scores[i];
        }
        return sum;
    }
};

int main() {
    int n; // number of students
    cin >> n;
    Student *s = new Student[n]; // an array of n students
    
    for(int i = 0; i < n; i++){
        s[i].input();
    }

    // calculate kristen's score
    int kristen_score = s[0].calculateTotalScore();

    // determine how many students scored higher than kristen
    int count = 0; 
    for(int i = 1; i < n; i++){
        int total = s[i].calculateTotalScore();
        if(total > kristen_score){
            count++;
        }
    }

    // print result
    cout << count;
    
    return 0;
}
*/










/*
//4.1
//Box It!
#include<bits/stdc++.h>

using namespace std;
//Implement the class Box  
//l,b,h are integers representing the dimensions of the box

// The class should have the following functions : 

// Constructors: 
// Box();
// Box(int,int,int);
// Box(Box);


// int getLength(); // Return box's length
// int getBreadth (); // Return box's breadth
// int getHeight ();  //Return box's height
// long long CalculateVolume(); // Return the volume of the box

//Overload operator < as specified
//bool operator<(Box& b)

//Overload operator << as specified
//ostream& operator<<(ostream& out, Box& B)
class Box
{
private:
    int l, b, h;
    
public:
    Box()
    {
        l = b = h = 0;
    }
    Box(int l, int b, int h)
    {
        this->l = l;
        this->b = b;
        this->h = h;
    }
    Box(Box *B)
    {
        l = B->l;
        b = B->b;
        h = B->h;
    }
    int getLength() {return l;}
    int getBreadth() {return b;}
    int getHeight() {return h;}
    long CalculateVolume()
    {
        long volume = long(l) * long(b) * long(h);
        return volume;
    }
    
    

};

bool operator<(Box obj1, Box obj2)
{
    if (obj1.getLength() < obj2.getLength())
        return 1;
    else if ((obj1.getBreadth() < obj2.getBreadth()) && (obj1.getLength() == obj2.getLength()))
        return 1;
    else if ((obj1.getHeight() < obj2.getHeight()) && (obj1.getBreadth() == obj2.getBreadth()) && (obj1.getLength() == obj2.getLength()))
        return 1;
    else
        return 0;
}

ostream & operator<<(ostream &s,Box obj)
{
        s << obj.getLength() << " " << obj.getBreadth() << " " << obj.getHeight();
        return s;
}
    

void check2()
{
	int n;
	cin>>n;
	Box temp;
	for(int i=0;i<n;i++)
	{
		int type;
		cin>>type;
		if(type ==1)
		{
			cout<<temp<<endl;
		}
		if(type == 2)
		{
			int l,b,h;
			cin>>l>>b>>h;
			Box NewBox(l,b,h);
			temp=NewBox;
			cout<<temp<<endl;
		}
		if(type==3)
		{
			int l,b,h;
			cin>>l>>b>>h;
			Box NewBox(l,b,h);
			if(NewBox<temp)
			{
				cout<<"Lesser\n";
			}
			else
			{
				cout<<"Greater\n";
			}
		}
		if(type==4)
		{
			cout<<temp.CalculateVolume()<<endl;
		}
		if(type==5)
		{
			Box NewBox(temp);
			cout<<NewBox<<endl;
		}

	}
}

int main()
{
	check2();
}
*/










/*
//5
//Structs
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

//
//    add code for struct here.
//
struct Student
{
  int age, standard;
  string first_name, last_name;  
};

int main() {
    Student st;
    
    cin >> st.age >> st.first_name >> st.last_name >> st.standard;
    cout << st.age << " " << st.first_name << " " << st.last_name << " " << st.standard;
    
    return 0;
}
*/










/*
//6
//Vector-Sort
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int main() {
    //Enter your code here. Read input from STDIN. Print output to STDOUT
    vector <int> v;
    int N, integer;
    
    cin >> N;
    for (int i = 0; i < N; i++)
    {
        cin >> integer;
        v.push_back(integer);
    }
    sort(v.begin(), v.end());
    for (int i = 0; i < v.size(); i++)
    {
        cout << v[i] << " ";
    }
    
    return 0;
}
*/










/*
//7
//Vector-Erase
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int main() {
    //Enter your code here. Read input from STDIN. Print output to STDOUT
    vector <int> v;
    int N, integer, x, a, b;
    
    cin >> N;
    for (int i = 0; i < N; i++)
    {
        cin >> integer;
        v.push_back(integer);
    }
    cin >> x;
    cin >> a >> b;

    v.erase(v.begin() + (x - 1));
    v.erase(v.begin() + (a - 1), v.begin() + (b - 1));
    
    cout << v.size() << endl;
    for (int i = 0; i < v.size(); i++)
    {
        cout << v[i] << " ";
    }
    
    return 0;
}
*/










/**/
//8
//Inherited Code
#include <iostream>
#include <string>
#include <sstream>
#include <exception>
using namespace std;

/* Define the exception here */
class BadLengthException: exception
{
private:
    int N;
    stringstream myN;
    string myNstring;
    //char* temp;
    
public:
    BadLengthException(int n): N(n) {}
    BadLengthException() {}
    virtual const char* what()
    {
        myN << N;
        myN.str();
        myNstring = myN.str();
        return myNstring.c_str();
        //return temp;
    }
};


bool checkUsername(string username) {
	bool isValid = true;
	int n = username.length();
	if(n < 5) {
		throw BadLengthException(n);
	}
	for(int i = 0; i < n-1; i++) {
		if(username[i] == 'w' && username[i+1] == 'w') {
			isValid = false;
		}
	}
	return isValid;
}

int main() {
	int T; cin >> T;
	while(T--) {
		string username;
		cin >> username;
		try {
			bool isValid = checkUsername(username);
			if(isValid) {
				cout << "Valid" << '\n';
			} else {
				cout << "Invalid" << '\n';
			}
		} catch (BadLengthException e) {
			cout << "Too short: " << e.what() << '\n';
		}
	}
	return 0;
}




