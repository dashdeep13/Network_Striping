rm -rf server_code client_code
gcc -o server_code server_code.c -lpthread
gcc -o client_code client_code.c -lpthread
