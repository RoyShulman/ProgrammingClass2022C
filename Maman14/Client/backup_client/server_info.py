from dataclasses import dataclass
from ipaddress import IPv4Address
from typing import Tuple


class InvalidServerInfo(Exception):
    pass


class ReadingServerInfoFileException(Exception):
    pass


@dataclass(frozen=True)
class ServerInfo:
    ip: IPv4Address
    port: int

    FILENAME = "server.info"

    @classmethod
    def from_file(cls) -> 'ServerInfo':
        """
        Create a ServerInfo from the default file path
        """
        try:
            with open(cls.FILENAME) as f:
                address = f.read()
        except Exception as e:
            raise ReadingServerInfoFileException(str(e))
        return cls.from_address(address)

    @classmethod
    def from_address(cls, address: str) -> 'ServerInfo':
        """
        Create a ServerInfo from the given address. 
        address is expected to be of format ip:port
        """
        try:
            ip, port = address.split(":")
            return cls(IPv4Address(ip), int(port))
        except Exception as e:
            raise InvalidServerInfo(str(e))


    def to_raw(self) -> Tuple[str, int]:
        return str(self.ip), self.port
