#include "client_logic.h"
#include "input_utils.h"

#include <cstring>
#include <iostream>
#include <limits>


void ClientLogic::run() {
    NamedPipe pipe = connect_to_server_pipe();

    bool running = true;

    while (running) {
        print_menu();

        int choice;
        choice = input_menu_choice("Your choice: ");

        switch (choice) {
        case 1:
            read_record(pipe.handle());
            break;

        case 2:
            modify_record(pipe.handle());
            break;

        case 0:
            send_exit(pipe.handle());
            running = false;
            break;

        default:
            std::cout << "Unknown command.\n";
            break;
        }
    }
}

void ClientLogic::print_menu() const {
    std::cout << "\nChoose operation:\n";
    std::cout << "1. Read employee record\n";
    std::cout << "2. Modify employee record\n";
    std::cout << "0. Exit\n";
}

void ClientLogic::read_record(HANDLE pipe) {
    int employee_id;

    employee_id = input_positive_int("Enter employee ID: ");

    ClientRequest request{};
    request.type = RequestType::ReadRecord;
    request.employee_id = employee_id;

    write_to_pipe(
        pipe,
        &request,
        sizeof(request)
    );

    ServerResponse response{};

    read_from_pipe(
        pipe,
        &response,
        sizeof(response)
    );

    std::cout << response.message << '\n';

    if (response.status == ResponseStatus::Success) {
        print_employee(response.data);

        wait_enter("Press Enter to finish access to this record...");

        send_release_read(pipe, employee_id);
    }
}

void ClientLogic::modify_record(HANDLE pipe) {
    int employee_id;

    employee_id = input_positive_int("Enter employee ID: ");

    ClientRequest request{};
    request.type = RequestType::ModifyRecord;
    request.employee_id = employee_id;

    write_to_pipe(
        pipe,
        &request,
        sizeof(request)
    );

    ServerResponse response{};

    read_from_pipe(
        pipe,
        &response,
        sizeof(response)
    );

    std::cout << response.message << '\n';

    if (response.status != ResponseStatus::Success) {
        return;
    }

    std::cout << "Current record:\n";
    print_employee(response.data);

    employee updated = response.data;

    std::string name = input_name("Enter new name: ");
    std::strcpy(updated.name, name.c_str());

    updated.hours = input_non_negative_double("Enter new hours: ");

    ClientRequest update_request{};
    update_request.type = RequestType::UpdateRecord;
    update_request.employee_id = employee_id;
    update_request.data = updated;

    wait_enter("Press Enter to finish access to this record...");

    write_to_pipe(
        pipe,
        &update_request,
        sizeof(update_request)
    );

    ServerResponse update_response{};

    read_from_pipe(
        pipe,
        &update_response,
        sizeof(update_response)
    );

    std::cout << update_response.message << '\n';

    wait_enter("Press Enter to finish access to this record...");

    send_release_write(pipe, employee_id);
}

void ClientLogic::send_release_read(HANDLE pipe, int employee_id) {
    ClientRequest request{};
    request.type = RequestType::ReleaseReadRecord;
    request.employee_id = employee_id;

    write_to_pipe(
        pipe,
        &request,
        sizeof(request)
    );
}

void ClientLogic::send_release_write(HANDLE pipe, int employee_id) {
    ClientRequest request{};
    request.type = RequestType::ReleaseWriteRecord;
    request.employee_id = employee_id;

    write_to_pipe(
        pipe,
        &request,
        sizeof(request)
    );
}

void ClientLogic::send_exit(HANDLE pipe) {
    ClientRequest request{};
    request.type = RequestType::ExitClient;

    write_to_pipe(
        pipe,
        &request,
        sizeof(request)
    );
}

void ClientLogic::print_employee(const employee& emp) const {
    std::cout << "Employee record:\n";
    std::cout << "ID: " << emp.num << '\n';
    std::cout << "Name: " << emp.name << '\n';
    std::cout << "Hours: " << emp.hours << '\n';
}

