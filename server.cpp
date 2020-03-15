#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <cstring>
#include <iostream>
#include <string>
#include "songs.h"
#include "users.h"

#define PORT 2908

using namespace std;

extern int errno;


typedef struct thData{
    int idThread;
    int cl;
}thData;

static void *treat(void *);
void raspunde(void *);

bool connectedUsers[100];
char sysPassword[100] = "bobdylan1941";


int main ()
{
  struct sockaddr_in server;
  struct sockaddr_in from;
  int sd;
  pthread_t th[100];
	int i=0;


  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]socket() error.\n");
      return errno;
    }
  int on=1;
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (PORT);

  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]bind() error.\n");
      return errno;
    }

  if (listen (sd, 2) == -1)
    {
      perror ("[server]listen() error.\n");
      return errno;
    }

  loadGenres();
  loadSongs();
  loadUsers();
  loadComments();

  printf ("[server]Port: %d\n",PORT);

  while (1)
    {
      int client;
      thData * td;
      socklen_t length = sizeof (from);

      fflush (stdout);

      if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
	{
      perror ("[server]accept() error.\n");
	  continue;
	}

	td=(struct thData*)malloc(sizeof(struct thData));
	td->idThread=i++;
	td->cl=client;

	pthread_create(&th[i], NULL, &treat, td);


    }
};


void displayReceived(char msg[], void *arg)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);
    printf ("[Thread %d]Received command: %s\n",tdL.idThread,msg);
}

static void *treat(void * arg)
{
		struct thData tdL;
		tdL= *((struct thData*)arg);
		fflush (stdout);
		pthread_detach(pthread_self());
		raspunde((struct thData*)arg);
		return(NULL);

};

void sendToClient_int(int x, void *arg)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);

    write (tdL.cl, &x, sizeof(int));
}

void sendToClient_string(string s, void *arg)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);
    char message[100];

    strcpy (message, s.c_str());
    write (tdL.cl, &message, sizeof(message));
    read (tdL.cl, &message, sizeof(message));
}

void sendSong_simple(int id, int nr, void *arg)
{
        struct thData tdL;
        tdL= *((struct thData*)arg);
        string msg;

        msg = to_string(nr) + ". ";
        msg = msg + songList[id].artist + " - ";
        msg = msg + songList[id].title;
        msg = msg + " (ID: " + to_string(songList[id].id);
        msg = msg + ", Score: " + to_string(songList[id].score);
        msg = msg + ") ";
        sendToClient_string(msg, arg);
}

void sendSong_detailed(int id, int nr, void *arg)
{
        struct thData tdL;
        tdL= *((struct thData*)arg);
        string msg;

        msg = to_string(nr) + ".";
        sendToClient_string(msg, arg);

        msg = "Title: " + songList[id].title;
        sendToClient_string(msg, arg);

        msg = "Artist: " + songList[id].artist;
        sendToClient_string(msg, arg);

        msg = "Genres: ";
        for (int j=0; j < songList[id].genre_count-1; j++ ) msg = msg + genreList[ songList[id].genre_id[j] ].name + ", ";
        msg = msg + genreList[ songList[id].genre_id[songList[id].genre_count-1] ].name;
        sendToClient_string(msg, arg);

        msg = "ID: " + to_string(songList[id].id);
        sendToClient_string(msg, arg);

        msg = "Score: " + to_string(songList[id].score);
        sendToClient_string(msg, arg);

        msg = "Link: " + songList[id].link;
        sendToClient_string(msg, arg);

        msg = "Description: " + songList[id].description + "\n";
        sendToClient_string(msg, arg);

}

bool quit(char command[100])
{
    if ( strcmp(command, "quit") == 0 )
        return true;
    return false;
}

int login_check(int user_id, void *arg)
{
    char ok[100] = "1";
    char err[100] = "0";
    struct thData tdL;
    tdL= *((struct thData*)arg);

    if (user_id == -1)
    {
        write (tdL.cl, &err, sizeof(err));
        return -1;
    }

    else if (connectedUsers[user_id])
    {
        write (tdL.cl, &ok, sizeof(ok));
        return 1;
    }
    else
    {
        write (tdL.cl, &err, sizeof(err));
        return 0;
    }

}

void top(void *arg, int user_id)
{
    //int f;
    string songl;
    char songline[100];
    //char command[100];
    //char response[100];
    struct thData tdL;
    tdL= *((struct thData*)arg);

    //login_check(user_id, arg);

    for ( int i=0; i<songCount; i++ )
    {
        sendSong_simple(i,i+1,arg);

    }

    songl = "end";
    strcpy (songline, songl.c_str());
    write (tdL.cl, &songline, sizeof(songline));

}

void registerAdmin(void *arg)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);

    string username, password;
    char command[100];
    char response[100];
    char error_ch[100] = "0";
    char ok_ch[100] = "1";

    read(tdL.cl, &command, sizeof(command));
    if (quit(command)) return;

    while ( strcmp(sysPassword, command) != 0 )
    {
        write(tdL.cl, &error_ch, sizeof(error_ch));
        read(tdL.cl, &command, sizeof(command));
        if (quit(command)) return;
    }

    write(tdL.cl, &ok_ch, 100);
    read(tdL.cl, &command, sizeof(command));
    if (quit(command)) return;
    username = command;

    while ( getUserID(username) != -1 )
    {
        write(tdL.cl, &error_ch, sizeof(error_ch));
        read(tdL.cl, &command, sizeof(command));
        if (quit(command)) return;
        username = command;
    }

    write(tdL.cl, &ok_ch, 100);
    read(tdL.cl, &command, sizeof(command));
    if (quit(command)) return;

    while ( strchr(command,' ') )
    {
        write(tdL.cl, &error_ch, sizeof(error_ch));
        read(tdL.cl, &command, sizeof(command));
        if (quit(command)) return;
    }

    password = command;
    write(tdL.cl, &ok_ch, 100);
    newUser(username, password, 1);
    saveUsers();

    return;
}

void registerUser(void *arg)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);

    string username, password;
    char command[100];
    char response[100];
    char error_ch[100] = "0";
    char ok_ch[100] = "1";

    read(tdL.cl, &command, sizeof(command));
    if (quit(command)) return;
    username = command;

    while ( getUserID(username) != -1 )
    {
        write(tdL.cl, &error_ch, sizeof(error_ch));
        read(tdL.cl, &command, sizeof(command));
        if (quit(command)) return;
        username = command;
    }

    write(tdL.cl, &ok_ch, 100);
    read(tdL.cl, &command, sizeof(command));
    if (quit(command)) return;

    while ( strchr(command,' ') )
    {
        write(tdL.cl, &error_ch, sizeof(error_ch));
        read(tdL.cl, &command, sizeof(command));
        if (quit(command)) return;
    }

    password = command;
    write(tdL.cl, &ok_ch, 100);
    newUser(username, password, 0);
    saveUsers();

    return;
}

void login(void *arg, int &user_id)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);

    char command[100];
    char password[100];
    //char response[100];
    char error_ch[100] = "0";
    char ok_ch[100] = "1";

    int check = login_check(user_id,arg);
    if (check == 1) return;

    write(tdL.cl, &ok_ch, sizeof(ok_ch));
    read(tdL.cl, &command, sizeof(command));
    if (quit(command)) return;
    displayReceived(command, arg);

    string username(command);

    while ( getUserID(username) == -1 )
    {
       write(tdL.cl, &error_ch, sizeof(error_ch));
       read(tdL.cl, &command, sizeof(command));
       displayReceived(command, arg);
       if (quit(command)) return;
       username = command;
    }

    int id = getUserID(username);
    strcpy(password, users[id].password.c_str());

    write(tdL.cl, &ok_ch, sizeof(ok_ch));
    read(tdL.cl, &command, sizeof(command));
    displayReceived(command, arg);
    if (quit(command)) return;

    while ( strcmp(command, password) != 0 )
    {
        write(tdL.cl, &error_ch, sizeof(error_ch));
        read(tdL.cl, &command, sizeof(command));
        displayReceived(command, arg);
        if (quit(command)) return;
    }

    connectedUsers[id] = true;
    user_id = id;
    write(tdL.cl, &ok_ch, sizeof(ok_ch));

}

void logout(void *arg, int user_id)
{
    int check = login_check(user_id, arg);

    if (check == 1 or check == -1)
        connectedUsers[user_id] = false;

}

void changePass(void *arg, int user_id)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);

    int i = login_check(user_id, arg);
    if ( i == -1 or i == 0 ) return;

    char newPassword[100];
    char ok[100] = "1";
    char err[100] = "0";
    read(tdL.cl, newPassword, 100);
    if (quit(newPassword)) return;

    if ( strchr(newPassword, ' ') or strcmp(newPassword,"quit") == 0 )
    {
        write(tdL.cl, err, 100);
        return;
    }

    else
    {
        users[user_id].password = newPassword;
        saveUsers();
        write(tdL.cl, ok, 100);
        return;
    }
}

void genres(void *arg, int user_id)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);
    char command[100];

    int check = login_check(user_id, arg);
    if (check == 0 or check == -1 )
        return;

    read(tdL.cl, command, 100);


    for (int i=1; i<genreCount; i++)
    {
        sendToClient_string( noUnderscore(genreList[i].name), arg );
    }

    write(tdL.cl, "end", 100);

}

void top(int g_id, int count, bool subgenres, bool details, void *arg)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);

    if (g_id == 0)
    {
        for (int i=0; i<songCount and i<count; i++ )
            if (details) sendSong_detailed(i, i+1, arg);
            else sendSong_simple(i, i+1, arg);
        return;
    }

    if (subgenres)
    {
        int i = 0, nr = 0;

        while ( i<songCount and nr<count )
        {
            song &s = songList[i];
            bool gasit = false;
            for (int j=0; j<s.genre_count and !gasit; j++)
                if ( isSubgenre(genreList[s.genre_id[j]],genreList[g_id]) )
                {
                    nr++;
                    gasit = true;
                    if (details) sendSong_detailed(i, nr, arg);
                    else sendSong_simple(i, nr, arg);
                }
            i++;
        }

    }

    else
    {
        int i = 0, nr = 0;

        while ( i<songCount and nr<count )
        {
            song &s = songList[i];
            bool gasit = false;
            for (int j=0; j<s.genre_count and !gasit; j++)
                if ( s.genre_id[j] == g_id )
                {
                    nr++;
                    gasit = true;
                    if (details) sendSong_detailed(i, nr, arg);
                    else sendSong_simple(i, nr, arg);
                }
            i++;
        }

    }

}

void top_config(void *arg, int user_id)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);

    int check = login_check(user_id, arg);
    if (check == 0 or check == -1 )
        return;

    char command[100];
    char ok[100] = "1";
    char err[100] = "0";
    string nume;
    int gen_id;
    int nr;
    bool subgenres;
    bool details;

    read(tdL.cl, &command, sizeof(command));
    displayReceived(command, arg);
    if (quit(command)) return;
    nume = command;

    gen_id = getGenreID(nume);
    if ( nume == "all" ) gen_id = 0;
    else if ( gen_id == -1 )
    {
        write(tdL.cl, err, 100);
        return;
    }
    write(tdL.cl, ok, 100);

    read(tdL.cl, &command, sizeof(command));
    displayReceived(command, arg);
    if (quit(command)) return;

    if ( strcmp(command,"yes") == 0 )
        subgenres = true;
    else if ( strcmp(command,"no") == 0 )
        subgenres = false;
    else
    {
        write(tdL.cl, err, 100);
        return;
    }
    write(tdL.cl, ok, 100);

    read(tdL.cl, &command, sizeof(command));
    displayReceived(command, arg);
    if (quit(command)) return;

    if ( strcmp(command,"all") == 0 )
        nr = songCount;
    else
        nr = atoi(command);

    write(tdL.cl, ok, 100);

    read(tdL.cl, &command, sizeof(command));
    displayReceived(command, arg);
    if (quit(command)) return;

    if ( strcmp(command,"yes") == 0 )
        details = true;
    else if ( strcmp(command,"no") == 0 )
        details = false;
    else
        { write(tdL.cl, err, 100); return; }

    write(tdL.cl, ok, 100);
    read(tdL.cl, &command, sizeof(command));

    top(gen_id,nr,subgenres,details,arg);
    write(tdL.cl, "end", 100);
}

void add_song(void *arg, int user_id)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);

    int check = login_check(user_id, arg);
    if (check == 0 or check == -1 )
        return;

    char command[100];
    char ok[100] = "1";
    char err[100] = "0";
    string name, artist, genre, description, link;
    int g_id;
    int genres[10];

    read(tdL.cl, &command, sizeof(command));
    displayReceived(command, arg);
    if (quit(command)) return;
    name = command;
    write(tdL.cl, ok, 100);

    read(tdL.cl, &command, sizeof(command));
    displayReceived(command, arg);
    if (quit(command)) return;
    artist = command;
    write(tdL.cl, ok, 100);

    read(tdL.cl, &command, sizeof(command));
    displayReceived(command, arg);
    if (quit(command)) return;
    genre = command;
    g_id = getGenreID(genre);
    if ( g_id == -1) { write(tdL.cl, err, 100); return;}
    else             write(tdL.cl, ok, 100);

    genres[0] = g_id;

    read(tdL.cl, &command, sizeof(command));
    displayReceived(command, arg);
    if (quit(command)) return;
    description = command;
    write(tdL.cl, ok, 100);

    read(tdL.cl, &command, sizeof(command));
    displayReceived(command, arg);
    if (quit(command)) return;
    link = command;
    write(tdL.cl, ok, 100);

    addSong(idMax+1, 0, 1, genres, name, artist, link, description );
    saveSongs();

}

void vote_song(void *arg, int user_id)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);

    int check = login_check(user_id, arg);
    if (check == 0 or check == -1 )
        return;

    char command[100];
    char ok[100] = "1";
    char err[100] = "0";
    char err2[100] = "2";
    int songID;

    read(tdL.cl, &command, sizeof(command));

    if ( users[user_id].votePrivilege == 0 )
    {
        write(tdL.cl, err, 100);
        return;
    }

    write(tdL.cl, ok, 100);


    read(tdL.cl, &command, sizeof(command));
    songID = atoi(command);
    displayReceived(command,arg);
    if (quit(command)) return;

    if ( isSongID(songID) == -1 )
    {
        write(tdL.cl, err, 100);
        return;
    }

    bool alreadyVoted = false;
    for ( int i=0; i < users[user_id].votedSongsCount; i++ )
        if ( users[user_id].votedSongs[i] == songID )
            alreadyVoted = true;

    if ( alreadyVoted )
    {
        write(tdL.cl, err2, 100);
        return;
    }

    vote(songID);
    int k = users[user_id].votedSongsCount;
    users[user_id].votedSongs[k] = songID;
    users[user_id].votedSongsCount++;

    saveUsers();
    saveSongs();


    write(tdL.cl, ok, 100);

}

void delete_song(void *arg, int user_id)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);

    int check = login_check(user_id, arg);
    if (check == 0 or check == -1 )
        return;

    char command[100];
    char ok[100] = "1";
    char err[100] = "0";
    int songID;
    string name;

    read(tdL.cl, &command, sizeof(command));

    if ( users[user_id].admin == 0 )
    {
        write(tdL.cl, err, 100);
        return;
    }

    write(tdL.cl, ok, 100);

    read(tdL.cl, &command, sizeof(command));
    displayReceived(command,arg);
    if (quit(command)) return;
    songID = atoi(command);

    if ( isSongID(songID) == -1 )
    {
        write(tdL.cl, err, 100);
        return;
    }

    deleteSong(songID);
    saveSongs();
    write(tdL.cl, ok, 100);


}

void restrict(void *arg, int user_id)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);

    int check = login_check(user_id, arg);
    if (check == 0 or check == -1 )
        return;

    char command[100];
    char ok[100] = "1";
    char err[100] = "0";
    char err2[100] = "2";
    int restrictID;
    string name;

    read(tdL.cl, &command, sizeof(command));

    if ( users[user_id].admin == 0 )
    {
        write(tdL.cl, err, 100);
        return;
    }

    write(tdL.cl, ok, 100);
    read(tdL.cl, &command, sizeof(command));
    displayReceived(command,arg);
    if (quit(command)) return;

    name = command;
    restrictID = getUserID(name);

    if ( restrictID == -1 )
    {
        write(tdL.cl, err, 100);
        return;
    }

    users[restrictID].votePrivilege = false;
    write(tdL.cl, ok, 100);

}

void add_comment(void *arg, int user_id)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);
    char command[100];
    char ok[100] = "1";
    char err[100] = "0";
    int song_id;
    string comment;

    int check = login_check(user_id, arg);
    if (check == 0 or check == -1 )
        return;

    read(tdL.cl, &command, sizeof(command));
    song_id = atoi(command);
    displayReceived(command,arg);
    if (quit(command)) return;

    if ( isSongID(song_id) == -1 )
    {
        write(tdL.cl, err, 100);
        return;
    }
    write(tdL.cl, ok, 100);

    read(tdL.cl, &command, sizeof(command));
    displayReceived(command,arg);
    comment = command;

    addComment(song_id, user_id, comment);
    saveComments();
    write(tdL.cl, ok, 100);

}

void show_comments(void *arg, int user_id)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);
    char command[100];
    char response[100];
    char ok[100] = "1";
    char err[100] = "0";
    int song_id;
    string comment;

    int check = login_check(user_id, arg);
    if (check == 0 or check == -1 )
        return;

    read(tdL.cl, &command, sizeof(command));
    song_id = atoi(command);
    displayReceived(command,arg);
    if ( isSongID(song_id) == -1 )
    {
        write(tdL.cl, err, 100);
        return;
    }
    write(tdL.cl, ok, 100);

    read(tdL.cl, &command, sizeof(command));

    for (int i=0; i<commentCount; i++)
        if ( commentList[i].song_id == song_id )
        {
            string msg = users[user_id].username + ":";
            msg = msg + " " + commentList[i].text;
            strcpy(response,msg.c_str());
            write(tdL.cl, response, 100);
            read(tdL.cl, &command, sizeof(command));
        }

    write(tdL.cl, "  ", 100);

}

void processCommand(char command[], int &user_id, void *arg)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);

    if (strcmp(command,"top") == 0)
        top_config(arg, user_id);

    else if (strcmp(command,"register") == 0)
        registerUser(arg);

    else if (strcmp(command,"register admin") == 0)
        registerAdmin(arg);

    else if (strcmp(command,"login") == 0)
        login(arg, user_id);

    else if (strcmp(command,"logout") == 0)
        logout(arg, user_id);

    else if (strcmp(command,"change password") == 0)
        changePass(arg, user_id);

    else if (strcmp(command,"genres") == 0)
        genres(arg, user_id);

    else if ( strcmp(command, "vote") == 0 )
        vote_song(arg, user_id);

    else if ( strcmp(command, "add song") == 0 )
        add_song(arg, user_id);

    else if ( strcmp(command, "restrict") == 0 )
        restrict(arg, user_id);

    else if ( strcmp(command, "add comment") == 0 )
        add_comment(arg, user_id);

    else if ( strcmp(command, "comments") == 0 )
        show_comments(arg, user_id);

    else if ( strcmp(command, "delete song") == 0 )
        delete_song(arg, user_id);
 }


void raspunde(void *arg)
{
    char command[100];
	struct thData tdL;
	tdL= *((struct thData*)arg);

    int user_id = -1;

	while (1)
    {

    read (tdL.cl, &command,sizeof(command)+1);
    displayReceived(command, arg);

    if (strcmp(command,"disconnect") == 0)
    {
        close ((intptr_t)arg);
        return;
    }

    else
        processCommand(command, user_id, arg);

    }

}
