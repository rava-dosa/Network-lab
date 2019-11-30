gcc -w -g server.c -o server
gcc -w -g client.c -o client
cp server 1server/
./server 5006 &
./client 5006 