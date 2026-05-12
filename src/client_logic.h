#pragma once

#include <windows.h>

#include "employee.h"
#include "named_pipe.h"
#include "protocol.h"

class ClientLogic {
public:
    void run();

private:
    void print_menu() const;

    void read_record(HANDLE pipe);
    void modify_record(HANDLE pipe);

    void send_release_read(HANDLE pipe, int employee_id);
    void send_release_write(HANDLE pipe, int employee_id);
    void send_exit(HANDLE pipe);

    void print_employee(const employee& emp) const;
};