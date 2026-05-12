#pragma once

#include "employee.h"

enum class RequestType {
    ReadRecord,
    ModifyRecord,
    UpdateRecord,
    ReleaseReadRecord,
    ReleaseWriteRecord,
    ExitClient
};

enum class ResponseStatus {
    Success,
    NotFound,
    Error
};

struct ClientRequest {
    RequestType type;
    int employee_id;
    employee data;
};

struct ServerResponse {
    ResponseStatus status;
    employee data;
    char message[100];
};