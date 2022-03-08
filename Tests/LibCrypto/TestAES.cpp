/*
 * Copyright (c) 2021, Peter Bocan  <me@pbocan.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibCrypto/BigInt/UnsignedBigInteger.h>
#include <LibCrypto/Checksum/Adler32.h>
#include <LibCrypto/Cipher/AES.h>
#include <LibTest/TestCase.h>
#include <cstring>

static ReadonlyBytes operator""_b(const char* string, size_t length)
{
    return ReadonlyBytes(string, length);
}

TEST_CASE(test_AES_CBC_name)
{
    Crypto::Cipher::AESCipher::CBCMode cipher("WellHelloFriends"_b, 128, Crypto::Cipher::Intent::Encryption);
    EXPECT_EQ(cipher.class_name(), "AES_CBC");
}

static auto test_aes_cbc_encrypt = [](auto& cipher, auto& result) {
    auto in = "This is a test! This is another test!"_b;
    auto out = cipher.create_aligned_buffer(in.size()).release_value();
    auto iv = ByteBuffer::create_zeroed(Crypto::Cipher::AESCipher::block_size()).release_value();
    auto out_span = out.bytes();
    cipher.encrypt(in, out_span, iv);
    EXPECT_EQ(out.size(), sizeof(result));
    EXPECT(memcmp(out_span.data(), result, out_span.size()) == 0);
};

TEST_CASE(test_AES_CBC_encrypt_with_128bit_key)
{
    u8 result[] {
        0xb8, 0x06, 0x7c, 0xf2, 0xa9, 0x56, 0x63, 0x58, 0x2d, 0x5c, 0xa1, 0x4b, 0xc5, 0xe3, 0x08,
        0xcf, 0xb5, 0x93, 0xfb, 0x67, 0xb6, 0xf7, 0xaf, 0x45, 0x34, 0x64, 0x70, 0x9e, 0xc9, 0x1a,
        0x8b, 0xd3, 0x70, 0x45, 0xf0, 0x79, 0x65, 0xca, 0xb9, 0x03, 0x88, 0x72, 0x1c, 0xdd, 0xab,
        0x45, 0x6b, 0x1c
    };
    Crypto::Cipher::AESCipher::CBCMode cipher("WellHelloFriends"_b, 128, Crypto::Cipher::Intent::Encryption);
    test_aes_cbc_encrypt(cipher, result);
}

TEST_CASE(test_AES_CBC_encrypt_with_192bit_key)
{
    u8 result[] {
        0xae, 0xd2, 0x70, 0xc4, 0x9c, 0xaa, 0x83, 0x33, 0xd3, 0xd3, 0xac, 0x11, 0x65, 0x35, 0xf7,
        0x19, 0x48, 0x7c, 0x7a, 0x8a, 0x95, 0x64, 0xe7, 0xc6, 0x0a, 0xdf, 0x10, 0x06, 0xdc, 0x90,
        0x68, 0x51, 0x09, 0xd7, 0x3b, 0x48, 0x1b, 0x8a, 0xd3, 0x50, 0x09, 0xba, 0xfc, 0xde, 0x11,
        0xe0, 0x3f, 0xcb
    };
    Crypto::Cipher::AESCipher::CBCMode cipher("Well Hello Friends! whf!"_b, 192, Crypto::Cipher::Intent::Encryption);
    test_aes_cbc_encrypt(cipher, result);
}

TEST_CASE(test_AES_CBC_encrypt_with_256bit_key)
{
    u8 result[] {
        0x0a, 0x44, 0x4d, 0x62, 0x9e, 0x8b, 0xd8, 0x11, 0x80, 0x48, 0x2a, 0x32, 0x53, 0x61, 0xe7,
        0x59, 0x62, 0x55, 0x9e, 0xf4, 0xe6, 0xad, 0xea, 0xc5, 0x0b, 0xf6, 0xbc, 0x6a, 0xcb, 0x9c,
        0x47, 0x9f, 0xc2, 0x21, 0xe6, 0x19, 0x62, 0xc3, 0x75, 0xca, 0xab, 0x2d, 0x18, 0xa1, 0x54,
        0xd1, 0x41, 0xe6
    };
    Crypto::Cipher::AESCipher::CBCMode cipher("WellHelloFriendsWellHelloFriends"_b, 256, Crypto::Cipher::Intent::Encryption);
    test_aes_cbc_encrypt(cipher, result);
}

TEST_CASE(test_AES_CBC_encrypt_with_unsigned_256bit_key)
{
    u8 result[] {
        0x18, 0x71, 0x80, 0x4c, 0x28, 0x07, 0x55, 0x3c, 0x05, 0x33, 0x36, 0x3f, 0x19, 0x38, 0x5c,
        0xbe, 0xf8, 0xb8, 0x0e, 0x0e, 0x66, 0x67, 0x63, 0x9c, 0xbf, 0x73, 0xcd, 0x82, 0xf9, 0xcb,
        0x9d, 0x81, 0x56, 0xc6, 0x75, 0x14, 0x8b, 0x79, 0x60, 0xb0, 0xdf, 0xaa, 0x2c, 0x2b, 0xd4,
        0xd6, 0xa0, 0x46
    };
    u8 key[] { 0x0a, 0x8c, 0x5b, 0x0d, 0x8a, 0x68, 0x43, 0xf7, 0xaf, 0xc0, 0xe3, 0x4e, 0x4b, 0x43, 0xaa, 0x28, 0x69, 0x9b, 0x6f, 0xe7, 0x24, 0x82, 0x1c, 0x71, 0x86, 0xf6, 0x2b, 0x87, 0xd6, 0x8b, 0x8f, 0xf1 };
    Crypto::Cipher::AESCipher::CBCMode cipher(ReadonlyBytes { key, sizeof(key) }, 256, Crypto::Cipher::Intent::Encryption);
    test_aes_cbc_encrypt(cipher, result);
}

// TODO: Test non-CMS padding options for AES CBC encrypt

static auto test_aes_cbc_decrypt = [](auto& cipher, auto& result, auto result_len) {
    auto true_value = "This is a test! This is another test!";
    auto in = ByteBuffer::copy(result, result_len).release_value();
    auto out = cipher.create_aligned_buffer(in.size()).release_value();
    auto iv = ByteBuffer::create_zeroed(Crypto::Cipher::AESCipher::block_size()).release_value();
    auto out_span = out.bytes();
    cipher.decrypt(in, out_span, iv);
    EXPECT_EQ(out_span.size(), strlen(true_value));
    EXPECT(memcmp(out_span.data(), true_value, strlen(true_value)) == 0);
};

TEST_CASE(test_AES_CBC_128bit_key_decrypt)
{
    u8 result[] {
        0xb8, 0x06, 0x7c, 0xf2, 0xa9, 0x56, 0x63, 0x58, 0x2d, 0x5c, 0xa1, 0x4b, 0xc5, 0xe3, 0x08,
        0xcf, 0xb5, 0x93, 0xfb, 0x67, 0xb6, 0xf7, 0xaf, 0x45, 0x34, 0x64, 0x70, 0x9e, 0xc9, 0x1a,
        0x8b, 0xd3, 0x70, 0x45, 0xf0, 0x79, 0x65, 0xca, 0xb9, 0x03, 0x88, 0x72, 0x1c, 0xdd, 0xab,
        0x45, 0x6b, 0x1c
    };
    Crypto::Cipher::AESCipher::CBCMode cipher("WellHelloFriends"_b, 128, Crypto::Cipher::Intent::Decryption);
    test_aes_cbc_decrypt(cipher, result, 48);
}

TEST_CASE(test_AES_CBC_192bit_key_decrypt)
{
    u8 result[] {
        0xae, 0xd2, 0x70, 0xc4, 0x9c, 0xaa, 0x83, 0x33, 0xd3, 0xd3, 0xac, 0x11, 0x65, 0x35, 0xf7,
        0x19, 0x48, 0x7c, 0x7a, 0x8a, 0x95, 0x64, 0xe7, 0xc6, 0x0a, 0xdf, 0x10, 0x06, 0xdc, 0x90,
        0x68, 0x51, 0x09, 0xd7, 0x3b, 0x48, 0x1b, 0x8a, 0xd3, 0x50, 0x09, 0xba, 0xfc, 0xde, 0x11,
        0xe0, 0x3f, 0xcb
    };
    Crypto::Cipher::AESCipher::CBCMode cipher("Well Hello Friends! whf!"_b, 192, Crypto::Cipher::Intent::Decryption);
    test_aes_cbc_decrypt(cipher, result, 48);
}

TEST_CASE(test_AES_CBC_256bit_key_decrypt)
{
    u8 result[] {
        0x0a, 0x44, 0x4d, 0x62, 0x9e, 0x8b, 0xd8, 0x11, 0x80, 0x48, 0x2a, 0x32, 0x53, 0x61, 0xe7,
        0x59, 0x62, 0x55, 0x9e, 0xf4, 0xe6, 0xad, 0xea, 0xc5, 0x0b, 0xf6, 0xbc, 0x6a, 0xcb, 0x9c,
        0x47, 0x9f, 0xc2, 0x21, 0xe6, 0x19, 0x62, 0xc3, 0x75, 0xca, 0xab, 0x2d, 0x18, 0xa1, 0x54,
        0xd1, 0x41, 0xe6
    };
    Crypto::Cipher::AESCipher::CBCMode cipher("WellHelloFriendsWellHelloFriends"_b, 256, Crypto::Cipher::Intent::Decryption);
    test_aes_cbc_decrypt(cipher, result, 48);
}

// TODO: Test non-CMS padding options for AES CBC decrypt

TEST_CASE(test_AES_CTR_name)
{
    Crypto::Cipher::AESCipher::CTRMode cipher("WellHelloFriends"_b, 128, Crypto::Cipher::Intent::Encryption);
    EXPECT_EQ(cipher.class_name(), "AES_CTR");
}

#define AS_BB(x) (ReadonlyBytes { (x), sizeof((x)) / sizeof((x)[0]) })

static auto test_aes_ctr_encrypt = [](auto key, auto ivec, auto in, auto out_expected) {
    // nonce is already included in ivec.
    Crypto::Cipher::AESCipher::CTRMode cipher(key, 8 * key.size(), Crypto::Cipher::Intent::Encryption);
    ByteBuffer out_actual = ByteBuffer::create_zeroed(in.size()).release_value();
    Bytes out_span = out_actual.bytes();
    cipher.encrypt(in, out_span, ivec);
    EXPECT_EQ(out_expected.size(), out_actual.size());
    EXPECT(memcmp(out_expected.data(), out_span.data(), out_expected.size()) == 0);
};

TEST_CASE(test_AES_CTR_128bit_key_encrypt_16bytes)
{
    u8 key[] {
        0xae, 0x68, 0x52, 0xf8, 0x12, 0x10, 0x67, 0xcc, 0x4b, 0xf7, 0xa5, 0x76, 0x55, 0x77, 0xf3, 0x9e
    };
    u8 ivec[] {
        0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 + 1 // See CTR.h
    };
    u8 in[] {
        0x53, 0x69, 0x6e, 0x67, 0x6c, 0x65, 0x20, 0x62, 0x6c, 0x6f, 0x63, 0x6b, 0x20, 0x6d, 0x73, 0x67
    };
    u8 out[] {
        0xe4, 0x09, 0x5d, 0x4f, 0xb7, 0xa7, 0xb3, 0x79, 0x2d, 0x61, 0x75, 0xa3, 0x26, 0x13, 0x11, 0xb8
    };
    test_aes_ctr_encrypt(AS_BB(key), AS_BB(ivec), AS_BB(in), AS_BB(out));
}

TEST_CASE(test_AES_CTR_128bit_key_encrypt_32bytes)
{
    u8 key[] {
        0x7e, 0x24, 0x06, 0x78, 0x17, 0xfa, 0xe0, 0xd7, 0x43, 0xd6, 0xce, 0x1f, 0x32, 0x53, 0x91, 0x63
    };
    u8 ivec[] {
        0x00, 0x6c, 0xb6, 0xdb, 0xc0, 0x54, 0x3b, 0x59, 0xda, 0x48, 0xd9, 0x0b, 0x00, 0x00, 0x00, 0x00 + 1 // See CTR.h
    };
    u8 in[] {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };
    u8 out[] {
        0x51, 0x04, 0xa1, 0x06, 0x16, 0x8a, 0x72, 0xd9, 0x79, 0x0d, 0x41, 0xee, 0x8e, 0xda, 0xd3, 0x88,
        0xeb, 0x2e, 0x1e, 0xfc, 0x46, 0xda, 0x57, 0xc8, 0xfc, 0xe6, 0x30, 0xdf, 0x91, 0x41, 0xbe, 0x28
    };
    test_aes_ctr_encrypt(AS_BB(key), AS_BB(ivec), AS_BB(in), AS_BB(out));
}

TEST_CASE(test_AES_CTR_128bit_key_encrypt_36bytes)
{
    u8 ivec[] {
        0x00, 0xe0, 0x01, 0x7b, 0x27, 0x77, 0x7f, 0x3f, 0x4a, 0x17, 0x86, 0xf0, 0x00, 0x00, 0x00, 0x00 + 1 // See CTR.h
    };
    u8 key[] {
        0x76, 0x91, 0xbe, 0x03, 0x5e, 0x50, 0x20, 0xa8, 0xac, 0x6e, 0x61, 0x85, 0x29, 0xf9, 0xa0, 0xdc
    };
    u8 in[] {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23
    };
    u8 out[] {
        0xc1, 0xcf, 0x48, 0xa8, 0x9f, 0x2f, 0xfd, 0xd9, 0xcf, 0x46, 0x52, 0xe9, 0xef, 0xdb, 0x72, 0xd7, 0x45, 0x40, 0xa4, 0x2b, 0xde, 0x6d, 0x78, 0x36, 0xd5, 0x9a, 0x5c, 0xea, 0xae, 0xf3, 0x10, 0x53, 0x25, 0xb2, 0x07, 0x2f
    };
    test_aes_ctr_encrypt(AS_BB(key), AS_BB(ivec), AS_BB(in), AS_BB(out));
}

TEST_CASE(test_AES_CTR_192bit_key_encrypt_32bytes)
{
    u8 key[] {
        0x7c, 0x5c, 0xb2, 0x40, 0x1b, 0x3d, 0xc3, 0x3c, 0x19, 0xe7, 0x34, 0x08, 0x19, 0xe0, 0xf6, 0x9c, 0x67, 0x8c, 0x3d, 0xb8, 0xe6, 0xf6, 0xa9, 0x1a
    };
    u8 ivec[] {
        0x00, 0x96, 0xb0, 0x3b, 0x02, 0x0c, 0x6e, 0xad, 0xc2, 0xcb, 0x50, 0x0d, 0x00, 0x00, 0x00, 0x00 + 1 // See CTR.h
    };
    u8 in[] {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };
    u8 out[] {
        0x45, 0x32, 0x43, 0xfc, 0x60, 0x9b, 0x23, 0x32, 0x7e, 0xdf, 0xaa, 0xfa, 0x71, 0x31, 0xcd, 0x9f, 0x84, 0x90, 0x70, 0x1c, 0x5a, 0xd4, 0xa7, 0x9c, 0xfc, 0x1f, 0xe0, 0xff, 0x42, 0xf4, 0xfb, 0x00
    };
    test_aes_ctr_encrypt(AS_BB(key), AS_BB(ivec), AS_BB(in), AS_BB(out));
}

TEST_CASE(test_AES_CTR_192bit_key_encrypt_36bytes)
{
    u8 key[] {
        0x02, 0xbf, 0x39, 0x1e, 0xe8, 0xec, 0xb1, 0x59, 0xb9, 0x59, 0x61, 0x7b, 0x09, 0x65, 0x27, 0x9b, 0xf5, 0x9b, 0x60, 0xa7, 0x86, 0xd3, 0xe0, 0xfe
    };
    u8 ivec[] {
        0x00, 0x07, 0xbd, 0xfd, 0x5c, 0xbd, 0x60, 0x27, 0x8d, 0xcc, 0x09, 0x12, 0x00, 0x00, 0x00, 0x00 + 1 // See CTR.h
    };
    u8 in[] {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23
    };
    u8 out[] {
        0x96, 0x89, 0x3f, 0xc5, 0x5e, 0x5c, 0x72, 0x2f, 0x54, 0x0b, 0x7d, 0xd1, 0xdd, 0xf7, 0xe7, 0x58, 0xd2, 0x88, 0xbc, 0x95, 0xc6, 0x91, 0x65, 0x88, 0x45, 0x36, 0xc8, 0x11, 0x66, 0x2f, 0x21, 0x88, 0xab, 0xee, 0x09, 0x35
    };
    test_aes_ctr_encrypt(AS_BB(key), AS_BB(ivec), AS_BB(in), AS_BB(out));
}

TEST_CASE(test_AES_CTR_256bit_encrypt_16bytes)
{
    u8 key[] {
        0x77, 0x6b, 0xef, 0xf2, 0x85, 0x1d, 0xb0, 0x6f, 0x4c, 0x8a, 0x05, 0x42, 0xc8, 0x69, 0x6f, 0x6c, 0x6a, 0x81, 0xaf, 0x1e, 0xec, 0x96, 0xb4, 0xd3, 0x7f, 0xc1, 0xd6, 0x89, 0xe6, 0xc1, 0xc1, 0x04
    };
    u8 ivec[] {
        0x00, 0x00, 0x00, 0x60, 0xdb, 0x56, 0x72, 0xc9, 0x7a, 0xa8, 0xf0, 0xb2, 0x00, 0x00, 0x00, 0x00 + 1 // See CTR.h
    };
    u8 in[] {
        0x53, 0x69, 0x6e, 0x67, 0x6c, 0x65, 0x20, 0x62, 0x6c, 0x6f, 0x63, 0x6b, 0x20, 0x6d, 0x73, 0x67
    };
    u8 out[] {
        0x14, 0x5a, 0xd0, 0x1d, 0xbf, 0x82, 0x4e, 0xc7, 0x56, 0x08, 0x63, 0xdc, 0x71, 0xe3, 0xe0, 0xc0
    };
    test_aes_ctr_encrypt(AS_BB(key), AS_BB(ivec), AS_BB(in), AS_BB(out));
}

TEST_CASE(test_AES_CTR_256bit_encrypt_32bytes)
{
    u8 key[] {
        0xf6, 0xd6, 0x6d, 0x6b, 0xd5, 0x2d, 0x59, 0xbb, 0x07, 0x96, 0x36, 0x58, 0x79, 0xef, 0xf8, 0x86, 0xc6, 0x6d, 0xd5, 0x1a, 0x5b, 0x6a, 0x99, 0x74, 0x4b, 0x50, 0x59, 0x0c, 0x87, 0xa2, 0x38, 0x84
    };
    u8 ivec[] {
        0x00, 0xfa, 0xac, 0x24, 0xc1, 0x58, 0x5e, 0xf1, 0x5a, 0x43, 0xd8, 0x75, 0x00, 0x00, 0x00, 0x00 + 1 // See CTR.h
    };
    u8 in[] {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };
    u8 out[] {
        0xf0, 0x5e, 0x23, 0x1b, 0x38, 0x94, 0x61, 0x2c, 0x49, 0xee, 0x00, 0x0b, 0x80, 0x4e, 0xb2, 0xa9, 0xb8, 0x30, 0x6b, 0x50, 0x8f, 0x83, 0x9d, 0x6a, 0x55, 0x30, 0x83, 0x1d, 0x93, 0x44, 0xaf, 0x1c
    };
    test_aes_ctr_encrypt(AS_BB(key), AS_BB(ivec), AS_BB(in), AS_BB(out));
}

TEST_CASE(test_AES_CTR_256bit_encrypt_36bytes)
{
    u8 key[] {
        0xff, 0x7a, 0x61, 0x7c, 0xe6, 0x91, 0x48, 0xe4, 0xf1, 0x72, 0x6e, 0x2f, 0x43, 0x58, 0x1d, 0xe2, 0xaa, 0x62, 0xd9, 0xf8, 0x05, 0x53, 0x2e, 0xdf, 0xf1, 0xee, 0xd6, 0x87, 0xfb, 0x54, 0x15, 0x3d
    };
    u8 ivec[] {
        0x00, 0x1c, 0xc5, 0xb7, 0x51, 0xa5, 0x1d, 0x70, 0xa1, 0xc1, 0x11, 0x48, 0x00, 0x00, 0x00, 0x00 + 1 // See CTR.h
    };
    u8 in[] {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23
    };
    u8 out[] {
        0xeb, 0x6c, 0x52, 0x82, 0x1d, 0x0b, 0xbb, 0xf7, 0xce, 0x75, 0x94, 0x46, 0x2a, 0xca, 0x4f, 0xaa, 0xb4, 0x07, 0xdf, 0x86, 0x65, 0x69, 0xfd, 0x07, 0xf4, 0x8c, 0xc0, 0xb5, 0x83, 0xd6, 0x07, 0x1f, 0x1e, 0xc0, 0xe6, 0xb8
    };
    test_aes_ctr_encrypt(AS_BB(key), AS_BB(ivec), AS_BB(in), AS_BB(out));
}

TEST_CASE(test_AES_CTR_256bit_encrypt_36bytes_with_high_counter)
{
    // This test checks whether counter overflow crashes.
    u8 key[] {
        0xff, 0x7a, 0x61, 0x7c, 0xe6, 0x91, 0x48, 0xe4, 0xf1, 0x72, 0x6e, 0x2f, 0x43, 0x58, 0x1d, 0xe2, 0xaa, 0x62, 0xd9, 0xf8, 0x05, 0x53, 0x2e, 0xdf, 0xf1, 0xee, 0xd6, 0x87, 0xfb, 0x54, 0x15, 0x3d
    };
    u8 ivec[] {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    };
    u8 in[] {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23
    };
    u8 out[] {
        // Pasted from the output. The actual success condition is
        // not crashing when incrementing the counter.
        0x6e, 0x8c, 0xfc, 0x59, 0x08, 0xa8, 0xc0, 0xf1, 0xe6, 0x85, 0x96, 0xe9, 0xc5, 0x40, 0xb6, 0x8b, 0xfe, 0x28, 0x72, 0xe2, 0x24, 0x11, 0x7e, 0x59, 0xef, 0xac, 0x5c, 0xe1, 0x06, 0x89, 0x09, 0xab, 0xf8, 0x90, 0x1c, 0x66
    };
    test_aes_ctr_encrypt(AS_BB(key), AS_BB(ivec), AS_BB(in), AS_BB(out));
}

static auto test_aes_ctr_decrypt = [](auto key, auto ivec, auto in, auto out_expected) {
    // nonce is already included in ivec.
    Crypto::Cipher::AESCipher::CTRMode cipher(key, 8 * key.size(), Crypto::Cipher::Intent::Decryption);
    ByteBuffer out_actual = ByteBuffer::create_zeroed(in.size()).release_value();
    auto out_span = out_actual.bytes();
    cipher.decrypt(in, out_span, ivec);
    EXPECT_EQ(out_expected.size(), out_span.size());
    EXPECT(memcmp(out_expected.data(), out_span.data(), out_expected.size()) == 0);
};

// From RFC 3686, Section 6
TEST_CASE(test_AES_CTR_128bit_decrypt_16bytes)
{
    // This test checks whether counter overflow crashes.
    u8 key[] {
        0xae, 0x68, 0x52, 0xf8, 0x12, 0x10, 0x67, 0xcc, 0x4b, 0xf7, 0xa5, 0x76, 0x55, 0x77, 0xf3, 0x9e
    };
    u8 ivec[] {
        0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 + 1 // See CTR.h
    };
    u8 out[] {
        0x53, 0x69, 0x6e, 0x67, 0x6c, 0x65, 0x20, 0x62, 0x6c, 0x6f, 0x63, 0x6b, 0x20, 0x6d, 0x73, 0x67
    };
    u8 in[] {
        0xe4, 0x09, 0x5d, 0x4f, 0xb7, 0xa7, 0xb3, 0x79, 0x2d, 0x61, 0x75, 0xa3, 0x26, 0x13, 0x11, 0xb8
    };
    test_aes_ctr_decrypt(AS_BB(key), AS_BB(ivec), AS_BB(in), AS_BB(out));
    // If encryption works, then decryption works, too.
}

TEST_CASE(test_AES_GCM_name)
{
    Crypto::Cipher::AESCipher::GCMMode cipher("WellHelloFriends"_b, 128, Crypto::Cipher::Intent::Encryption);
    EXPECT_EQ(cipher.class_name(), "AES_GCM");
}

TEST_CASE(test_AES_GCM_128bit_encrypt_empty)
{
    Crypto::Cipher::AESCipher::GCMMode cipher("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"_b, 128, Crypto::Cipher::Intent::Encryption);
    u8 result_tag[] { 0x58, 0xe2, 0xfc, 0xce, 0xfa, 0x7e, 0x30, 0x61, 0x36, 0x7f, 0x1d, 0x57, 0xa4, 0xe7, 0x45, 0x5a };
    Bytes out;
    auto tag = ByteBuffer::create_uninitialized(16).release_value();
    cipher.encrypt({}, out, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"_b, {}, tag);
    EXPECT(memcmp(result_tag, tag.data(), tag.size()) == 0);
}

TEST_CASE(test_AES_GCM_128bit_encrypt_zeros)
{
    Crypto::Cipher::AESCipher::GCMMode cipher("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"_b, 128, Crypto::Cipher::Intent::Encryption);
    u8 result_tag[] { 0xab, 0x6e, 0x47, 0xd4, 0x2c, 0xec, 0x13, 0xbd, 0xf5, 0x3a, 0x67, 0xb2, 0x12, 0x57, 0xbd, 0xdf };
    u8 result_ct[] { 0x03, 0x88, 0xda, 0xce, 0x60, 0xb6, 0xa3, 0x92, 0xf3, 0x28, 0xc2, 0xb9, 0x71, 0xb2, 0xfe, 0x78 };
    auto tag = ByteBuffer::create_uninitialized(16).release_value();
    auto out = ByteBuffer::create_uninitialized(16).release_value();
    auto out_bytes = out.bytes();
    cipher.encrypt("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"_b, out_bytes, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"_b, {}, tag);
    EXPECT(memcmp(result_ct, out.data(), out.size()) == 0);
    EXPECT(memcmp(result_tag, tag.data(), tag.size()) == 0);
}

TEST_CASE(test_AES_GCM_128bit_encrypt_multiple_blocks_with_iv)
{
    Crypto::Cipher::AESCipher::GCMMode cipher("\xfe\xff\xe9\x92\x86\x65\x73\x1c\x6d\x6a\x8f\x94\x67\x30\x83\x08"_b, 128, Crypto::Cipher::Intent::Encryption);
    u8 result_tag[] { 0x4d, 0x5c, 0x2a, 0xf3, 0x27, 0xcd, 0x64, 0xa6, 0x2c, 0xf3, 0x5a, 0xbd, 0x2b, 0xa6, 0xfa, 0xb4 };
    u8 result_ct[] { 0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24, 0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c, 0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0, 0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e, 0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c, 0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05, 0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97, 0x3d, 0x58, 0xe0, 0x91, 0x47, 0x3f, 0x59, 0x85 };
    auto tag = ByteBuffer::create_uninitialized(16).release_value();
    auto out = ByteBuffer::create_uninitialized(64).release_value();
    auto out_bytes = out.bytes();
    cipher.encrypt(
        "\xd9\x31\x32\x25\xf8\x84\x06\xe5\xa5\x59\x09\xc5\xaf\xf5\x26\x9a\x86\xa7\xa9\x53\x15\x34\xf7\xda\x2e\x4c\x30\x3d\x8a\x31\x8a\x72\x1c\x3c\x0c\x95\x95\x68\x09\x53\x2f\xcf\x0e\x24\x49\xa6\xb5\x25\xb1\x6a\xed\xf5\xaa\x0d\xe6\x57\xba\x63\x7b\x39\x1a\xaf\xd2\x55"_b,
        out_bytes,
        "\xca\xfe\xba\xbe\xfa\xce\xdb\xad\xde\xca\xf8\x88\x00\x00\x00\x00"_b,
        {},
        tag);
    EXPECT(memcmp(result_ct, out.data(), out.size()) == 0);
    EXPECT(memcmp(result_tag, tag.data(), tag.size()) == 0);
}

TEST_CASE(test_AES_GCM_128bit_encrypt_with_aad)
{
    Crypto::Cipher::AESCipher::GCMMode cipher("\xfe\xff\xe9\x92\x86\x65\x73\x1c\x6d\x6a\x8f\x94\x67\x30\x83\x08"_b, 128, Crypto::Cipher::Intent::Encryption);
    u8 result_tag[] { 0x4d, 0x5c, 0x2a, 0xf3, 0x27, 0xcd, 0x64, 0xa6, 0x2c, 0xf3, 0x5a, 0xbd, 0x2b, 0xa6, 0xfa, 0xb4 };
    u8 result_ct[] { 0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24, 0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c, 0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0, 0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e, 0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c, 0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05, 0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97, 0x3d, 0x58, 0xe0, 0x91, 0x47, 0x3f, 0x59, 0x85 };
    auto tag = ByteBuffer::create_uninitialized(16).release_value();
    auto out = ByteBuffer::create_uninitialized(64).release_value();
    auto out_bytes = out.bytes();
    cipher.encrypt(
        "\xd9\x31\x32\x25\xf8\x84\x06\xe5\xa5\x59\x09\xc5\xaf\xf5\x26\x9a\x86\xa7\xa9\x53\x15\x34\xf7\xda\x2e\x4c\x30\x3d\x8a\x31\x8a\x72\x1c\x3c\x0c\x95\x95\x68\x09\x53\x2f\xcf\x0e\x24\x49\xa6\xb5\x25\xb1\x6a\xed\xf5\xaa\x0d\xe6\x57\xba\x63\x7b\x39\x1a\xaf\xd2\x55"_b,
        out_bytes,
        "\xca\xfe\xba\xbe\xfa\xce\xdb\xad\xde\xca\xf8\x88\x00\x00\x00\x00"_b,
        {},
        tag);
    EXPECT(memcmp(result_ct, out.data(), out.size()) == 0);
    EXPECT(memcmp(result_tag, tag.data(), tag.size()) == 0);
}

TEST_CASE(test_AES_GCM_128bit_decrypt_empty)
{
    Crypto::Cipher::AESCipher::GCMMode cipher("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"_b, 128, Crypto::Cipher::Intent::Encryption);
    u8 input_tag[] { 0x58, 0xe2, 0xfc, 0xce, 0xfa, 0x7e, 0x30, 0x61, 0x36, 0x7f, 0x1d, 0x57, 0xa4, 0xe7, 0x45, 0x5a };
    Bytes out;
    auto consistency = cipher.decrypt({}, out, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"_b, {}, { input_tag, 16 });
    EXPECT_EQ(consistency, Crypto::VerificationConsistency::Consistent);
    EXPECT_EQ(out.size(), 0u);
}

TEST_CASE(test_AES_GCM_128bit_decrypt_zeros)
{
    Crypto::Cipher::AESCipher::GCMMode cipher("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"_b, 128, Crypto::Cipher::Intent::Encryption);
    u8 input_tag[] { 0xab, 0x6e, 0x47, 0xd4, 0x2c, 0xec, 0x13, 0xbd, 0xf5, 0x3a, 0x67, 0xb2, 0x12, 0x57, 0xbd, 0xdf };
    u8 input_ct[] { 0x03, 0x88, 0xda, 0xce, 0x60, 0xb6, 0xa3, 0x92, 0xf3, 0x28, 0xc2, 0xb9, 0x71, 0xb2, 0xfe, 0x78 };
    u8 result_pt[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    auto out = ByteBuffer::create_uninitialized(16).release_value();
    auto out_bytes = out.bytes();
    auto consistency = cipher.decrypt({ input_ct, 16 }, out_bytes, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"_b, {}, { input_tag, 16 });
    EXPECT_EQ(consistency, Crypto::VerificationConsistency::Consistent);
    EXPECT(memcmp(result_pt, out.data(), out.size()) == 0);
}

TEST_CASE(test_AES_GCM_128bit_decrypt_multiple_blocks_with_iv)
{
    Crypto::Cipher::AESCipher::GCMMode cipher("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"_b, 128, Crypto::Cipher::Intent::Encryption);
    u8 input_tag[] { 0xab, 0x6e, 0x47, 0xd4, 0x2c, 0xec, 0x13, 0xbd, 0xf5, 0x3a, 0x67, 0xb2, 0x12, 0x57, 0xbd, 0xdf };
    u8 input_ct[] { 0x03, 0x88, 0xda, 0xce, 0x60, 0xb6, 0xa3, 0x92, 0xf3, 0x28, 0xc2, 0xb9, 0x71, 0xb2, 0xfe, 0x78 };
    u8 result_pt[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    auto out = ByteBuffer::create_uninitialized(16).release_value();
    auto out_bytes = out.bytes();
    auto consistency = cipher.decrypt({ input_ct, 16 }, out_bytes, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"_b, {}, { input_tag, 16 });
    EXPECT_EQ(consistency, Crypto::VerificationConsistency::Consistent);
    EXPECT(memcmp(result_pt, out.data(), out.size()) == 0);
}

TEST_CASE(test_AES_GCM_128bit_decrypt_multiple_blocks_with_aad)
{
    Crypto::Cipher::AESCipher::GCMMode cipher("\xfe\xff\xe9\x92\x86\x65\x73\x1c\x6d\x6a\x8f\x94\x67\x30\x83\x08"_b, 128, Crypto::Cipher::Intent::Encryption);
    u8 input_tag[] { 0x93, 0xae, 0x16, 0x97, 0x49, 0xa3, 0xbf, 0x39, 0x4f, 0x61, 0xb7, 0xc1, 0xb1, 0x2, 0x4f, 0x60 };
    u8 input_ct[] { 0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24, 0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c, 0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0, 0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e, 0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c, 0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05, 0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97, 0x3d, 0x58, 0xe0, 0x91, 0x47, 0x3f, 0x59, 0x85 };
    u8 result_pt[] { 0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5, 0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a, 0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda, 0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72, 0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53, 0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25, 0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57, 0xba, 0x63, 0x7b, 0x39, 0x1a, 0xaf, 0xd2, 0x55 };
    auto out = ByteBuffer::create_uninitialized(64).release_value();
    auto out_bytes = out.bytes();
    auto consistency = cipher.decrypt(
        { input_ct, 64 },
        out_bytes,
        "\xca\xfe\xba\xbe\xfa\xce\xdb\xad\xde\xca\xf8\x88\x00\x00\x00\x00"_b,
        "\xde\xad\xbe\xef\xfa\xaf\x11\xcc"_b,
        { input_tag, 16 });
    EXPECT(memcmp(result_pt, out.data(), out.size()) == 0);
    EXPECT_EQ(consistency, Crypto::VerificationConsistency::Consistent);
}
