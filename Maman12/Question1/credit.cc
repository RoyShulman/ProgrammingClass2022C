#include "credit.h"

static int get_credit(int ID) {
    return ID;
} 
 
bool is_entitled_for_promotional_gift(int ID) {
    unsigned int bound = 750;
    int credit = get_credit(ID);
    return (credit >= bound);
}

bool is_entitled_for_promotional_gift_fixed(int ID) {
    unsigned int bound = 750;
    int credit = get_credit(ID);
    if (credit < 0) {
        return false;
    }
    return ((unsigned int)credit >= bound);
}
