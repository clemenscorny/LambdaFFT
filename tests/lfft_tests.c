/* LambdaFFT is covered by the new BSD license
 *
 * Copyright (c) 2011-2013, Clemens Korner
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <inttypes.h>
#include <math.h>
#include <stdlib.h>

#include <check.h>

#include "lfft.h"
#include "lfft_fft.c"

#define B_0000 0x0
#define B_0001 0x1
#define B_0010 0x2
#define B_0011 0x3
#define B_0100 0x4
#define B_0101 0x5
#define B_0110 0x6
#define B_0111 0x7
#define B_1000 0x8
#define B_1001 0x9
#define B_1010 0xA
#define B_1011 0xB
#define B_1100 0xC
#define B_1101 0xD
#define B_1110 0xE
#define B_1111 0xF

START_TEST(test_f__lfft_is_power_2)
{
    uint16_t i;

    fail_unless(_lfft_is_power_2(0x0000) == false, "False assumption: _lfft_is_power_2(0x0000) == false\n", i);
    fail_unless(_lfft_is_power_2(0x0003) == false, "False assumption: _lfft_is_power_2(0x0003) == false\n", i);
    fail_unless(_lfft_is_power_2(0x0005) == false, "False assumption: _lfft_is_power_2(0x0005) == false\n", i);
    fail_unless(_lfft_is_power_2(0x0006) == false, "False assumption: _lfft_is_power_2(0x0006) == false\n", i);
    fail_unless(_lfft_is_power_2(0x0007) == false, "False assumption: _lfft_is_power_2(0x0007) == false\n", i);
    fail_unless(_lfft_is_power_2(0x0009) == false, "False assumption: _lfft_is_power_2(0x0009) == false\n", i);
    fail_unless(_lfft_is_power_2(0x000A) == false, "False assumption: _lfft_is_power_2(0x000A) == false\n", i);
    fail_unless(_lfft_is_power_2(0x000B) == false, "False assumption: _lfft_is_power_2(0x000B) == false\n", i);
    fail_unless(_lfft_is_power_2(0x000C) == false, "False assumption: _lfft_is_power_2(0x000C) == false\n", i);
    fail_unless(_lfft_is_power_2(0x000D) == false, "False assumption: _lfft_is_power_2(0x000D) == false\n", i);
    fail_unless(_lfft_is_power_2(0x000E) == false, "False assumption: _lfft_is_power_2(0x000E) == false\n", i);
    fail_unless(_lfft_is_power_2(0x000F) == false, "False assumption: _lfft_is_power_2(0x000F) == false\n", i);

    // test all numbers 2^x from 0x0000 to 0xFFFF
    for(i = 0; i < 8; ++i)
    {
        fail_unless(_lfft_is_power_2(1<<i) == true, "False assumption: _lfft_is_power_2(1<<%d) == true\n", i);
    }
}
END_TEST

void test_f_lfft_isqrt_test_loop(uint32_t start, uint32_t stop)
{
#define ROUND(x) (floor(x)+0.5f)

    uint32_t i;
    uint16_t lfft_sqrt_result;
    uint32_t sqrt_result; // must be uint32_t because the result of sqrt(0xFFFF) = UINT16_MAX+1

    for(i = start; i < stop; ++i)
    {
        lfft_sqrt_result = lfft_isqrt(i);
        sqrt_result = ROUND(sqrt((float)i));
        fail_unless(abs(lfft_sqrt_result-sqrt_result) <= 1,
                "False assumption: lfft_isqrt(%"PRIu16")=%"PRIu16" | "
                "sqrt(%"PRIu32")=%"PRIu16"\n", i, lfft_sqrt_result, i, sqrt_result);
    }

#undef ROUND
}

START_TEST(test_f_lfft_isqrt)
{

    test_f_lfft_isqrt_test_loop(0, 0xFFFF);
    test_f_lfft_isqrt_test_loop(0xFFFF0000, 0xFFFFFFFF);
}
END_TEST

void test_switching_table_test_loop(uint16_t samples, uint16_t * switching_table)
{
    uint16_t i;
    lfft_Fft fft;

    if(!lfft_fft_new(&fft, samples))
    {
        for(i = 0; i < samples; ++i)
        {
            fail_unless(fft.switching_table[i] == switching_table[i],
                    "False assumption: fft.switching_table[%"PRIu16"]=%"PRIu16" | "
                    "switching_table[%"PRIu16"]=%"PRIu16" (Samples = %"PRIu16")\n",
                    i, fft.switching_table[i], i, switching_table[i], samples);
        }
        lfft_fft_delete(&fft);
    }
}

START_TEST(test_switching_table)
{
    uint16_t samples;
    uint16_t * switching_table;

    samples = 2;
    switching_table = (uint16_t *) calloc(samples, sizeof(uint16_t));
    switching_table[0] = B_0000; switching_table[1] = B_0001;
    test_switching_table_test_loop(samples, switching_table);
    free(switching_table);

    samples = 4;
    switching_table = (uint16_t *) calloc(samples, sizeof(uint16_t));
    switching_table[0] = B_0000; switching_table[1] = B_0010;
    switching_table[2] = B_0001; switching_table[3] = B_0011;
    test_switching_table_test_loop(samples, switching_table);
    free(switching_table);

    samples = 8;
    switching_table = (uint16_t *) calloc(samples, sizeof(uint16_t));
    switching_table[0] = B_0000; switching_table[1] = B_0100;
    switching_table[2] = B_0010; switching_table[3] = B_0110;
    switching_table[4] = B_0001; switching_table[5] = B_0101;
    switching_table[6] = B_0011; switching_table[7] = B_0111;
    test_switching_table_test_loop(samples, switching_table);
    free(switching_table);

    samples = 16;
    switching_table = (uint16_t *) calloc(samples, sizeof(uint16_t));
    switching_table[0]  = B_0000; switching_table[1]  = B_1000;
    switching_table[2]  = B_0100; switching_table[3]  = B_1100;
    switching_table[4]  = B_0010; switching_table[5]  = B_1010;
    switching_table[6]  = B_0110; switching_table[7]  = B_1110;
    switching_table[8]  = B_0001; switching_table[9]  = B_1001;
    switching_table[10] = B_0101; switching_table[11] = B_1101;
    switching_table[12] = B_0011; switching_table[13] = B_1011;
    switching_table[14] = B_0111; switching_table[15] = B_1111;
    test_switching_table_test_loop(samples, switching_table);
    free(switching_table);
}
END_TEST

void test_fft_ifft_loop_int(uint16_t samples, const int32_t real[], const int32_t imag[])
{
    uint16_t i;
    bool error = false;
    int32_t * result_fft_real = (int32_t *) calloc(samples, sizeof(int32_t));;
    int32_t * result_fft_imag = (int32_t *) calloc(samples, sizeof(int32_t));;
    lfft_Fft fft;
    char buffer[0xfff];
    char str[0xff];

    if(!lfft_fft_new(&fft, samples))
    {
        sprintf(buffer, "False assumption:\n");
        lfft_fft_complex(&fft, real, imag);
        for(i = 0; i < samples; ++i)
        {
            result_fft_real[i] = lfft_fft_result_real_at(&fft, i);
            result_fft_imag[i] = lfft_fft_result_imag_at(&fft, i);
        }

        lfft_ifft_complex(&fft, result_fft_real, result_fft_imag);
        
        for(i = 0; i < samples; ++i)
        {
            if((lfft_fft_result_real_at(&fft, i) != real[i]) ||
                    (lfft_fft_result_imag_at(&fft, i) != imag[i]))
            {
                error = true;
            }

            sprintf(str, "lfft_fft_result_real_at(&fft, %"PRIu16")=%"PRId32" | "
                    "real[%"PRIu16"]=%"PRId32"\n", i, lfft_fft_result_real_at(&fft, i),
                    i, real[i]);
            strcat(buffer, str);
            sprintf(str, "lfft_fft_result_imag_at(&fft, %"PRIu16")=%"PRId32" | "
                    "imag[%"PRIu16"]=%"PRId32"\n\n", i, lfft_fft_result_imag_at(&fft, i),
                    i, imag[i]);
            strcat(buffer, str);
        }

        fail_if(error, "%s", buffer);
        lfft_fft_delete(&fft);
    }

    free(result_fft_real);
    free(result_fft_imag);
}

START_TEST(test_fft_ifft)
{
    uint16_t i;
    uint16_t samples = 8;
    int32_t * data_real = (int32_t *) calloc(samples, sizeof(int32_t));
    int32_t * data_imag = (int32_t *) calloc(samples, sizeof(int32_t));
    int32_t * n_zeros   = (int32_t *) calloc(samples, sizeof(int32_t));
    int32_t * n_ones    = (int32_t *) calloc(samples, sizeof(int32_t));

    memset(n_zeros, 0, samples*sizeof(int32_t));
    for(i = 0; i < samples; ++i)
    {
        n_ones[i]  = 1;
    }

    // data_real = {0, 0, 0, 0, 0, 0, 0, 0}
    // data_imag = {0, 0, 0, 0, 0, 0, 0, 0}
    memcpy(data_real, n_zeros, samples*sizeof(int32_t));
    memcpy(data_imag, n_zeros, samples*sizeof(int32_t));
    test_fft_ifft_loop_int(samples, data_real, data_imag);


    // data_real = {1, 1, 1, 1, 1, 1, 1, 1}
    // data_imag = {0, 0, 0, 0, 0, 0, 0, 0}
    memcpy(data_real, n_ones,  samples*sizeof(int32_t));
    memcpy(data_imag, n_zeros, samples*sizeof(int32_t));
    test_fft_ifft_loop_int(samples, data_real, data_imag);

    // data_real = {0, 0, 0, 0, 0, 0, 0, 0}
    // data_imag = {1, 1, 1, 1, 1, 1, 1, 1}
    memcpy(data_real, n_zeros, samples*sizeof(int32_t));
    memcpy(data_imag, n_ones,  samples*sizeof(int32_t));
    test_fft_ifft_loop_int(samples, data_real, data_imag);

    // data_real = {1, 1, 1, 1, 1, 1, 1, 1}
    // data_imag = {1, 1, 1, 1, 1, 1, 1, 1}
    memcpy(data_real, n_ones, samples*sizeof(int32_t));
    memcpy(data_imag, n_ones, samples*sizeof(int32_t));
    test_fft_ifft_loop_int(samples, data_real, data_imag);

    free(data_real);
    free(data_imag);
    free(n_zeros);
    free(n_ones);
}
END_TEST

Suite* a_suite()
{
    Suite * suite = suite_create ("lfft");
    TCase * tcase = tcase_create ("case");
    tcase_add_test(tcase, test_f__lfft_is_power_2);
    tcase_add_test(tcase, test_f_lfft_isqrt);
    tcase_add_test(tcase, test_switching_table);
    tcase_add_test(tcase, test_fft_ifft);
    suite_add_tcase(suite, tcase);

    return suite;
}

int main()
{
    int number_failed;
    Suite * suite = a_suite();
    SRunner * runner = srunner_create(suite);
    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

