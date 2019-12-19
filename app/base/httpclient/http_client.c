#include "http_client.h"
#include "http_parser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h> 

const int kSelectRead	= 1 << 0;
const int kSelectWrite	= 1 << 1;
const int kSelectError	= 1 << 2;

static int http_init_flag = 0;

#ifdef WINCE
static char* strdup(const char* src)
{
	char* dst = NULL;
	int len = 0;
	if(src == NULL)
	{
		return NULL;
	}
	len = strlen(src);
	dst = (char*)malloc(len + 1);
	if(dst == NULL)
	{
		return NULL;
	}
	strcpy(dst, src);
	return dst;
}

#endif

#if FT_SUPPORT_HTTPS

static void http_ssl_free(ft_http_client_t* http)
{
	if(http->ssl != NULL)
	{
		SSL_shutdown(http->ssl);
		SSL_free(http->ssl);
		http->ssl = NULL;
	}
	if(http->ctx != NULL) 
	{
		SSL_CTX_free(http->ctx); 
		http->ctx = NULL;
	}
}

#endif

int ft_http_init()
{
#if FT_SUPPORT_HTTPS

#ifdef USEOPENSSL
	OpenSSL_add_all_algorithms();
	ERR_load_BIO_strings();
	ERR_load_crypto_strings();
	SSL_load_error_strings();
#endif

	if(SSL_library_init() < 0)
	{
		return -1;
	}
	http_init_flag = 1;
#endif

#ifdef WIN32
	{
		WORD    version = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(version, &data);
	}

#endif

	return 0;
}

void ft_http_deinit()
{
#ifdef WIN32
	{
		WSACleanup();
	}
#endif
}

ft_http_client_t* ft_http_new()
{
	ft_http_client_t* http = (ft_http_client_t*)calloc(1, sizeof(ft_http_client_t));

	return http;
}


void ft_http_destroy(ft_http_client_t* http)
{
	if(http == NULL) return;
	
	free_member(http->body);
	free_member(http->header_field);
	free_member(http->header_value);
	free_member(http->redirect_url);
	free_member(http->filename);
	close_socket(http->fd);
	close_file(http->pf);

#if FT_SUPPORT_HTTPS
	http_ssl_free(http);
#endif

	free(http);

}

int ft_http_get_error_code(ft_http_client_t* http)
{
	if(http == NULL)
	{
		return -1;
	}
	return http->error_code;
}

int ft_http_get_status_code(ft_http_client_t* http)
{
	if(http == NULL)
	{
		return -1;
	}
	return http->status_code;
}

int ft_http_set_timeout(ft_http_client_t* http, int timeout)
{
	if(http == NULL)
	{
		return -1;
	}
	if(timeout < 0) 
	{
		http->timeout = 0;
	}
	else
	{
		http->timeout = timeout;
	}

	return 0;
}

static int socket_noblocking(socket_t fd, int noblocking)
{
#ifdef WIN32
	return ioctlsocket(fd, FIONBIO, &noblocking);
#else
	int flags;
	if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
	{
		return -1;
	}
	if(noblocking)
	{
		return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	}
	else
	{
		return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
	}
#endif
	return 0;
}

static int last_error()
{
#ifdef WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
	return 0;
}

#ifdef WIN32
static const unsigned __int64 epoch = ((unsigned __int64) 116444736000000000ULL);

static int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	FILETIME    file_time;
	SYSTEMTIME  system_time;
	ULARGE_INTEGER ularge;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	ularge.LowPart = file_time.dwLowDateTime;
	ularge.HighPart = file_time.dwHighDateTime;

	tp->tv_sec = (long) ((ularge.QuadPart - epoch) / 10000000L);
	tp->tv_usec = (long) (system_time.wMilliseconds * 1000);

	return 0;
}

#endif

static int socket_select(ft_http_client_t* http, int mode, int timeout)
{
	fd_set rfd, wfd, efd;
	int r = 0;
	int error = 0;
	int remaind = timeout;
#ifdef WIN32
	int len = sizeof(error);
#else
	socklen_t len = sizeof(error);
#endif
	struct timeval tv, start, elapsed;

    struct timezone tz;

	gettimeofday(&start, &tz);
	while(remaind > 0)
	{
		 if(mode & kSelectRead)	{ FD_ZERO(&rfd); FD_SET(http->fd, &rfd); }
		 if(mode & kSelectWrite){ FD_ZERO(&wfd); FD_SET(http->fd, &wfd); }
		 if(mode & kSelectError){ FD_ZERO(&efd); FD_SET(http->fd, &efd); }

		tv.tv_sec = remaind / 1000;
		tv.tv_usec = remaind % 1000 * 1000;

		r = select(http->fd+1,
			(mode & kSelectRead)	? &rfd : NULL,
			(mode & kSelectWrite)	? &wfd : NULL,
			(mode & kSelectError)	? &efd : NULL,
			&tv);

		if( r == 0)
		{
			return -1; /* timeout */
		}

		if( r > 0) 
		{
			if(getsockopt(http->fd, SOL_SOCKET, SO_ERROR, (char*)&error, &len) == 0 && error == 0)
			{
				return 0;
			}
			else
			{
				return -1;
			}
		}

		if( r < 0 )
		{
			if(last_error() == HTTP_EINTR)
			{
				gettimeofday(&elapsed, 0);
				remaind = timeout - ((int)(elapsed.tv_sec - start.tv_sec) * 1000 + (int)(elapsed.tv_usec - start.tv_usec) / 1000);

				continue;
			}
			else
			{
				return -1;
			}
		}
	};

	return -1;
}



static int _field_value_malloc(char** str, unsigned short* cur_size, unsigned short* size, const char* at, size_t length)
{
	if(*str == NULL)
	{
#define DEFAULT_HEADER_SIZE 128
		*size = length > DEFAULT_HEADER_SIZE ? length: DEFAULT_HEADER_SIZE;
		*str = (char*)calloc(1, *size + 1);
		if(*str == NULL)
		{
			return -1;
		}
		*cur_size = 0;
	}
	else if(*cur_size + length > *size)
	{
		*size = *cur_size + length;
		*str = (char*)realloc(*str, *size + 1);
		if(*str == NULL)
		{
			return -1;
		}
	}
	memcpy(*str + *cur_size, at, length);
	*cur_size += length;
	(*str)[*cur_size] = '\0';
	return 0;
}

static int parser_field_value(ft_http_client_t* http)
{
	if(http->cur_value_size > 0 && 
		http->cur_field_size > 0 &&
		http->header_field && http->header_value)
	{
		if(_stricmp(http->header_field, "Location") == 0)
		{
			free_member(http->redirect_url);
			http->redirect_url = _strdup(http->header_value);

			http->redirect = 1; return -1;
		}
		else if(_stricmp(http->header_field, CONTENT_LENGTH_STR) == 0)
		{
			http->content_length = atol(http->header_value);
		}
        else if(_stricmp(http->header_field, "Content-Range") == 0)
        {
            sscanf(http->header_value, "bytes %*d-%*d/%lu", &http->total_length);
        }
		else
		{
			/* extract other header field value */
		}
		http->cur_field_size = 0;
		http->cur_value_size = 0;
	}

	return 0;
}


static int on_header_field_cb(http_parser* parser, const char *at, size_t length)
{
	ft_http_client_t* http = (ft_http_client_t*)parser->data;
    
    http->status_code = parser->status_code;    
	if(http->parser_statue == PARSERD_VALUE)
	{
		if( parser_field_value(http) != 0)
		{
			return -1;
		}
	}
	http->parser_statue = PARSERD_FIELD;
	return _field_value_malloc(&http->header_field, &http->cur_field_size, &http->field_size, at, length);
}

static int on_header_value_cb(http_parser* parser, const char *at, size_t length)
{
	ft_http_client_t* http = (ft_http_client_t*)parser->data;

    http->status_code = parser->status_code;
	if(http->parser_statue == PARSERD_FIELD || http->parser_statue == PARSERD_VALUE)
	{
		http->parser_statue = PARSERD_VALUE;
		return _field_value_malloc(&http->header_value, &http->cur_value_size, &http->value_size, at, length);
	}
	return 0;
}

static int on_status_cb(http_parser* parser, const char *at, size_t length)
{
	ft_http_client_t* http = (ft_http_client_t*)parser->data;
    
	http->status_code = parser->status_code;
	return 0;
}

static int on_headers_complete_cb(http_parser* parser)
{
	ft_http_client_t* http = (ft_http_client_t*)parser->data;

    http->status_code = parser->status_code;
	if(parser_field_value(http) != 0)
	{
		return -1;
	}
	free_member(http->header_field);
	free_member(http->header_value);
	http->parser_statue = PARSERD_BODY;
	http->cur_field_size = http->cur_value_size = 0;
	return 0;
}

static int on_download_file_cb(http_parser* parser, const char *data, size_t len)
{
	ft_http_client_t* http = (ft_http_client_t*)parser->data;
    http->status_code = parser->status_code;
    
	if(http->status_code >= 200 && http->status_code <= 299)
	{
		if(!http->pf && http->filename)
		{
			http->pf = fopen(http->filename, "wb");
		}

		if(!http->pf)
		{
			return -1;
		}

        if (fwrite(data, 1, len, http->pf) != len)
        {
            return -1;
        }

        http->receive_length += len;
		if( http->recv_cb && 
            http->recv_cb(http->status_code, http->receive_length, http->content_length, http->user) != 0)
		{
			return -1;
		}
	}

	return 0;
}


static int on_download_range_cb(http_parser *parser, const char *data, size_t len)
{
    ft_http_client_t *http = (ft_http_client_t*)parser->data;
    http->status_code = parser->status_code;

    if (http->status_code == 206)
    {
        if (!http->pf && http->filename)
        {
            http->pf = fopen(http->filename, "ab");
        }

        if (!http->pf)
        {
            return -1;
        }

        if (fseek(http->pf, http->receive_length, SEEK_SET) != 0 ||
            fwrite(data, 1, len, http->pf) != len)
        {
            return -1;
        }

        http->receive_length += len;

        if (http->recv_cb &&
            http->recv_cb(http->status_code, http->receive_length, http->total_length, http->user) != 0)
        {
            return -1;
        }
            
    }
    return 0;
}

static int on_body_cb(http_parser* parser, const char *at, size_t length)
{
	ft_http_client_t* http = (ft_http_client_t*)parser->data;

    http->status_code = parser->status_code;
	if(http->body == NULL)
	{
		
		if(http->content_length > 0)
		{
			/* ֱ��һ�η����㹻�ռ䣬�����η��� */
			http->body = (char*)calloc(1, http->content_length + 1);
		}
		else
		{
			http->body = (char*)calloc(1, length + 1);
		}
	}
	else
	{
		if(http->content_length <= 0)
		{
			http->body = (char*)realloc(http->body, http->body_len + length + 1);
		}
	}
	if(http->body == NULL)
	{
		return -1;
	}
	memcpy(http->body + http->body_len, at, length);
	http->body_len += length;


	return 0;
}

static int on_message_complete_cb(http_parser* parser)
{
    ft_http_client_t* http = (ft_http_client_t*)parser->data;

    http->status_code = parser->status_code;
	return 0;
}

static int http_check_error(ft_http_client_t* http, int mode, int ret)
{
	int error_code;
#if FT_SUPPORT_HTTPS
	if(http->proto_type == PROTO_HTTPS)
	{
		int error =  SSL_get_error(http->ssl, ret);
		if(SSL_ERROR_ZERO_RETURN == error)
		{
			return -1;
		}
		else if(error == SSL_ERROR_WANT_WRITE || 
			error == SSL_ERROR_WANT_READ)
		{
			return 0;
		}
		else if(SSL_ERROR_SYSCALL == error)
		{
			goto check_select;
		}
		else
		{
			return -1;
		}
	}
	check_select:
#endif
	error_code = last_error();
	if(error_code == HTTP_EINTR)
	{
		return 0;
	}
	else if(error_code == HTTP_EINPROGRESS || error_code == HTTP_EWOULDBLOCK)
	{
		if(socket_select(http, mode, http->timeout) == 0)
		{
			return 0;
		}
	}
	return -1;
}

static int http_read_write(ft_http_client_t* http, const char* data, int len, int read)
{
	int n = 0, r = 0;

	do
	{
		if(http->exit == 1)
		{
			return -1;
		}

		if(http->proto_type == PROTO_HTTPS)
		{
#if FT_SUPPORT_HTTPS
			r = read ? SSL_read(http->ssl, (char*)data + n, len - n) : SSL_write(http->ssl, data + n, len - n);
#endif
		}
		else
		{
			r = read ? recv(http->fd, (char*)data + n, len - n, 0) : send(http->fd, data + n, len - n, 0);
		}
		
		if(r > 0)
		{
			n += r;
		}
		else if(r == 0)
		{
			return n;
		}
		else
		{
			if(http_check_error(http, read ? kSelectRead : kSelectWrite, r) == 0)
			{
				continue;
			}
			return -1;
		}
	}while(n < len);

	return n;
}

#if FT_SUPPORT_HTTPS

static int http_ssl_connect(ft_http_client_t* http)
{
	int ssl_ret = 0;
	int remaind = http->timeout;
	struct timeval start, elapsed;
    struct timezone tz;
	http->ctx = SSL_CTX_new(SSLv23_client_method());
	if(http->ctx == NULL)
	{
		return -1;
	}
	http->ssl = SSL_new(http->ctx);
	if(http->ssl == NULL)
	{
		return -1;
	}
	if(SSL_set_fd(http->ssl, http->fd) == 0)
	{
		return -1;
	}

	gettimeofday(&start, &tz);

	do
	{
		ssl_ret = SSL_connect(http->ssl);

		gettimeofday(&elapsed, 0);
		remaind = http->timeout - ((int)(elapsed.tv_sec - start.tv_sec) * 1000 + (int)(elapsed.tv_usec - start.tv_usec) / 1000);

		if(ssl_ret > 0)
		{
			return 0;
		}
		else
		{
			if(remaind <= 0)
			{
				return -1;
			}
			else if(http_check_error(http, kSelectRead+kSelectWrite, ssl_ret) == 0)
			{
				continue;
			}
			return -1;
		}
	}while(1);

	return -1;
}

#endif

static int http_connect_host(ft_http_client_t* http, const char* url, struct http_parser_url* u)
{
	struct sockaddr_in sin;
	char host[256] = {0};
	int r;
	unsigned short port = 80;

	if(u->field_set & (1 << UF_SCHEMA))
	{
		if(_strnicmp("http://", url + u->field_data[UF_SCHEMA].off, 7) == 0)
		{
			port = 80; http->proto_type = PROTO_HTTP;
		}
#if FT_SUPPORT_HTTPS
		else if(_strnicmp("https://", url + u->field_data[UF_SCHEMA].off, 8) == 0)
		{
			port = 443; http->proto_type = PROTO_HTTPS;
		}
#endif
		else
		{
			return ERR_URL_INVALID_PROTO;
		}
	}

	if(!(u->field_set & (1 << UF_HOST)))
	{
		return ERR_URL_INVALID_HOST;
	}

	if(u->field_set & (1 << UF_PORT))
	{
		port = (unsigned short)atoi(url + u->field_data[UF_PORT].off);
	}
	
	memset(&sin, 0, sizeof(struct sockaddr_in));
	memcpy(host, url + u->field_data[UF_HOST].off, u->field_data[UF_HOST].len);
	if(host[0] >= '0' && host[0] <= '9')
	{
		sin.sin_addr.s_addr = (unsigned long)inet_addr(host);
	}
	else
	{
		struct hostent* he = gethostbyname(host);
		if(he == NULL || he->h_addrtype != AF_INET)
		{
			return ERR_URL_RESOLVED_HOST;
		}
		sin.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
	}

	if(sin.sin_addr.s_addr == INADDR_NONE)
	{
		return ERR_URL_RESOLVED_HOST;
	}

	sin.sin_port = htons(port);
	sin.sin_family = AF_INET;

	close_socket(http->fd);

	http->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(http->fd == HTTP_INVALID_SOCKET)
	{
		return ERR_SOCKET_CREATE;
	}
	
	{
		struct linger linger;
		linger.l_onoff = 1;
		linger.l_linger = 0;  /* �ر�close wait */

		if(setsockopt(http->fd,SOL_SOCKET, SO_LINGER,(const char *) &linger,sizeof(linger)) != 0)
		{
			return ERR_SOCKET_SET_OPT;
		}
		if(socket_noblocking(http->fd, 1) != 0)
		{
			return ERR_SOCKET_NOBLOCKING;
		}
	}

	do
	{
		r = connect(http->fd, (struct sockaddr*)&sin, sizeof(sin));
		if(r < 0)
		{
			int error = last_error();
			if(error == HTTP_EINTR)
			{
				continue;
			}
			else if( error == HTTP_EINPROGRESS || error == HTTP_EWOULDBLOCK ||  error == HTTP_EALREADY)
			{
				if(socket_select(http, kSelectWrite, http->timeout) == 0)
				{
					break;
				}
				else
				{
					return ERR_SOCKET_CONNECT_TIMEOUT;
				}
			}
			return ERR_SOCKET_CONNECT;
		}
	}while(1);

#if FT_SUPPORT_HTTPS
	if(http->proto_type == PROTO_HTTPS)
	{
		if(http_ssl_connect(http) != 0)
		{
			return ERR_SSL_CONNECT;
		}
	}
#endif

	return ERR_OK;
}

static int http_internal_sync_request(ft_http_client_t* http, const char* url, 
											  const char* post_data, int post_data_len,
											  const char* user_header, int user_header_len)
{
	http_parser_settings parser_setting;
	struct http_parser parser;
	struct http_parser_url u;
	int r = 0;
	int parsed = 0;
	//unsigned short port = 80;

	free_member(http->body);
	free_member(http->header_field);
	free_member(http->header_value);
	close_socket(http->fd);
	

#if FT_SUPPORT_HTTPS
	http_ssl_free(http);
#endif

	http->redirect = 0;
	http->body_len = 0;
    http->receive_length = 0;
	http->content_length = 0;
	http->cur_field_size = 0;
	http->cur_value_size = 0;
	http->field_size = 0;
	http->value_size = 0;
	http->parser_statue = 0;
	http->error_code = 0;

	if(http->timeout == 0) http->timeout = 15000;

	if( http_parser_parse_url(url, strlen(url), 0, &u) != 0 )
	{
		return (http->error_code = ERR_URL_INVALID);
	}


	r = http_connect_host(http, url, &u);
	if(r != ERR_OK)
	{
		return http->error_code = r;
	}
	

#define CHECK(nwirte) \
	if((nwirte) <= 0) { return http->error_code = ERR_SOCKET_WRITE; }

	if(http->method == M_GET)
	{
		CHECK(http_read_write(http, "GET ", 4, 0));
	}
	else if(http->method == M_POST)
	{
		CHECK(http_read_write(http, CONTENT_POST_STR, strlen(CONTENT_POST_STR), 0));
		printf("send===============>%s\n",CONTENT_POST_STR);
	}
#if 0
	if(u.field_set & (1 << UF_PATH))
	{
		CHECK(http_read_write(http, url + u.field_data[UF_PATH].off, u.field_data[UF_PATH].len, 0));
		printf("send===============>u.field_data[UF_PATH].off = %d\n",u.field_data[UF_PATH].off);
	}
	else
	{
		CHECK(http_read_write(http, "/", 1, 0));
		printf("send===============>%s\n","/");
	}

	if(u.field_set & (1 << UF_QUERY))
	{
		CHECK(http_read_write(http, "?", 1, 0));
		CHECK(http_read_write(http, url + u.field_data[UF_QUERY].off, u.field_data[UF_QUERY].len, 0));
		printf("send===============>%s\n","?");
		printf("send===============>u.field_data[UF_QUERY].off = %d\n",u.field_data[UF_QUERY].off);
	}
   #endif 
	CHECK(http_read_write(http, CONTENT_HOST_STR, strlen(CONTENT_HOST_STR), 0));
	//CHECK(http_read_write(http, url + u.field_data[UF_HOST].off, u.field_data[UF_HOST].len, 0));
	CHECK(http_read_write(http, CONTENT_ENCODE_STR, strlen(CONTENT_ENCODE_STR), 0));
	CHECK(http_read_write(http, CRLF  ACCEPT_STR  DEFAULT_USER_AGENT_STR  CONNECT_STR,
		2 + strlen(CONNECT_STR) + strlen(ACCEPT_STR) + strlen(DEFAULT_USER_AGENT_STR), 0));
	printf("send===============>%s\n",CONTENT_HOST_STR);
	printf("send===============>%s\n",CONTENT_ENCODE_STR);
	printf("send===============>%s\n",CRLF  ACCEPT_STR  DEFAULT_USER_AGENT_STR  CONNECT_STR);
#if 0
    printf("SEND:\r\n----------------\r\nHTTP/1.1\r\nHost: %*s\r\n%s\r\n", u.field_data[UF_HOST].len,
        url + u.field_data[UF_HOST].off, ACCEPT_STR  DEFAULT_USER_AGENT_STR  CONNECT_STR);
#endif

	if(user_header && user_header_len > 0)
	{
		CHECK(http_read_write(http, user_header, user_header_len, 0));
	}

    if(http->method == M_POST && post_data && post_data_len > 0)
	{
		char len_data[256] = {0};
		int n = sprintf(len_data, "%s:%d\r\n", CONTENT_LENGTH_STR, post_data_len);
		CHECK(http_read_write(http, len_data, n, 0));
		printf("send===============>%s\n",len_data);
	}

    CHECK(http_read_write(http, CRLF, 2, 0));
	printf("send===============>%s\n",CRLF);

	if(http->method == M_POST && post_data && post_data_len > 0)
	{
		CHECK(http_read_write(http, post_data, post_data_len, 0));
		printf("send===============>%s\n",post_data);
	}

	memset(&parser_setting, 0, sizeof(parser_setting));
	parser_setting.on_body = http->download ? on_download_file_cb : on_body_cb;
	parser_setting.on_message_complete = on_message_complete_cb;
	parser_setting.on_header_field = on_header_field_cb;
	parser_setting.on_header_value = on_header_value_cb;
	parser_setting.on_headers_complete = on_headers_complete_cb;
	parser_setting.on_status = on_status_cb;

	http_parser_init(&parser, HTTP_RESPONSE);
	parser.data = http;
    
	do
	{
		int nread;

		if(http->download == 0 && http->parser_statue == PARSERD_BODY && http->body && http->content_length > 0)
		{
			nread = http_read_write(http, http->body+http->body_len, http->content_length - http->body_len, 1);
			if(nread > 0)
			{
				http->body_len += nread;
                printf("BODY\r\n----------------\r\n%*s\r\n", http->body_len, http->body);
                break;
			}

			//todo: if iss download, use mmap to write data
		}
		else
		{
			char buf[RECV_BUF_SIZE + 1] = {0};

			nread = http_read_write(http, buf, RECV_BUF_SIZE, 1);
            printf("RECV(%d)\r\n----------------\r\n%.*s\r\n", nread, nread, buf);

			if(nread > 0)
			{
				parsed = http_parser_execute(&parser, &parser_setting, buf, nread);

                printf("PARS(c=%d, l=%d)\r\n----------------\r\n", parser.status_code, parsed);
				if(http->redirect)
				{
					break;
				}

				if(parsed != nread)
				{
                    printf("read error, %d, %d\r\n", parsed, nread);
					return http->error_code = ERR_PARSE_REP;
				}
			}
		}

		if(nread == 0)
		{
			break;
		}
		else if(nread < 0)
		{
			return http->error_code = ERR_SOCKET_READ;
		}

	} while (1);

	if(http->redirect == 1)
	{
		return http_internal_sync_request(http, http->redirect_url, post_data, post_data_len, user_header, user_header_len);
	}
	else
	{
		if(http->download)
		{
			if(http->pf) 
			{
				fclose(http->pf);
				http->pf = NULL;
			}
		}
		if(http->body && http->body_len > 0)
		{
			http->body[http->body_len] = '\0';
		}
	}

	return http->error_code;

}

static int http_internal_sync_getrange(
    ft_http_client_t* http, 
    const char* url)
{
	http_parser_settings parser_setting;
	struct http_parser parser;
	struct http_parser_url u;
	int r = 0, index = 0;
	//unsigned short port = 80;
    char range[128];

	free_member(http->body);
	free_member(http->header_field);
	free_member(http->header_value);
	close_socket(http->fd);

#if FT_SUPPORT_HTTPS
	http_ssl_free(http);
#endif

	http->redirect = 0;
	http->body_len = 0;
	http->cur_field_size = 0;
	http->cur_value_size = 0;
	http->field_size = 0;
	http->value_size = 0;
	http->parser_statue = 0;
	http->error_code = 0;

	if(http->timeout == 0) http->timeout = 15000;

	if( http_parser_parse_url(url, strlen(url), 0, &u) != 0 )
	{
		return (http->error_code = ERR_URL_INVALID);
	}


	r = http_connect_host(http, url, &u);
	if(r != ERR_OK)
	{
		return http->error_code = r;
	}
	

#define CHECK(nwirte) \
	if((nwirte) <= 0) { return http->error_code = ERR_SOCKET_WRITE; }

    CHECK(http_read_write(http, "GET ", 4, 0));

	if(u.field_set & (1 << UF_PATH))
	{
		CHECK(http_read_write(http, url + u.field_data[UF_PATH].off, u.field_data[UF_PATH].len, 0));
	}
	else
	{
		CHECK(http_read_write(http, "/", 1, 0));
	}

	if(u.field_set & (1 << UF_QUERY))
	{
		CHECK(http_read_write(http, "?", 1, 0));
		CHECK(http_read_write(http, url + u.field_data[UF_QUERY].off, u.field_data[UF_QUERY].len, 0));
	}
    
	CHECK(http_read_write(http, " HTTP/1.1\r\nHost: ", 17, 0));
	CHECK(http_read_write(http, url + u.field_data[UF_HOST].off, u.field_data[UF_HOST].len, 0));
	CHECK(http_read_write(http, CRLF  ACCEPT_STR  DEFAULT_USER_AGENT_STR  CONNECT_STR,
		2 + strlen(CONNECT_STR) + strlen(ACCEPT_STR) + strlen(DEFAULT_USER_AGENT_STR), 0));

    sprintf(range, "Range: bytes=%lu-\r\n", http->receive_length);
    CHECK(http_read_write(http, range, strlen(range), 0));
#if 0
    printf("SEND:\r\n----------------\r\nHTTP/1.1\r\nHost: %*s\r\n%s\r\n", u.field_data[UF_HOST].len,
        url + u.field_data[UF_HOST].off, ACCEPT_STR  DEFAULT_USER_AGENT_STR  CONNECT_STR);
#endif

    CHECK(http_read_write(http, CRLF, 2, 0));

	memset(&parser_setting, 0, sizeof(parser_setting));
	parser_setting.on_body = on_download_range_cb;
	parser_setting.on_message_complete = on_message_complete_cb;
	parser_setting.on_header_field = on_header_field_cb;
	parser_setting.on_header_value = on_header_value_cb;
	parser_setting.on_headers_complete = on_headers_complete_cb;
	parser_setting.on_status = on_status_cb;

	http_parser_init(&parser, HTTP_RESPONSE);
	parser.data = http;
    
	do
	{
		char buf[RECV_BUF_SIZE + 1] = {0};
		int nread = http_read_write(http, buf, RECV_BUF_SIZE, 1);

        printf("RECV(%d)\r\n----------------\r\n", nread);
        if (index++ == 0)
        {
            printf("%.512s\r\n", buf);
        }

        if (!nread)
        {
            break;
        }

		if (nread > 0)
		{
			int parsed = http_parser_execute(&parser, &parser_setting, buf, nread);

            printf("PARS(c=%d, l=%d)\r\n----------------\r\n", parser.status_code, parsed);

            if(http->redirect)
			{
				break;
			}

			if(parsed != nread)
			{
                printf("read error, %d, %d\r\n", parsed, nread);
				return http->error_code = ERR_PARSE_REP;
			}
		}
		else
		{
			return http->error_code = ERR_SOCKET_READ;
		}

	} while (1);

	if(http->pf) 
	{
		fclose(http->pf);
		http->pf = NULL;
	}

	return http->error_code;

}

const char* ft_http_sync_get(ft_http_client_t* http, const char *url)
{
	if(http == NULL)
	{
		return NULL;
	}

	http->method = M_GET;
	http->download = 0;

	http->error_code = http_internal_sync_request(http, url, 0, 0, 0, 0);

	return http->body;
}

const char* ft_http_sync_post(ft_http_client_t *http, const char *url, const char *data, int len)
{
    if(http == NULL)
	{
		return NULL;
	}

	if(!http_init_flag)
	{
		//ft_http_init();
		http_init_flag = 1;
	}

	http->method = M_POST;
	http->download = 0;

	http->error_code = http_internal_sync_request(http, url, data, len, 0, 0);

	return http->body;
}


const char* ft_http_sync_post_file(ft_http_client_t* http, const char* url, const char* filepath)
{
	//static const char* boundary = "-------------------------87142694621188";

	return NULL;
}

int ft_http_sync_download_file(ft_http_client_t* http, const char* url, const char* filepath)
{
    int retry;
    
	if(http == NULL)
	{
		return -1;
	}

	http->method = M_GET;
	http->download = 1;
	
	free_member(http->filename);
	if(filepath != NULL)
	{
		http->filename = _strdup(filepath);

		if(http->filename == NULL)
		{
			return http->error_code = ERR_OUT_MEMORY;
		}
	}

    for (http->content_length = 0, retry = 0; !http->content_length && retry < 10; retry++)
    {
        printf(" Downloading \"%s\", try %d\r\n", filepath, retry + 1);
        if(http_internal_sync_request(http, url, 0, 0, 0, 0) == ERR_OK)
    	{
    		return ERR_OK;
    	}
    }

    for (; retry < 10; retry++)
    {
        printf(" Downloading \"%s\", try %d\r\n", filepath, retry + 1);
        if (http_internal_sync_getrange(http, url) == ERR_OK)
        {
            return ERR_OK;
        }
    }
    
	return http->error_code;
}

int ft_http_cancel_request(ft_http_client_t* http)
{
	if(http && http->fd != HTTP_INVALID_SOCKET)
	{
		close_socket(http->fd);
	}

	return 0;
}

int ft_http_wait_done(ft_http_client_t* http)
{
	return 0;
}

int ft_http_exit(ft_http_client_t* http)
{
	if(http) http->exit = 1;

	return 0;
}

int ft_http_set_data_recv_cb(ft_http_client_t* http, data_recv_cb_t cb, void* user)
{
	if(http)
	{
		http->user = user;
		http->recv_cb = cb;
	}
	return 0;
}

