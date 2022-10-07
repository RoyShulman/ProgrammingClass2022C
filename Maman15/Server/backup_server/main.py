import socket
import sys
import logging
from pathlib import Path
from backup_server.server_manager import ServerManager
from backup_server.connection_interface import SocketConnectionInterface
from backup_server.server_model import ServerModel
from backup_server.encryption_utils import EncryptionUtils


def setup_logging():
    # taken from https://docs.python.org/3/library/logging.html
    log_format = "%(asctime)s | %(threadName)s | %(levelname)s | %(name)s: %(message)s"
    logging.basicConfig(level=logging.DEBUG,
                        format=log_format,
                        handlers=[logging.StreamHandler(sys.stdout)])


def run_server(base_storage_path: Path):
    server_socket = socket.socket()
    connection = SocketConnectionInterface(server_socket)
    model = ServerModel()
    encryption_utils = EncryptionUtils()
    with ServerManager(connection, model,
                       encryption_utils, base_storage_path).get_server() as server:
        server.server_requests()


BACKUP_DIRECTORY = Path("/tmp/backupsrv")


def main():
    setup_logging()
    BACKUP_DIRECTORY.mkdir(exist_ok=True)
    run_server(BACKUP_DIRECTORY)


if __name__ == "__main__":
    main()
