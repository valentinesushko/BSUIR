#include "RubiksCube.h"

#include <algorithm>
#include <fstream>
#include <istream>
#include <ostream>
#include <stdexcept>

namespace {

struct Vec3 {
    int x;
    int y;
    int z;
};

Vec3 faceNormal(RubiksCube::Face face) {
    switch (face) {
    case RubiksCube::Face::Up:
        return Vec3{0, 1, 0};
    case RubiksCube::Face::Down:
        return Vec3{0, -1, 0};
    case RubiksCube::Face::Left:
        return Vec3{-1, 0, 0};
    case RubiksCube::Face::Right:
        return Vec3{1, 0, 0};
    case RubiksCube::Face::Front:
        return Vec3{0, 0, 1};
    case RubiksCube::Face::Back:
        return Vec3{0, 0, -1};
    }
    throw std::out_of_range("invalid face");
}

Vec3 faceRight(RubiksCube::Face face) {
    switch (face) {
    case RubiksCube::Face::Up:
        return Vec3{1, 0, 0};
    case RubiksCube::Face::Down:
        return Vec3{1, 0, 0};
    case RubiksCube::Face::Left:
        return Vec3{0, 0, 1};
    case RubiksCube::Face::Right:
        return Vec3{0, 0, -1};
    case RubiksCube::Face::Front:
        return Vec3{1, 0, 0};
    case RubiksCube::Face::Back:
        return Vec3{-1, 0, 0};
    }
    throw std::out_of_range("invalid face");
}

Vec3 faceDown(RubiksCube::Face face) {
    switch (face) {
    case RubiksCube::Face::Up:
        return Vec3{0, 0, 1};
    case RubiksCube::Face::Down:
        return Vec3{0, 0, -1};
    case RubiksCube::Face::Left:
        return Vec3{0, -1, 0};
    case RubiksCube::Face::Right:
        return Vec3{0, -1, 0};
    case RubiksCube::Face::Front:
        return Vec3{0, -1, 0};
    case RubiksCube::Face::Back:
        return Vec3{0, -1, 0};
    }
    throw std::out_of_range("invalid face");
}

RubiksCube::Color faceColor(RubiksCube::Face face) {
    switch (face) {
    case RubiksCube::Face::Up:
        return RubiksCube::Color::White;
    case RubiksCube::Face::Down:
        return RubiksCube::Color::Yellow;
    case RubiksCube::Face::Left:
        return RubiksCube::Color::Orange;
    case RubiksCube::Face::Right:
        return RubiksCube::Color::Red;
    case RubiksCube::Face::Front:
        return RubiksCube::Color::Green;
    case RubiksCube::Face::Back:
        return RubiksCube::Color::Blue;
    }
    throw std::out_of_range("invalid face");
}

int faceIndex(RubiksCube::Face face) {
    switch (face) {
    case RubiksCube::Face::Up:
        return 0;
    case RubiksCube::Face::Down:
        return 1;
    case RubiksCube::Face::Left:
        return 2;
    case RubiksCube::Face::Right:
        return 3;
    case RubiksCube::Face::Front:
        return 4;
    case RubiksCube::Face::Back:
        return 5;
    }
    throw std::out_of_range("invalid face");
}

int dot(const Vec3& lhs, const Vec3& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vec3 cross(const Vec3& lhs, const Vec3& rhs) {
    return Vec3{
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x};
}

bool sameVector(const Vec3& lhs, const Vec3& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

Vec3 rotateVector(const Vec3& vector, const Vec3& axis, int sign) {
    const int projection = dot(vector, axis);
    const Vec3 perpendicular = cross(axis, vector);
    return Vec3{
        axis.x * projection + sign * perpendicular.x,
        axis.y * projection + sign * perpendicular.y,
        axis.z * projection + sign * perpendicular.z};
}

Vec3 stickerPosition(RubiksCube::Face face, int row, int column) {
    const Vec3 normal = faceNormal(face);
    const Vec3 right = faceRight(face);
    const Vec3 down = faceDown(face);
    const int horizontal = column - 1;
    const int vertical = row - 1;
    return Vec3{
        normal.x + horizontal * right.x + vertical * down.x,
        normal.y + horizontal * right.y + vertical * down.y,
        normal.z + horizontal * right.z + vertical * down.z};
}

bool locateSticker(const Vec3& position,
                   const Vec3& normal,
                   RubiksCube::Face& face,
                   int& row,
                   int& column) {
    bool found = false;
    for (int index = 0; index < RubiksCube::kFaceCount; ++index) {
        const RubiksCube::Face candidate = static_cast<RubiksCube::Face>(index);
        if (sameVector(faceNormal(candidate), normal)) {
            face = candidate;
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }

    row = dot(position, faceDown(face)) + 1;
    column = dot(position, faceRight(face)) + 1;
    return row >= 0 && row < RubiksCube::kSize
        && column >= 0 && column < RubiksCube::kSize;
}

char colorToChar(RubiksCube::Color color) {
    switch (color) {
    case RubiksCube::Color::White:
        return 'W';
    case RubiksCube::Color::Yellow:
        return 'Y';
    case RubiksCube::Color::Green:
        return 'G';
    case RubiksCube::Color::Blue:
        return 'B';
    case RubiksCube::Color::Red:
        return 'R';
    case RubiksCube::Color::Orange:
        return 'O';
    }
    throw std::out_of_range("invalid color");
}

bool charToColor(char character, RubiksCube::Color& color) {
    if (character >= 'a' && character <= 'z') {
        character = static_cast<char>(character - 'a' + 'A');
    }
    switch (character) {
    case 'W':
        color = RubiksCube::Color::White;
        return true;
    case 'Y':
        color = RubiksCube::Color::Yellow;
        return true;
    case 'G':
        color = RubiksCube::Color::Green;
        return true;
    case 'B':
        color = RubiksCube::Color::Blue;
        return true;
    case 'R':
        color = RubiksCube::Color::Red;
        return true;
    case 'O':
        color = RubiksCube::Color::Orange;
        return true;
    default:
        return false;
    }
}

bool isWhitespace(char character) {
    return character == ' '
        || character == '\t'
        || character == '\n'
        || character == '\r'
        || character == '\v'
        || character == '\f';
}

} // namespace

RubiksCube::RubiksCube() {
    for (int face = 0; face < kFaceCount; ++face) {
        const Color color = faceColor(static_cast<Face>(face));
        for (int index = 0; index < kStickersPerFace; ++index) {
            stickers_[face][index] = color;
        }
    }
}

RubiksCube::RubiksCube(const RubiksCube& other)
    : stickers_(other.stickers_) {
}

RubiksCube& RubiksCube::operator=(const RubiksCube& other) {
    if (this != &other) {
        stickers_ = other.stickers_;
    }
    return *this;
}

RubiksCube::~RubiksCube() = default;

RubiksCube::Color RubiksCube::at(Face face, int row, int column) const {
    const int index = faceIndex(face);
    if (row < 0 || row >= kSize || column < 0 || column >= kSize) {
        throw std::out_of_range("sticker index out of range");
    }
    return stickers_[index][row * kSize + column];
}

void RubiksCube::set(Face face, int row, int column, Color color) {
    const int index = faceIndex(face);
    if (row < 0 || row >= kSize || column < 0 || column >= kSize) {
        throw std::out_of_range("sticker index out of range");
    }
    stickers_[index][row * kSize + column] = color;
}

void RubiksCube::rotate(Face face, Turn turn) {
    const Vec3 axis = faceNormal(face);

    int sign = 0;
    switch (turn) {
    case Turn::Clockwise:
        sign = -1;
        break;
    case Turn::CounterClockwise:
        sign = 1;
        break;
    default:
        throw std::out_of_range("invalid turn");
    }

    std::array<std::array<Color, kStickersPerFace>, kFaceCount> result = stickers_;

    for (int sourceIndex = 0; sourceIndex < kFaceCount; ++sourceIndex) {
        const Face sourceFace = static_cast<Face>(sourceIndex);
        const Vec3 sourceNormal = faceNormal(sourceFace);
        for (int row = 0; row < kSize; ++row) {
            for (int column = 0; column < kSize; ++column) {
                const Vec3 position = stickerPosition(sourceFace, row, column);
                if (dot(position, axis) != 1) {
                    continue;
                }

                const Vec3 newPosition = rotateVector(position, axis, sign);
                const Vec3 newNormal = rotateVector(sourceNormal, axis, sign);

                Face destinationFace = Face::Up;
                int destinationRow = 0;
                int destinationColumn = 0;
                if (!locateSticker(newPosition,
                                   newNormal,
                                   destinationFace,
                                   destinationRow,
                                   destinationColumn)) {
                    throw std::logic_error("invalid sticker mapping");
                }

                result[faceIndex(destinationFace)]
                      [destinationRow * kSize + destinationColumn]
                    = stickers_[sourceIndex][row * kSize + column];
            }
        }
    }

    stickers_ = result;
}

bool RubiksCube::isSolved() const {
    for (int face = 0; face < kFaceCount; ++face) {
        const Color canonical = faceColor(static_cast<Face>(face));
        for (int index = 0; index < kStickersPerFace; ++index) {
            if (stickers_[face][index] != canonical) {
                return false;
            }
        }
    }
    return true;
}

void RubiksCube::randomize(std::mt19937& rng) {
    std::array<Color, kFaceCount * kStickersPerFace> pool{};
    int poolIndex = 0;
    for (int colorIndex = 0; colorIndex < kFaceCount; ++colorIndex) {
        const Color color = static_cast<Color>(colorIndex);
        for (int copy = 0; copy < kStickersPerFace; ++copy) {
            pool[poolIndex] = color;
            ++poolIndex;
        }
    }

    std::shuffle(pool.begin(), pool.end(), rng);

    for (int face = 0; face < kFaceCount; ++face) {
        for (int index = 0; index < kStickersPerFace; ++index) {
            stickers_[face][index] = pool[face * kStickersPerFace + index];
        }
    }
}

void RubiksCube::randomize() {
    std::random_device device;
    std::mt19937 rng(device());
    randomize(rng);
}

bool RubiksCube::loadFromFile(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        return false;
    }

    RubiksCube loaded;
    if (!(input >> loaded)) {
        return false;
    }

    *this = loaded;
    return true;
}

bool RubiksCube::saveToFile(const std::string& path) const {
    std::ofstream output(path);
    if (!output) {
        return false;
    }

    output << *this;
    output.close();
    return !output.fail();
}

bool operator==(const RubiksCube& lhs, const RubiksCube& rhs) {
    return lhs.stickers_ == rhs.stickers_;
}

bool operator!=(const RubiksCube& lhs, const RubiksCube& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out, const RubiksCube& cube) {
    for (int face = 0; face < RubiksCube::kFaceCount; ++face) {
        for (int index = 0; index < RubiksCube::kStickersPerFace; ++index) {
            out.put(colorToChar(cube.stickers_[face][index]));
        }
        out.put('\n');
    }
    return out;
}

std::istream& operator>>(std::istream& in, RubiksCube& cube) {
    std::array<std::array<RubiksCube::Color, RubiksCube::kStickersPerFace>,
               RubiksCube::kFaceCount>
        parsed{};

    int count = 0;
    const int total = RubiksCube::kFaceCount * RubiksCube::kStickersPerFace;
    while (count < total) {
        char character = '\0';
        if (!in.get(character)) {
            in.setstate(std::ios::failbit);
            return in;
        }
        if (isWhitespace(character)) {
            continue;
        }

        RubiksCube::Color color = RubiksCube::Color::White;
        if (!charToColor(character, color)) {
            in.setstate(std::ios::failbit);
            return in;
        }

        const int face = count / RubiksCube::kStickersPerFace;
        const int index = count % RubiksCube::kStickersPerFace;
        parsed[face][index] = color;
        ++count;
    }

    cube.stickers_ = parsed;
    return in;
}

std::string RubiksCube::toString(Face face) {
    switch (face) {
    case Face::Up:
        return "U";
    case Face::Down:
        return "D";
    case Face::Left:
        return "L";
    case Face::Right:
        return "R";
    case Face::Front:
        return "F";
    case Face::Back:
        return "B";
    }
    throw std::out_of_range("invalid face");
}

std::string RubiksCube::toString(Color color) {
    switch (color) {
    case Color::White:
        return "White";
    case Color::Yellow:
        return "Yellow";
    case Color::Green:
        return "Green";
    case Color::Blue:
        return "Blue";
    case Color::Red:
        return "Red";
    case Color::Orange:
        return "Orange";
    }
    throw std::out_of_range("invalid color");
}

std::string RubiksCube::toString(Turn turn) {
    switch (turn) {
    case Turn::Clockwise:
        return "Clockwise";
    case Turn::CounterClockwise:
        return "CounterClockwise";
    }
    throw std::out_of_range("invalid turn");
}
