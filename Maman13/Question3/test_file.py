class Test:
    def __init__(self) -> None:
        print("This is __init__")
        self.a = 5

    def method(self):
        print("This is method")

    def method2(self, b):
        print(f"This is method2 with {b}")

    @staticmethod
    def static_method(a, b):
        print(f"This is a static method with {a} {b}")
