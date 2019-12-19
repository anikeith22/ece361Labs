#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "globals.h"
// completed by anikeith bankar 1004073478 and jay bihola 1004178310

// include the following libraries

struct account_struct {
    char cID[cID_len];
    char cPW[cPW_len];
    struct account_struct *next_user;
    struct account_struct *prev_user;

    // current session
    bool inSession;
    bool loggedIn;
    int connectedToSession;

};

// functions to create new account
struct account_struct* new_account_char(struct account_struct** list, char new_cID[], char new_cPW[]); // based on cID and cPW
struct account_struct* new_account(struct account_struct** list, struct account_struct* user); // based on user struct
bool username_exists(struct account_struct* list, char cID[cID_len]); // checks if username exists
struct account_struct* returnUser(struct account_struct* list, char cID[cID_len]);
// function to create an entry for the account_struct
struct account_struct* new_user(char new_cID[], char new_PW[]);

// function to delete user
struct account_struct* delete_account_by_user(struct account_struct** list, char user[cID_len]);

// function to see if user is connected to a session
bool is_user_connected(struct account_struct* list, char user[]);

// function to free a list of users
void free_user_list(struct account_struct** list);

// function to initialize user list based on file
struct account_struct* create_list();

// function to update text file with all users
void update_list(struct account_struct *list);

// function to print current user list
void print_user_list(struct account_struct* list);
//char *print_logged_in_users(struct account_struct *list);

// functions for login and logout, updates list
bool user_logout(struct account_struct **list, char user[]);
int user_login(struct account_struct **list, char user[], char pass[]);

// function to connect user to session sID
int user_connect(struct account_struct **list, char user[], int sID);
int user_disconnect(struct account_struct **list, char user[]);

char* logged_in_users( struct account_struct *list);

struct session_struct {
  char session_name[cID_len];
  int sID;
  struct account_struct *connected_users;
  int userCount;
  bool isActive;
};

// functions to deal with session name and id
void modify_name(struct session_struct **session, char new_name[]);
//char *return_name(struct session_struct *session);

// function to update and return isActive
void modify_isActive(struct session_struct **session, bool newStatus);
bool session_isActive(struct session_struct *session);

// functions to update and return sID
void modify_sID(struct session_struct **session, int newsID);
int session_sID(struct session_struct *session);

// functions to deal with users in sID struct
bool add_user_to_session(struct session_struct **session, struct account_struct *account, int sID);
void remove_from_session_by_user(struct session_struct **session, char user[]);
void remove_from_session(struct session_struct **session, struct account_struct *account);
void update_user_count(struct session_struct **session);
int return_user_count(struct session_struct *session);

// function to create a session
bool create_session(struct session_struct **session, char new_name[], int sID);

// function to print session
void print_session(struct session_struct *session);
void print_all_sessions(struct session_struct *session[]);
void print_users_in_session(struct session_struct *session);

#endif
