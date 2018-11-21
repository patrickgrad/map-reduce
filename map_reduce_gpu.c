#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
int main(int argc, char* argv[] )
{
 
    // Size of vectors
    int n = 10;
 
    // Input vectors
    double *restrict a;
    double *restrict b;
    // Output vector
    double *restrict c;
 
    // Size, in bytes, of each vector
    size_t bytes = n*sizeof(double);
 
    // Allocate memory for each vector
    a = (double*)malloc(bytes);
    b = (double*)malloc(bytes);
    c = (double*)malloc(bytes);
 
    // Initialize content of input vectors, vector a[i] = sin(i)^2 vector b[i] = cos(i)^2
    int i;
    for(i = 0; i < n; i++)
    {
        a[i] = i;
        b[i] = i;
    }  
 
    // sum component wise and save result into vector c
    #pragma acc kernels copyin(a[0:n],b[0:n]), copyout(c[0:n])
    for(i = 0; i < n; i++)
    {
        c[i] = a[i] + b[i];
    }
 
    for(i = 0; i < n; i++)
    {
        printf("c[%i]: %f\n", i , c[i]);
    }
 
    // Release memory
    free(a);
    free(b);
    free(c);
 
    return 0;
}