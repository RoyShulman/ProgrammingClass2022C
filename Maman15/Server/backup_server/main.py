import socket
import tempfile
from pathlib import Path
from backup_server.server_manager import ServerManager
from backup_server.connection_interface import SocketConnectionInterface
from backup_server.server_model import ServerModel
from backup_server.encryption_utils import EncryptionUtils


def main():
    server_socket = socket.socket()
    connection = SocketConnectionInterface(server_socket)
    model = ServerModel()
    encryption_utils = EncryptionUtils()
    # Just for this program, in real life application we don't want to delete the user files
    # but I don't want the files to be kept forever
    with tempfile.TemporaryDirectory() as tmpdir:
        with ServerManager(connection, model,
                           encryption_utils, Path(tmpdir)).get_server() as server:
            server.server_requests()


if __name__ == "__main__":
    main()
