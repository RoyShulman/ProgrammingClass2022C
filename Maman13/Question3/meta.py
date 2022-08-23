from dataclasses import dataclass
from pathlib import Path
from importlib import import_module
from typing import Any, Callable, List
from functools import wraps
import ast
import inspect


@dataclass
class Method:
    name: str
    fn: Any


class ModuleInspector:
    @staticmethod
    def get_classes(filename: Path) -> List[Any]:
        """
        Get all classes in a given filename
        """
        module = import_module(filename.stem)
        return [obj for _, obj in inspect.getmembers(module, inspect.isclass)]

    @staticmethod
    def get_methods(obj: Any) -> List[Method]:
        """
        Get all methods for the given obj
        """
        return [Method(name, fn) for name, fn in inspect.getmembers(obj, inspect.isfunction)]

    @staticmethod
    def wrap_methods(obj: Any, methods: List[Method], code: str) -> None:
        for method in methods:
            setattr(obj, method.name, method_wrapper(method.fn, code))

    @staticmethod
    def get_method_params(obj: Any, method: Any) -> List:
        """
        Return a list of correct arguments for the given method.
        We set self to the obj, and None to all the others
        """
        args = []
        params = inspect.signature(method).parameters
        for param in params.keys():
            if param == 'self':
                args.append(obj)
            else:
                args.append(None)
        return args

    @staticmethod
    def run_methods(obj: Any, methods: List[Method]) -> None:
        """
        Run the given method
        """
        for m in methods:
            args = ModuleInspector.get_method_params(obj, m.fn)
            getattr(obj, m.name)(*args)


def method_wrapper(original_method: Callable[..., Any], added_code: str):
    """
    Wrap the method with the added code.
    """
    @wraps(original_method)
    def with_added_code(self, *args, **kwargs):
        eval(added_code)
        return original_method(self, *args, **kwargs)
    return with_added_code


@dataclass(frozen=True)
class CodeToAdd:
    filename: Path
    code: str

    def is_valid(self) -> bool:
        """
        Validate the filename exists, and the code is valid python code
        """
        if not self.filename.exists():
            return False
        try:
            ast.parse(self.code)
        except SyntaxError:
            return False
        return True

    @classmethod
    def get(cls) -> 'CodeToAdd':
        filename = input("Enter python file name: ")
        code = input("Enter a python code: ")
        return cls(Path(filename), code)

    def add_code_to_file(self) -> None:
        """
        Add code to the file. We get the classes from the file, 
        then we wrap the methods with the added code and finally 
        call the function with the correct number of arguments
        """
        inspector = ModuleInspector()
        classes: List[Any] = inspector.get_classes(self.filename)
        for c in classes:
            methods = inspector.get_methods(c)
            inspector.wrap_methods(c, methods, self.code)
            inspector.run_methods(c, methods)


def main():
    to_add = CodeToAdd.get()
    try:
        if not to_add.is_valid():
            print("Invalid arguments received")
            return

        to_add.add_code_to_file()
    except Exception as e:
        print("Got Exception!")
        print(repr(e))


if __name__ == "__main__":
    main()
