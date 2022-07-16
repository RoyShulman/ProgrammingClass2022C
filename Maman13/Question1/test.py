import unittest
import past
import rain

class Tests(unittest.TestCase):
    def test_past(self):
        expected = ["adopted", "baked", "beamed", "cooked", "timed", "grilled", "waved", "hired"]
        self.assertListEqual(expected, past.past_tense)

    def test_rain(self):
        self.assertEqual(4, rain.num_rainy_months)
        self.assertEqual(75, rain.THRESHOLD)

if __name__ == "__main__":
    unittest.main()
