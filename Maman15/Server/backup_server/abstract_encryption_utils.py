from abc import ABC, abstractmethod


class AbstractEncryptionUtils(ABC):
    """
    Abstract class to allow testing the server
    with predefined keys.
    This is useful for unit tests where we want
    the keys to not be random
    """
    @staticmethod
    @abstractmethod
    def get_aes_key(size: int) -> bytes:
        raise NotImplementedError

    @staticmethod
    @abstractmethod
    def rsa_encrypt(plain: bytes, public_key: bytes) -> bytes:
        raise NotImplementedError

    @staticmethod
    @abstractmethod
    def aes_decrypt(cipher: bytes, aes_key: bytes) -> bytes:
        raise NotImplementedError
