from pathlib import Path


class InvalidPortInfoFile(Exception):
    pass


class PortInfoFileNotFound(Exception):
    pass


class PortInfo:
    """
    The port the server should listen on
    """

    def __init__(self, port: int) -> None:
        self.port = port

    @classmethod
    def from_file(cls, filepath: Path = Path("port.info")):
        """
        Read the port from a given filepath
        """
        if not filepath.exists():
            raise PortInfoFileNotFound(filepath)
        try:
            with open(filepath) as f:
                port = int(f.read())
            return cls(port)
        except Exception as e:
            raise InvalidPortInfoFile from e
