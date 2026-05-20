#include "named_pipe.h"
#include "protocol.h"
#include "winapi_error.h"

#include <stdexcept>

std::string get_pipe_name() {
    return "\\\\.\\pipe\\employee_pipe";
}

NamedPipe::NamedPipe(HANDLE handle)
    : handle_(handle) {
}

NamedPipe::~NamedPipe() {
    close();
}

//move-конструктор нужен, чтобы передать владение ресурсом из одного объекта в другой.
//мой ресурс - дескриптор именованного канала.
NamedPipe::NamedPipe(NamedPipe&& other) noexcept
    : handle_(other.handle_) {
    other.handle_ = INVALID_HANDLE_VALUE;
}

NamedPipe& NamedPipe::operator=(NamedPipe&& other) noexcept {
    if (this != &other) {
        close();
        handle_ = other.handle_;
        other.handle_ = INVALID_HANDLE_VALUE;
    }

    return *this;
}

HANDLE NamedPipe::handle() const {
    return handle_;
}

void NamedPipe::close() {
    if (handle_ != INVALID_HANDLE_VALUE && handle_ != NULL) {
        CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
    }
}

NamedPipe create_server_pipe() {
    HANDLE pipe = CreateNamedPipeA(
        get_pipe_name().c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        sizeof(ServerResponse),
        sizeof(ClientRequest),
        INFINITE,
        NULL
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        throw WinApiError::from_last_error("CreateNamedPipeA failed");
    }

    return NamedPipe(pipe);
}

void wait_client_connection(const NamedPipe& pipe) {
    BOOL connected = ConnectNamedPipe(
        pipe.handle(),
        NULL
    );

    if (!connected) {
        DWORD error = GetLastError();

        if (error != ERROR_PIPE_CONNECTED) {
            throw WinApiError("ConnectNamedPipe failed", error);
        }
    }
}

NamedPipe connect_to_server_pipe() {
    if (!WaitNamedPipeA(get_pipe_name().c_str(), 5000)) {
        throw WinApiError::from_last_error("WaitNamedPipeA failed");
    }

    HANDLE pipe = CreateFileA(
        get_pipe_name().c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        throw WinApiError::from_last_error("CreateFileA failed");
    }

    return NamedPipe(pipe);
}

void write_to_pipe(HANDLE pipe, const void* data, DWORD size) {
    DWORD bytes_written = 0;

    BOOL success = WriteFile(
        pipe,
        data,
        size,
        &bytes_written,
        NULL
    );

    if (!success) {
        throw WinApiError::from_last_error("WriteFile failed");
    }

    if (bytes_written != size) {
        throw std::runtime_error("WriteFile wrote incomplete data");
    }
}

void read_from_pipe(HANDLE pipe, void* data, DWORD size) {
    DWORD bytes_read = 0;

    BOOL success = ReadFile(
        pipe,
        data,
        size,
        &bytes_read,
        NULL
    );

    if (!success) {
        throw WinApiError::from_last_error("ReadFile failed");
    }

    if (bytes_read != size) {
        throw std::runtime_error("ReadFile read incomplete data");
    }
}