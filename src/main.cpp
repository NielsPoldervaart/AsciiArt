#include <iostream>
#include <string>
#include <fstream>
#include "Image.h"

int main() {
    std::cout << "=== ASCII Art Converter ===" << std::endl;
    std::cout << "Please drag and drop an image here, or type the absolute file path: " << std::endl;

    std::string userInput;
    std::getline(std::cin, userInput);

    std::cout << "\nLoading: " << userInput << std::endl;

    const Image myImage(userInput);

    if (!myImage.IsValid())
    {
        std::cout << "Failed to load image. Double-check the path!" << std::endl;
        return 1;
    }

    std::cout << "Converting to ASCII...\n" << std::endl;

    std::ofstream outFile("ascii.txt");

    // Palette of characters. index 0 is space (dark), 9 is light @ (light).
    std::string asciiChars = " .:-=+*#%@";
    int numChars = asciiChars.length();

    for (int y = 0; y < myImage.height; y++)
    {
        for (int x = 0; x < myImage.width; x++)
        {
            int index = (y * myImage.width + x) * myImage.channels;

            int r = myImage.pixelData[index];
            int g = myImage.pixelData[index + 1];
            int b = myImage.pixelData[index + 2];

            int brightness = (r + g + b) / 3;
            int charIndex = (brightness * (numChars - 1)) / 255;

            outFile << asciiChars[charIndex] << asciiChars[charIndex];
        }

        outFile << "\n";
    }

    outFile.close();
    std::cout << "Success! Saved as 'ascii.txt' in your build folder!" << std::endl;

    return 0;
}