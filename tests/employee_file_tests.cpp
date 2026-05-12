#include <gtest/gtest.h>

#include "employee_file.h"

#include <cstdio>
#include <vector>

TEST(EmployeeFileTests, CreateAndFindEmployeeById) {
    const std::string filename = "test_employees.dat";

    std::vector<employee> employees = {
        {1, "Ann", 10.5},
        {2, "Bob", 20.0},
        {3, "Kate", 30.25}
    };

    create_employee_file(filename, employees);

    employee result{};

    ASSERT_TRUE(find_employee_by_id(filename, 2, result));
    EXPECT_EQ(result.num, 2);
    EXPECT_STREQ(result.name, "Bob");
    EXPECT_DOUBLE_EQ(result.hours, 20.0);

    std::remove(filename.c_str());
}

TEST(EmployeeFileTests, ReturnFalseIfEmployeeNotFound) {
    const std::string filename = "test_employees_not_found.dat";

    std::vector<employee> employees = {
        {1, "Ann", 10.5}
    };

    create_employee_file(filename, employees);

    employee result{};

    EXPECT_FALSE(find_employee_by_id(filename, 100, result));

    std::remove(filename.c_str());
}

TEST(EmployeeFileTests, UpdateEmployeeById) {
    const std::string filename = "test_employees_update.dat";

    std::vector<employee> employees = {
        {1, "Ann", 10.5},
        {2, "Bob", 20.0}
    };

    create_employee_file(filename, employees);

    employee updated = { 2, "Mike", 45.5 };

    ASSERT_TRUE(update_employee_by_id(filename, updated));

    employee result{};

    ASSERT_TRUE(find_employee_by_id(filename, 2, result));
    EXPECT_EQ(result.num, 2);
    EXPECT_STREQ(result.name, "Mike");
    EXPECT_DOUBLE_EQ(result.hours, 45.5);

    std::remove(filename.c_str());
}

TEST(EmployeeFileTests, ReturnFalseWhenFileIsEmpty) {
    const std::string filename = "test_empty_employees.dat";

    std::vector<employee> employees = {};

    create_employee_file(filename, employees);

    employee result{};

    EXPECT_FALSE(find_employee_by_id(filename, 1, result));

    std::remove(filename.c_str());
}

TEST(EmployeeFileTests, ReturnFalseWhenUpdatingMissingEmployee) {
    const std::string filename = "test_update_missing.dat";

    std::vector<employee> employees = {
        {1, "Ann", 10.5}
    };

    create_employee_file(filename, employees);

    employee updated = { 100, "Mike", 45.5 };

    EXPECT_FALSE(update_employee_by_id(filename, updated));

    std::remove(filename.c_str());
}

TEST(EmployeeFileTests, UpdateFirstAndLastEmployees) {
    const std::string filename = "test_update_edges.dat";

    std::vector<employee> employees = {
        {1, "Ann", 10.5},
        {2, "Bob", 20.0},
        {3, "Kate", 30.25}
    };

    create_employee_file(filename, employees);

    employee first_updated = { 1, "Alex", 11.0 };
    employee last_updated = { 3, "Mary", 33.0 };

    ASSERT_TRUE(update_employee_by_id(filename, first_updated));
    ASSERT_TRUE(update_employee_by_id(filename, last_updated));

    employee result{};

    ASSERT_TRUE(find_employee_by_id(filename, 1, result));
    EXPECT_STREQ(result.name, "Alex");
    EXPECT_DOUBLE_EQ(result.hours, 11.0);

    ASSERT_TRUE(find_employee_by_id(filename, 3, result));
    EXPECT_STREQ(result.name, "Mary");
    EXPECT_DOUBLE_EQ(result.hours, 33.0);

    std::remove(filename.c_str());
}