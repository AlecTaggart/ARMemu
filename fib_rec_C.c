   
int fib_rec_C(int n){
   if ( n == 0 )
        return 0;
    if ( n == 1 )
        return 1;
 
    return fib_rec_C(n-1) + fib_rec_C(n-2);
}