#include "input_utils.h"

#include <iostream>
#include <sstream>
#include <cctype>

bool is_letters_only(const std::string& text) {
    if (text.empty() || text.size() > 9) {
        return false;
    }

    for (unsigned char ch : text) {

        if ((ch >= 'A' && ch <= 'Z') ||
            (ch >= 'a' && ch <= 'z')) {
            continue;
        }

        if (ch >= 128) {
            continue;
        }

        return false;
    }

    return true;
}

int input_positive_int(const std::string& prompt) {
    std::string line;
    int value;
    char extra;

    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);

        std::stringstream ss(line);

        if ((ss >> value) && !(ss >> extra) && value > 0) {
            return value;
        }

        std::cout << "Error: enter a positive integer number.\n";
    }
}

double input_non_negative_double(const std::string& prompt) {
    std::string line;
    double value;
    char extra;

    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);

        std::stringstream ss(line);

        if ((ss >> value) && !(ss >> extra) && value >= 0) {
            return value;
        }

        std::cout << "Error: enter a non-negative number.\n";
    }
}

std::string input_name(const std::string& prompt) {
    std::string value;

    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);

        if (is_letters_only(value)) {
            return value;
        }

        std::cout << "Error: name must contain only letters and be no longer than 9 characters.\n";
    }
}

int input_menu_choice(const std::string& prompt) {
    std::string line;
    int value;
    char extra;

    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);

        std::stringstream ss(line);

        if ((ss >> value) && !(ss >> extra) && value >= 0 && value <= 2) {
            return value;
        }

        std::cout << "Error: enter 0, 1 or 2.\n";
    }
}

void wait_enter(const std::string& prompt) {
    std::string line;
    std::cout << prompt;
    std::getline(std::cin, line);
}

std::string input_filename(const std::string& prompt) {
    std::string value;

    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);

        if (!value.empty()) {
            return value;
        }

        std::cout << "Error: file name cannot be empty.\n";
    }
}

int input_int_in_range(
    const std::string& prompt,
    int min_value,
    int max_value
) {
    std::string line;
    int value;
    char extra;

    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);

        std::stringstream ss(line);

        if ((ss >> value) &&
            !(ss >> extra) &&
            value >= min_value &&
            value <= max_value) {

            return value;
        }

        std::cout
            << "Error: enter integer number from "
            << min_value
            << " to "
            << max_value
            << ".\n";
    }
}