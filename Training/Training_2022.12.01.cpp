/*
#include <stdio.h>
#include <cstdlib>
#include <iostream>
using namespace std;

void update(int *a, int *b) 
{
    int temp;
    temp = *a + *b;
    *b = abs(*a - *b);
    *a = temp;
}

int main() 
{
    int a, b;
    int *pa = &a, *pb = &b;
    scanf("%d %d", &a, &b);
    update(pa, pb);
    printf("%d\n%d", a, b);

    return 0;
}
*/


/*
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int main()
{
    int n, q;
    vector <vector <int>> a;
    vector <vector <int>> :: iterator iterate;
    vector <vector <int>> :: reverse_iterator R_iterate;
    int k;
    
    //scanf("%d %d", &n, &q);
    
    vector<vector<int>> vect
    {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
      
    
    for (vector<int> vect1D : vect)
    {
        for (int x : vect1D)
        {
            cout << x << " ";
        }    
        cout << endl;
    }

    for (int i = 0; i < vect.size(); i++)
    {
        for (int j = 0; j < vect[i].size(); j++)
        {
            cout << vect[i][j] << " ";
        }    
        cout << endl;
    }
  
    
    
    return 0;
}
*/




#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int main()
{
    int n, q;
    
    vector <vector <int>> :: iterator iterate;
    vector <vector <int>> :: reverse_iterator R_iterate;
    int k;
    int i, j;
    
    scanf("%d %d", &n, &q);
    vector<vector<int>> vect(n, vector<int>(0));

    for (int x = 0; x < n; x++)
    {
        scanf("%d", &k);
        //printf("n = %d, k = %d ", n, k);
        for (int y = 0; y < k; y++)
        {
            int temp;
            scanf("%d", &temp);
            vect[x].push_back(temp);
        }
        //cout << endl;
    }
    
    for (int x = 0; x < q; x++)
    {
        scanf("%d %d", &i, &j);
        cout << vect[i][j] << endl;
    }
    
    
    /*
    for (int i = 0; i < vect.size(); i++)
    {
        for (int j = 0; j < vect[i].size(); j++)
        {
            cout << vect[i][j] << " ";
        }
        cout << endl;
    }
    */
    /*
    for (int i = 1; i <= 6; i++)
        g1.push_back(i);
 
    cout << "Output of begin and end\t:\t";
    for (i = g1.begin(); i != g1.end(); ++i)
        cout << *i << '\t';
 
    cout << endl << endl;
    cout << "Output of rbegin and rend\t:\t";
    for (ir = g1.rbegin(); ir != g1.rend(); ++ir)
        cout << *ir << '\t';
    */
    
    return 0;
}