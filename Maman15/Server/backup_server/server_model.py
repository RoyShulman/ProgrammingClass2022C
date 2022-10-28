import sqlite3
import threading
from pathlib import Path
from datetime import datetime
from uuid import UUID
from dataclasses import dataclass
from typing import Any, Tuple, List, Optional
from abc import ABC, abstractmethod
import uuid


class ClientWithSpecifiedUUIDNotFound(Exception):
    pass


class ClientDoesNotExist(Exception):
    pass


@dataclass
class Client:
    uuid: UUID
    name: str
    public_key: bytes
    last_seen: datetime
    aes_key: bytes

    @classmethod
    def from_sql_row(cls, client_uuid: bytes, name: bytes, public_key: bytes, last_seen: bytes, aes_key: bytes):
        return cls(uuid.UUID(bytes=client_uuid), name.decode(), public_key, datetime.fromisoformat(last_seen.decode()), aes_key)

    def to_sql_row(self) -> Tuple[bytes, str, bytes, datetime, bytes]:
        return self.uuid.bytes, self.name, self.public_key, self.last_seen, self.aes_key


class AbstractServerModel(ABC):
    """
    Abstract interface to the database.
    Allows unit testing the server without a database, and to change our database if at some point
    in the future we want to change from mysql to a different one
    """
    @abstractmethod
    def create_tables_if_needed(self) -> None:
        """
        Create the wanted tables in the database if they don't already exist
        """
        raise NotImplementedError

    @abstractmethod
    def get_clients(self) -> List[Client]:
        """
        Get a list of all clients registered in the database
        """
        raise NotImplementedError

    @abstractmethod
    def get_client(self, client_uuid: UUID) -> Client:
        """
        Get a single client by the given uuid
        """
        raise NotImplementedError

    @abstractmethod
    def is_client_registered(self, client_name: str) -> bool:
        """
        Check if the client with the given name is registered
        """
        raise NotImplementedError

    @abstractmethod
    def register_client(self, client: Client) -> None:
        """
        Insert the given client into the database
        """
        raise NotImplementedError

    @abstractmethod
    def update_client_keys(self, client_uuid: UUID, public_key: bytes, aes_key: bytes) -> None:
        """
        Update the aes and public key of the given client by uuid
        """
        raise NotImplementedError

    @abstractmethod
    def store_file(self, client_uuid: UUID, filename: str, path: Path) -> None:
        """
        Insert the file with the given client filename and it's path on the server for the uuid into
        the database
        """
        raise NotImplementedError

    @abstractmethod
    def remove_file(self, client_uuid: UUID, filename: str) -> None:
        """
        Delete a filename for the given client uuid from the database
        """
        raise NotImplementedError

    @abstractmethod
    def get_client_aes_key(self, client_uuid: UUID) -> bytes:
        """
        Get the aes of the given client
        """
        raise NotImplementedError

    @abstractmethod
    def does_client_uuid_exist(self, client_uuid: UUID) -> bool:
        """
        Check if the given uuid is in the database
        """
        raise NotImplementedError

    @abstractmethod
    def set_file_verified(self, client_uuid: UUID, filename: str) -> None:
        """
        Update the filename to verified in the database
        """
        raise NotImplementedError

    @abstractmethod
    def get_file_path(self, client_uuid: UUID, filename: str) -> Path:
        """
        Get the local path of the filename
        """
        raise NotImplementedError

    @abstractmethod
    def update_last_seen_time(self, client_uuid: UUID, last_seen: Optional[datetime] = None) -> None:
        """
        Update the time for the last message we saw from the client
        """
        raise NotImplementedError


class ServerModel(AbstractServerModel):
    CLIENTS_TABLE = "clients"
    FILES_TABLE = "files"
    MAX_FILENAME_SIZE = 255

    def __init__(self, database_path: Path = Path("server.db")) -> None:
        # Allow for multi threaded reading - note we still keep writing serial
        self.conn = sqlite3.connect(database_path, check_same_thread=False)
        self.conn.text_factory = bytes
        self.write_lock = threading.Lock()
        self.create_tables_if_needed()

    def execute_write_query(self, query: str, *args: Tuple[Any, ...]) -> None:
        with self.write_lock:
            self.conn.execute(query, *args)
            self.conn.commit()

    def create_tables_if_needed(self):
        clients_table = f"CREATE TABLE IF NOT EXISTS {self.CLIENTS_TABLE}("\
            f"ID BINARY(16) primary key,"\
            f"Name VARCHAR(127),"\
            f"PublicKey BINARY(160),"\
            f"LastSeen TIMESTAMP,"\
            f"AESKey BINARY(32))"

        # TODO: is the id supposed to be primary key?
        files_table = f"CREATE TABLE IF NOT EXISTS {self.FILES_TABLE}("\
            f"ID BINARY(16),"\
            f"FileName VARCHAR({self.MAX_FILENAME_SIZE}),"\
            f"PathName VARCHAR({self.MAX_FILENAME_SIZE}),"\
            f"Verified BIT"\
            f")"

        self.execute_write_query(clients_table)
        self.execute_write_query(files_table)

    def get_clients(self) -> List[Client]:
        query = f"SELECT * FROM {self.CLIENTS_TABLE}"
        cursor = self.conn.execute(query)
        return [Client.from_sql_row(*row) for row in cursor.fetchall()]

    def register_client(self, client: Client):
        query = f"INSERT INTO {self.CLIENTS_TABLE} VALUES(?, ?, ?, ?, ?)"
        self.execute_write_query(query, client.to_sql_row())

    def is_client_registered(self, client_name: str) -> bool:
        query = f"SELECT * FROM {self.CLIENTS_TABLE} where Name = ?"
        cursor = self.conn.execute(query, (client_name, ))
        if cursor.fetchall():
            return True
        return False

    def get_client(self, client_uuid: UUID) -> Client:
        query = f'SELECT * FROM {self.CLIENTS_TABLE} WHERE ID = ?"'
        cursor = self.conn.execute(query, (client_uuid.bytes, ))
        found_clients = [Client.from_sql_row(*cursor.fetchall()[0])]
        if not found_clients:
            raise ClientWithSpecifiedUUIDNotFound(client_uuid)
        return found_clients[0]

    def update_client_keys(self, client_uuid: UUID, public_key: bytes, aes_key: bytes) -> None:
        query = f'UPDATE {self.CLIENTS_TABLE} SET Publickey = ?, AESKey = ? WHERE ID = ?'
        self.execute_write_query(query,
                                 ("public_key", aes_key, client_uuid.bytes))

    def store_file(self, client_uuid: UUID, filename: str, path: Path) -> None:
        query = f"INSERT INTO {self.FILES_TABLE} VALUES(?, ?, ?, ?)"
        # File are initially unverified
        self.execute_write_query(
            query, (client_uuid.bytes, filename, str(path), False))

    def remove_file(self, client_uuid: UUID, filename: str) -> None:
        query = f"DELETE FROM {self.FILES_TABLE} WHERE ID = ? AND FileName = ?"
        self.execute_write_query(query, (client_uuid.bytes, filename))

    def get_client_aes_key(self, client_uuid: UUID) -> bytes:
        query = f'SELECT AESKey FROM {self.CLIENTS_TABLE} WHERE ID = ?'
        cursor = self.conn.execute(query, (client_uuid.bytes, ))
        found = cursor.fetchall()
        if not found:
            raise ClientDoesNotExist(client_uuid)
        return found[0][0]

    def does_client_uuid_exist(self, client_uuid: UUID) -> bool:
        query = f"SELECT * FROM {self.CLIENTS_TABLE} where ID = ?"
        cursor = self.conn.execute(query, (client_uuid.bytes, ))
        if cursor.fetchall():
            return True
        return False

    def set_file_verified(self, client_uuid: UUID, filename: str) -> None:
        query = f'UPDATE {self.FILES_TABLE} SET Verified = 1 WHERE ID = ? AND FileName = ?'
        self.execute_write_query(query, (client_uuid.bytes, filename))

    def get_file_path(self, client_uuid: UUID, filename: str) -> Path:
        query = f'SELECT PathName FROM {self.FILES_TABLE} WHERE ID = ? AND FileName = ?'
        cursor = self.conn.execute(query, (client_uuid.bytes, filename))
        return Path(cursor.fetchall()[0][0].decode())

    def update_last_seen_time(self, client_uuid: UUID, last_seen: Optional[datetime] = None) -> None:
        if not last_seen:
            last_seen = datetime.now()
        query = f'UPDATE {self.CLIENTS_TABLE} SET LastSeen = ? WHERE ID = ?'
        self.execute_write_query(query, (last_seen, client_uuid.bytes))
