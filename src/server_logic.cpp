#include "server_logic.h"

#include <cstring>

enum class ActiveLock {
    None,
    Read,
    Write
};

ServerLogic::ServerLogic(const std::string& filename)
    : filename_(filename) {
}

void ServerLogic::process_client(HANDLE pipe) {
    ActiveLock active_lock = ActiveLock::None;
    int locked_employee_id = 0;

    try {
        while (true) {
            ClientRequest request{};

            read_from_pipe(
                pipe,
                &request,
                sizeof(request)
            );

            switch (request.type) {
            case RequestType::ReadRecord:
                if (handle_read_request(pipe, request)) {
                    active_lock = ActiveLock::Read;
                    locked_employee_id = request.employee_id;
                }
                break;

            case RequestType::ModifyRecord:
                if (handle_modify_request(pipe, request)) {
                    active_lock = ActiveLock::Write;
                    locked_employee_id = request.employee_id;
                }
                break;

            case RequestType::UpdateRecord:
                handle_update_request(pipe, request);
                break;

            case RequestType::ReleaseReadRecord:
                locks_.unlock_for_read(request.employee_id);
                active_lock = ActiveLock::None;
                break;

            case RequestType::ReleaseWriteRecord:
                locks_.unlock_for_write(request.employee_id);
                active_lock = ActiveLock::None;
                break;

            case RequestType::ExitClient:
                return;
            }
        }
    }
    catch (...) {
        if (active_lock == ActiveLock::Read) {
            locks_.unlock_for_read(locked_employee_id);
        }
        else if (active_lock == ActiveLock::Write) {
            locks_.unlock_for_write(locked_employee_id);
        }

        throw;
    }
}

bool ServerLogic::handle_read_request(
    HANDLE pipe,
    const ClientRequest& request
) {
    locks_.lock_for_read(request.employee_id);

    employee emp{};

    bool found = find_employee_by_id(
        filename_,
        request.employee_id,
        emp
    );

    ServerResponse response{};

    if (found) {
        response.status = ResponseStatus::Success;
        response.data = emp;

        std::strcpy(
            response.message,
            "Record successfully read."
        );
    }
    else {
        response.status = ResponseStatus::NotFound;

        std::strcpy(
            response.message,
            "Employee not found."
        );

        locks_.unlock_for_read(request.employee_id);
    }

    write_to_pipe(
        pipe,
        &response,
        sizeof(response)
    );

    return found;
}

bool ServerLogic::handle_modify_request(
    HANDLE pipe,
    const ClientRequest& request
) {
    locks_.lock_for_write(request.employee_id);

    employee emp{};

    bool found = find_employee_by_id(
        filename_,
        request.employee_id,
        emp
    );

    ServerResponse response{};

    if (found) {
        response.status = ResponseStatus::Success;
        response.data = emp;

        std::strcpy(
            response.message,
            "Record locked for modification."
        );
    }
    else {
        response.status = ResponseStatus::NotFound;

        std::strcpy(
            response.message,
            "Employee not found."
        );

        locks_.unlock_for_write(request.employee_id);
    }

    write_to_pipe(
        pipe,
        &response,
        sizeof(response)
    );

    return found;
}

void ServerLogic::handle_update_request(
    HANDLE pipe,
    const ClientRequest& request
) {
    bool updated = update_employee_by_id(
        filename_,
        request.data
    );

    ServerResponse response{};

    if (updated) {
        response.status = ResponseStatus::Success;

        std::strcpy(
            response.message,
            "Record updated successfully."
        );
    }
    else {
        response.status = ResponseStatus::Error;

        std::strcpy(
            response.message,
            "Update failed."
        );
    }

    write_to_pipe(
        pipe,
        &response,
        sizeof(response)
    );
}