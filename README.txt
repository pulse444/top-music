Compilare:

Folositi fisierul Makefile sau executati in teminal urmatoarele comenzi:
g++ server.cpp -pthread -o TopMusicServer.bin
g++ main.cpp -o TopMusicClient.bin

Executare:

./TopMusicServer.bin
./TopMusicClient.bin 127.0.0.1 2908

Continut:

-fisiere sursa (.cpp sau .h)
-fisiere text pentru stocarea datelor serverului (songs.txt, genres.txt, users.txt, comments.txt)

Instructiuni:

Pentru a folosi comenzile (exceptand login si register), este nevoie mai intai sa va autentificati folosind comanda "login", sau sa creati un cont folosind comanda "register".

exemplu de cont obisnuit:
user: radu
pass: radu123

exemplu de cont administrator:
user: admin
pass: admin

Comenzi obisnuite:

   login             autentificare
   logout            deautentificare
   register          cont nou
   
   top               initiaza configuratorul pentru a afisa topul melodiilor unui anumit gen muzical
                     pentru gen si nr. melodii se poate introduce valoarea "all"
   
   add song          adauga o melodie
   add comment       adauga un comentariu la o melodie
   vote              voteaza o melodie
   genres            afiseaza genurile muzicale
   comment           adauga un comentariu la o melodie
   quit              anuleaza o comanda aflata in desfasurare
   disconnect        deconecteaza clientul de la server

Comenzi exclusive conturilor de tip administrator:

   register admin    cont nou admin, necesita introducerea parolei de sistem ("bobdylan1941")
   restrict          restrictioneaza dreptul unui utilizator de a vota
   delete song       sterge o melodie
