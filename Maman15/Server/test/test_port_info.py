import unittest
from pathlib import Path

from server.port_info import PortInfo, InvalidPortInfoFile


class PortInfoTest(unittest.TestCase):
    def test_bad_file(self):
        p = Path("test_file")
        for data in ["abc", "1ax12", ""]:
            with self.subTest(data=data):
                with p.open("w") as f:
                    f.write(data)
                with self.assertRaises(InvalidPortInfoFile):
                    PortInfo.from_file(p)
                p.unlink()

    def test_good_file(self):
        p = Path("test_file")
        with p.open("w") as f:
            f.write("1234")
        port_info = PortInfo.from_file(p)
        self.assertEqual(1234, port_info.port)
        p.unlink()


if __name__ == "__main__":
    unittest.main()
