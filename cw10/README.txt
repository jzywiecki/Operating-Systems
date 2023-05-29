Przykładowe wywołanie:
Terminal 1:
 ./server 5324 /tmp/adress         - nasz serwer
Terminal 2:
./client clientname2 inet 127.0.0.1 5324  - klient łączący się przez sieć
Terminal 3:
./client clientname1 local /tmp/adress  - klient łączący się przez lokalny socket

Zastosowałem mechanizm kqueue, ponieważ epoll nie jest dostępny na systemie MacOS.
Jest on troche mniej optymalny, ale działa.

