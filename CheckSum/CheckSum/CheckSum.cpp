#include <iostream>
#include <stdio.h>
#include <string>

using std::cout;
using std::cin;
using std::string;
using std::endl;

int main(int argc, char* argv[])
{

    // Check for input line.
    if (argc < 2)
    {
        printf("Please provide input sentences on the command line\n");
        return 1;
    }

    for (int j = 1; j < argc; j++)
    {
        string input = argv[j];

        // Check for $ at start.
        if (input[0] != '$')
        {
            printf("Input sentence must begin with $\n");
            return 1;
        }

        // Compute the checksum.
        int crc = 0;
        for (int i = 1; i < input.size(); i++)
            crc ^= input[i];  // This is XOR.

        // Format the output.
        printf("%s*%02X\n", input.c_str(), crc);
    }
}
