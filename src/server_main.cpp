#include "employee_file.h"
#include "named_pipe.h"
#include "server_logic.h"
#include "input_utils.h"

#include <windows.h>

#include <iostream>
#include <string>
#include <vector>
#include <limits>

constexpr int MAX_CLIENT_COUNT = 10;

struct ThreadData {
    ServerLogic* server;
};

bool employee_id_exists(
    const std::vector<employee>& employees,
    int id
) {
    for (const auto& emp : employees) {
        if (emp.num == id) {
            return true;
        }
    }

    return false;
}

void input_employees(std::vector<employee>& employees) {
    int count;

    count = input_positive_int("Enter number of employees: ");

    employees.resize(count);

    for (int i = 0; i < count; ++i) {
        std::cout << "\nEmployee #" << i + 1 << '\n';

        while (true) {
            int id = input_positive_int("ID: ");

            if (!employee_id_exists(employees, id)) {
                employees[i].num = id;
                break;
            }

            std::cout << "Error: employee with this ID already exists.\n";
        }

        std::string name = input_name("Name: ");
        std::strcpy(employees[i].name, name.c_str());

        employees[i].hours = input_non_negative_double("Hours: ");
    }
}


void start_client_processes(int count) {
    char server_path[MAX_PATH];

    GetModuleFileNameA(
        NULL,
        server_path,
        MAX_PATH
    );

    std::string server_exe_path = server_path;
    std::string folder = server_exe_path.substr(
        0,
        server_exe_path.find_last_of("\\/")
    );

    std::string client_path = folder + "\\Client.exe";

    for (int i = 0; i < count; ++i) {
        STARTUPINFOA startup_info{};
        PROCESS_INFORMATION process_info{};

        startup_info.cb = sizeof(startup_info);

        BOOL success = CreateProcessA(
            client_path.c_str(),
            NULL,
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            folder.c_str(),
            &startup_info,
            &process_info
        );

        if (!success) {
            std::cerr << "Failed to start Client.exe. Path: "
                << client_path
                << "\nError: "
                << GetLastError()
                << '\n';
            continue;
        }

        CloseHandle(process_info.hThread);
        CloseHandle(process_info.hProcess);
    }
}

DWORD WINAPI serve_one_client(LPVOID parameter) {
    ThreadData* data = static_cast<ThreadData*>(parameter);

    try {
        NamedPipe pipe = create_server_pipe();

        wait_client_connection(pipe);

        data->server->process_client(pipe.handle());
    }
    catch (const std::exception& error) {
        std::cerr << "Client service error: " << error.what() << '\n';
    }

    return 0;
}

int main() {
    try {
        std::string filename;

        filename = input_filename("Enter binary file name: ");

        std::vector<employee> employees;
        input_employees(employees);

        create_employee_file(filename, employees);

        std::cout << "\nInitial file content:\n";
        print_employee_file(filename);

        int client_count;

        client_count = input_int_in_range(
            "\nEnter number of client processes (1-" +
            std::to_string(MAX_CLIENT_COUNT) +
            "): ",
            1,
            MAX_CLIENT_COUNT
        );

        ServerLogic server(filename);

        std::vector<HANDLE> thread_handles(client_count);
        std::vector<DWORD> thread_ids(client_count);
        std::vector<ThreadData> thread_data(client_count);

        for (int i = 0; i < client_count; ++i) {
            thread_data[i].server = &server;

            thread_handles[i] = CreateThread(
                NULL,
                0,
                serve_one_client,
                &thread_data[i],
                0,
                &thread_ids[i]
            );

            if (thread_handles[i] == NULL) {
                std::cerr << "CreateThread failed. Error: "
                    << GetLastError()
                    << '\n';
            }
        }

        start_client_processes(client_count);

        for (int i = 0; i < client_count; ++i) {
            if (thread_handles[i] != NULL) {
                WaitForSingleObject(
                    thread_handles[i],
                    INFINITE
                );

                CloseHandle(thread_handles[i]);
            }
        }

        std::cout << "\nModified file content:\n";
        print_employee_file(filename);

        wait_enter("\nPress Enter to finish server...");
    }
    catch (const std::exception& error) {
        std::cerr << "Server error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}