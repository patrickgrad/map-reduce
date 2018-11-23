// #include <stdio.h>
// #include <stdlib.h>
// #include <math.h>
 
// int main(int argc, char* argv[] )
// {
 
//     // Size of vectors
//     int n = 10;
 
//     // Input vectors
//     double *restrict a;
//     double *restrict b;
//     // Output vector
//     double *restrict c;
 
//     // Size, in bytes, of each vector
//     size_t bytes = n*sizeof(double);
 
//     // Allocate memory for each vector
//     a = (double*)malloc(bytes);
//     b = (double*)malloc(bytes);
//     c = (double*)malloc(bytes);
 
//     // Initialize content of input vectors, vector a[i] = sin(i)^2 vector b[i] = cos(i)^2
//     int i;
//     for(i = 0; i < n; i++)
//     {
//         a[i] = i;
//         b[i] = i;
//     }  
 
//     // sum component wise and save result into vector c
//     #pragma acc kernels copyin(a[0:n],b[0:n]), copyout(c[0:n])
//     for(i = 0; i < n; i++)
//     {
//         c[i] = a[i] + b[i];
//     }
 
//     for(i = 0; i < n; i++)
//     {
//         printf("c[%i]: %f\n", i , c[i]);
//     }
 
//     // Release memory
//     free(a);
//     free(b);
//     free(c);
 
//     return 0;
// }

#include <string>
#include <cstdio>
#include <vector>
#include <map>
#include <iostream>
#include <cstdlib>

using namespace std;

// Part of Map-Reduce implementation
static map<string, vector<string> > data_input;
static map<string, vector<string> > data_temp;
static unsigned int n;
static string reduce_key;
static vector<void ((*)(const string &, const string &))> map_kernels_vector;
static vector<void ((*)(const string &, const vector<string> &))> reduce_kernels_vector;

// Part of Map-Reduce implementation
void EmitIntermediate(const string & key, const string & value);
void Emit(const string & value);

// Implemented by end user
void map0(const string & key, const string & value);
void reduce0(const string & key, const vector<string> & values);

int main()
{
    // Setup kernels
    n = 1;
    map_kernels_vector.push_back(&map0);
    reduce_kernels_vector.push_back(&reduce0);

    // Setup input data
    data_input["0"].push_back("1");
    data_input["0"].push_back("2");
    data_input["0"].push_back("3");

    data_input["1"].push_back("1");

    void (**map_kernels)(const string &, const string &) = map_kernels_vector.data();
    void (**reduce_kernels)(const string &, const vector<string> &) = reduce_kernels_vector.data();

    for(unsigned int i = 0 ; i < n; i++)
    {
        // Section is data parallel
        for(map<string, vector<string> >::iterator it = data_input.begin(); it != data_input.end(); it++)
        {
            string key = it->first;
            string * values = (it->second).data();
            unsigned int values_len = (it->second).size();
            void (*map)(const string &, const string &) = map_kernels[i];

            #pragma acc kernels
            for(unsigned int j = 0; j < values_len; j++)
            {
                //map(key, values[j]);
                values[j] += 1;
            }
        }

        // When parallizing, sync here
        data_input = data_temp;
        data_temp.clear();

        // Section is data parallel
        for(map<string, vector<string> >::iterator it = data_input.begin(); it != data_input.end(); it++)
        {
            reduce_key = it->first;
            reduce_kernels[i](it->first, it->second);
        }

        // When parallizing, sync here
        data_input = data_temp;
        data_temp.clear();
    }

    // Print data out

    cout << "0 : ";
    for(unsigned int i = 0; i < data_input["0"].size(); i++)
    {
        cout << data_input["0"][i] << " ";
    }
    cout << endl;

    cout << "1 : ";
    for(unsigned int i = 0; i < data_input["1"].size(); i++)
    {
        cout << data_input["1"][i] << " ";
    }
    cout << endl;

    cout << "2 : ";
    for(unsigned int i = 0; i < data_input["2"].size(); i++)
    {
        cout << data_input["2"][i] << " ";
    }
    cout << endl;

    cout << "3 : ";
    for(unsigned int i = 0; i < data_input["3"].size(); i++)
    {
        cout << data_input["3"][i] << " ";
    }
    cout << endl;

    return 0;
}

// Part of Map-Reduce implementation
void EmitIntermediate(const string & key, const string & value)
{
    data_temp[key].push_back(value);
}

// Part of Map-Reduce implementation
void Emit(const string & value)
{
    data_temp[reduce_key].push_back(value);
}

// Implemented by end user
void map0(const string & key, const string & value)
{
    int i = atoi(key.c_str()) + 1;
    EmitIntermediate(to_string(i), value);
}

// Implemented by end user
void reduce0(const string & key, const vector<string> & values)
{
    unsigned int sum = 0;
    for(unsigned int i = 0; i < values.size(); i++)
    {
        sum += atoi(values[i].c_str());
    }

    Emit(to_string(sum));
}
