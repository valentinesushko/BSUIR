#include "RubiksCube.h"

#include <UnitTest++/UnitTest++.h>

#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>
#include <string>

namespace {

using Color = RubiksCube::Color;
using Face = RubiksCube::Face;
using Turn = RubiksCube::Turn;

constexpr int kTotalStickers = 54;

const Face kAllFaces[] = {
    Face::Up,
    Face::Down,
    Face::Left,
    Face::Right,
    Face::Front,
    Face::Back};

const Color kAllColors[] = {
    Color::White,
    Color::Yellow,
    Color::Green,
    Color::Blue,
    Color::Red,
    Color::Orange};

std::string solvedText()
{
    return "WWWWWWWWW\n"
           "YYYYYYYYY\n"
           "OOOOOOOOO\n"
           "RRRRRRRRR\n"
           "GGGGGGGGG\n"
           "BBBBBBBBB\n";
}

int countColor(const RubiksCube& cube, Color color)
{
    int count = 0;
    for (const Face face : kAllFaces) {
        for (int row = 0; row < RubiksCube::kSize; ++row) {
            for (int column = 0; column < RubiksCube::kSize; ++column) {
                if (cube.at(face, row, column) == color) {
                    ++count;
                }
            }
        }
    }
    return count;
}

bool faceHasColor(const RubiksCube& cube, Face face, Color color)
{
    for (int row = 0; row < RubiksCube::kSize; ++row) {
        for (int column = 0; column < RubiksCube::kSize; ++column) {
            if (cube.at(face, row, column) != color) {
                return false;
            }
        }
    }
    return true;
}

std::string dataFile(const std::string& name)
{
    return std::string(RUBIKS_DATA_DIR) + "/" + name;
}

} // namespace

TEST(DefaultConstructorCreatesSolvedCube)
{
    const RubiksCube cube;
    CHECK(cube.isSolved());
}

TEST(DefaultConstructorUsesCanonicalFaceColors)
{
    const RubiksCube cube;

    CHECK(faceHasColor(cube, Face::Up, Color::White));
    CHECK(faceHasColor(cube, Face::Down, Color::Yellow));
    CHECK(faceHasColor(cube, Face::Left, Color::Orange));
    CHECK(faceHasColor(cube, Face::Right, Color::Red));
    CHECK(faceHasColor(cube, Face::Front, Color::Green));
    CHECK(faceHasColor(cube, Face::Back, Color::Blue));
}

TEST(DefaultCubeHasNineStickersOfEachColor)
{
    const RubiksCube cube;

    for (const Color color : kAllColors) {
        CHECK_EQUAL(9, countColor(cube, color));
    }
    CHECK_EQUAL(kTotalStickers,
                RubiksCube::kFaceCount * RubiksCube::kStickersPerFace);
}

TEST(AtReturnsTheStoredSticker)
{
    RubiksCube cube;
    cube.set(Face::Front, 2, 1, Color::White);

    CHECK(cube.at(Face::Front, 2, 1) == Color::White);
}

TEST(SetChangesOnlyTheRequestedSticker)
{
    RubiksCube cube;
    cube.set(Face::Back, 1, 2, Color::Red);

    CHECK(cube.at(Face::Back, 1, 2) == Color::Red);
    CHECK_EQUAL(8, countColor(cube, Color::Blue));
    CHECK_EQUAL(10, countColor(cube, Color::Red));
}

TEST(AtThrowsForInvalidRowOrColumn)
{
    const RubiksCube cube;

    CHECK_THROW(cube.at(Face::Up, -1, 0), std::out_of_range);
    CHECK_THROW(cube.at(Face::Up, 0, -1), std::out_of_range);
    CHECK_THROW(cube.at(Face::Up, RubiksCube::kSize, 0), std::out_of_range);
    CHECK_THROW(cube.at(Face::Up, 0, RubiksCube::kSize), std::out_of_range);
}

TEST(SetThrowsForInvalidRowOrColumn)
{
    RubiksCube cube;

    CHECK_THROW(cube.set(Face::Up, -1, 0, Color::Red), std::out_of_range);
    CHECK_THROW(cube.set(Face::Up, 0, -1, Color::Red), std::out_of_range);
    CHECK_THROW(cube.set(Face::Up, RubiksCube::kSize, 0, Color::Red),
                std::out_of_range);
    CHECK_THROW(cube.set(Face::Up, 0, RubiksCube::kSize, Color::Red),
                std::out_of_range);
}

TEST(AtAndSetThrowForInvalidFace)
{
    const RubiksCube constantCube;
    RubiksCube cube;
    const Face invalid = static_cast<Face>(99);

    CHECK_THROW(constantCube.at(invalid, 0, 0), std::out_of_range);
    CHECK_THROW(cube.set(invalid, 0, 0, Color::Red), std::out_of_range);
}

TEST(CopyConstructorCopiesEverySticker)
{
    RubiksCube original;
    original.set(Face::Up, 0, 0, Color::Red);
    original.set(Face::Front, 2, 2, Color::Yellow);

    const RubiksCube copy(original);

    CHECK(original == copy);
    CHECK(copy.at(Face::Up, 0, 0) == Color::Red);
    CHECK(copy.at(Face::Front, 2, 2) == Color::Yellow);
}

TEST(CopyConstructorCreatesAnIndependentCube)
{
    RubiksCube original;
    RubiksCube copy(original);

    copy.set(Face::Down, 1, 1, Color::Green);

    CHECK(original.at(Face::Down, 1, 1) == Color::Yellow);
    CHECK(copy.at(Face::Down, 1, 1) == Color::Green);
    CHECK(original != copy);
}

TEST(CopyAssignmentCopiesEverySticker)
{
    RubiksCube source;
    source.set(Face::Left, 0, 2, Color::White);
    source.rotate(Face::Right, Turn::Clockwise);
    RubiksCube target;

    target = source;

    CHECK(source == target);
    CHECK(target.at(Face::Left, 0, 2) == Color::White);
}

TEST(CopyAssignmentCreatesAnIndependentCube)
{
    RubiksCube source;
    RubiksCube target;

    target = source;
    target.set(Face::Back, 0, 0, Color::Green);

    CHECK(source.at(Face::Back, 0, 0) == Color::Blue);
    CHECK(target.at(Face::Back, 0, 0) == Color::Green);
}

TEST(CopyAssignmentHandlesSelfAssignment)
{
    RubiksCube cube;
    cube.set(Face::Up, 1, 1, Color::Red);
    const RubiksCube expected(cube);

    RubiksCube& result = (cube = cube);

    CHECK(&result == &cube);
    CHECK(cube == expected);
}

TEST(EqualityAcceptsTwoIdenticalCubes)
{
    RubiksCube first;
    RubiksCube second;
    first.rotate(Face::Up, Turn::Clockwise);
    second.rotate(Face::Up, Turn::Clockwise);

    CHECK(first == second);
    CHECK(!(first != second));
}

TEST(InequalityDetectsOneDifferentSticker)
{
    RubiksCube first;
    RubiksCube second;
    second.set(Face::Front, 1, 2, Color::White);

    CHECK(first != second);
    CHECK(!(first == second));
}

TEST(RotateClockwiseThenCounterClockwiseRestoresEveryFace)
{
    for (const Face face : kAllFaces) {
        RubiksCube cube;
        cube.set(face, 0, 0, Color::Red);
        cube.set(Face::Front, 1, 1, Color::Blue);
        const RubiksCube original(cube);

        cube.rotate(face, Turn::Clockwise);
        cube.rotate(face, Turn::CounterClockwise);

        CHECK(cube == original);
    }
}

TEST(FourClockwiseTurnsRestoreEveryFace)
{
    for (const Face face : kAllFaces) {
        RubiksCube cube;
        cube.set(face, 2, 1, Color::White);
        const RubiksCube original(cube);

        for (int turn = 0; turn < 4; ++turn) {
            cube.rotate(face, Turn::Clockwise);
        }

        CHECK(cube == original);
    }
}

TEST(FourCounterClockwiseTurnsRestoreEveryFace)
{
    for (const Face face : kAllFaces) {
        RubiksCube cube;
        cube.set(face, 1, 2, Color::Yellow);
        const RubiksCube original(cube);

        for (int turn = 0; turn < 4; ++turn) {
            cube.rotate(face, Turn::CounterClockwise);
        }

        CHECK(cube == original);
    }
}

TEST(RotatePreservesTheNumberOfStickersOfEachColor)
{
    RubiksCube cube;
    std::mt19937 rng(2024);
    cube.randomize(rng);

    for (const Face face : kAllFaces) {
        cube.rotate(face, Turn::Clockwise);

        for (const Color color : kAllColors) {
            CHECK_EQUAL(9, countColor(cube, color));
        }
    }
}

TEST(RotateLeavesTheOppositeFaceUnchanged)
{
    const Face opposite[] = {
        Face::Down,
        Face::Up,
        Face::Right,
        Face::Left,
        Face::Back,
        Face::Front};
    const Color oppositeColor[] = {
        Color::Yellow,
        Color::White,
        Color::Red,
        Color::Orange,
        Color::Blue,
        Color::Green};

    for (int index = 0; index < RubiksCube::kFaceCount; ++index) {
        RubiksCube cube;
        cube.rotate(kAllFaces[index], Turn::Clockwise);

        CHECK(faceHasColor(cube, opposite[index], oppositeColor[index]));
    }
}

TEST(RotateKeepsTheFaceCenterFixed)
{
    for (const Face face : kAllFaces) {
        RubiksCube cube;
        cube.set(face, 1, 1, Color::Red);

        cube.rotate(face, Turn::Clockwise);

        CHECK(cube.at(face, 1, 1) == Color::Red);
    }
}

TEST(UpClockwiseCyclesTheFourAdjacentTopRows)
{
    RubiksCube cube;
    cube.set(Face::Front, 0, 0, Color::Red);
    cube.set(Face::Left, 0, 0, Color::Yellow);
    cube.set(Face::Back, 0, 0, Color::Orange);
    cube.set(Face::Right, 0, 0, Color::Blue);

    cube.rotate(Face::Up, Turn::Clockwise);

    CHECK(cube.at(Face::Left, 0, 0) == Color::Red);
    CHECK(cube.at(Face::Back, 0, 0) == Color::Yellow);
    CHECK(cube.at(Face::Right, 0, 0) == Color::Orange);
    CHECK(cube.at(Face::Front, 0, 0) == Color::Blue);
}

TEST(UpClockwiseRotatesTheUpFaceItself)
{
    RubiksCube cube;
    cube.set(Face::Up, 0, 0, Color::Green);

    cube.rotate(Face::Up, Turn::Clockwise);

    CHECK(cube.at(Face::Up, 0, 2) == Color::Green);
}

TEST(RotateThrowsForInvalidFaceOrTurn)
{
    RubiksCube cube;

    CHECK_THROW(cube.rotate(static_cast<Face>(99), Turn::Clockwise),
                std::out_of_range);
    CHECK_THROW(cube.rotate(Face::Up, static_cast<Turn>(99)),
                std::out_of_range);
}

TEST(RandomizePlacesNineStickersOfEachColor)
{
    RubiksCube cube;
    std::mt19937 rng(12345);

    cube.randomize(rng);

    for (const Color color : kAllColors) {
        CHECK_EQUAL(9, countColor(cube, color));
    }
}

TEST(RandomizeWithTheSameSeedProducesTheSameCube)
{
    RubiksCube first;
    RubiksCube second;
    std::mt19937 firstRng(777);
    std::mt19937 secondRng(777);

    first.randomize(firstRng);
    second.randomize(secondRng);

    CHECK(first == second);
}

TEST(RandomizeWithDifferentSeedsProducesDifferentCubes)
{
    RubiksCube first;
    RubiksCube second;
    std::mt19937 firstRng(1);
    std::mt19937 secondRng(2);

    first.randomize(firstRng);
    second.randomize(secondRng);

    CHECK(first != second);
}

TEST(RandomizeWithFixedSeedProducesAnUnsolvedCube)
{
    RubiksCube cube;
    std::mt19937 rng(42);

    cube.randomize(rng);

    CHECK(!cube.isSolved());
}

TEST(RandomizeWithoutArgumentsPreservesColorCounts)
{
    RubiksCube cube;

    cube.randomize();

    for (const Color color : kAllColors) {
        CHECK_EQUAL(9, countColor(cube, color));
    }
}

TEST(StreamOutputMatchesTheSolvedFileFormat)
{
    const RubiksCube cube;
    std::ostringstream output;

    output << cube;

    CHECK(output.str() == solvedText());
}

TEST(StreamInputParsesASolvedCube)
{
    RubiksCube cube;
    std::istringstream input(solvedText());

    input >> cube;

    CHECK(!input.fail());
    CHECK(cube.isSolved());
}

TEST(StreamInputSkipsWhitespaceBetweenStickers)
{
    std::string spaced;
    for (const char character : solvedText()) {
        if (character != '\n') {
            spaced += ' ';
            spaced += character;
        }
    }
    RubiksCube cube;
    std::istringstream input(spaced);

    input >> cube;

    CHECK(!input.fail());
    CHECK(cube.isSolved());
}

TEST(StreamInputAcceptsLowercaseLetters)
{
    std::string lower = solvedText();
    for (char& character : lower) {
        if (character >= 'A' && character <= 'Z') {
            character = static_cast<char>(character - 'A' + 'a');
        }
    }
    RubiksCube cube;
    std::istringstream input(lower);

    input >> cube;

    CHECK(!input.fail());
    CHECK(cube.isSolved());
}

TEST(StreamInputRejectsAnInvalidCharacterWithoutChangingTheCube)
{
    RubiksCube cube;
    cube.set(Face::Up, 0, 0, Color::Red);
    const RubiksCube expected(cube);
    std::istringstream input(std::string(kTotalStickers, 'X'));

    input >> cube;

    CHECK(input.fail());
    CHECK(cube == expected);
}

TEST(StreamInputRejectsATruncatedCubeWithoutChangingTheCube)
{
    RubiksCube cube;
    cube.set(Face::Up, 0, 0, Color::Red);
    const RubiksCube expected(cube);
    std::istringstream input("WWWYYY");

    input >> cube;

    CHECK(input.fail());
    CHECK(cube == expected);
}

TEST(StreamOutputAndInputRoundTrip)
{
    RubiksCube original;
    original.rotate(Face::Front, Turn::Clockwise);
    original.rotate(Face::Right, Turn::CounterClockwise);
    std::stringstream stream;

    stream << original;

    RubiksCube restored;
    stream >> restored;

    CHECK(original == restored);
}

TEST(LoadFromFileReadsTheSolvedSample)
{
    RubiksCube cube;
    std::mt19937 rng(5);
    cube.randomize(rng);

    CHECK(cube.loadFromFile(dataFile("solved_cube.txt")));
    CHECK(cube.isSolved());
}

TEST(LoadFromFileReadsTheScrambledSample)
{
    RubiksCube cube;

    CHECK(cube.loadFromFile(dataFile("scrambled_cube.txt")));
    CHECK(!cube.isSolved());
    for (const Color color : kAllColors) {
        CHECK_EQUAL(9, countColor(cube, color));
    }
}

TEST(LoadFromFileLeavesTheCubeUnchangedWhenTheFileIsMissing)
{
    RubiksCube cube;
    cube.set(Face::Down, 0, 0, Color::Red);
    const RubiksCube expected(cube);

    CHECK(!cube.loadFromFile(dataFile("file_that_does_not_exist.txt")));
    CHECK(cube == expected);
}

TEST(LoadFromFileLeavesTheCubeUnchangedWhenParsingFails)
{
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / "rubiks_bad_state.txt";
    {
        std::ofstream output(path);
        output << "not a cube";
    }

    RubiksCube cube;
    cube.set(Face::Down, 0, 0, Color::Red);
    const RubiksCube expected(cube);

    CHECK(!cube.loadFromFile(path.string()));
    CHECK(cube == expected);

    std::filesystem::remove(path);
}

TEST(SaveToFileAndLoadFromFileRoundTrip)
{
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / "rubiks cube round trip.txt";
    RubiksCube original;
    original.rotate(Face::Up, Turn::Clockwise);
    original.rotate(Face::Left, Turn::CounterClockwise);

    CHECK(original.saveToFile(path.string()));

    RubiksCube restored;
    CHECK(restored.loadFromFile(path.string()));
    CHECK(restored == original);

    std::filesystem::remove(path);
}

TEST(SaveToFileFailsForAnUnwritablePath)
{
    const RubiksCube cube;

    CHECK(!cube.saveToFile(
        "/nonexistent-rubiks-directory/state.txt"));
}

TEST(ToStringConvertsEveryFace)
{
    CHECK(RubiksCube::toString(Face::Up) == "U");
    CHECK(RubiksCube::toString(Face::Down) == "D");
    CHECK(RubiksCube::toString(Face::Left) == "L");
    CHECK(RubiksCube::toString(Face::Right) == "R");
    CHECK(RubiksCube::toString(Face::Front) == "F");
    CHECK(RubiksCube::toString(Face::Back) == "B");
}

TEST(ToStringConvertsEveryColor)
{
    CHECK(RubiksCube::toString(Color::White) == "White");
    CHECK(RubiksCube::toString(Color::Yellow) == "Yellow");
    CHECK(RubiksCube::toString(Color::Green) == "Green");
    CHECK(RubiksCube::toString(Color::Blue) == "Blue");
    CHECK(RubiksCube::toString(Color::Red) == "Red");
    CHECK(RubiksCube::toString(Color::Orange) == "Orange");
}

TEST(ToStringConvertsEveryTurn)
{
    CHECK(RubiksCube::toString(Turn::Clockwise) == "Clockwise");
    CHECK(RubiksCube::toString(Turn::CounterClockwise)
          == "CounterClockwise");
}

TEST(ToStringThrowsForInvalidValues)
{
    CHECK_THROW(RubiksCube::toString(static_cast<Face>(99)),
                std::out_of_range);
    CHECK_THROW(RubiksCube::toString(static_cast<Color>(99)),
                std::out_of_range);
    CHECK_THROW(RubiksCube::toString(static_cast<Turn>(99)),
                std::out_of_range);
}

TEST(SolvedCheckRejectsAUniformButWronglyColoredFace)
{
    RubiksCube cube;
    for (int row = 0; row < RubiksCube::kSize; ++row) {
        for (int column = 0; column < RubiksCube::kSize; ++column) {
            cube.set(Face::Up, row, column, Color::Red);
        }
    }

    CHECK(!cube.isSolved());
}
