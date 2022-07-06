#pragma once

#include <string>
#include <list>
#include "USocial.h"
#include "Post.h"
#include "Message.h"

using std::string;
using std::list;

class User {
protected:
    User() = default;
    virtual ~User();

    User(const User& other);

    User& operator=(const User& other);

    USocial* us_;
    unsigned long id_;
    string name_;
    list<unsigned long> friends;

    list<Post*> posts_;
    list<Message*> receivedMsgs_;

    friend class USocial;

     /**
     * @brief Send a message to the specified user. They are responsible for freeing the memory
     * 
     * @param user User to send the message to
     * @param message The message to send
     * @param checkIsFriend Should we check if the user we are sending to is our friend
     * 
     */
    void sendMessageInternal(User* user, Message* const message, bool checkIsFriend) const;

public:
    unsigned long getId() const { return id_; }
    const string& getName() const { return name_; }

    /**
     * @brief Adds a user to the friends list
     * Trying to add the same user twice will result in an exception
     * 
     * @param user friend to add
     */
    void addFriend(User* user);

    /**
     * @brief Removes a friend from the friends list
     * @note Removing a non existent friend is a no operation and is accepted
     * 
     * @param user friend to remove
     */
    void removeFriend(User* user);

    /**
     * @brief Add a post to the posts list
     * 
     * @param text Text the post should contain
     */
    void post(const string& text);

    /**
     * @brief Add a post to the posts list
     * 
     * @param text Text the post should contain
     * @param media Media the post should contain
     */
    void post(const string& text, Media* media);

    const list<Post*>& getPosts() const { return posts_; };

    /**
     * @brief Store a message in the receivedMessages list
     * 
     */
    void receiveMessage(Message* message);

    /**
     * @brief Send a message to the specified user. They are responsible for freeing the memory
     * 
     * @param user User to send the message to
     * @param message The message to send
     * 
     */
    virtual void sendMessage(User* user, Message* message) const;

    /**
     * @brief Print all the received messages
     * 
     */
    void viewReceivedMessages() const;

    /**
     * @brief Prints all posts by the user's friends
     * 
     */
    void viewFriendsPosts() const;
};
