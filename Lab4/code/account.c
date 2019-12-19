#include "account.h"
// completed by anikeith bankar 1004073478 and jay bihola 1004178310


static struct account_struct *copy_account(struct account_struct *A) {
  struct account_struct *B = (struct account_struct *)malloc(sizeof(struct account_struct));
  strcpy(B->cID,A->cID);
  strcpy(B->cPW,A->cPW);
  B->connectedToSession = A->connectedToSession;
  B->inSession = A->inSession;
  B->loggedIn = A->loggedIn;
  return B;
}

// creating new accounts
struct account_struct* new_account_char(struct account_struct** list, char new_cID[], char new_cPW[]) {

  if (username_exists(*list, new_cID)) {
    printf("Error, username exists\n");
    return *list;
  }

  struct account_struct *new_account = NULL, *curr_account = NULL, *root = NULL;
  new_account = (struct account_struct*)malloc(sizeof(struct account_struct)); 
  strcpy(new_account->cID, new_cID);
  strcpy(new_account->cPW, new_cPW);
  new_account->inSession = false;
  new_account->loggedIn = false;
  new_account->connectedToSession = -1;
  new_account->next_user = NULL;

  root = *list;
  curr_account = *list;

  if (root == NULL) {
    new_account->prev_user = NULL;
    *list = new_account;
    return *list;
  }


  while (curr_account->next_user != NULL) {
    curr_account = curr_account -> next_user;
  }

  curr_account->next_user = new_account;
  new_account->prev_user = curr_account;

  return root;
}

struct account_struct* new_account(struct account_struct** list, struct account_struct* user) { // based on user struct
  
  if (username_exists(*list, user->cID)) {
    printf("Error, username exists\n");
    return *list;
  }

  struct account_struct *temp = (struct account_struct*)malloc(sizeof(struct account_struct));
  
  strcpy(temp->cID,user->cID);
  strcpy(temp->cPW,user->cPW);
  temp->prev_user = NULL;
  temp->next_user = NULL;
  temp->inSession = user->inSession;
  temp->loggedIn = user->loggedIn;
  temp->connectedToSession = user->connectedToSession;
  
//  // note function only supports inserting user that is a node on its own (i.e., next and prev are null)
//  if (user->next_user != NULL || user->prev_user != NULL) {
//    printf("Invalid insertion\n");
//    return *list;
//  }

  struct account_struct *curr_account = *list, *root = *list;
  
  if (root == NULL) {
    temp->prev_user = NULL;
    *list = temp;
    return *list;
  }
  
  while (curr_account->next_user != NULL) {
    curr_account = curr_account->next_user;
  }

  curr_account->next_user = temp;
  temp->prev_user = curr_account;

  return root;

}

bool username_exists(struct account_struct* list, char cID[cID_len]) { // checks if username exist

  struct account_struct *root = list;

  while (root != NULL) {
    if (strcmp(root->cID,cID)==0)
      return true;
    root = root->next_user;
  }

  return false;
}

struct account_struct* returnUser(struct account_struct* list, char cID[cID_len]) {
  struct account_struct *root = list;
  while (root != NULL) {
    if (strcmp(root->cID,cID) == 0){
      struct account_struct *returnval = copy_account(root);
      return returnval;
    }
    root = root->next_user;
  }
  return NULL;
}

// function to create new user structs
struct account_struct* new_user(char new_cID[], char new_cPW[]) {
  
  struct account_struct *returnval = NULL;
  returnval = (struct account_struct*)malloc(sizeof(struct account_struct));

  strcpy(returnval->cID,new_cID);
  strcpy(returnval->cPW,new_cPW);
  returnval->inSession = false;
  returnval->loggedIn = false;
  returnval->connectedToSession = -1;
  returnval->next_user = NULL;
  returnval->prev_user = NULL;
  return returnval;
}

// function to delete user
struct account_struct* delete_account_by_user(struct account_struct** list, char user[cID_len]) {

  if (*list == NULL) return *list;

  struct account_struct *root = *list, *curr_account = *list, *prev_account = NULL, *next_account = (*list)->next_user;

  // delete if first node
  if (strcmp(curr_account->cID,user) == 0) {
    *list = next_account;
    free (curr_account);
    return *list;
  }

  while (strcmp(curr_account->cID, user) != 0) {
    prev_account = curr_account;
    curr_account = curr_account->next_user;
    if (curr_account == NULL) {
      printf("User not found\n");
      break;
    }
    next_account = curr_account->next_user;
  }

  // if it is last node
  if (curr_account->next_user == NULL) {
    prev_account->next_user = NULL;
    printf("user found, is last user, deleting\n");
    free(curr_account);
  } else { // it is not last node
    prev_account->next_user = next_account;
    printf("user found\n");
    next_account->prev_user = prev_account;
    free(curr_account);
  }
  
  return root;
}

// function to see if user is connected to a session
bool is_user_connected(struct account_struct* list, char user[]) {
  if (list == NULL) return false;

  bool connected = false;
  struct account_struct *curr_account = list;

  while (curr_account != NULL) {
    if (strcmp(curr_account->cID,user)==0) {
      connected = curr_account->inSession;
      break;
    }
    curr_account = curr_account->next_user;
  }
  
  if (connected) {
    printf("User: %s, is connected\n",user);
  } else {
    printf("User: %s, is not connected\n", user);
  }
  return connected;
}

// function to free a list of users
void free_user_list(struct account_struct** list) {
  struct account_struct *current_account = *list;
  struct account_struct *next_account;

  while (current_account != NULL) {
    next_account = current_account->next_user;
    free(current_account);
    current_account = next_account;
  }

  *list = NULL;

}

// function to initialize user list based on file
struct account_struct* create_list() {
  FILE *filepointer; 
  filepointer = fopen(dir,"r+");
  char newcID[100], newcPW[100];
  if (filepointer == NULL) {
    printf("File Directory: %s, not found\n",dir);
    return NULL;
  }
  struct account_struct *new_list = NULL;
  int iteration = 0;
  while (fscanf(filepointer, "%s %s\n", newcID, newcPW) != EOF) {
    iteration++;
    if (strlen(newcID) > 30 || strlen(newcPW) > 30) {
      printf("Account #%d, has cID or cPW exceeding limit of 30 characters: continuing to next one\n",iteration);
      continue;
    }
    new_list = new_account_char(&new_list,newcID, newcPW);
  }
  fclose(filepointer);
  return new_list;

}

// function to update text file with all users
void update_list(struct account_struct *list) {
  FILE *filepointer;
  filepointer = fopen(dir, "w+");
  struct  account_struct *current_account = list;

  print_user_list(list);
  while (current_account != NULL) {
    fprintf(filepointer,"%s",current_account->cID);
      //fwrite(current_account->cID, 1, sizeof(current_account->cID), filepointer);
    fprintf(filepointer, " ");
    //fwrite(current_account->cPW, 1, sizeof(current_account->cPW), filepointer);
    fprintf(filepointer,"%s",current_account->cPW);
    fprintf(filepointer, "\n");
    current_account = current_account->next_user;
    if (current_account == NULL) break;
  }
  fclose(filepointer);

}

// function to print
void print_user_list(struct account_struct* list) {
  if (list == NULL) {
    printf("Empty user list\n");
    return;
  }
  
  struct account_struct* root = list;
  int user_no = 0;
  while (root != NULL) {
    user_no++;
    printf("User Number: %d, User Name: %s, Password: %s, Logged In: %d, Connected: %d, Session: %d\n", user_no, root->cID, root->cPW, root->loggedIn, root->inSession, root->connectedToSession);
    root = root->next_user;
  }

}

// functions for login and logout, updates lisr
bool user_logout(struct account_struct **list, char user[]) {
  
  struct account_struct *root = *list;

  while (root != NULL) {
    if (strcmp(root-> cID,user) == 0) {
      root->loggedIn = false;
      root->inSession = false;
      root->connectedToSession = -1;
      return root->loggedIn;
    }
    root = root->next_user;
    if (root == NULL) return false;
  }

  return root->loggedIn;

}

int user_login(struct account_struct **list, char user[], char pass[]) {

  struct account_struct *root = *list;

  while (root != NULL) {
    if (strcmp(root->cID, user) == 0) {
      if (strcmp(root->cPW, pass) == 0) {
        if (root->loggedIn == true) {
          // case that user is already logged in
          return -3;
        }
        root->loggedIn = true;
        root->inSession = false;
        return 1;
      } else {
        // case that password is wrong
        return -1;
      }
    }
    root = root->next_user;
    if (root == NULL) {
      // case that user is not found
      return -2;
    }
  }
  
  return 0;
}

// function to connect user to session sID
// -2 indicates user not found
int user_connect(struct account_struct **list, char user[], int sID) {

  struct account_struct *root = *list;

  while (root != NULL) {
    if (strcmp(root->cID, user) == 0 && root->loggedIn) {
      root->inSession = true;
      root->connectedToSession = sID;
      break;
    }
    root = root->next_user;
    if (root == NULL) {
      return -2;
    }
  }
  return root->connectedToSession;
}

int user_disconnect(struct account_struct **list, char user[]) {

  struct account_struct *root = *list;

  while (root != NULL) {
    if (strcmp(root->cID, user) == 0) {
      root->inSession = false;
      root->connectedToSession = -1;
      break;
    }
    root = root->next_user;
    if (root == NULL) {
      return -2;
    }
  }
  return root->connectedToSession;
}

// char * logged_in_users(struct account_struct *list) {
//   if (list == NULL) {
//     return "empty";
//   }
//   char returnval[1000] = "Logged in users\n";
//   struct account_struct* root = list;
//   while (root != NULL) {
//     if (root->loggedIn == 1) {
//       strcat(returnval,root->cID);
//       strcat(returnval,"\n");
//     }
//     root = root->next_user;
//   }
//   return returnval;
//   //returnval = temp;
// }

// functions to deal with session name and id
void modify_name(struct session_struct **session, char new_name[]) {
    strcpy((*session)->session_name, new_name);
}
char *return_name(struct session_struct *session) {
    return session->session_name;
}

// function to update and return isActive
void modify_isActive(struct session_struct **session, bool newStatus) {
    (*session)->isActive = newStatus;
}
bool session_isActive(struct session_struct *session) {
    return session->isActive;
}

// functions to update and return sID
void modify_sID(struct session_struct **session, int newsID) {
    (*session)->sID = newsID;
}
int session_sID(struct session_struct *session) {
    return session->sID;
}

// functions to deal with users in sID struct
bool add_user_to_session(struct session_struct **session, struct account_struct *account, int sID){
    bool temp = (*session)->isActive;
    if (temp) {
      account->inSession = true;
      account->connectedToSession = sID;
      new_account(&(*session)->connected_users, account);
      (*session)->userCount++;
      (*session)->isActive = true;
      return true;
    }
    return false;
}
void remove_from_session_by_user(struct session_struct **session, char user[]) {
    if (username_exists((*session)->connected_users, user)) {
        delete_account_by_user(&(*session)->connected_users, user);
        (*session)->userCount--;
        (*session)->isActive = true;
        if ((*session)->userCount == 0) {
            (*session)->isActive = false;
        }
        printf("Removed Successfully\n");
        return;
    }
    printf("Unable to find user in session\n");
}
void update_user_count(struct session_struct **session) {
    int iterator = 0;
    struct account_struct *current_account = (*session)->connected_users;
    
    while (current_account != NULL) {
        iterator++;
        current_account = current_account->next_user;
    }
    
    (*session)->userCount = iterator;
}
int return_user_count(struct session_struct *session) {
    return session->userCount;
}

// function to create a session
bool create_session(struct session_struct **session, char new_name[], int sID) {
    modify_name(&(*session), new_name);
    modify_isActive(&(*session), false);
    modify_sID(&(*session), sID);
    return true;
}

// function to print session
void print_session(struct session_struct *session) {
    if (session != NULL) {
        printf("Session Name: %s, Session ID: %d, User Count: %d, Session Active: %d\n", session->session_name, session->sID, session->userCount, session->isActive);
    } else {
        printf("Session does not exist\n");
    }
}
void print_all_sessions(struct session_struct *session[]) {
    for (int i = 0; i < max_sessions; i++) {
        print_session(session[i]);
    }
}
void print_users_in_session(struct session_struct *session) {
    print_user_list(session->connected_users);
}
