import secrets
from abc import ABC, abstractmethod
from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_OAEP, AES
from Crypto.Util.Padding import unpad


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

    @staticmethod
    @abstractmethod
    def rsa_encrypt(plain: bytes, public_key: bytes) -> bytes:
        raise NotImplementedError

    @staticmethod
    @abstractmethod
    def aes_decrypt(cipher: bytes, aes_key: bytes) -> bytes:
        raise NotImplementedError


class EncryptionUtils(AbstractEncryptionUtils):
    @staticmethod
    def get_aes_key(size: int) -> bytes:
        return secrets.token_bytes(size)

    @staticmethod
    def rsa_encrypt(plain: bytes, public_key: bytes) -> bytes:
        # taken from https://pycryptodome.readthedocs.io/en/latest/src/examples.html#encrypt-data-with-rsa
        recipient_key = RSA.import_key(public_key)
        cipher_rsa = PKCS1_OAEP.new(recipient_key)
        return cipher_rsa.encrypt(plain)

    @staticmethod
    def aes_decrypt(cipher: bytes, aes_key: bytes) -> bytes:
        # taken from https://pycryptodome.readthedocs.io/en/latest/src/examples.html#encrypt-data-with-rsa
        cipher_aes = AES.new(key=aes_key, mode=AES.MODE_CBC, iv=b"\x00"*16)
        return unpad(cipher_aes.decrypt(cipher), cipher_aes.block_size)
