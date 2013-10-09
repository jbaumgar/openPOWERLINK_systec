/****************************************************************************

  (c) SYSTEC electronic GmbH, D-08468 Heinsdorfergrund, Am Windrad 2
      www.systec-electronic.com

  Project:      openPOWERLINK

  Description:  Wrapper for BSD socket API for Linux kernel

  License:

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    3. Neither the name of SYSTEC electronic GmbH nor the names of its
       contributors may be used to endorse or promote products derived
       from this software without prior written permission. For written
       permission, please contact info@systec-electronic.com.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

    Severability Clause:

        If a provision of this License is or becomes illegal, invalid or
        unenforceable in any jurisdiction, that shall not affect:
        1. the validity or enforceability in that jurisdiction of any other
           provision of this License; or
        2. the validity or enforceability in other jurisdictions of that or
           any other provision of this License.

  -------------------------------------------------------------------------

                $RCSfile$

                $Author$

                $Revision$  $Date$

                $State$

                Build Environment:
                Dev C++ and GNU-Compiler for m68k

  -------------------------------------------------------------------------

  Revision History:

  2006/08/25 d.k.:   start of implementation

****************************************************************************/

#include <linux/version.h>
#include <linux/net.h>
#include <linux/in.h>
#include "SocketLinuxKernel.h"


/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*          G L O B A L   D E F I N I T I O N S                            */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


//---------------------------------------------------------------------------
// const defines
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// local types
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// module global vars
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// local function prototypes
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//  Kernel Module specific Data Structures
//---------------------------------------------------------------------------


//=========================================================================//
//                                                                         //
//          P U B L I C   F U N C T I O N S                                //
//                                                                         //
//=========================================================================//

//---------------------------------------------------------------------------
//
// Function:
//
// Description:
//
//
//
// Parameters:
//
//
// Returns:
//
//
// State:
//
//---------------------------------------------------------------------------

SOCKET socket(int af, int type, int protocol)
{
int rc;
SOCKET socket;

    rc = sock_create_kern(af, type, protocol, &socket);
    if (rc < 0)
    {
        socket = NULL;
        goto Exit;
    }

Exit:
    return socket;
}

int bind(SOCKET socket_p, const struct sockaddr *addr, int addrlen)
{
int rc;

    rc = socket_p->ops->bind(socket_p,
                          (struct sockaddr *)addr,
                          addrlen);

    return rc;
}

int closesocket(SOCKET socket_p)
{
    sock_release(socket_p);

    return 0;
}

int listen(SOCKET socket_p, int backlog)
{
int rc;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 10)
    rc = kernel_listen(socket_p, backlog);
#else
    rc = -1;
#endif

    return rc;
}

SOCKET accept(SOCKET socket_p, struct sockaddr *addr, int* addrlen)
{
SOCKET newsocket;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 10)
int rc;

    rc = kernel_accept(socket_p, &newsocket, 0);
    if (rc < 0)
    {
        newsocket = NULL;
    }
#else
    newsocket = NULL;
#endif

    return newsocket;
}

int recv(SOCKET socket_p, char* buf, int len, int flags)
{
int rc;
struct msghdr msg;
struct kvec iov;

    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_name = NULL;     // will be struct sock_addr
    msg.msg_namelen = 0;
    iov.iov_len = len;
    iov.iov_base = buf;

    rc = kernel_recvmsg(socket_p, &msg, &iov, 1, iov.iov_len, 0);

    return rc;
}

int recvfrom(SOCKET socket_p, char* buf, int len, int flags, struct sockaddr *from, int * fromlen)
{
int rc;
struct msghdr msg;
struct kvec iov;

    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_name = from;     // will be struct sock_addr
    msg.msg_namelen = *fromlen;
    iov.iov_len = len;
    iov.iov_base = buf;

    rc = kernel_recvmsg(socket_p, &msg, &iov, 1, iov.iov_len, 0);

    return rc;
}

int send(SOCKET socket_p, const char* buf, int len, int flags)
{
int rc;
struct msghdr msg;
struct kvec iov;

    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_name = NULL;     // will be struct sock_addr
    msg.msg_namelen = 0;
    msg.msg_flags = 0;
    iov.iov_len = len;
    iov.iov_base = (char *)buf;

    rc = kernel_sendmsg(socket_p, &msg, &iov, 1, len);

    return rc;
}

int sendto(SOCKET socket_p, const char* buf, int len, int flags, const struct sockaddr *to, int tolen)
{
int rc;
struct msghdr msg;
struct kvec iov;

    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_name = (struct sockaddr *)to;     // will be struct sock_addr
    msg.msg_namelen = tolen;
    msg.msg_flags = 0;
    iov.iov_len = len;
    iov.iov_base = (char *)buf;

    rc = kernel_sendmsg(socket_p, &msg, &iov, 1, len);

    return rc;
}


// EOF

