import unittest
import os
from pathlib import Path

from backup_client.backup_info import BackupInfo, ReadingBackupInfoFileException


class BackupInfoTest(unittest.TestCase):
    def setUp(self) -> None:
        try:
            os.unlink("backup.info")
        except FileNotFoundError:
            pass

    def tearDown(self) -> None:
        try:
            os.unlink("backup.info")
        except FileNotFoundError:
            pass

    def test_from_file(self):
        with open("backup.info", "w") as f:
            f.write("backup_info_test.py\n")
        backup = BackupInfo.from_file(Path("."))
        self.assertEqual(1, len(backup.files))
        self.assertEqual(Path("backup_info_test.py"), backup.files[0])

    def test_non_existent_file(self):
        with self.assertRaises(ReadingBackupInfoFileException):
            BackupInfo.from_file(Path("."))


if __name__ == "__main__":
    unittest.main()
