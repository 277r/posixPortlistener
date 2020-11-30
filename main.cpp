#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

int main()
{
	// create socket
	int listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener == -1)
	{
		std::cout << "oh no in stage creating socket";
		return -1;
	}
	// bind socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(49152);
	inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

	if (bind(listener, (sockaddr *)&hint, sizeof(hint)) == -1)
	{
		std::cout << "oh no in stage bind socket";
		return -2;
	}
	// mark socket for listening
	if (listen(listener, SOMAXCONN) == -1)
	{
		std::cout << "oh no in stage listening";
		return -3;
	}
	// make it accept call
	sockaddr_in client;
	socklen_t clientSize = sizeof(client);
	char host[NI_MAXHOST];
	char svc[NI_MAXSERV];

	int clientSock = accept(listener, (sockaddr *)&client, &clientSize);
	if (clientSock == -1)
	{
		std::cout << "oh no in stage accepting call";
		return -4;
	}
	close(listener);

	memset(host, 0, NI_MAXHOST);
	memset(svc, 0, NI_MAXSERV);

	int result = getnameinfo((sockaddr *)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
	if (result)
	{
		std::cout << "connected on " << host << '\n' << svc;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << "connected on " << host << '\n' << ntohs(client.sin_port);
	}

	char buffer[4096];
	while (true){
		// clear buffer
		memset(buffer, 0, 4096);
		
		// wait for incoming
		int bytesRecieved = recv(clientSock, buffer, 4096, 0);

		// handle incoming
		if (bytesRecieved == -1){
			std::cout << "oh no in stage recieve shit";
			break;
		}
		if (bytesRecieved == 0){
			std::cout << "disconnecte";
			break;
		}

		// recieve here
		std::cout << std::string(buffer, 0, bytesRecieved);

		// go out
		send(clientSock, buffer, bytesRecieved + 1, 0);
	}
	close(clientSock);

	return 0;
}