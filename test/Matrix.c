int a[625], b[625], c[625];

int widthCnt(int num)
{
    int x, ret;
    if(num >= 0)
    {
        x = num;
        ret = 1;
    }
    else
    {
        x = 0 - num;
        ret = 2;
    }
    while(x > 9)
    {
        x = x / 10;
        ret = ret + 1;
    }
    return 10 - ret;
}

int main()
{
    int ma, na, mb, nb;
    int i, j, k;
    scan(ma, na);
    i = 0;
    while(i < na * ma)
    {
        scan(a[i]);
        i = i + 1;
    }
    scan(mb, nb);
    i = 0;
    while(i < nb * mb)
    {
        scan(b[i]);
        i = i + 1;
    }
    if(na != mb)
    {
        print("Incompatible Dimensions\n");
        return 0;
    }
    i = 0;
    while(i < ma)
    {
        j = 0;
        while(j < nb)
        {
            k = 0;
            while(k < na)
            {
                c[i * nb + j] = c[i * nb + j] + a[i * na + k] * b[k * nb + j];
                k = k + 1;
            }
            j = j + 1;
        }
        i = i + 1;
    }
    i = 0;
    while(i < ma)
    {
        j = 0;
        while(j < nb)
        {
            k = widthCnt(c[i * nb + j]);
            while(k > 0)
            {
                print(" ");
                k = k - 1;
            }
            print(c[i * nb + j]);
            j = j + 1;
        }
        print("\n");
        i = i + 1;
    }
    return 0;
}