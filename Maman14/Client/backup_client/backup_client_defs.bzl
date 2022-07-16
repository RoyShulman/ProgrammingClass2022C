"""
This is the same as test_defs.bzl, feels a bit like duplication but I hope it's ok?
"""

def backup_client_binary(name, **kwargs):
    native.py_binary(
        name = name,
        imports = [".."],
        **kwargs
    )
