#include <gtest/gtest.h>

#include "protocol.h"

TEST(ProtocolTests, ClientRequestStoresReadRequest) {
    ClientRequest request{};

    request.type = RequestType::ReadRecord;
    request.employee_id = 5;

    EXPECT_EQ(request.type, RequestType::ReadRecord);
    EXPECT_EQ(request.employee_id, 5);
}

TEST(ProtocolTests, ClientRequestStoresReleaseReadRequest) {
    ClientRequest request{};

    request.type = RequestType::ReleaseReadRecord;
    request.employee_id = 7;

    EXPECT_EQ(request.type, RequestType::ReleaseReadRecord);
    EXPECT_EQ(request.employee_id, 7);
}

TEST(ProtocolTests, ClientRequestStoresReleaseWriteRequest) {
    ClientRequest request{};

    request.type = RequestType::ReleaseWriteRecord;
    request.employee_id = 8;

    EXPECT_EQ(request.type, RequestType::ReleaseWriteRecord);
    EXPECT_EQ(request.employee_id, 8);
}

TEST(ProtocolTests, ServerResponseStoresEmployeeData) {
    ServerResponse response{};

    response.status = ResponseStatus::Success;
    response.data.num = 10;
    response.data.hours = 35.5;

    EXPECT_EQ(response.status, ResponseStatus::Success);
    EXPECT_EQ(response.data.num, 10);
    EXPECT_DOUBLE_EQ(response.data.hours, 35.5);
}

TEST(ProtocolTests, ClientRequestStoresEmployeeDataForUpdate) {
    ClientRequest request{};

    request.type = RequestType::UpdateRecord;
    request.employee_id = 3;
    request.data.num = 3;
    request.data.hours = 40.0;

    EXPECT_EQ(request.type, RequestType::UpdateRecord);
    EXPECT_EQ(request.employee_id, 3);
    EXPECT_EQ(request.data.num, 3);
    EXPECT_DOUBLE_EQ(request.data.hours, 40.0);
}