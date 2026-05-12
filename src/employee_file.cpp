#include "employee_file.h"

#include <fstream>
#include <iostream>

void create_employee_file(
    const std::string& filename,
    const std::vector<employee>& employees
) {
    std::ofstream file(filename, std::ios::binary);

    for (const auto& emp : employees) {
        file.write(
            reinterpret_cast<const char*>(&emp),
            sizeof(employee)
        );
    }
}

void print_employee_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    employee emp;

    std::cout << "\nEmployees:\n";

    while (
        file.read(
            reinterpret_cast<char*>(&emp),
            sizeof(employee)
        )
        ) {
        std::cout
            << "ID: " << emp.num
            << ", Name: " << emp.name
            << ", Hours: " << emp.hours
            << '\n';
    }
}

bool find_employee_by_id(
    const std::string& filename,
    int id,
    employee& result
) {
    std::ifstream file(filename, std::ios::binary);

    employee emp;

    while (
        file.read(
            reinterpret_cast<char*>(&emp),
            sizeof(employee)
        )
        ) {
        if (emp.num == id) {
            result = emp;
            return true;
        }
    }

    return false;
}

bool update_employee_by_id(
    const std::string& filename,
    const employee& updated_employee
) {
    std::fstream file(
        filename,
        std::ios::binary |
        std::ios::in |
        std::ios::out
    );

    employee emp;

    while (
        file.read(
            reinterpret_cast<char*>(&emp),
            sizeof(employee)
        )
        ) {
        if (emp.num == updated_employee.num) {

            file.seekp(
                -static_cast<int>(sizeof(employee)),
                std::ios::cur
            );

            file.write(
                reinterpret_cast<const char*>(&updated_employee),
                sizeof(employee)
            );

            return true;
        }
    }

    return false;
}