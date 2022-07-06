#include "BusinessUser.h"

void BusinessUser::sendMessage(User* user, Message* const message) const {
    sendMessageInternal(user, message, false);
}
