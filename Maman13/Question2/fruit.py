class AppleBasket:
    def __init__(self, color: str, quantity: int) -> None:
        self.apple_color = color
        self.apple_quantity = quantity

    def increase(self):
        self.apple_quantity += 1

    def __str__(self) -> str:
        return f"A basket of {self.apple_quantity} {self.apple_color} apples."

if __name__ == "__main__":
    red_basket = AppleBasket("red", 4)
    blue_basket = AppleBasket("blue", 50)
    print(f"Example1: {red_basket}")
    print(f"Example2: {blue_basket}")
