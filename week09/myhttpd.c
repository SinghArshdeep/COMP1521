// COMP1521 19t2 ... myhttpd.c: a very simple web server in C
//
// 2019-07-27	Jashank Jeremy <jashank.jeremy@unsw.edu.au>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#include <time.h>

#define CR   "\r"
#define LF   "\n"
#define CRLF CR LF

#define HTTP_1_0 "HTTP/1.0"
#define HTTP_1_1 "HTTP/1.1"

#define H_CONTENT_TYPE "Content-Type"
#define H_SERVER       "Server"

#define MIME_PLAIN_UTF8 "text/plain; charset=utf-8"
#define MIME_HTML_UTF8 "text/html; charset=utf-8"
#define SERVER_NAME     "cs1521-19t2-lab09/1.0"

/// To make it clear what's a socket and what's not, here's a handy
/// `typedef' which we'll use for anything that's a socket.
typedef int socket_t;

/// And to avoid saying `struct' repeatedly, here are shorthand names.
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

static socket_t server_socket_new (in_port_t port, int queue_len);
static bool handle_connection (
	socket_t client, sockaddr_in client_addr, socklen_t client_addr_len,
	int *n_requests, int *n_responses);

static void addr_to_dotted_octets (uint32_t addr, char *buf);

#define PORTUID_BASE 15000
#define PORTUID_MOD  1000

int main (void)
{
	setbuf (stdout, NULL);

	/// To stop everybody colliding sockets, we try to come up with a
	/// port number that's specific to you.
	uid_t uid = getuid ();
	in_port_t port = PORTUID_BASE + (uid % PORTUID_MOD);

	socket_t server = server_socket_new (port, 10);
	printf ("---- Listening on <http://localhost:%hu/>...\n", port);

	int n_requests = 0, n_responses = 0;

	/// Now, we invoke `accept(2)' on the socket.  It blocks until a new
	/// connection is available, then returns:
	///
	///  * `client' -- an open socket that's connected to the client and
	///    on which we can `send(2)' and `recv(2)' data;
	///
	///  * `client_addr' and `client_addr_len' -- information about the
	///    remote socket we've connected to, as a `sockaddr_in', which
	///    includes the remote address and port.
	socket_t client;
	sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof (client_addr);
	while (
		(client = accept (
			server,
			(sockaddr *) &client_addr,
			(socklen_t *) &client_addr_len)) >= 0
	) {
		handle_connection (
			client, client_addr, client_addr_len,
			&n_requests, &n_responses);
	}

	if (client < 0) warn ("accept");
	close (server);

	return EXIT_SUCCESS;
}


/// Creates a new socket on `port', with an accept queue length of
/// `queue_len'.
static socket_t server_socket_new (in_port_t port, int queue_len)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		perror("ERROR");
	sockaddr_in name = {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr = {.s_addr = INADDR_ANY}
	};

	if ((bind(sockfd, (struct sockaddr*) &name, sizeof(name))) < 0) {
		perror("ERROR");
	}

	if (listen(sockfd, queue_len) < 0) {
		perror("NOT LISTENING\n");
	} 

	return sockfd;
}


/// We've received an incoming connection, `client', at `client_addr';
/// handle that request.
static bool handle_connection (
	socket_t client, sockaddr_in client_addr, socklen_t client_addr_len,
	int *n_requests, int *n_responses)
{
	assert (client_addr_len == sizeof (sockaddr_in));

	/// Report about the address that we got connected to.
	char addr[12];
	addr_to_dotted_octets (client_addr.sin_addr.s_addr, addr);
	printf ("<--> Connection from %s:%hu!\n", addr, client_addr.sin_port);

	/// Try to receive data from the socket into `request'.]
	char request[BUFSIZ];
	ssize_t request_len;
	if ((request_len = recv (client, request, BUFSIZ, 0)) < 0) {
		warn ("recv"); close (client); return false;
	}

	/// Make sure the request text is at least NUL-terminated,
	/// before (hopefully) printing it as a string.
	///
	/// Some web browsers insist on sending compressed requests; in
	/// that case, you may find things go horribly awry, but there's
	/// not much we can do in that case.
	request[request_len] = '\0';
	printf ("<--- Request %d:\n%s\n", (*n_requests)++, request);
	char *p = request;
	strsep(&p, " ");
	char *page = strsep(&p, " ");
	char name[BUFSIZ];

	for (int i = 0; page[i] != '\0'; i++)
	{
		if (page[i] == '?')
		{

			int cnt = 0;
			for (int j = i+1; page[j] != '\0'; j++)
			{
				name[cnt] = page[j];
				cnt++;
			}
			name[cnt] = '\0';
			break;
		}
	}
	/// We always send back this response for any request.
	char response[BUFSIZ] = {
		"HTTP/1.0 200 OK" CRLF
		H_CONTENT_TYPE ": " MIME_HTML_UTF8 CRLF
		H_SERVER ": " SERVER_NAME CRLF
		CRLF
	};
	// calculate time
	time_t rawtime;
  	struct tm * timeinfo;
  	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );

	if (strcmp(page,"/") == 0)
	{
		strcat(response, "<h2>myhttpd running!</h2>");
		strcat(response,"\0");
	}
	else if (strcmp(page,"/hello") == 0)
	{
		strcat(response, "<h2>Hello!</h2>");
		strcat(response,"\0");
	}
	else if (strcmp(page, "/date") == 0){
		strcat(response, "<h2>");
		strcat(response, asctime(timeinfo));
		strcat(response,"</h2>");
		strcat(response,"\0");
	}
	else if (strcmp(page,"/nonexistent") == 0)
	{
		strcat(response, "<h2>404 Page Not Found</h2>");
		strcat(response,"\0");
	}
	else if (strncmp(page, "hello?", 6)){
		strcat(response, "<h2>Hello, ");
		strcat(response, name);
		strcat(response, "!</h2>");
		strcat(response,"\0");	
	}else
    {
        strcat(response, "<h2>404 Page Not Found</h2>");
        strcat(response,"\0");
    }

	size_t response_len = strlen (response);

	printf ("---> Response %d:\n%s\n", (*n_responses)++, response);
	if (send (client, response, response_len, 0) < (ssize_t) response_len) {
		warn ("send"); close (client); return false;
	}

	/// OK, and disconnect from the client.
	printf ("---- Closing connection.\n");
	close (client);

	return true;
}


/// Format an IPv4 address as four decimal values from 0 to 255.
static void addr_to_dotted_octets (uint32_t addr, char *buf)
{
	addr = ntohl (addr);
	snprintf (
		buf, 12, "%u.%u.%u.%u",
		addr >> 24 & 0xff,
		addr >> 16 & 0xff,
		addr >>  8 & 0xff,
		addr       & 0xff
	);
}
