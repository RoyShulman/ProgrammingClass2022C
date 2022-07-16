import multiprocessing
from pathlib import Path
from backup_client.client import get_client_from_file, ErrorResponseException, UnexpectedResponseException
from backup_client.connection_manager import NetworkConnectionManager
from backup_client.backup_info import BackupInfo


class ClientFlowException(Exception):
    pass


def main():
    try:
        backup_info = BackupInfo.from_file(Path(__file__).parent)
        with get_client_from_file(NetworkConnectionManager()) as client:
            try:
                backup_files = client.get_available_backup_files()
                # We don't want to get here
                raise ClientFlowException("There should be no files yet")
            except ErrorResponseException as e:
                print(f"List files result: {str(e)}")

            backup_name1 = client.backup_file(backup_info.files[0])
            print(f"Successful backup of {backup_name1}")

            backup_name2 = client.backup_file(backup_info.files[1])
            print(f"Successful backup of {backup_name2}")

            backup_files = client.get_available_backup_files()
            print(f"Current backup files: {backup_files}")
            assert backup_files == [f.name for f in backup_info.files]

            restore_result = client.restore_file(backup_name1)
            print(f"Restore payoad: {restore_result}")
            with open(backup_info.files[0], "rb") as f:
                assert restore_result == f.read(), restore_result

            client.delete_file(backup_name1)

            try:
                client.restore_file(backup_name1)
                # We don't want to get here
                raise ClientFlowException("The file shouldn't exist")
            except ErrorResponseException as e:
                print(f"Last restore result: {str(e)}")

    except ErrorResponseException as e:
        print(f"Get an error response: {str(e)}")
    except UnexpectedResponseException as e:
        print(f"Unexpected response: {str(e)}")
    except Exception as e:
        print(f"Unknown exception: {str(e)}")
        raise


if __name__ == "__main__":
    p1 = multiprocessing.Process(target=main)
    p2 = multiprocessing.Process(target=main)
    p3 = multiprocessing.Process(target=main)
    p4 = multiprocessing.Process(target=main)
    p1.start()
    p2.start()
    p3.start()
    p4.start()
