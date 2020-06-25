public class Program 
{
    public static void Main() 
    {
        string name = "a";
        int a = initialization_variable(name);
        name = "b";
        int b = initialization_variable(name);
        int i = a;

        while (i > 0)
        {
            if (a % i == 0)
            {
                if (b % i == 0)
                {
                    print_message_with_numb("Наибольший общий делитель: ", i);
                    i = 0;
                }
            }
            i--;
        }
    }

    public static int initialization_variable(string name_variable) {
        int return_value;
        Console.Write("Введите значение переменной ");
        Console.Write(name_variable);
        Console.Write(": ");
        return_value = Console.ReadLine();
        return return_value;
    }

    public static void print_message_with_numb(string message, int numb) {
        Console.Write(message);
        Console.WriteLine(numb);  
    }
}