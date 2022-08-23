import unittest
import meta
from pathlib import Path
import test_file


class ModuleInspectorTest(unittest.TestCase):
    def setUp(self) -> None:
        self.inspector = meta.ModuleInspector()
        self.file = Path("test_file.py")

    def test_get_classes(self):
        classes = self.inspector.get_classes(self.file)
        self.assertEqual(1, len(classes))
        self.assertIs(test_file.Test, classes[0])

    def test_get_methods(self):
        c = self.inspector.get_classes(self.file)[0]
        self.assertCountEqual([meta.Method("__init__", test_file.Test.__init__),
                               meta.Method("method", test_file.Test.method),
                               meta.Method("method2", test_file.Test.method2),
                               meta.Method("static_method", test_file.Test.static_method)], self.inspector.get_methods(c))

    def test_wrap_methods(self):
        c = self.inspector.get_classes(self.file)[0]
        methods = self.inspector.get_methods(c)
        self.inspector.wrap_methods(c, methods, "print('hello')")
        new_methods = self.inspector.get_methods(c)
        for original_method, new_method in zip(methods, new_methods):
            self.assertNotEqual(new_method, original_method)


class MetaTest(unittest.TestCase):
    def test_non_existent_file(self):
        to_add = meta.CodeToAdd(Path("nope"), "print('hello')")
        self.assertFalse(to_add.is_valid())

    def test_bad_code(self):
        to_add = meta.CodeToAdd(Path(__file__), "print 'hello'")
        self.assertFalse(to_add.is_valid())

    def test_non_python_file(self):
        to_add = meta.CodeToAdd(Path("empty_file.py"), "print('hello')")
        self.assertFalse(to_add.is_valid())


if __name__ == "__main__":
    unittest.main()
