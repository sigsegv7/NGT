/*
 * Copyright (c) 2023 Ian Marco Moffett and the Osmora Team.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Lyra nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ngt/checksum.h>
#include <stdio.h>

/*
 * Connect to peer over NGT/IPv4
 *
 * @hdr: Pointer to zero'd NGT header.
 * @req_caps: Caps to request.
 * @channel: Channel to connect over.
 *
 * XXX: This assumes little endian.
 * TODO: Don't assume little endian.
 */
int
ngt_inet_connect(struct ngt_hdr *hdr, uint8_t req_caps, uint16_t channel,
                 const char *addr)
{
    int rsockfd;
    int status;
    struct sockaddr_in peer;

    hdr->caps = req_caps;
    hdr->channel = channel;
    hdr->id = NGT_ID;
    hdr->hdr_type = REQ_OPEN;           /* Requet to open a connection */
    hdr->checksum = ngt_gen_csum(hdr);  /* XXX: Keep as last field set */

    /*
     * Open our raw socket...
     *
     * TODO XXX: Figure out a good protocol number for this.
     *           As of now, 2 shall do... We won't even need this
     *           later on as we'll eventually replace IP as well,
     *           the final product should not be NGT/IP.
     */
    if ((rsockfd = socket(PF_INET, SOCK_RAW, 2)) < 0) {
        return rsockfd;
    }

    status = bind(rsockfd, (struct sockaddr *)&peer,
                  sizeof(struct sockaddr));

    if (status < 0) {
        return status;
    }

    /* Set information about peer to socket */
    peer.sin_addr.s_addr = inet_addr(addr);
    peer.sin_family = AF_INET;
    peer.sin_port = channel;

    /* Connect the peer */
    status = connect(rsockfd, (struct sockaddr *)&peer,
                     sizeof(struct sockaddr));

    if (status < 0) {
        close(rsockfd);
        return status;
    }

    /*
     * Now, we must send the header to the peer.
     *
     * XXX: First we are ensuring the length field in the
     *      packet header is 0. There is *no need* for any
     *      information to be included after the header during
     *      a request to connect besides maybe some zero-padding
     *      *if needed*... The server shall also verify that during
     *      a connection request, the length is indeed 0.
     */
    hdr->length = 0;
    sendto(rsockfd, (void *)hdr, sizeof(*hdr), 0,
           (struct sockaddr *)&peer, sizeof(struct sockaddr));

    /*
     * TODO XXX: Actually read in a response and do
     *           something about it. For now we'll just
     *           close the connection and do nothing for
     *           testing purposes
     */
    close(rsockfd);
    return 0;
}
