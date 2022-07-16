import unittest
import fruit
import bank

class FruitTest(unittest.TestCase):
    def test_basket(self):
        color = "red"
        quantity = 5
        basket = fruit.AppleBasket(color, quantity)
        self.assertEqual(color, basket.apple_color)
        self.assertEqual(quantity, basket.apple_quantity)

    def test_increase(self):
        quantity = 5
        basket = fruit.AppleBasket("blue", quantity)
        basket.increase()
        self.assertEqual(quantity + 1, basket.apple_quantity)

    def test_str(self):
        expected = "A basket of 3 green apples."
        basket = fruit.AppleBasket("green", 3)
        self.assertEqual(expected, str(basket))

class BankAccountTest(unittest.TestCase):
    def test_bank(self):
        name = "myname"
        amt = 1337
        account = bank.BankAccount(name, amt)
        self.assertEqual(name, account.name)
        self.assertEqual(amt, account.amt)

    def test_str(self):
        name = "myname"
        amt = 1337
        expected = f"Your account, myname, has 1337 dollars."
        account = bank.BankAccount(name, amt)
        self.assertEqual(expected, str(account))


if __name__ == "__main__":
    unittest.main()
