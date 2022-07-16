from dataclasses import dataclass
from typing import List
from pathlib import Path


class ReadingBackupInfoFileException(Exception):
    pass


@dataclass(frozen=True)
class BackupInfo:
    files: List[Path]

    FILENAME = "backup.info"

    @classmethod
    def from_file(cls):
        """
        Create a BackupInfo from the default filename
        """
        try:
            with open(cls.FILENAME) as f:
                return cls([Path(p.strip()) for p in f.readlines()])
        except Exception as e:
            raise ReadingBackupInfoFileException(str(e))
