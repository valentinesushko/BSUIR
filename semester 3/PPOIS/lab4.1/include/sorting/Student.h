#pragma once

#include <iosfwd>
#include <string>

namespace sorting {

class Student {
public:
    Student(std::string name, int group, double gpa);

    const std::string& name() const noexcept;
    int group() const noexcept;
    double gpa() const noexcept;

    bool operator<(const Student& other) const noexcept;
    bool operator==(const Student& other) const noexcept;

    std::string describe() const;

private:
    std::string name_;
    int group_;
    double gpa_;
};

std::ostream& operator<<(std::ostream& stream, const Student& student);

}  // namespace sorting
