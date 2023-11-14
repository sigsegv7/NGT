/*
 * Copyright (c) 2023 Ian Marco Moffett and the Osmora Project
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
 * 3. Neither the name of VegaOS nor the names of its
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

#ifndef _NGT_H_
#define _NGT_H_ 1

#include <stdint.h>

#define NGT_ID 'NGT'

/* Header types */
#define REQ_OPEN        'O'     /* Open Connection (0x43) */
#define REQ_ACCEPT      'A'     /* Accept connection (0x53) */
#define REQ_DATA_XCHG   'X'     /* Data exchange (0x58) */
#define CONN_KILL       'K'     /* Kill connection (0x4B) */

/*
 * NGT header. This header goes above
 * every NGT packet to describe the purpose
 * of the packet, and allowing the server to
 * determine the integrity of the packet.
 *
 * *EVERY FIELD MUST BE LITTLE ENDIAN*
 *
 * - The `id' field must be 'NGT' for every packet.
 *   The server shall reject any packet when its
 *   header does not contain 'NGT' within its `id'
 *   field.
 *
 * - The `checksum' field helps the server verify
 *   the integrity of the packet header. The server
 *   shall reject any packet if it determines that
 *   this field does not equate to the sum of *every*
 *   field added together, bitshifted right (>>) by 1.
 *
 * - The `channel' field determines the type of service
 *   to be used over NGT. Think of this field like a port
 *   number.
 *
 * - The `length' field specifies the length of the packet
 *   in bytes.
 *
 * - The `caps' field describes standard functionality
 *   implemented by the server. Bits set are marked as
 *   implemented. See CAPS FIELD below.
 *
 * - The `hdr_type' field describes the type of header.
 *   See header type defines above.
 *
 * - The `seq' field is used to ensure the order of packets
 *   and can be used to identify a previous packet for internal
 *   server purposes e.g identifying a retransmitted packet.
 *
 * During the start of a connection, the client shall send
 * this header to the server with fields set, `hdr_type' as
 * REQ_OPEN and `caps' set to requested capabilities. The
 * server shall than either reject the connection by taking
 * the header received and setting `hdr_type' as CONN_KILL then
 * sending it back to the client, or accept the connection by taking
 * the header received, setting `hdr_type' as REQ_ACCEPT and setting
 * `caps' as `requested_caps & server_caps' where `requested_caps' is
 * the caps requested by the client and `server_caps' is the caps
 * supported by the server.
 *
 * -- CAPS FIELD --
 *
 *                       Bit number
 *                      /
 *      0 1 2 3 4 5 6 7
 *      ~ ~ ~ ~ ~ ~ ~ ~
 *      P R R R R R R R
 *                      \
 *                       Purpose
 *
 * P: Periodic Connection Check
 * R: Reserved, keep zero.
 *
 * - The `P' cap indicates that the server can periodically
 *   check if the connection is still alive.
 *
 */
struct __attribute__((packed)) ngt_hdr {
    uint32_t id;
    uint32_t checksum;
    uint16_t channel;
    uint16_t length;
    uint8_t caps;
    uint8_t hdr_type;
    uint8_t seq;
#if defined(__GNUC__)
    /* Start of data */
    __extension__ char data[0];
#endif  /* defined(__GNUC__) */
};

#endif  /* !_NGT_H_ */
