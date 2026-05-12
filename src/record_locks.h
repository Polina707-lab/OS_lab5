#pragma once

#include <windows.h>
#include <unordered_map>

class RecordLocks {
public:
    RecordLocks();
    ~RecordLocks();

    RecordLocks(const RecordLocks&) = delete;
    RecordLocks& operator=(const RecordLocks&) = delete;

    void lock_for_read(int employee_id);
    void unlock_for_read(int employee_id);

    void lock_for_write(int employee_id);
    void unlock_for_write(int employee_id);

private:
    struct RecordState {
        int readers_count = 0;
        bool writer_active = false;
    };

    HANDLE mutex_;
    HANDLE event_;

    std::unordered_map<int, RecordState> states_;

    void lock_mutex();
    void unlock_mutex();
};