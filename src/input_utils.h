#pragma once

#include <string>

int input_positive_int(const std::string& prompt);
double input_non_negative_double(const std::string& prompt);
std::string input_name(const std::string& prompt);
int input_menu_choice(const std::string& prompt);
void wait_enter(const std::string& prompt);
std::string input_filename(const std::string& prompt);
int input_int_in_range(
    const std::string& prompt,
    int min_value,
    int max_value
);