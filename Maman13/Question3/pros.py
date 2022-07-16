from dataclasses import dataclass
from typing import Any
import sys
import inspect


@dataclass
class User:
    name: str
    profession: str


class Engineer(User):
    PROFESSION = "Engineer"

    def __init__(self, name: str):
        super().__init__(name, self.PROFESSION)


class Technician(User):
    PROFESSION = "Technician"

    def __init__(self, name: str) -> None:
        return super().__init__(name, self.PROFESSION)


class Hairdresser(User):
    PROFESSION = "Hairdresser"

    def __init__(self, name: str) -> None:
        return super().__init__(name, self.PROFESSION)


class Politician(User):
    PROFESSION = "Politician"

    def __init__(self, name: str) -> None:
        return super().__init__(name, self.PROFESSION)


class ElectricalEngineer(Engineer):
    pass


class ComputerEngineer(Engineer):
    pass


class MechanichalEngineer(Engineer):
    pass


class BaseClassNotFoundException(Exception):
    pass


class WantedBaseClassIsNotAClassException(Exception):
    pass


class ClassAlreadyExistsException(Exception):
    pass


class InvalidNameException(Exception):
    pass


@dataclass(frozen=True)
class CustomClassDefs:
    name: str
    base_cls: str
    method: str
    attribute: str


def get_new_class(defs: CustomClassDefs) -> Any:
    if defs.base_cls:
        try:
            wanted_class = getattr(sys.modules[__name__], defs.base_cls)
            if not inspect.isclass(wanted_class):
                raise WantedBaseClassIsNotAClassException(defs.base_cls)
            base_class = tuple([wanted_class])
        except AttributeError:
            raise BaseClassNotFoundException(defs.base_cls)
    else:
        base_class = ()

    try:
        getattr(sys.modules[__name__], defs.name)
        raise ClassAlreadyExistsException(defs.name)
    except AttributeError:
        # This is good since it means the new class doesn't exist
        pass

    for n in [defs.name, defs.attribute, defs.method]:
        if not n:
            raise InvalidNameException(n)

    # No need to check that the base and the new class aren't the same because
    # then it means one of them already exists and we fail with ClassAlreadyExistsException,
    # or they both don't exist and then we fail with BaseClassNotFoundException

    print(f"Class {defs.name} created with base class: {defs.base_cls}")
    return type(defs.name, base_class, {defs.attribute: None,
                                        defs.method: lambda: print(f"running function {defs.method}")})


def get_custom_class_defs() -> CustomClassDefs:
    name = input("Please enter the name of new class: ")
    base_cls = input("Please enter name of base class (blank if none): ")
    method = input(f"Please enter name of new method for class {name}: ")
    attribute = input(f"Please enter name of new attribute for class {name}: ")
    return CustomClassDefs(name, base_cls, method, attribute)


def print_new_cls(cls: Any):
    print(f"Class __name__ is: {cls.__name__}")
    print(f"Class __dict__ is: {cls.__dict__}")


def main():
    cls_defs = get_custom_class_defs()
    try:
        new_cls = get_new_class(cls_defs)
        print_new_cls(new_cls)
    except Exception as e:
        print(repr(e))


if __name__ == "__main__":
    main()
