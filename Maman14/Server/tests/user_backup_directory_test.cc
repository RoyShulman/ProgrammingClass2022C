#include "Maman14/Server/user_backup_directory.h"

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace bfs = boost::filesystem;
using std::string;
using std::vector;

class UserBackupDirectoryTest : public ::testing::Test {
protected:
    UserBackupDirectoryTest() : filename("coolfile"), filename2("supercoolfile.jpeg") {
        directory = bfs::temp_directory_path();
    }

    void SetUp() override {
        bfs::remove_all(directory / filename);
        bfs::remove_all(directory / filename2);
    }

    void TearDown() override {
        bfs::remove_all(directory / filename);
        bfs::remove_all(directory / filename2);
    }

public:
    string filename;
    string filename2;
    bfs::path directory;
};

static vector<uint8_t> read_file(bfs::path file) {
    std::ifstream ifs(file.string());
    vector<unsigned char> buffer(std::istreambuf_iterator<char>(ifs), {});
    return buffer;
}

static vector<uint8_t> get_payload() {
    string s("coolpayload");
    vector<uint8_t> payload(s.begin(), s.end());
    return payload;
}

TEST_F(UserBackupDirectoryTest, test_file_created) {
    UserBackupDirectory backup_directory(directory);
    vector<uint8_t> payload = get_payload();

    backup_directory.backup_file(filename, payload);

    ASSERT_EQ(payload, read_file(directory / filename));
}

TEST_F(UserBackupDirectoryTest, test_backup_twice_throws) {
    UserBackupDirectory backup_directory(directory);
    vector<uint8_t> payload = get_payload();

    backup_directory.backup_file(filename, payload);

    EXPECT_THROW(
        {
            try {
                backup_directory.backup_file(filename, payload);
            } catch (const FileAlreadyExistsException& e) {
                string message = "Filename: " + (directory / filename).string() + " already exists";
                EXPECT_STREQ(message.c_str(), e.what());
                EXPECT_EQ(directory / filename, e.get_full_path());
                EXPECT_EQ(filename, e.get_filename());
                throw;
            }
        },
        FileAlreadyExistsException);
}

TEST_F(UserBackupDirectoryTest, test_get_filenames) {
    UserBackupDirectory backup_directory(directory);
    backup_directory.backup_file(filename, get_payload());
    backup_directory.backup_file(filename2, get_payload());

    vector<string> expected{"coolfile", "supercoolfile.jpeg"};
    ASSERT_EQ(expected, backup_directory.get_backup_filenames());
}

TEST_F(UserBackupDirectoryTest, test_get_backup_file_content) {
    UserBackupDirectory backup_directory(directory);
    vector<uint8_t> payload = get_payload();
    backup_directory.backup_file(filename, payload);

    ASSERT_EQ(payload, backup_directory.get_backup_file_content(filename));
}

TEST_F(UserBackupDirectoryTest, test_file_not_found) {
    UserBackupDirectory backup_directory(directory);

    EXPECT_THROW(
        {
            try {
                backup_directory.get_backup_file_content(filename);
            } catch (const FileNotFoundException& e) {
                string message = "Filename: " + (directory / filename).string() + " not found";
                EXPECT_STREQ(message.c_str(), e.what());
                EXPECT_EQ(directory / filename, e.get_full_path());
                EXPECT_EQ(filename, e.get_filename());
                throw;
            }
        },
        FileNotFoundException);
}
