#pragma once

#include <string>

#include "employee_file.h"
#include "named_pipe.h"
#include "protocol.h"
#include "record_locks.h"

class ServerLogic {
public:
    explicit ServerLogic(const std::string& filename);

    void process_client(HANDLE pipe);

private:
    bool handle_read_request(
        HANDLE pipe,
        const ClientRequest& request
    );

    bool handle_modify_request(
        HANDLE pipe,
        const ClientRequest& request
    );

    void handle_update_request(
        HANDLE pipe,
        const ClientRequest& request
    );


private:
    std::string filename_;
    RecordLocks locks_;
};