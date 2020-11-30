

// it works (not really)

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

class mdata
{
public:
	char *data = new char[4097];
	int dataSize;
};

bool startsWith(std::string &data, std::string testdata){
	for (int i = 0; i < data.size() && i < testdata.size(); i++){
		if (data[i] != testdata[i])
		{
			return false;
		}
	}
	return true;
}
bool contains(std::string &data, std::string testdata)
{
	bool thingy = false;
	for (int i = 0; i < data.size(); i++)
	{
		for (int ii = 0; ii < testdata.size() && thingy == false; ii++){
			if (data[i] != testdata[ii])
			{
				thingy = true;
			}
			thingy = false;
		}
		}
	return true;
}

mdata handleRequest(int dataSize, char data[4096])
{
	int i = 0;
	bool reading = true;
	std::string sdata = std::string(data, dataSize);
	std::string currentline;
	char *returndata = new char[4097];
	memset(returndata, 0, 4097);

	//returndata = "HTTP/1.1 200 OK\n\nhello world";
	std::cout << std::string(data, dataSize) << '\n';

	int contentsize;
	std::string contentSizeButSTR;
	while (reading)
	{
		// get current line
		for (i; i < dataSize && data[i] != '\n'; i++)
		{
			currentline += data[i];
		}
		i++;
		if (i >= dataSize)
		{
			reading = false;
		}
		// handle currentline
		//std::cout << currentline << '\n';
		if (startsWith(currentline, "POST")){
			std::cout << "starts with POST" << '\n';
		}
		else if (startsWith(currentline, "Host"))
		{
			std::cout << "starts with Host" << '\n';
		}
		else if (startsWith(currentline, "Accept"))
		{
			std::cout << "starts with Accept" << '\n';
		}
		else if (startsWith(currentline, "Content-Type"))
		{
			std::cout << "starts with Content-type" << '\n';
			// check for json
		}
		else if (startsWith(currentline, "Content-Length"))
		{
			std::cout << "starts with Content-length" << '\n';
			// store json size
			for (int i = 15; i < currentline.size(); i++){
				contentSizeButSTR += currentline[i];
			}
			contentsize = std::stoi(contentSizeButSTR);
			std::cout << "content size:" << contentsize << '\n';
		}
		else if (startsWith(currentline, "{"))
		{
			std::cout << "starts with actual data" << '\n';
			// handle json
		}
		// clear currentline
		currentline.clear();
	}

	// handle data, make sure the output has \0 as last character
	return {returndata, 29};
}

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
	std::cout << "\n\nerrors from here will be displayed differently, the errors here will be single character size, and will be ignored, if it fucks up it'll dump current job and continue the next one";
	// loop through calls
	while (true)
	{
		int clientSock = accept(listener, (sockaddr *)&client, &clientSize);
		if (clientSock == -1)
		{
			std::cout << "0";
		}
		//close(listener);

		memset(host, 0, NI_MAXHOST);
		memset(svc, 0, NI_MAXSERV);

		int result = getnameinfo((sockaddr *)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
		if (result)
		{
			// disable "connected on IP, port"
			//std::cout << "connected on " << host << '\n' << svc;
		}
		else
		{
			inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
			// disable output
			//std::cout << "connected on " << host << '\n' << ntohs(client.sin_port);
		}

		char buffer[4096];

		// clear buffer
		memset(buffer, 0, 4096);

		// wait for incoming
		int bytesRecieved = recv(clientSock, buffer, 4096, 0);

		// handle incoming
		if (bytesRecieved == -1)
		{
			std::cout << "1";
		}
		else if (bytesRecieved == 0)
		{
			// server don't care about disconnection
			//std::cout << "disconnecte";
		}

		// recieve here
		else
		{
			// disable request output
			//std::cout << std::string(buffer, 0, bytesRecieved);

			// handle request
			mdata sendBack = handleRequest(bytesRecieved, buffer);
			// go out
			send(clientSock, sendBack.data, sendBack.dataSize, 0);
			//send(clientSock, buffer, bytesRecieved + 1, 0);
		}

		close(clientSock);
	}

	return 0;
}