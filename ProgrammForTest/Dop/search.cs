public class Program
{
    public static void Main()
    {
        string stroka = "String", podstroka = "Str";

        Console.Write("Строка:");
        Console.WriteLine(stroka);

        Console.Write("Подстрока:");
        Console.WriteLine(podstroka);
    
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

            if(count == 5) {
                Console.Write("Является подстрокой!");
                i = stroka.Length;
            }
            i++;
        }
    }
}