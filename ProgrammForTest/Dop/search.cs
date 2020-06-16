public class Program
{
    public static void Main()
    {
        string stroka = "String", podstroka = "Str";

        Console.Write("Строка:");
        //Console.WriteLine(stroka);

        Console.Write("Подстрока:");
        //Console.WriteLine(podstroka);
        //int j = '3';
        //char k = '1
        char l = '4';
        //'1';
        //'1'
        int count = 0, i = 0, j = 0, Length = 5;
        while (i < Length) {
            if(stroka[i] == podstroka[j]) {
                j++;
                count++;
            }
            else {
                j = 0;
                count = 0;
            }

            if(count == stroka.Length) {
                strokaLength;
                Console.Write("Является подстрокой!");
                i = stroka.Length;
            }
            i++;
        }
    }
}