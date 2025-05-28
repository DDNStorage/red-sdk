/******************************************************************************
 *
 * @file
 * @copyright
 *                               --- WARNING ---
 *
 *     This work contains trade secrets of DataDirect Networks, Inc.  Any
 *     unauthorized use or disclosure of the work, or any part thereof, is
 *     strictly prohibited. Any use of this work without an express license
 *     or permission is in violation of applicable laws.
 *
 * @copyright DataDirect Networks, Inc. CONFIDENTIAL AND PROPRIETARY
 * @copyright DataDirect Networks Copyright, Inc. (c) 2021-2025. All rights reserved.
 *
 * @section DESCRIPTION
 *
 *   Name:       basic_test.cpp
 *   Project:    RED
 *
 *   Description: Basic unit tests for RED S3 client implementation
 *
 *   Created:    3/20/2025
 *   Author(s):  Bryant Ly (bly@ddn.com)
 *
 ******************************************************************************/
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../../examples/cpp/simple_s3/simple_s3_client.hpp"
#include "mock_red_client.hpp"
#include "test_utils.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrEq;

class RfsBasicTest : public TestBase
{
protected:
    void SetUp() override
    {
        TestBase::SetUp(); /* Call base class setup */
        mock_client = new MockRedClient();
        client      = std::make_unique<s3client>(nullptr,
                                                 std::unique_ptr<IRedClient>(mock_client));
    }

    void TearDown() override
    {
        client.reset();
        TestBase::TearDown(); /* Call base class teardown */
    }

    MockRedClient            *mock_client;
    std::unique_ptr<s3client> client;
};

TEST_F(RfsBasicTest, CreateBucket)
{
    SetTestCategory(TestCategory::UNIT);
    SCOPED_TRACE("Testing bucket creation"); /* Adds context to failure messages */

    rfs_dataset_hndl_t ds_hndl = {reinterpret_cast<void *>(1)};

    EXPECT_CALL(*mock_client,
                obtain_dataset(StrEq("test_bucket"), StrEq("infinia"), _, _, _))
        .WillOnce(DoAll(SetArgPointee<3>(ds_hndl), Return(RED_SUCCESS)));

    EXPECT_CALL(*mock_client, close_dataset(ds_hndl, _)).WillOnce(Return(RED_SUCCESS));

    auto bucket = client->create_bucket("infinia", "test_bucket");
    ASSERT_FALSE(bucket.expired())
        << "Bucket creation failed - returned expired weak_ptr";
}

TEST_F(RfsBasicTest, PutObject)
{
    SetTestCategory(TestCategory::INTEGRATION);
    SCOPED_TRACE("Testing object creation and data writing");

    /* Setup test data */
    rfs_dataset_hndl_t ds_hndl       = {reinterpret_cast<void *>(1)};
    rfs_open_hndl_t    root_oh       = {2};
    rfs_open_hndl_t    obj_oh        = {3};
    const char        *data          = "test data";
    size_t             data_len      = strlen(data);
    ssize_t            bytes_written = data_len;

    /* Create bucket expectations */
    EXPECT_CALL(*mock_client,
                obtain_dataset(StrEq("test_bucket"), StrEq("infinia"), _, _, _))
        .WillOnce(DoAll(SetArgPointee<3>(ds_hndl), Return(RED_SUCCESS)));

    auto bucket = client->create_bucket("infinia", "test_bucket");
    ASSERT_FALSE(bucket.expired()) << "Bucket creation failed";

    /* Setup object creation and write expectations */
    {
        testing::InSequence seq; /* Enforce order of operations */

        EXPECT_CALL(*mock_client, open_root(ds_hndl, _, _))
            .WillOnce(DoAll(SetArgPointee<1>(root_oh), Return(RED_SUCCESS)));

        EXPECT_CALL(*mock_client, openat(root_oh, StrEq("test_object"), _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<4>(obj_oh), Return(RED_SUCCESS)));

        EXPECT_CALL(*mock_client, pwrite(obj_oh, _, data_len, 0, _, _))
            .WillOnce(DoAll(SetArgPointee<4>(bytes_written), Return(RED_SUCCESS)));

        EXPECT_CALL(*mock_client, close(obj_oh, _)).WillOnce(Return(RED_SUCCESS));

        EXPECT_CALL(*mock_client, close(root_oh, _)).WillOnce(Return(RED_SUCCESS));

        EXPECT_CALL(*mock_client, close_dataset(ds_hndl, _))
            .WillOnce(Return(RED_SUCCESS));
    }

    auto status = client->put_object(bucket, "test_object", (void *)data, data_len);
    ASSERT_EQ(status, RED_SUCCESS)
        << "Failed to put object - status: " << red_strerror(status);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    /* Add our custom test listener */
    auto &listeners = testing::UnitTest::GetInstance()->listeners();
    delete listeners.Release(listeners.default_result_printer());
    listeners.Append(new TestEventListener);

    return RUN_ALL_TESTS();
}
