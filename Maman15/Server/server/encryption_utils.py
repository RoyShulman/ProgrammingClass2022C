from abc import ABC, abstractmethod
import secrets


class AbstractEncryptionUtils(ABC):
    """
    Abstract class to allow testing the server
    with predefined keys.
    This is useless for unit tests where we want
    the keys to not be random
    """
    @staticmethod
    @abstractmethod
    def get_aes_key(size: int) -> bytes:
        raise NotImplementedError


class EncryptionUtils(AbstractEncryptionUtils):
    @staticmethod
    def get_aes_key(size: int) -> bytes:
        return secrets.token_bytes(size)
