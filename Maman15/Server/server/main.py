from pathlib import Path
import socket
import tempfile
from server.server_manager import ServerManager
from server.connection_interface import SocketConnectionInterface
from server.server_model import ServerModel
from server.encryption_utils import EncryptionUtils


def main():
    server_socket = socket.socket()
    connection = SocketConnectionInterface(server_socket)
    model = ServerModel()
    encryption_utils = EncryptionUtils()
    # Just for this program, in real life application we don't want to delete the user files
    # but I don't want the files to be kept forever
    with tempfile.TemporaryDirectory() as tmpdir:
        server = ServerManager(connection, model,
                               encryption_utils, Path(tmpdir)).get_server()
        server.server_requests()


if __name__ == "__main__":
    main()
