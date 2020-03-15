#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

using namespace std;

int genreTree[300];

string underscore( string title )
{
    string title2 = title;
    replace(title2.begin(), title2.end(), ' ', '_' ); //elimina underscore din numele genului
    return title2;
}

string noUnderscore( string title )
{
    string title2 = title;
    replace(title2.begin(), title2.end(), '_', ' ' ); //elimina underscore din numele genului
    return title2;
}

struct genre {
    int    id;
    int    parent;
    int    childCount = 0;
    int    child[20];
    string name;
};

int genreCount;
genre genreList[100];


void getChildren( genre g, int v[])
{
    if ( g.childCount == 0 ) return;

    for ( int i=0; i<g.childCount; i++ )
    {
        int j = 0;
        while ( v[j] !=0 ) j++;
        v[j] = g.child[i];
        getChildren( genreList[ g.child[i] ], v);
    }

}

void addGenre ( int id, string name, int parent )
{
    genreList[id].id = id;
    genreList[id].name = name;
    genreList[id].parent = parent;

    genre &Parent = genreList[parent];
    int &i = genreList[parent].childCount;

    i++;
    Parent.child[i-1] = id;

    genreCount++;
}

void loadGenres ()
{
    genreCount = 1;
    ifstream f("genres.txt");
    int id, parent;
    string name;

    while ( f>>id>>name>>parent )
    {
        name = noUnderscore(name);
        addGenre(id, name, parent);
    }
}

int isSubgenre(genre s, genre g)
{
    if ( s.id == 0 ) return 0;
    else
        if ( s.id == g.id )
            return ( 1 + isSubgenre(genreList[s.parent], g) );
        else
            return ( 0 + isSubgenre(genreList[s.parent], g) );
}

int getGenreID(string nume)
{
    for (int i=0; i<=genreCount; i++)
        if ( genreList[i].name == nume )
            return i;
    return -1;
}
