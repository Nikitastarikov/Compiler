using System;

class Program
{
    static int LongestCommonSubstring(string a, string b, int sizea, int sizeb)
    {
        int n = sizea;
        int m = sizeb;
        int[] array = new int[n, m];
        int maxValue = 0;
        int maxI = 0;
        int i = 0;
        int j = 0;
        while (i < n)
        {
            j = 0;
            while (j < m)
            {
                if (a[i] == b[j])
                {
                    if (i == 0 || j == 0)
                    {
                        array[i, j] = 1;
                    }
                    else
                    {
                        array[i, j] = array[i - 1, j - 1] + 1;
                    }

                    if (array[i, j] > maxValue)
                    {
                        maxValue = array[i, j];
                        maxI = i;
                    }
                }
                j = j + 1;
            }
            i = i + 1;
        }
        return maxValue - (maxI + 1 - maxValue);
    }
    static void Main()
    {
        //Console.Write("First the word: ");
        string s = Console.ReadLine();
        //Console.Write("Second the word: ");
        string st = Console.ReadLine();
        int result = LongestCommonSubstring(s, st, 10, 5);
        Console.Write("maxsubstring : {0}", result);
        //Console.ReadLine();
    }
}