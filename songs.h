#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include "genres.h"

using namespace std;

struct song{
    int id;
    int score;
    int genre_count;
    int genre_id[10];
    string title;
    string artist;
    string link;
    string description;
};

song songList[1000];
int songCount = 0;
int idMax = -1;

struct comment{
    int song_id;
    int user_id;
    string text;
};

comment commentList[1000];
int commentCount = 0;

/*void addSong(int id, int score, string title, string artist, string link, string description)
{
    songList[id].id = id;
    songList[id].score = score;
    songList[id].title = title;
    songList[id].artist = artist;
    songList[id].link = link;
    songList[id].description = description;
    songCount++;
}*/


void addSong(int id, int score, int genre_count, int genres[], string title, string artist, string link, string description)
{
    int k = songCount;
    songList[k].id = id;
    songList[k].score = score;
    songList[k].genre_count = genre_count;
    for (int i=0; i<genre_count; i++) songList[k].genre_id[i] = genres[i];
    songList[k].title = title;
    songList[k].artist = artist;
    songList[k].link = link;
    songList[k].description = description;
    songCount++;
}

void loadSongs ()
{
    ifstream f;
    songCount = 0;
    f.open("songs.txt");
    int id, score, genre_count;
    int genres[10];
    string title, artist, link, description;

    while ( f>>id>>score>>title>>artist>>link>>description>>genre_count )
    {
        if (id > idMax) idMax = id;
        title = noUnderscore(title);
        artist = noUnderscore(artist);
        description = noUnderscore(description);
        for (int i=0; i<genre_count; i++) f >> genres[i];
        addSong(id, score, genre_count, genres, title, artist, link, description);
    }
    f.close();
}

int deleteSong (int id)
{
    int index = -1;

    for (int i=0; i<songCount; i++)
        if ( songList[i].id == id )
        {
            index = i;
            i = songCount + 1;
        }

    if (index == -1) return -1;

    for (int i = index + 1; i<=songCount; i++)
        songList[i-1] = songList[i];

    songCount--;
    return 1;
}

void saveSongs ()
{
    ofstream g;
    g.open("songs.txt", ios::out | ios::trunc);

    for (int i=0; i<songCount; i++)
    {
        song &s = songList[i];
        g << s.id << ' ' <<s.score<<' '<<underscore(s.title)<<' '<<underscore(s.artist)<<' '<<s.link<<' '<<underscore(s.description)<<' '<<s.genre_count<<' ';
        for (int j=0; j<s.genre_count; j++) g << s.genre_id[j] << ' ';
        g << '\n';

    }

    g.close();

}

void displaySong_simple(int id, int nr)
{
        cout << nr << ". ";
        cout << songList[id].artist << " - ";
        cout << songList[id].title;
        cout << " (ID: " << songList[id].id;
        cout << ", Score: " << songList[id].score;
        cout << ") " << endl;
}

void displaySong_detailed(int id, int nr)
{
        cout << nr << "." << endl;
        cout << "Title: " << songList[id].title << endl;
        cout << "Artist: " << songList[id].artist << endl;
        cout << "ID: " << songList[id].id << endl;
        cout << "Genres: ";
        for (int j=0; j < songList[id].genre_count-1; j++ ) cout << genreList[ songList[id].genre_id[j] ].name << ", ";
        cout << genreList[ songList[id].genre_id[songList[id].genre_count-1] ].name << endl;
        cout << "Score: " << songList[id].score << endl;
        cout << "Link: " << songList[id].link << endl;
        cout << "Description: " << songList[id].description << endl;
        cout << endl;
}

void displaySongs()
{
    for ( int i=0; i<songCount; i++ )
        displaySong_simple(i, i+1);
}



void raiseRank(int poz)
{
    songList[poz].score++;
    song aux = songList[poz];
    int j = poz-1;

    while ( songList[poz].score > songList[j].score and j>=0 )
        j--;

    for (int i = poz-1; i >= j+1; i--)
        songList[i+1] = songList[i];

    songList[j+1] = aux;

}

int isSongID(int id)
{
    for (int i=0; i<songCount; i++)
        if ( songList[i].id == id )
        return i;

    return -1;
}

int vote(int id)
{
    for (int i=0; i<songCount; i++)
        if ( songList[i].id == id )
        {
            raiseRank(i);
            return 1;
        }
    return 0;
}

void loadComments()
{
    ifstream f;
    f.open("comments.txt");
    int user_id, song_id;
    string text;
    commentCount = 0;

    while (f>>song_id>>user_id>>text)
    {
        commentList[commentCount].song_id = song_id;
        commentList[commentCount].user_id = user_id;
        commentList[commentCount].text = noUnderscore(text);
        commentCount++;
    }
}

void saveComments()
{
    ofstream g;
    g.open("comments.txt", ios::out | ios::trunc);

    for (int i=0; i<commentCount; i++)
    {
        g <<  commentList[i].song_id << ' ' << commentList[i].user_id << ' ' << underscore(commentList[i].text) << '\n';
    }
}

void addComment(int song, int user, string text)
{
    commentList[commentCount].song_id = song;
    commentList[commentCount].user_id = user;
    commentList[commentCount].text = text;
    commentCount++;
}

void displayComments(int song_id)
{
    for (int i=0; i<commentCount; i++)
        if ( commentList[i].song_id == song_id )
            cout << commentList[i].text << endl;
}


