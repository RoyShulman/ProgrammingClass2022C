"""
Contains nice utils functions for tests
"""

def backup_client_test(name, **kwargs):
    native.py_test(
        name = name,
        imports = ["../"],
        **kwargs
    )
