#pragma once

#include <string>
#include <vector>

#include "employee.h"

void create_employee_file(
    const std::string& filename,
    const std::vector<employee>& employees
);

void print_employee_file(const std::string& filename);

bool find_employee_by_id(
    const std::string& filename,
    int id,
    employee& result
);

bool update_employee_by_id(
    const std::string& filename,
    const employee& updated_employee
);