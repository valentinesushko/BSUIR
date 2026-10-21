#include "ConsoleUI.h"

#include <limits>

ConsoleUI::ConsoleUI(std::istream& input, std::ostream& output)
    : cube_(), in_(input), out_(output) {
}

void ConsoleUI::run() {
    while (true) {
        printMenu();
        out_ << "Enter choice: " << std::flush;

        int choice = 0;
        ReadResult result = readInteger(choice);
        if (result == ReadResult::EndOfInput) {
            out_ << "\nEnd of input. Exiting.\n";
            return;
        }
        if (result == ReadResult::Invalid) {
            continue;
        }

        switch (choice) {
        case 0:
            out_ << "Exiting.\n";
            return;
        case 1:
            showCube();
            break;
        case 2:
            cube_.randomize();
            out_ << "Cube randomized.\n";
            break;
        case 3:
            result = loadFromFile();
            if (result == ReadResult::EndOfInput) {
                return;
            }
            break;
        case 4:
            result = rotateFace();
            if (result == ReadResult::EndOfInput) {
                return;
            }
            break;
        case 5:
            checkSolved();
            break;
        case 6:
            result = saveToFile();
            if (result == ReadResult::EndOfInput) {
                return;
            }
            break;
        default:
            out_ << "Invalid choice. Enter a number from the menu.\n";
            break;
        }
    }
}

void ConsoleUI::printMenu() const {
    out_ << "\nRubik's Cube\n"
         << "1. Show cube\n"
         << "2. Randomize\n"
         << "3. Load from file\n"
         << "4. Rotate a face\n"
         << "5. Check whether solved\n"
         << "6. Save to file\n"
         << "0. Exit\n";
}

void ConsoleUI::showCube() const {
    const RubiksCube::Face faces[] = {
        RubiksCube::Face::Up,
        RubiksCube::Face::Down,
        RubiksCube::Face::Left,
        RubiksCube::Face::Right,
        RubiksCube::Face::Front,
        RubiksCube::Face::Back};

    out_ << "\nCurrent cube:\n";
    for (const RubiksCube::Face face : faces) {
        out_ << '\n' << RubiksCube::toString(face) << ":\n";
        for (int row = 0; row < RubiksCube::kSize; ++row) {
            out_ << "  ";
            for (int column = 0; column < RubiksCube::kSize; ++column) {
                out_ << colorSymbol(cube_.at(face, row, column));
                if (column + 1 < RubiksCube::kSize) {
                    out_ << ' ';
                }
            }
            out_ << '\n';
        }
    }
    out_ << "\nLegend: W=White Y=Yellow G=Green B=Blue R=Red O=Orange\n";
}

void ConsoleUI::checkSolved() const {
    if (cube_.isSolved()) {
        out_ << "The cube is solved.\n";
    } else {
        out_ << "The cube is not solved.\n";
    }
}

ConsoleUI::ReadResult ConsoleUI::loadFromFile() {
    std::string path;
    const ReadResult result = readLine("Enter file path: ", path);
    if (result != ReadResult::Success) {
        return result;
    }

    if (path.empty()) {
        out_ << "No file path provided.\n";
        return ReadResult::Invalid;
    }

    if (cube_.loadFromFile(path)) {
        out_ << "Cube loaded from \"" << path << "\".\n";
    } else {
        out_ << "Failed to load cube from \"" << path << "\".\n";
    }
    return ReadResult::Success;
}

ConsoleUI::ReadResult ConsoleUI::saveToFile() {
    std::string path;
    const ReadResult result = readLine("Enter file path: ", path);
    if (result != ReadResult::Success) {
        return result;
    }

    if (path.empty()) {
        out_ << "No file path provided.\n";
        return ReadResult::Invalid;
    }

    if (cube_.saveToFile(path)) {
        out_ << "Cube saved to \"" << path << "\".\n";
    } else {
        out_ << "Failed to save cube to \"" << path << "\".\n";
    }
    return ReadResult::Success;
}

ConsoleUI::ReadResult ConsoleUI::rotateFace() {
    RubiksCube::Face face = RubiksCube::Face::Up;
    ReadResult result = readFace(face);
    if (result != ReadResult::Success) {
        return result;
    }

    RubiksCube::Turn turn = RubiksCube::Turn::Clockwise;
    result = readTurn(turn);
    if (result != ReadResult::Success) {
        return result;
    }

    cube_.rotate(face, turn);
    out_ << "Rotated face " << RubiksCube::toString(face)
         << " (" << RubiksCube::toString(turn) << ").\n";
    return ReadResult::Success;
}

ConsoleUI::ReadResult ConsoleUI::readInteger(int& value) {
    if (!(in_ >> value)) {
        if (in_.eof()) {
            return ReadResult::EndOfInput;
        }
        in_.clear();
        in_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        out_ << "Invalid input. Enter a whole number from the menu.\n";
        return ReadResult::Invalid;
    }

    in_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return ReadResult::Success;
}

ConsoleUI::ReadResult ConsoleUI::readLine(const std::string& prompt,
                                          std::string& line) {
    out_ << prompt << std::flush;
    if (!std::getline(in_, line)) {
        out_ << "\nEnd of input. Exiting.\n";
        return ReadResult::EndOfInput;
    }
    return ReadResult::Success;
}

ConsoleUI::ReadResult ConsoleUI::readFace(RubiksCube::Face& face) {
    std::string line;
    const ReadResult result = readLine("Enter face (U/D/L/R/F/B): ", line);
    if (result != ReadResult::Success) {
        return result;
    }

    const std::string value = trim(line);
    if (value.size() != 1 || !parseFace(value[0], face)) {
        out_ << "Invalid face. Use U, D, L, R, F or B.\n";
        return ReadResult::Invalid;
    }
    return ReadResult::Success;
}

ConsoleUI::ReadResult ConsoleUI::readTurn(RubiksCube::Turn& turn) {
    std::string line;
    const ReadResult result =
        readLine("Enter direction (c=clockwise, a=counterclockwise): ", line);
    if (result != ReadResult::Success) {
        return result;
    }

    const std::string value = trim(line);
    if (value.size() != 1 || !parseTurn(value[0], turn)) {
        out_ << "Invalid direction. Use c for clockwise or a for "
                "counterclockwise.\n";
        return ReadResult::Invalid;
    }
    return ReadResult::Success;
}

std::string ConsoleUI::trim(const std::string& text) {
    const std::string whitespace = " \t\r\n\v\f";
    const std::string::size_type first = text.find_first_not_of(whitespace);
    if (first == std::string::npos) {
        return std::string();
    }
    const std::string::size_type last = text.find_last_not_of(whitespace);
    return text.substr(first, last - first + 1);
}

char ConsoleUI::colorSymbol(RubiksCube::Color color) {
    const std::string name = RubiksCube::toString(color);
    return name.empty() ? '?' : name.front();
}

bool ConsoleUI::parseFace(char character, RubiksCube::Face& face) {
    if (character >= 'a' && character <= 'z') {
        character = static_cast<char>(character - 'a' + 'A');
    }

    switch (character) {
    case 'U':
        face = RubiksCube::Face::Up;
        return true;
    case 'D':
        face = RubiksCube::Face::Down;
        return true;
    case 'L':
        face = RubiksCube::Face::Left;
        return true;
    case 'R':
        face = RubiksCube::Face::Right;
        return true;
    case 'F':
        face = RubiksCube::Face::Front;
        return true;
    case 'B':
        face = RubiksCube::Face::Back;
        return true;
    default:
        return false;
    }
}

bool ConsoleUI::parseTurn(char character, RubiksCube::Turn& turn) {
    if (character >= 'A' && character <= 'Z') {
        character = static_cast<char>(character - 'A' + 'a');
    }

    switch (character) {
    case 'c':
        turn = RubiksCube::Turn::Clockwise;
        return true;
    case 'a':
        turn = RubiksCube::Turn::CounterClockwise;
        return true;
    default:
        return false;
    }
}
