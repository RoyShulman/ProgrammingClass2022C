class BankAccount:
    def __init__(self, name: str, amt: int) -> None:
        self.name = name
        self.amt = amt

    def __str__(self) -> str:
        return f"Your account, {self.name}, has {self.amt} dollars."

if __name__ == "__main__":
    t1 = BankAccount("Bob", 100)
    print(t1)
