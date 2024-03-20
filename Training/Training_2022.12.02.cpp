//1
/*
//Operator Overloading

#include<iostream>

using namespace std;

class Complex
{
public:
    int a,b;
    void input(string s)
    {
        int v1=0;
        int i=0;
        while(s[i]!='+')
        {
            v1=v1*10+s[i]-'0';
            i++;
        }
        while(s[i]==' ' || s[i]=='+'||s[i]=='i')
        {
            i++;
        }
        int v2=0;
        while(i<s.length())
        {
            v2=v2*10+s[i]-'0';
            i++;
        }
        a=v1;
        b=v2;
    }
};

Complex operator+(Complex &obj1, Complex &obj2)
{
    Complex temp;
    temp.a = obj1.a + obj2.a;
    temp.b = obj1.b + obj2.b;
    return temp;
}
ostream & operator<<(ostream &s,Complex obj)
{
    s << obj.a << "+i" << obj.b;
    return s;
}
//Overload operators + and << for the class complex
//+ should add two complex numbers as (a+ib) + (c+id) = (a+c) + i(b+d)
//<< should print a complex number in the format "a+ib"

int main()
{
    Complex x,y;
    string s1,s2;
    cin>>s1;
    cin>>s2;
    x.input(s1);
    y.input(s2);
    Complex z=x+y;
    cout<<z<<endl;
}
*/









//2.1
/*
//initialize() working
#include<bits/stdc++.h>

using namespace std;

//Define the structs Workshops and Available_Workshops.
//Implement the functions initialize and CalculateMaxWorkshops
struct Workshop 
{
    int start_time, duration, end_time;
};
struct Available_Workshops
{
    int n;
    Workshop * arr = new Workshop();
};


Available_Workshops* initialize(int* start_time, int* duration, int n)
{
    cout << "\n\n\nSTARTING DEBUGGING inside 'initialize()'\n";
    cout << "start_time = " << *start_time << endl;
    cout << "duration = " << *duration << endl;
    cout << "n = " << n << endl << endl;

    Available_Workshops * obj = new Available_Workshops();
    obj->n = n;
    //obj.arr = new Workshop[n];
    for (int i = 0; i < n; i++)
    {
        obj->arr[n].start_time = *start_time;
        obj->arr[n].duration = *duration;
        obj->arr[n].end_time = obj->arr[n].start_time + obj->arr[n].duration;
        
        cout << "Continuing debugging, Loop # " << i << endl;
        cout << "obj.arr[n].start_time = " << obj->arr[n].start_time << endl;
        cout << "obj.arr[0].start_time address = " << &obj->arr[0].start_time << endl;
        cout << "obj.arr[n].duration = " << obj->arr[n].duration << endl;
        cout << "obj.arr[n].end_time = " << obj->arr[n].end_time << endl;
        cout << "n = " << n << endl;

        if (i != (n - 1))
        {
            start_time++;
            duration++;
        }

    }

    cout << "\nobj = " << obj <<endl;
    return obj;
}
int CalculateMaxWorkshops(Available_Workshops * Ptr)
{
    cout << "\n\n\nSTARTING DEBUGGING inside 'CalculateMaxWorkshops()'\n";
    cout << "n = " << Ptr->n << endl;
    cout << "ptr->arr[0].start_time = " << Ptr->arr[0].start_time << endl;
    cout << "ptr->arr[0].start_time address = " << &Ptr->arr[0].start_time << endl;
    cout << "ptr->arr[0].duration = " << Ptr->arr[0].duration << endl;
    cout << "ptr->arr[0].end_time = " << Ptr->arr[0].end_time << endl << endl;
    int previous__end_time, max = 0;
    
    for (int i = 0; i < Ptr->n; i++)
    {
        cout << "Continuing debugging, Loop # " << i << endl;


        
        //if (i == 0)
        //{
        //    previous__end_time = ptr->arr[i].end_time;
        //}
        //else
        //{
        //    if (previous__end_time < ptr->arr[i].start_time)
        //        max++;
        //    previous__end_time = ptr->arr[i].end_time;
        //}
        
    }
    
    return 0;
}

int main(int argc, char *argv[]) {
    int n; // number of workshops
    cin >> n;
    // create arrays of unknown size n
    int* start_time = new int[n];
    int* duration = new int[n];

    for(int i=0; i < n; i++){
        cin >> start_time[i];
    }
    for(int i = 0; i < n; i++){
        cin >> duration[i];
    }

    Available_Workshops * ptr;
    ptr = initialize(start_time,duration, n);
    cout << "ptr = " << ptr;
    cout << CalculateMaxWorkshops(ptr) << endl;
    return 0;
}
*/









//2.2
/*
#include<bits/stdc++.h>

using namespace std;

//Define the structs Workshops and Available_Workshops.
//Implement the functions initialize and CalculateMaxWorkshops
struct Workshop 
{
    int start_time, duration, end_time;
};
struct Available_Workshops
{
    int n;
    Workshop * arr = new Workshop[n];
};


Available_Workshops* initialize(int* start_time, int* duration, int n)
{
    //cout << "\n\n\nSTARTING DEBUGGING inside 'initialize()'\n";
    //cout << "start_time = " << *start_time << endl;
    //cout << "duration = " << *duration << endl;
    //cout << "n = " << n << endl << endl;

    Available_Workshops * obj = new Available_Workshops();
    
    obj->n = n;
    //obj.arr = new Workshop[n];
    for (int i = 0; i < n; i++)
    {
        obj->arr[i].start_time = *start_time;
        obj->arr[i].duration = *duration;
        obj->arr[i].end_time = obj->arr[i].start_time + obj->arr[i].duration;
        
        //cout << "Continuing debugging, Loop # " << i << endl;
        //cout << "obj.arr[i].start_time = " << obj->arr[i].start_time << endl;
        //cout << "obj.arr[i].duration = " << obj->arr[i].duration << endl;
        //cout << "obj.arr[i].end_time = " << obj->arr[i].end_time << endl;
        //cout << "n = " << n << endl;

        if (i != (n - 1))
        {
            start_time++;
            duration++;
        }

    }
    //cout << "obj.arr[0].start_time address = " << &obj->arr[0].start_time << endl;
    //cout << "\nobj = " << obj <<endl;
    return obj;
}
int CalculateMaxWorkshops(Available_Workshops * Ptr)
{
    //cout << "\n\n\nSTARTING DEBUGGING inside 'CalculateMaxWorkshops()'\n";
    //cout << "n = " << Ptr->n << endl;
    
    int previous__end_time, max = 0;

    //cout << "ptr->arr[0].start_time address = " << &Ptr->arr[0].start_time << endl;
    
    for (int i = 0; i < Ptr->n; i++)
    {
        //cout << "Continuing debugging, Loop # " << i << endl;
        //cout << "ptr->arr[i].start_time = " << Ptr->arr[i].start_time << endl;
        //cout << "ptr->arr[i].duration = " << Ptr->arr[i].duration << endl;
        //cout << "ptr->arr[i].end_time = " << Ptr->arr[i].end_time << endl << endl;
        if (i != ((Ptr->n) - 1))
        {
            if (Ptr->arr[i].end_time <= Ptr->arr[i + 1].start_time)
                max++;
        }
        else
        {
            max++;
        }

        
    }
    
    return max;
}

int main(int argc, char *argv[]) {
    int n; // number of workshops
    cin >> n;
    // create arrays of unknown size n
    int* start_time = new int[n];
    int* duration = new int[n];

    for(int i=0; i < n; i++){
        cin >> start_time[i];
    }
    for(int i = 0; i < n; i++){
        cin >> duration[i];
    }

    Available_Workshops * ptr;
    ptr = initialize(start_time,duration, n);
    //cout << "ptr = " << ptr;
    cout << CalculateMaxWorkshops(ptr) << endl;
    return 0;
}
*/









/*
#include<bits/stdc++.h>

using namespace std;

//Define the structs Workshops and Available_Workshops.
//Implement the functions initialize and CalculateMaxWorkshops
struct Workshop 
{
    int start_time, duration, end_time;
};
struct Available_Workshops
{
    int n;
    Workshop * arr = new Workshop[n];
};

Available_Workshops* initialize(int* start_time, int* duration, int n)
{
    Available_Workshops * obj = new Available_Workshops();
    obj->n = n;
    for (int i = 0; i < n; i++)
    {
        obj->arr[i].start_time = *start_time;
        obj->arr[i].duration = *duration;
        obj->arr[i].end_time = obj->arr[i].start_time + obj->arr[i].duration;
        if (i != (n - 1))
        {
            start_time++;
            duration++;
        }
    }
    return obj;
}
int CalculateMaxWorkshops(Available_Workshops * Ptr)
{
    int i, j, min, temp;
   for (i = 0; i < Ptr->n - 1; i++) {
      min = i;
      for (j = i + 1; j < Ptr->n; j++)
      if (Ptr->arr[j].start_time < Ptr->arr[min].start_time)
      min = j;
      temp = Ptr->arr[i].start_time;
      Ptr->arr[i].start_time = Ptr->arr[min].start_time;
      Ptr->arr[min].start_time = temp;
   }

    int previous__end_time, max = 0;
    for (int i = 0; i < Ptr->n; i++)
    {
        if (i != ((Ptr->n) - 1))
        {
            if (Ptr->arr[i].end_time <= Ptr->arr[i + 1].start_time)
                max++;
        }
        else
        {
            max++;
        } 
    }
    return max;
}

int main(int argc, char *argv[]) {
    int n; // number of workshops
    cin >> n;
    // create arrays of unknown size n
    int* start_time = new int[n];
    int* duration = new int[n];

    for(int i=0; i < n; i++){
        cin >> start_time[i];
    }
    for(int i = 0; i < n; i++){
        cin >> duration[i];
    }

    Available_Workshops * ptr;
    ptr = initialize(start_time,duration, n);
    //cout << "ptr = " << ptr;
    cout << CalculateMaxWorkshops(ptr) << endl;
    return 0;
}
*/











#include <iostream>
#include <vector>

using namespace std;

int main()
{
    int input;
    vector<int> vector1;
    cin >> input;
    

    return 0;
}