#include <iostream>
class Foo {
   public:
    Foo() {
        baz();
    }
    virtual void baz() {
        std::cout << "Foo::baz()" << std::endl;
    }
};

class Bar : public Foo {
   public:
    Bar() {}
    virtual void baz() {
        std::cout << "Bar::baz()" << std::endl;
    }
};

int main() {
    Foo *pFoo = new Bar();
    delete pFoo;
    return 0;
}