#include "sorting/Student.h"

#include <ostream>
#include <stdexcept>
#include <tuple>
#include <utility>

namespace sorting {

Student::Student(std::string name, int group, double gpa)
    : name_(std::move(name)), group_(group), gpa_(gpa) {
    if (name_.empty()) {
        throw std::invalid_argument("student name must not be empty");
    }
    if (group_ <= 0) {
        throw std::invalid_argument("student group must be positive");
    }
    if (gpa_ < 0.0 || gpa_ > 10.0) {
        throw std::invalid_argument("student gpa must be within [0, 10]");
    }
}

const std::string& Student::name() const noexcept {
    return name_;
}

int Student::group() const noexcept {
    return group_;
}

double Student::gpa() const noexcept {
    return gpa_;
}

bool Student::operator<(const Student& other) const noexcept {
    return std::tie(gpa_, name_, group_) <
           std::tie(other.gpa_, other.name_, other.group_);
}

bool Student::operator==(const Student& other) const noexcept {
    return std::tie(name_, group_, gpa_) ==
           std::tie(other.name_, other.group_, other.gpa_);
}

std::string Student::describe() const {
    return name_ + " (group " + std::to_string(group_) + ", GPA " +
           std::to_string(gpa_) + ")";
}

std::ostream& operator<<(std::ostream& stream, const Student& student) {
    return stream << student.describe();
}

}  // namespace sorting
