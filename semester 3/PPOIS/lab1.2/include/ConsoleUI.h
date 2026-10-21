#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include "RubiksCube.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <string>

/**
 * @class ConsoleUI
 * @brief Console front end for the RubiksCube model.
 *
 * The class owns a RubiksCube and communicates with the user only through the
 * supplied input and output streams. It uses public RubiksCube methods only, so
 * the user interface stays independent of the model implementation.
 */
class ConsoleUI {
public:
    /**
     * @brief Constructs the console interface.
     * @param input Stream used to read user commands.
     * @param output Stream used to print prompts and results.
     */
    explicit ConsoleUI(std::istream& input = std::cin,
                       std::ostream& output = std::cout);

    /**
     * @brief Runs the menu loop until the user chooses to exit or input ends.
     */
    void run();

private:
    enum class ReadResult {
        Success,
        Invalid,
        EndOfInput
    };

    void printMenu() const;
    void showCube() const;
    void checkSolved() const;

    ReadResult loadFromFile();
    ReadResult saveToFile();
    ReadResult rotateFace();

    ReadResult readInteger(int& value);
    ReadResult readLine(const std::string& prompt, std::string& line);
    ReadResult readFace(RubiksCube::Face& face);
    ReadResult readTurn(RubiksCube::Turn& turn);

    static std::string trim(const std::string& text);
    static char colorSymbol(RubiksCube::Color color);
    static bool parseFace(char character, RubiksCube::Face& face);
    static bool parseTurn(char character, RubiksCube::Turn& turn);

    RubiksCube cube_;
    std::istream& in_;
    std::ostream& out_;
};

#endif
