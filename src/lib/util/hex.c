/*
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

/** A generic string buffer structure for string printing and parsing
 *
 * @file src/lib/util/sbuff.c
 *
 * @copyright 2020 Arran Cudbard-Bell \<a.cudbardb@freeradius.org\>
 */
RCSID("$Id$")

#include <freeradius-devel/util/hex.h>

static char const hextab[] = "0123456789abcdef";

/** Convert hex strings to binary data
 *
 * @param[out] out	Output buffer to write to.
 * @param[in] in	Input string.
 * @return
 *	- >=0 the number of bytes written to out.
 *	- <0 number of bytes we would have needed to copy the next hexit.
 */
ssize_t fr_hex2bin(fr_dbuff_t *out, fr_sbuff_t *in)
{
	size_t total = 0;

	while (!FR_SBUFF_CANT_EXTEND_LOWAT(in, 2)) {
		char *c1, *c2;

		if(!(c1 = memchr(hextab, tolower((int) *in->p), sizeof(hextab))) ||
		   !(c2 = memchr(hextab, tolower((int) *(in->p + 1)), sizeof(hextab)))) break;

		FR_DBUFF_BYTES_IN_RETURN(out, ((c1 - hextab) << 4) + (c2 - hextab));

		fr_sbuff_advance(in, 2);
		total++;
	};

	return total;
}

/** Convert binary data to a hex string
 *
 * Ascii encoded hex string will not be prefixed with '0x'
 *
 * @warning If the output buffer isn't long enough, we have a buffer overflow.
 *
 * @param[out] out	Output buffer to write to.
 * @param[in] in	input.
 * @return
 *	- >=0 the number of bytes written to out.
 *	- <0 number of bytes we would have needed to print the next hexit.
 */
ssize_t fr_bin2hex(fr_sbuff_t *out, fr_dbuff_t *in)
{
	size_t	total = 0;

	while (fr_dbuff_remaining(in) > 0) {	/* Fixme to be extension check */
		FR_SBUFF_IN_CHAR_RETURN(out, hextab[((*in->p) >> 4) & 0x0f], hextab[*in->p & 0x0f]);

		fr_dbuff_advance(in, 1);
		total += 2;
	};
	return total;
}

/** Convert binary data to a hex string
 *
 * Ascii encoded hex string will not be prefixed with '0x'
 *
 * @param[in] ctx to alloc buffer in.
 * @param[in] bin input.
 * @param[in] inlen of bin input.
 * @return length of data written to buffer.
 */
char *fr_abin2hex(TALLOC_CTX *ctx, uint8_t const *bin, size_t inlen)
{
	char *buff;

	buff = talloc_array(ctx, char, (inlen << 2));
	if (!buff) return NULL;

	fr_bin2hex(&FR_SBUFF_OUT(buff, (inlen * 2) + 1), &FR_DBUFF_TMP(bin, inlen));

	return buff;
}