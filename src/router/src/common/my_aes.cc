/* Copyright (c) 2002, 2017, Oracle and/or its affiliates. All rights reserved.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#include "router_config.h"
#include "mysqlrouter/my_aes.h"
#include "mysqlrouter/my_aes_impl.h"
#include <string.h>
#include <stdint.h>

namespace myaes {

/**
  Transforms an arbitrary long key into a fixed length AES key

  AES keys are of fixed length. This routine takes an arbitrary long key
  iterates over it in AES key length increment and XORs the bytes with the
  AES key buffer being prepared.
  The bytes from the last incomplete iteration are XORed to the start
  of the key until their depletion.
  Needed since crypto function routines expect a fixed length key.

  @param [in] key               Key to use for real key creation
  @param [in] key_length        Length of the key
  @param [out] rkey             Real key (used by OpenSSL/YaSSL)
  @param [out] opmode           encryption mode
*/

void my_aes_create_key(const unsigned char *key, uint32_t key_length,
                       uint8_t *rkey, enum my_aes_opmode opmode)
{
  const uint32_t key_size= my_aes_opmode_key_sizes[opmode] / 8;
  uint8_t *rkey_end;                              /* Real key boundary */
  uint8_t *ptr;                                   /* Start of the real key*/
  uint8_t *sptr;                                  /* Start of the working key */
  uint8_t *key_end= ((uint8_t *)key) + key_length;  /* Working key boundary*/

  rkey_end= rkey + key_size;

  memset(rkey, 0, key_size);          /* Set initial key  */

  for (ptr= rkey, sptr= (uint8_t *)key; sptr < key_end; ptr++, sptr++)
  {
    if (ptr == rkey_end)
      /*  Just loop over tmp_key until we used all key */
      ptr= rkey;
    *ptr^= *sptr;
  }
}

}
