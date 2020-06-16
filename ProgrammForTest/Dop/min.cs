class Program
{
    static int initarrayelement(string message, int numberelemnet)
    {
        int returnvalue;
        Console.Write(message);
        Console.Write(numberelemnet);
        Console.Write(": ");
        returnvalue = Console.ReadLine();
        return returnvalue;
    }

    static void Main()
    {
        int[] numbers = new int[5];
        int min, minIndex;
        int i = 0;

        Console.Write("Введите 5 элементов массива:");

        while (i < 5)
        {
            numbers[i] = initarrayelement("Введите элемент ", i);
            i = i + 1;
        }

        min = numbers[0];
        minIndex = 0;
        i = 0;

        while (i < 5)
        {
            if (min > numbers[i])
            {
                min = numbers[i];
                minIndex = i;
            }
            i = i + 1;
        }
        Console.Write("Минимальный элемент массива: ");
        Console.Write(numbers[minIndex]);
    }
}