n = 1000000;
a = vector(n)+0.1;
b = vector(n)+0.1;
c = vector(n);

tic = time();
i = 0;
while (i<n) { 
    temp = b[i]+c[i];
    if(temp>0)
        a[i]=temp;
    i = i+1;
}
print(time()-tic);