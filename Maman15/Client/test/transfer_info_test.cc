#include "Maman15/Client/transfer_info.h"

#include <gtest/gtest.h>

#include "Maman15/Client/util/string_utils.h"

TEST(test_transfer_info, valid_file) {
    client::TransferInfo info{bfs::path("Maman15/Client/test/resources/transfer.info")};
    client::TransferInfo expected{client::util::string_to_endpoint("127.0.0.1:1234"),
                                  "Cool Name",
                                  bfs::path("cooldirectory/coolpath.a")};
    ASSERT_EQ(info, expected);
}

TEST(test_transfer_info, too_short_file) {
    ASSERT_THROW(
        {
            client::TransferInfo info{bfs::path("Maman15/Client/test/resources/too_short_transfer.info")};
        },
        client::InvalidTransferInfoFile);
}
