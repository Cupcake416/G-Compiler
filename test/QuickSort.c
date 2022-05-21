#include <stdio.h>
int a[10000];

void sort(int l, int r)
{
    if(l >= r) return;
    int mid, i, j;
    mid = a[(l + r) / 2];
    i = l;
    j = r;
    while(i <= j)
    {
        while(a[i] < mid) i = i + 1;
        while(a[j] > mid) j = j - 1;
        if(i <= j)
        {
            int tmp;
            tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
            i = i + 1;
            j = j - 1;
        }
    }
    sort(l, j);
    sort(i, r);
}

int main()
{
    int n, i;
    scanf("%d", &n);
    // scan(n);
    i = 0;
    while(i < n)
    {
        scanf("%d", &a[i]);
        // scan(a[i]);
        i = i + 1;
    }
    sort(0, n - 1);
    i = 0;
    while(i < n)
    {
        printf("%d\n", a[i]);
        // print(a[i], "\n");
        i = i + 1;
    }
    return 0;
}