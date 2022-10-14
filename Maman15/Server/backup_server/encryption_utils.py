import secrets
from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_OAEP, AES
from Crypto.Util.Padding import unpad

from backup_server.abstract_encryption_utils import AbstractEncryptionUtils

"""
Why is this in a different file from abstract_encryption_utils.py as 
opposed to connection_interface.py which has both the 
interface and the implementation in the same file?

In order for us to unit test the server code we create a mock
implementation for AbstractEncryptionUtils. To allow us to test
using CI/CD without installing pycroptodome, we only import 
pycroptodome in this file, and upload abstract_encryption_utils.py
without this file. As opposed to connection_intreface which has no
external dependencies
"""


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
