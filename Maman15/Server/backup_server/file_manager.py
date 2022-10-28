from pathlib import Path
from uuid import uuid4
from typing import Optional


class FileManager:
    def __init__(self, base_path: Path) -> None:
        self.base_path = base_path

    def get_random_path(self) -> Path:
        """
        Generate a random path in the base directory
        """
        return self.base_path / uuid4().hex

    def store_file(self, content: bytes, path: Optional[Path] = None) -> Path:
        """
        Write the given file content to the given path, and return the path of the created file.
        Since default arguments are evaluated once we can't set the default path to be get_random_path, 
        so we use None instead
        """
        if path is None:
            path = self.get_random_path()
        with path.open("wb") as f:
            f.write(content)
        return path

    def remove_file(self, path: Path):
        """
        Delete the given path
        """
        path.unlink()
