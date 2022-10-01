"""
Contains nice utils functions for tests
"""

def py_server_test(name, **kwargs):
    native.py_test(
        name = name,
        imports = ["../"],
        **kwargs
    )
