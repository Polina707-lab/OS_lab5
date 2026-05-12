#include <gtest/gtest.h>

#include "record_locks.h"

#include <windows.h>

LONG read_long(volatile LONG* value) {
    return InterlockedCompareExchange(
        const_cast<LONG*>(value),
        0,
        0
    );
}

struct ReaderData {
    RecordLocks* locks;
    int employee_id;
    volatile LONG* readers_inside;
    HANDLE entered_event;
};

struct WriterData {
    RecordLocks* locks;
    int employee_id;
    volatile LONG* writer_entered;
    HANDLE entered_event;
};

DWORD WINAPI reader_thread(LPVOID parameter) {
    ReaderData* data = static_cast<ReaderData*>(parameter);

    data->locks->lock_for_read(data->employee_id);

    InterlockedIncrement(const_cast<LONG*>(data->readers_inside));
    SetEvent(data->entered_event);

    Sleep(100);

    data->locks->unlock_for_read(data->employee_id);

    return 0;
}

DWORD WINAPI writer_thread(LPVOID parameter) {
    WriterData* data = static_cast<WriterData*>(parameter);

    data->locks->lock_for_write(data->employee_id);

    InterlockedExchange(const_cast<LONG*>(data->writer_entered), 1);
    SetEvent(data->entered_event);

    data->locks->unlock_for_write(data->employee_id);

    return 0;
}

TEST(RecordLocksTests, SeveralReadersCanReadSameRecord) {
    RecordLocks locks;

    volatile LONG readers_inside = 0;

    HANDLE event1 = CreateEventA(NULL, TRUE, FALSE, NULL);
    HANDLE event2 = CreateEventA(NULL, TRUE, FALSE, NULL);

    ASSERT_TRUE(event1 != NULL);
    ASSERT_TRUE(event2 != NULL);

    ReaderData data1{ &locks, 1, &readers_inside, event1 };
    ReaderData data2{ &locks, 1, &readers_inside, event2 };

    DWORD thread_id1;
    DWORD thread_id2;

    HANDLE thread1 = CreateThread(NULL, 0, reader_thread, &data1, 0, &thread_id1);
    HANDLE thread2 = CreateThread(NULL, 0, reader_thread, &data2, 0, &thread_id2);

    ASSERT_TRUE(thread1 != NULL);
    ASSERT_TRUE(thread2 != NULL);

    HANDLE events[] = { event1, event2 };

    DWORD wait_result = WaitForMultipleObjects(2, events, TRUE, 1000);

    EXPECT_EQ(wait_result, WAIT_OBJECT_0);
    EXPECT_EQ(read_long(&readers_inside), 2);

    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);

    CloseHandle(thread1);
    CloseHandle(thread2);
    CloseHandle(event1);
    CloseHandle(event2);
}

TEST(RecordLocksTests, WriterWaitsWhileReaderIsActive) {
    RecordLocks locks;

    volatile LONG writer_entered = 0;

    HANDLE entered_event = CreateEventA(NULL, TRUE, FALSE, NULL);
    ASSERT_TRUE(entered_event != NULL);

    locks.lock_for_read(1);

    WriterData data{ &locks, 1, &writer_entered, entered_event };

    DWORD thread_id;

    HANDLE thread = CreateThread(NULL, 0, writer_thread, &data, 0, &thread_id);
    ASSERT_TRUE(thread != NULL);

    DWORD wait_result = WaitForSingleObject(entered_event, 100);

    EXPECT_EQ(wait_result, WAIT_TIMEOUT);
    EXPECT_EQ(read_long(&writer_entered), 0);

    locks.unlock_for_read(1);

    wait_result = WaitForSingleObject(entered_event, 1000);

    EXPECT_EQ(wait_result, WAIT_OBJECT_0);
    EXPECT_EQ(read_long(&writer_entered), 1);

    WaitForSingleObject(thread, INFINITE);

    CloseHandle(thread);
    CloseHandle(entered_event);
}

TEST(RecordLocksTests, ReaderWaitsWhileWriterIsActive) {
    RecordLocks locks;

    volatile LONG readers_inside = 0;

    HANDLE entered_event = CreateEventA(NULL, TRUE, FALSE, NULL);
    ASSERT_TRUE(entered_event != NULL);

    locks.lock_for_write(1);

    ReaderData data{ &locks, 1, &readers_inside, entered_event };

    DWORD thread_id;

    HANDLE thread = CreateThread(NULL, 0, reader_thread, &data, 0, &thread_id);
    ASSERT_TRUE(thread != NULL);

    DWORD wait_result = WaitForSingleObject(entered_event, 100);

    EXPECT_EQ(wait_result, WAIT_TIMEOUT);
    EXPECT_EQ(read_long(&readers_inside), 0);

    locks.unlock_for_write(1);

    wait_result = WaitForSingleObject(entered_event, 1000);

    EXPECT_EQ(wait_result, WAIT_OBJECT_0);
    EXPECT_EQ(read_long(&readers_inside), 1);

    WaitForSingleObject(thread, INFINITE);

    CloseHandle(thread);
    CloseHandle(entered_event);
}

TEST(RecordLocksTests, DifferentRecordsDoNotBlockEachOther) {
    RecordLocks locks;

    volatile LONG readers_inside = 0;

    HANDLE entered_event = CreateEventA(NULL, TRUE, FALSE, NULL);
    ASSERT_TRUE(entered_event != NULL);

    locks.lock_for_write(1);

    ReaderData data{ &locks, 2, &readers_inside, entered_event };

    DWORD thread_id;

    HANDLE thread = CreateThread(NULL, 0, reader_thread, &data, 0, &thread_id);
    ASSERT_TRUE(thread != NULL);

    DWORD wait_result = WaitForSingleObject(entered_event, 1000);

    EXPECT_EQ(wait_result, WAIT_OBJECT_0);
    EXPECT_EQ(read_long(&readers_inside), 1);

    locks.unlock_for_write(1);

    WaitForSingleObject(thread, INFINITE);

    CloseHandle(thread);
    CloseHandle(entered_event);
}