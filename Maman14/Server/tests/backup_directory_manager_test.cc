#include "Maman14/Server/backup_directory_manager.h"

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace bfs = boost::filesystem;
using std::string;
using std::vector;

class BackupDirectoryManagerTest : public ::testing::Test {
protected:
    BackupDirectoryManagerTest() : filename("coolfile"), filename2("supercoolfile.jpeg"), user_id(123) {
        directory = bfs::temp_directory_path();
    }

    void SetUp() override {
        bfs::remove_all(directory / std::to_string(user_id));
    }

    void TearDown() override {
        bfs::remove_all(directory / std::to_string(user_id));
    }

public:
    string filename;
    string filename2;
    bfs::path directory;
    user_id_t user_id;
};

static vector<uint8_t> get_payload() {
    string s("coolpayload");
    vector<uint8_t> payload(s.begin(), s.end());
    return payload;
}

TEST_F(BackupDirectoryManagerTest, test_directory_created_for_user) {
    BackupDirectoryManager manager;
    manager.backup_file_for_user_id(user_id, filename, get_payload());
    ASSERT_TRUE(bfs::exists(bfs::temp_directory_path() / std::to_string(user_id)));
    ASSERT_TRUE(bfs::is_directory(bfs::temp_directory_path() / std::to_string(user_id)));
}

TEST_F(BackupDirectoryManagerTest, test_num_backup_directories) {
    BackupDirectoryManager manager;
    vector<uint8_t> payload = get_payload();
    manager.backup_file_for_user_id(user_id, filename, payload);

    ASSERT_EQ(1, manager.get_num_backup_directories());
}

TEST_F(BackupDirectoryManagerTest, test_get_backup_filenames_for_user) {
    BackupDirectoryManager manager;
    vector<uint8_t> payload = get_payload();
    manager.backup_file_for_user_id(user_id, filename, payload);
    manager.backup_file_for_user_id(user_id, filename2, payload);
    vector<string> filenames{filename, filename2};

    ASSERT_EQ(filenames, manager.get_backup_filenames_for_user(user_id));
}

TEST_F(BackupDirectoryManagerTest, test_get_file_content_for_user) {
    BackupDirectoryManager manager;
    vector<uint8_t> payload = get_payload();
    manager.backup_file_for_user_id(user_id, filename, payload);

    ASSERT_EQ(payload, manager.get_file_content_for_user(user_id, filename));
}
