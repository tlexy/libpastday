#include "socket_util.h"
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

namespace SocketUtil
{
	void setNonBlockAndCloseOnExec(int sockfd)
	{
    	// non-block
    	int flags = ::fcntl(sockfd, F_GETFL, 0);
		flags |= O_NONBLOCK;
		int ret = ::fcntl(sockfd, F_SETFL, flags);
	
		// close-on-exec
		flags = ::fcntl(sockfd, F_GETFD, 0);
    	flags |= FD_CLOEXEC;
  		ret = ::fcntl(sockfd, F_SETFD, flags);
  		(void)ret;
	}

	void perrExit(const char* str)
	{
		perror(str);
		exit(1);
	}

	int Socket(int family, int type, int protocol, bool nonblock)
	{
		int n;
		if ((n = socket(family, type, protocol)) < 0)
		{
			perrExit("socket error");
		}
		if (nonblock)
		{
			setNonBlockAndCloseOnExec(n);
		}
		return n;
	}

	void Bind(int sockfd, struct sockaddr* sa, socklen_t salen)
	{
		if (bind(sockfd, sa, salen) < 0)
		{
			perrExit("bind error");
		}
	}

	void Bind(int sockfd, IpAddress& ipaddr)
	{
		if (bind(sockfd, ipaddr.rawAddressPtr(), ipaddr.addressLength()) < 0)
		{
			perrExit("bind error");
		}
	}

	void Connect(int sockfd, struct sockaddr* sa, socklen_t salen)
	{
		if (connect(sockfd, sa, salen) < 0)
		{
			perrExit("connect error");
		}
	}

	void Connect(int sockfd, IpAddress& ipaddr)
	{
		if (connect(sockfd, ipaddr.rawAddressPtr(), ipaddr.addressLength()) < 0)
		{
			perrExit("connect error");
		}
	}

	int Accept(int sockfd, struct sockaddr* r_sa, socklen_t* r_salen, bool flag)
	{
		int connfd = accept(sockfd, r_sa, r_salen);
		if (connfd < 0)
		{
			int Error = errno;
			if (!(Error == EAGAIN
			|| Error == EWOULDBLOCK
			|| Error == EINTR
			|| Error == EPROTO
			|| Error == EPERM
			|| Error == EMFILE))
			{
				//errno = Error;???
				perrExit("accept error");
			}
		}
		if (flag)
		{
			setNonBlockAndCloseOnExec(connfd);
		}
		return connfd;
	}

	int Accept(int sockfd, IpAddress& ipaddr, bool flag)
	{
		socklen_t len = ipaddr.addressLength();
		int connfd = accept(sockfd, ipaddr.rawAddressPtr(), &len);
		if (connfd < 0)
		{
			int Error = errno;
			if (!(Error == EAGAIN
			|| Error == EWOULDBLOCK
			|| Error == EINTR
			|| Error == EPROTO
			|| Error == EPERM
			|| Error == EMFILE))
			{
				perrExit("accept error");
			}
		}
		if (flag)
		{
			setNonBlockAndCloseOnExec(connfd);
		}
		return connfd;
	}

	void Listen(int sockfd, int max)
	{
		int ret = listen(sockfd, max);
		if (ret < 0)
		{
			perrExit("listen error");
		}
	}

	ssize_t Read(int fd, void* buf, size_t nbytes)
	{
		ssize_t n;
	again:
		if ((n = read(fd, buf, nbytes)) < 0)
		{
			if (errno == EINTR)
			{
				goto again;
			}
			else
			{
				return -1;
			}
		}
		return n;
	}

	ssize_t Write(int fd, const void* buf, size_t nbytes)
	{
		ssize_t n;
	again:
		if ((n = write(fd, buf, nbytes) < 0))
		{
			if (errno == EINTR)
			{
				goto again;
			}
			else
			{
				return -1;
			}
		}
		return n;
	}
	
	void Close(int fd)
	{
		if (close(fd) < 0)
		{
			perrExit("close error");
		}
	}

	void ShutdownWrite(int sockfd)
	{
		if (shutdown(sockfd, SHUT_WR) < 0)
		{
			//perrExit("shut down error");
			perror("shut down error");
		}
	}
	



}
