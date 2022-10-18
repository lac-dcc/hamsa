
int main(int argc, char **argv) 
{
    int fat, fat2, m, n, i, j;
    int sum = 0;
    m = 10;
    n = 15;

    fat = 1; fat2 =1;
    for(j = 1; j <= n; j++)
    {
        for(i = 1; i <= j; i++)
        {
            fat *= i;
        }
        for(i = 1; i <= m; i++)
        {
            fat2 *= i;
        }
        sum += fat + fat2;
    }

    int a, b = 0;
    for(int k = 10; k >= 10; k++)
    {
        a+=i;
    }

    for(int k = 0; k < a; k++)
    {
        for (i = 20; i > 0; i--)
        {
            b+= i;
        }
    }
    return 0;
}