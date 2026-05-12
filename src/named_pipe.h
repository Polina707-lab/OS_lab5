#pragma once

#include <windows.h>
#include <string>

class NamedPipe {
public:
    explicit NamedPipe(HANDLE handle);
    ~NamedPipe();

    NamedPipe(const NamedPipe&) = delete;
    NamedPipe& operator=(const NamedPipe&) = delete;

    NamedPipe(NamedPipe&& other) noexcept;
    NamedPipe& operator=(NamedPipe&& other) noexcept;

    HANDLE handle() const;

    void close();

private:
    HANDLE handle_;
};

std::string get_pipe_name();

NamedPipe create_server_pipe();

void wait_client_connection(const NamedPipe& pipe);

NamedPipe connect_to_server_pipe();

void write_to_pipe(HANDLE pipe, const void* data, DWORD size);

void read_from_pipe(HANDLE pipe, void* data, DWORD size);