#include <iostream>
#include <string>
#include "Image.h"
#include "AsciiGenerator.h"

int main()
{
    std::cout << "=== ASCII Art Converter ===\n";
    std::cout << "Please paste the absolute file path to your image: \n> ";

    std::string userInput;
    std::getline(std::cin, userInput);

    Image myImage(userInput);

    if (!myImage.IsValid())
    {
        std::cout << "Failed to load image. Double-check your path!\n";
        return 1;
    }

    myImage.Resize(35);
    myImage.PrintInfo();

    std::cout << "\nChoose a style:\n1. Standard Gradient\n2. Custom Word Art\n> ";
    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "2")
    {
        std::cout << "Enter your custom word (e.g., THINK, MATRIX, GHOST): ";
        std::string customWord;
        std::getline(std::cin, customWord);

        std::cout << "Converting to Word Art...\n";
        AsciiGenerator::GenerateWordArt(myImage, "output.txt", customWord);
    }
    else
    {
        std::cout << "Converting to Standard ASCII...\n";
        AsciiGenerator::GenerateStandard(myImage, "output.txt");
    }

    std::cout << "Success! Saved as 'output.txt' in your build folder.\n";

    return 0;
}