#include <iostream>
#include <string>
#include <cctype>

void drawTable(int rows, int columns, char table[8][8])
{
    //Print column letters
    std::cout << "    ";
    for (int col = 0; col < columns; col++)
    {
        std::cout << char('A' + col) << "    ";
    }
    std::cout << std::endl;

    //Print rows
    for (int row = 0; row < rows; row++)
    {
        //Print row numbers
        std::cout << row + 1 << " ";

        for (int col = 0; col < columns; col++)
        {
            //Print cell divider
            std::cout << "|" << table[row][col] << "   ";
        }

        std::cout << "|" << std::endl;

        //Print horizontal divider
        for (int col = 0; col < columns; col++)
        {
            std::cout << "-----";
        }
        std::cout << "---" << std::endl;
    }
}

bool validInput(std::string input)
{
    //Input validation
    input[0] = std::toupper(input[0]);
    return (input.length() == 2 && std::isalpha(input[0]) && std::isdigit(input[1]) &&
            input[0] >= 'A' && input[0] <= 'H' && input[1] >= '1' && input[1] <= '8');
}

void positionPrompt(char table[8][8])
{
    std::string userInput;
    int row, col;

    while (true)
    {
        std::cout << "Enter a cell number followed by a letter. (e.g., A1, b5) or type 'quit' to exit: ";
        std::cin >> userInput;

        if (userInput == "quit")
        {
            break;
        }

        if (!validInput(userInput))
        {
            std::cout << "Please enter a valid cell number followed by a letter. (e.g., A1, b5): " << std::endl;
            continue;
        }

        col = userInput[0] - 'A';
        row = userInput[1] - '1';

        if (table[row][col] == 'X')
        {
            std::cout << "This cell is already occupied. Enter a different cell number followed by a letter. (e.g., A1, b5):" << std::endl;
        }
        else
        {
            table[row][col] = 'X';
            drawTable(8, 8, table);
        }
    }
}

int main()
{
    char table[8][8];
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            table[i][j] = ' ';
        }
    }

    drawTable(8, 8, table);

    positionPrompt(table);

    return 0;
}
