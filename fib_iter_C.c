

int fib_iter_C(int n){
    int i;
    int f1 = 0;
    int f2 = 1;
    int fi;
 
    if(n == 0)
        return 0;
    if(n == 1)
        return 1;
 
    for(i = 2 ; i <= n ; i++ )
    {
        fi = f1 + f2;
        f1 = f2;
        f2 = fi;
    }
    return fi;
}
