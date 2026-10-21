#ifndef RUBIKS_CUBE_H
#define RUBIKS_CUBE_H

#include <array>
#include <iosfwd>
#include <random>
#include <string>

/**
 * @file RubiksCube.h
 * @brief Model of a 3x3x3 Rubik's Cube independent of any user interface.
 */

/**
 * @class RubiksCube
 * @brief Stores the 54 stickers of a 3x3x3 Rubik's Cube and implements face turns.
 *
 * The cube is a pure model: it has no console or graphical user interface and can
 * be embedded in any front end. Each face is stored as nine stickers in row-major
 * order. Row 0 is the top row and column 0 is the left column as seen from
 * outside that face.
 *
 * @section rotation_convention Rotation convention
 *
 * A right-handed coordinate system is used: +x points right, +y points up and
 * +z points toward the viewer (front). The outward normal of every face is:
 * - Up:    +y
 * - Down:  -y
 * - Front: +z
 * - Back:  -z
 * - Right: +x
 * - Left:  -x
 *
 * When looking directly at a face from outside the cube, the on-screen
 * directions (right, down) are:
 * - Front: (+x, -y)
 * - Back:  (-x, -y)
 * - Up:    (+x, +z)
 * - Down:  (+x, -z)
 * - Right: (-z, -y)
 * - Left:  (+z, -y)
 *
 * A clockwise turn is a -90 degree rotation about that face's outward normal;
 * a counter-clockwise turn is a +90 degree rotation about the same normal. The
 * whole layer of 21 stickers (9 on the face and 12 on the adjacent strips) is
 * rotated as one rigid body: colors travel with their positions and the face
 * orientations travel with their normals. Consequently every turn is a
 * permutation and the number of stickers of each color never changes.
 */
class RubiksCube {
public:
    /**
     * @brief Identifies one of the six faces of the cube.
     */
    enum class Face {
        Up = 0,    ///< The top face (outward normal +y).
        Down,      ///< The bottom face (outward normal -y).
        Left,      ///< The left face (outward normal -x).
        Right,     ///< The right face (outward normal +x).
        Front,     ///< The front face (outward normal +z).
        Back       ///< The back face (outward normal -z).
    };

    /**
     * @brief Identifies one of the six possible sticker colors.
     */
    enum class Color {
        White = 0, ///< White, used on the solved Up face.
        Yellow,    ///< Yellow, used on the solved Down face.
        Green,     ///< Green, used on the solved Front face.
        Blue,      ///< Blue, used on the solved Back face.
        Red,       ///< Red, used on the solved Right face.
        Orange     ///< Orange, used on the solved Left face.
    };

    /**
     * @brief Direction of a face turn.
     */
    enum class Turn {
        Clockwise,        ///< -90 degrees about the outward normal.
        CounterClockwise  ///< +90 degrees about the outward normal.
    };

    /// Number of stickers along one edge of the cube.
    static constexpr int kSize = 3;
    /// Number of faces of the cube.
    static constexpr int kFaceCount = 6;
    /// Number of stickers on a single face.
    static constexpr int kStickersPerFace = kSize * kSize;

    /**
     * @brief Constructs the canonical solved cube.
     *
     * Up is White, Down is Yellow, Left is Orange, Right is Red, Front is
     * Green and Back is Blue, with every face uniform. isSolved() returns true
     * for this arrangement.
     */
    RubiksCube();

    /**
     * @brief Creates an independent deep copy of another cube.
     * @param other Cube to copy.
     */
    RubiksCube(const RubiksCube& other);

    /**
     * @brief Assigns an independent deep copy of another cube.
     * @param other Cube to copy.
     * @return Reference to this cube.
     *
     * Self-assignment is safe.
     */
    RubiksCube& operator=(const RubiksCube& other);

    /**
     * @brief Destroys the cube.
     *
     * The class owns no dynamic resources, so destruction is trivial and safe.
     */
    ~RubiksCube();

    /**
     * @brief Returns the color of one sticker.
     * @param face Face that owns the sticker.
     * @param row Row index, 0 is the top row as seen from outside the face.
     * @param column Column index, 0 is the left column as seen from outside.
     * @return Color stored at the requested position.
     * @throws std::out_of_range If the face, row or column is invalid.
     */
    Color at(Face face, int row, int column) const;

    /**
     * @brief Changes the color of one sticker.
     * @param face Face that owns the sticker.
     * @param row Row index, 0 is the top row as seen from outside the face.
     * @param column Column index, 0 is the left column as seen from outside.
     * @param color New color of the sticker.
     * @throws std::out_of_range If the face, row or column is invalid.
     */
    void set(Face face, int row, int column, Color color);

    /**
     * @brief Rotates one face and the adjacent strips of its layer.
     * @param face Face to turn.
     * @param turn Clockwise or counter-clockwise direction.
     * @throws std::out_of_range If the face or turn value is invalid.
     *
     * The turn follows the convention documented at the top of this class. All
     * 21 stickers of the layer move as a rigid body and the turn is always a
     * permutation of the stickers.
     */
    void rotate(Face face, Turn turn);

    /**
     * @brief Checks whether the cube is in the canonical solved arrangement.
     * @return true only when every face is uniform and each face has its
     *         canonical solved color (Up White, Down Yellow, Left Orange,
     *         Right Red, Front Green, Back Blue).
     */
    bool isSolved() const;

    /**
     * @brief Fills the cube with a random permutation of the sticker colors.
     * @param rng Random number generator supplied by the caller.
     *
     * Exactly nine stickers of each Color are placed; the resulting state need
     * not be solvable. The supplied generator is used as the only source of
     * randomness, so the caller controls reproducibility.
     */
    void randomize(std::mt19937& rng);

    /**
     * @brief Fills the cube with a random permutation seeded from std::random_device.
     *
     * Exactly nine stickers of each Color are placed; the resulting state need
     * not be solvable.
     */
    void randomize();

    /**
     * @brief Loads a cube state from a text file.
     * @param path Path of the file to read.
     * @return true on success, false if the file cannot be opened or parsed.
     *
     * On failure the cube is left unchanged.
     */
    bool loadFromFile(const std::string& path);

    /**
     * @brief Saves the cube state to a text file.
     * @param path Path of the file to write.
     * @return true on success, false if the file cannot be opened or written.
     */
    bool saveToFile(const std::string& path) const;

    /**
     * @brief Compares all 54 stickers of two cubes.
     * @param lhs Left-hand cube.
     * @param rhs Right-hand cube.
     * @return true when every sticker is equal.
     */
    friend bool operator==(const RubiksCube& lhs, const RubiksCube& rhs);

    /**
     * @brief Compares all 54 stickers of two cubes.
     * @param lhs Left-hand cube.
     * @param rhs Right-hand cube.
     * @return true when at least one sticker differs.
     */
    friend bool operator!=(const RubiksCube& lhs, const RubiksCube& rhs);

    /**
     * @brief Writes six lines of nine sticker characters.
     * @param out Output stream.
     * @param cube Cube to write.
     * @return The output stream.
     *
     * Faces are written in the order Up, Down, Left, Right, Front, Back. Each
     * line contains nine characters, row-major, followed by a newline. Colors
     * are written as W, Y, G, B, R and O.
     */
    friend std::ostream& operator<<(std::ostream& out, const RubiksCube& cube);

    /**
     * @brief Reads a cube state from a stream.
     * @param in Input stream.
     * @param cube Destination cube.
     * @return The input stream.
     *
     * Leading and embedded whitespace is skipped. Exactly 54 color characters
     * are read, accepting either uppercase or lowercase W, Y, G, B, R and O.
     * The parsed state is committed only if all 54 characters are valid.
     * Otherwise failbit is set and @p cube is left unchanged.
     */
    friend std::istream& operator>>(std::istream& in, RubiksCube& cube);

    /**
     * @brief Converts a face to its single-letter textual representation.
     * @param face Face to convert.
     * @return "U", "D", "L", "R", "F" or "B".
     * @throws std::out_of_range If @p face is not a valid Face value.
     */
    static std::string toString(Face face);

    /**
     * @brief Converts a color to its textual representation.
     * @param color Color to convert.
     * @return "White", "Yellow", "Green", "Blue", "Red" or "Orange".
     * @throws std::out_of_range If @p color is not a valid Color value.
     */
    static std::string toString(Color color);

    /**
     * @brief Converts a turn to its textual representation.
     * @param turn Turn to convert.
     * @return "Clockwise" or "CounterClockwise".
     * @throws std::out_of_range If @p turn is not a valid Turn value.
     */
    static std::string toString(Turn turn);

private:
    /// Stickers indexed by face, then row-major position on that face.
    std::array<std::array<Color, kStickersPerFace>, kFaceCount> stickers_{};
};

#endif
