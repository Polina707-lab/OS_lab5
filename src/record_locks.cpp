#include "record_locks.h"
#include "winapi_error.h"

#include <stdexcept>

RecordLocks::RecordLocks() {
    mutex_ = CreateMutexA(
        NULL,
        FALSE,
        NULL
    );

    if (mutex_ == NULL) {
        throw WinApiError::from_last_error("CreateMutexA failed");
    }

    event_ = CreateEventA(
        NULL,
        TRUE,
        FALSE,
        NULL
    );

    if (event_ == NULL) {
        CloseHandle(mutex_);
        throw WinApiError::from_last_error("CreateEventA failed");
    }
}

RecordLocks::~RecordLocks() {
    if (event_ != NULL) {
        CloseHandle(event_);
    }

    if (mutex_ != NULL) {
        CloseHandle(mutex_);
    }
}

void RecordLocks::lock_mutex() {
    DWORD result = WaitForSingleObject(mutex_, INFINITE);

    if (result != WAIT_OBJECT_0) {
        throw WinApiError("WaitForSingleObject mutex failed", GetLastError());
    }
}

void RecordLocks::unlock_mutex() {
    if (!ReleaseMutex(mutex_)) {
        throw WinApiError::from_last_error("ReleaseMutex failed");
    }
}

void RecordLocks::lock_for_read(int employee_id) {
    lock_mutex();

    while (states_[employee_id].writer_active) {
        ResetEvent(event_);
        unlock_mutex();

        WaitForSingleObject(
            event_,
            INFINITE
        );

        lock_mutex();
    }

    states_[employee_id].readers_count++;

    unlock_mutex();
}

void RecordLocks::unlock_for_read(int employee_id) {
    lock_mutex();

    auto& state = states_[employee_id];

    if (state.readers_count > 0) {
        state.readers_count--;
    }

    SetEvent(event_);
    if (!SetEvent(event_)) {
        unlock_mutex();
        throw WinApiError::from_last_error("SetEvent failed");
    }

    unlock_mutex();
}

void RecordLocks::lock_for_write(int employee_id) {
    lock_mutex();

    while (
        states_[employee_id].readers_count > 0 ||
        states_[employee_id].writer_active
        ) {
        ResetEvent(event_);
        unlock_mutex();

        WaitForSingleObject(
            event_,
            INFINITE
        );

        lock_mutex();
    }

    states_[employee_id].writer_active = true;

    unlock_mutex();
}

void RecordLocks::unlock_for_write(int employee_id) {
    lock_mutex();

    states_[employee_id].writer_active = false;

    SetEvent(event_);
    if (!SetEvent(event_)) {
        unlock_mutex();
        throw WinApiError::from_last_error("SetEvent failed");
    }

    unlock_mutex();
}