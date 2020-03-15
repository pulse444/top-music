#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int userCount = 0;

struct user{
    int user_id;
    bool admin;
    string username;
    string password;
    int votePrivilege = 1;
    int votedSongs[100];
    int votedSongsCount = 0;
};

user users[100];

bool validUsername(string name)
{
    for (int i=0; i<userCount; i++)
        if ( users[i].username == name )
            return 0;
    return 1;
}

int getUserID(string name)
{
    for (int i=0; i<userCount; i++)
        if ( users[i].username == name )
            return i;
    return -1;
}

bool canVote(int user_id)
{
    return users[user_id].votePrivilege;
}

void newUser(string user, string pass, bool admin)
{
    users[userCount].user_id = userCount;
    users[userCount].username = user;
    users[userCount].password = pass;
    users[userCount].admin = admin;
    userCount++;

}

void loadUsers()
{
    userCount = 0;
    ifstream f;
    f.open("users.txt");
    int user_id, song_count;
    int admin, votepriv;
    string user, pass;

    while (f>>user_id>>admin>>user>>pass>>votepriv>>song_count)
    {
        int k = userCount;
        users[k].user_id = user_id;
        users[k].admin = admin;
        users[k].username = user;
        users[k].password = pass;
        users[k].votePrivilege = votepriv;
        users[k].votedSongsCount = song_count;

        for (int i = 0; i < song_count; i++)
            f >> users[k].votedSongs[i];

        userCount++;
    }
}

void saveUsers()
{
    ofstream f;
    f.open("users.txt", ios::out | ios::trunc);

    for (int i=0; i<userCount; i++)
    {
        user &u = users[i];
        f << u.user_id << ' ' << u.admin << ' ' << u.username << ' ' << u.password << ' ' << u.votePrivilege << ' ' <<u.votedSongsCount;

        for (int j = 0; j < u.votedSongsCount; j++)
            f << ' ' << u.votedSongs[j];
        f << endl;
    }

    f.close();
}

