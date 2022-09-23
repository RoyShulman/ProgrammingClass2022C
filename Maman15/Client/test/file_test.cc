#include "Maman15/Client/util/file.h"

#include <gtest/gtest.h>

TEST(file_test, test_lines) {
    bfs::path path{"Maman15/Client/test/resources/file_with_lines.txt"};
    client::util::File f{path};
    vector<string> lines = f.read_lines();
    ASSERT_EQ(3, lines.size());
    for (size_t i = 0; i < 3; i++) {
        ASSERT_EQ(std::to_string(i), lines[i]);
    }
}

TEST(file_test, file_not_found) {
    bfs::path path{"stuff.txt"};
    ASSERT_THROW(
        {
            client::util::File f{path};
        },
        client::util::FileNotFoundException);
}
