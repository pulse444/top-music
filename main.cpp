#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <pthread.h>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

extern int errno;
int port;

int sd;
struct sockaddr_in server;

char usr[50] = "0";

int login_check()
{
    char response[100];
    read(sd, &response, 100);

    if ( strcmp(response,"0") == 0 )
    {
        return 0;
    }
    else
        return 1;

}

bool quit(char command[100])
{
    if ( strcmp(command, "quit") == 0 )
        return true;
    return false;
}

void login()
{
    int check = login_check();

    if (check == 1)
    {
        cout<< endl<< "You are already logged in." << endl << endl;
        return;
    }

    char command2[100];
    char response2[100];
    char temp_usr[50];

    read(sd, &response2, 100);

    cout << endl << "username: ";
    cin.getline(command2,100);
    write(sd, &command2, 100);
    if (quit(command2)) { cout<<endl; return; }
    read(sd, &response2, 100);

    while ( strcmp(response2, "0") == 0 )
    {
       cout<< "This username does not exist." << endl << "username: ";
       cin.getline(command2,100);
       write(sd, &command2, 100);
       if (quit(command2)) { cout<<endl; return; }
       read(sd, &response2, 100);
    }

    strcpy(temp_usr, command2);

    if (strcmp(response2,"1") == 0)
    {
        cout << "password: ";
        cin.getline(command2,100);
        write(sd, &command2, 100);
        if (quit(command2)) { cout<<endl; return; }
        read(sd, &response2, 100);

        while ( strcmp(response2, "0") == 0 )
        {
           cout<< "Wrong password." << endl << "password: ";
           cin.getline(command2,100);
           write(sd, &command2, 100);
           if (quit(command2)) { cout<<endl; return; }
           read(sd, &response2, 100);
        }

        if (strcmp(response2,"1") == 0)
            cout<<"You have logged in." << endl << endl;
        strcpy(usr, temp_usr);
    }

}

void registerUser()
{
    char command[100];
    char response[100];

    cout << endl << "Choose an username: ";
    cin.getline(command, 100);
    write(sd, &command, 100);
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, 100);

    while ( strcmp(response, "0") == 0 )
    {
        cout << "Username is invalid (contains whitespaces) or already taken." << endl << "Choose an username: ";
        cin.getline(command, 100);
        write(sd, &command, 100);
        if (quit(command)) { cout<<endl; return; }
        read(sd, &response, 100);
    }

    cout << "Choose a password: ";
    cin.getline(command, 100);
    write(sd, &command, 100);
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, 100);

    while ( strcmp(response, "0") == 0 )
    {
        cout << "Invalid password." << endl << "Choose a password: ";
        cin.getline(command, 100);
        write(sd, &command, 100);
        if (quit(command)) { cout<<endl; return; }
        read(sd, &response, 100);
    }

    cout << "You have successfully created the account." << endl << endl;
}

void registerAdmin()
{
    char command[100];
    char response[100];

    cout << endl << "Enter the system password: ";
    cin.getline(command, 100);
    write(sd, &command, 100);
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, 100);

    while ( strcmp(response, "0") == 0 )
    {
        cout << "Wrong system password." << endl << "Enter the system password: ";
        cin.getline(command, 100);
        write(sd, &command, 100);
        if (quit(command)) { cout<<endl; return; }
        read(sd, &response, 100);
    }

    cout << endl << "Choose an username: ";
    cin.getline(command, 100);
    write(sd, &command, 100);
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, 100);

    while ( strcmp(response, "0") == 0 )
    {
        cout << "Username is invalid (contains whitespaces) or already taken." << endl << "Choose an username: ";
        cin.getline(command, 100);
        write(sd, &command, 100);
        if (quit(command)) { cout<<endl; return; }
        read(sd, &response, 100);
    }

    cout << "Choose a password: ";
    cin.getline(command, 100);
    write(sd, &command, 100);
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, 100);

    while ( strcmp(response, "0") == 0 )
    {
        cout << "Invalid password." << endl << "Choose a password: ";
        cin.getline(command, 100);
        write(sd, &command, 100);
        if (quit(command)) { cout<<endl; return; }
        read(sd, &response, 100);
    }

    cout << "You have successfully created the account." << endl << endl;
}

void logout()
{
    char response[100];
    read (sd, &response,100);

    if ( strcmp(response,"0") == 0)
        cout<< "You are not authenticated." << endl << endl;
    else
    {
        cout<< "You have logged out." << endl << endl;
        strcpy(usr, "0");
    }

}

void change_password()
{
    char newPassword[100];
    char response[100];

    int check = login_check();
    if (check == 0)
    {
        cout<< "You are not authenticated." << endl << endl;
        return;
    }

    cout << endl << "New password: ";
    cin.getline(newPassword,100);
    write(sd, &newPassword, 100);
    if (quit(newPassword)) { cout<<endl; return; }

    read(sd, &response, 100);

    if ( strcmp(response, "0") == 0 )
        cout << "Invalid password." << endl << endl;
    else
        cout << "Password changed." << endl << endl;

}

void genres()
{
    char response[100];
    char ok[100] = "1";
    int nr = 1;
    //read (sd, &response,100);

    int check = login_check();
    if (check == 0)
    {
        cout<< "You are not authenticated." << endl << endl;
        return;
    }

    write (sd, &ok, sizeof(ok));

    cout<<endl;
    read (sd, &response, sizeof(response));
    while ( strcmp(response, "end") != 0 )
    {
        cout<< nr << ". " << response << endl;
        write(sd, &ok, sizeof(ok));
        read (sd, &response, sizeof(response));
        nr++;
    }

    cout<<endl;
}

void top_config()
{
    char command[100];
    char response[100];

    int check = login_check();
    if (check == 0)
    {
        cout<< "You are not authenticated." << endl << endl;
        return;
    }

    cout << endl << "Genre: ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));

    if ( strcmp(response,"0") == 0 )
    {
        cout<<"Invalid genre." << endl << endl;
        return;
    }

    cout << "Include subgenres? (yes/no) ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));

    if ( strcmp(response,"0") == 0 )
    {
        cout<<"Invalid choice." << endl << endl;
        return;
    }

    cout << "Number of songs: ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));

    cout << "Show all song details? (yes/no): ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));

    if ( strcmp(response,"0") == 0 )
    {
        cout<<"Invalid response." << endl << endl;
        return;
    }

    strcpy(command,"ok");

    write(sd, &command, sizeof(command));
    read(sd, &response, sizeof(response));

    cout << endl;
    while ( strcmp(response,"end") != 0 )
    {
        cout << response << endl;
        write(sd, &command, sizeof(command));
        read(sd, &response, sizeof(response));
    }

    cout << endl;

}

void vote_song()
{
    int check = login_check();
    if (check == 0)
    {
        cout<< endl << "You are not authenticated." << endl << endl;
        return;
    }


    char command[100];
    char response[100];

    write(sd, &command, sizeof(command));
    read(sd, &response, sizeof(response));
    if ( strcmp(response,"0") == 0 )
    {
        cout<< endl << "You cannot vote." << endl << endl;
        return;
    }

    cout << "Song ID: ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));

    if ( strcmp(response,"0") == 0 )
    {
        cout << "Invalid ID!" << endl << endl;
        return;
    }

    if ( strcmp(response,"2") == 0 )
    {
        cout << "You have already voted this song."<< endl << endl;
        return;
    }

    cout << "You have successfully voted."<< endl << endl;

}

void restrict()
{
    int check = login_check();
    if (check == 0)
    {
        cout<< endl << "You are not authenticated." << endl << endl;
        return;
    }


    char command[100];
    char response[100];

    write(sd, &command, sizeof(command));
    read(sd, &response, sizeof(response));
    if ( strcmp(response,"0") == 0 )
    {
        cout<< endl << "You are not an administrator." << endl << endl;
        return;
    }

    cout << "User ID: ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));

}

void addsong()
{
    char command[100];
    char response[100];

    int check = login_check();
    if (check == 0)
    {
        cout<< endl << "You are not authenticated." << endl << endl;
        return;
    }

    cout << endl << "Name: ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));

    cout << "Artist: ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));

    cout << "Genre: ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));
    if ( strcmp(response,"0") == 0) { cout << "Invalid genre." << endl << endl; return;}

    cout << "Description: ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));

    cout << "Link: ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));

    cout << endl << endl;

}

void add_comment()
{
    char command[100];
    char response[100];

    int check = login_check();
    if (check == 0)
    {
        cout<< endl << "You are not authenticated." << endl << endl;
        return;
    }

    cout << endl << "Song ID: ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));

    if ( strcmp(response,"0") == 0 )
    {
        cout << "Invalid ID!" << endl << endl;
        return;
    }

    cout << "Comment: ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));

    cout << "Comment added." << endl << endl;

}

void delete_song()
{
    int check = login_check();
    if (check == 0)
    {
        cout<< endl << "You are not authenticated." << endl << endl;
        return;
    }


    char command[100];
    char response[100];

    write(sd, &command, sizeof(command));
    read(sd, &response, sizeof(response));
    if ( strcmp(response,"0") == 0 )
    {
        cout<< endl << "You are not an administrator." << endl << endl;
        return;
    }

    cout << endl << "Song ID: ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));

    if ( strcmp(response,"0") == 0 )
    {
        cout << "Invalid ID!" << endl << endl;
        return;
    }

    cout << "Song deleted." << endl << endl;

}

void show_comments()
{
    int check = login_check();
    if (check == 0)
    {
        cout<< endl << "You are not authenticated." << endl << endl;
        return;
    }

    char command[100];
    char response[100];
    cout << endl << "Song ID: ";
    cin.getline(command,100);
    write(sd, &command, sizeof(command));
    if (quit(command)) { cout<<endl; return; }
    read(sd, &response, sizeof(response));

    if ( strcmp(response,"0") == 0 )
    {
        cout << "Invalid ID!" << endl << endl;
        return;
    }

    write(sd, &command, sizeof(command));

    while ( strcmp(response,"  ") != 0 )
    {
        read(sd, &response, sizeof(response));
        cout << response << endl;
        write(sd, &command, sizeof(command));
    }

}

void processCommand( char command[] )
{

    if ( strcmp(command, "genres") == 0 )
        genres();

    else if ( strcmp(command, "vote") == 0 )
        vote_song();

    else if ( strcmp(command, "top") == 0 )
        top_config();

    else if ( strcmp(command, "add song") == 0 )
        addsong();

    else if ( strcmp(command, "register") == 0 )
        registerUser();

    else if ( strcmp(command, "register admin") == 0 )
        registerAdmin();

   else if ( strcmp(command, "restrict") == 0 )
       restrict();

    else if ( strcmp(command, "login") == 0 )
        login();

    else if ( strcmp(command, "logout") == 0 )
        logout();

    else if ( strcmp(command, "change password") == 0 )
        change_password();

    else if ( strcmp(command, "add comment") == 0 )
        add_comment();

   else if ( strcmp(command, "delete song") == 0 )
        delete_song();

   else if ( strcmp(command, "comments") == 0 )
        show_comments();

    else if (strcmp(command,"quit") == 0)
     {
        cout << endl << "Type \"quit\" to cancel an ongoing command."<<endl << endl;
        return;
     }

    else if (strcmp(command,"disconnect") == 0)
     {
        cout << "Disconnected."<<endl;
        close (sd);
         return;
     }

    else
    {
        cout << endl << "Invalid command!" << endl << endl;
    }

}

int main (int argc, char *argv[])
{

  char command[100];

  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  port = atoi (argv[2]);

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons (port);

  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }

  cout << "Welcome! You have succesfully connected to TopMusic." << endl
       <<"Please authenticate using \"login\" to use the service. " <<endl
       <<"Create an account using \"register\"." << endl
       <<"Type \"disconnect\" to exit." << endl << endl;


  while ( strcmp(command,"disconnect") != 0 )
  {
      if ( strcmp(usr,"0") == 0 ) cout << "command: ";
      else cout << usr << "> command: ";

    cin.getline(command,100);

    write (sd,&command,100);
    processCommand(command);
  }

}
