/**
 * @file    cvec2.c
 * @author  Isaac Garzon
 * @since   22/05/2019
 *
 * @section DESCRIPTION
 *
 *  An improved generic vector implementation in ANSI C.
 *
 * @section LICENSE
 *
 *  Copyright (c) 2019 Isaac Garzon
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE
 */

#include <stdlib.h>
#include <string.h>
#include "cvec2.h"

#define VEC2_INITIAL_CAPACITY   8
#define VEC2_SWAP_SIZE          24

#define VEC2_GET(vec_ptr, el_size, idx) (vec2_data(vec_ptr) + ((idx) * (el_size)))

#define vec2_start(vec_ptr)             ((vec_ptr)->_start)
#define vec2_mem(vec_ptr, el_size)      (vec2_data(vec_ptr) - (vec2_start(vec_ptr) * el_size))

/**
 * Definition of the generic vec structure used by the code in this file.
 */
struct _vec2_impl_struct VEC2_BODY(unsigned char);

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int _vec2_reserve(struct _vec2_impl_struct *vec_ptr, size_t additional, size_t el_size)
{
    /* Check if we need to do anything */
    if (additional > vec2_capacity(vec_ptr) - vec2_size(vec_ptr))
    {
        unsigned char *new_mem = NULL;
        size_t alloc_size, new_capacity = vec2_capacity(vec_ptr) + additional;

        /* Avoid integer overflow */
        if (new_capacity < vec2_capacity(vec_ptr))
        {
            return FALSE;
        }

        alloc_size = new_capacity * el_size;

        /* Avoid integer overflow */
        if (alloc_size / el_size != new_capacity)
        {
            return FALSE;
        }

        new_mem = (unsigned char *)realloc(vec2_mem(vec_ptr, el_size), alloc_size);

        /* Check if allocation succeeded */
        if (new_mem == NULL)
        {
            return FALSE;
        }

        /* Set the new values */
        vec_ptr->data = new_mem + (el_size * vec2_start(vec_ptr));
        vec_ptr->capacity = new_capacity;
    }

    return TRUE;
}

static int _vec2_create_hole(struct _vec2_impl_struct *vec_ptr, size_t idx, size_t len, size_t el_size)
{
    /* Don't allow adding elements in arbitrary place that is beyond vec2_size(vec_ptr)
     * and avoid integer overflow */
    if ((len <= 0) || (idx > vec2_size(vec_ptr)) ||
        (vec2_size(vec_ptr) + len < vec2_size(vec_ptr)))
    {
        return FALSE;
    }

    /* Check if we need to reserve more memory */
    if (vec2_size(vec_ptr) + len > vec2_capacity(vec_ptr))
    {
        unsigned int addition = vec2_capacity(vec_ptr) ? (vec2_capacity(vec_ptr) >> 1) : VEC2_INITIAL_CAPACITY;

        /* Make sure we reserve enough to store len elements */
        while (addition < len)
        {
            size_t next_addition = (addition << 1) + (addition >> 1);

            if (next_addition < addition)
            {
                addition = len;
                break;
            }

            addition = next_addition;
        }

        /* Try to reserve place to fit at least len elements */
        while (!_vec2_reserve(vec_ptr, addition, el_size))
        {
            /* Divide by two and try again unless reserve failed even for exactly len elemnts more */
            if ((addition > len) && ((addition >> 1) < len))
            {
                addition = len;
            }
            else if ((addition >>= 1) < len)
            {
                return FALSE;
            }
        }
    }

    /* Perform all sorts of crazy calculations only if we already have some elements that
     * we might need to move around. */
    if (vec2_size(vec_ptr) > 0)
    {
        /* Check if we can create free slot at idx with simple pointer regression */
        if ((idx == 0) && (vec2_start(vec_ptr) >= len))
        {
            vec2_start(vec_ptr) -= len;
            vec_ptr->data -= len * el_size;
        }
        /* Otherwise, unlss we're required to shove the new elements at the end and we have
         * enough space there, we now need to shuffle things around */
        else if ((idx < vec2_size(vec_ptr)) ||
                 (len > (vec2_capacity(vec_ptr) - (vec2_start(vec_ptr) + vec2_size(vec_ptr)))))
        {
            /* If creating a hole at the end, assume that the pattern is end insertion and just
             * shift everything to the beginning to make room */
            size_t shift_back = (idx == vec2_size(vec_ptr)) || (len > vec2_start(vec_ptr)) ?
                vec2_start(vec_ptr) : len;

            vec2_start(vec_ptr) -= shift_back;
            vec_ptr->data -= shift_back * el_size;

            memmove(vec2_data(vec_ptr), VEC2_GET(vec_ptr, el_size, shift_back), idx * el_size);

            /* Shift elements forward if after shifting back we still need more room */
            if ((idx < vec2_size(vec_ptr)) && (len > shift_back))
            {
                memmove(VEC2_GET(vec_ptr, el_size, idx + len),
                        VEC2_GET(vec_ptr, el_size, idx + shift_back),
                        (len - shift_back) * el_size);
            }
        }
    }

    return TRUE;
}

static int _vec2_remove(struct _vec2_impl_struct *vec_ptr, size_t idx, size_t len, size_t el_size, void *opt_out_val)
{
    /* Make sure that the index is reachable */
    if ((len > vec2_size(vec_ptr)) || (vec2_size(vec_ptr) - len < idx))
    {
        return FALSE;
    }

    /* Check if we actually need to do anything */
    if (len)
    {
        /* Check if we need to copy out the values that are being removed */
        if (opt_out_val)
        {
            memcpy(opt_out_val, VEC2_GET(vec_ptr, el_size, idx), len * el_size);
        }

        vec_ptr->size -= len;

        /* Check if we need to shift elements around because of the removal */
        if (vec2_size(vec_ptr))
        {
            /* Check if we can get it done with a simple start pointer advancement */
            if (idx == 0)
            {
                vec2_start(vec_ptr) += len;
                vec_ptr->data += len * el_size;
            }
            else if (idx < vec2_size(vec_ptr))
            {
                /* Shift back len slots from `idx + len` to `idx` */
                memmove(VEC2_GET(vec_ptr, el_size, idx),
                        VEC2_GET(vec_ptr, el_size, idx + len),
                        (vec2_size(vec_ptr) - idx) * el_size);
            }
        }
    }

    return TRUE;
}

static void _vec2_clear(struct _vec2_impl_struct *vec_ptr, size_t el_size)
{
    if (vec2_data(vec_ptr) != NULL)
    {
        free(vec2_mem(vec_ptr, el_size));
    }

    memset(vec_ptr, 0, sizeof(struct _vec2_impl_struct));
}

int _vec2_impl_init(struct _vec2_impl_struct *vec_ptr)
{
    if (vec_ptr == NULL)
    {
        return FALSE;
    }

    memset(vec_ptr, 0, sizeof(struct _vec2_impl_struct));

    return TRUE;
}

int _vec2_impl_reserve(struct _vec2_impl_struct *vec_ptr, size_t additional, size_t el_size)
{
    if (!_vec2_impl_valid(vec_ptr) || !el_size)
    {
        return FALSE;
    }

    return _vec2_reserve(vec_ptr, additional, el_size);
}

int _vec2_impl_shrink(struct _vec2_impl_struct *vec_ptr, size_t el_size)
{
    if (!_vec2_impl_valid(vec_ptr) || !el_size)
    {
        return FALSE;
    }

    if (vec2_size(vec_ptr) == 0)
    {
        _vec2_clear(vec_ptr, el_size);
    }
    else if ((vec2_size(vec_ptr) < vec2_capacity(vec_ptr)) &&
             (vec2_capacity(vec_ptr) > VEC2_INITIAL_CAPACITY))
    {
        unsigned char *new_mem = NULL;
        size_t new_capacity = (vec2_size(vec_ptr) > VEC2_INITIAL_CAPACITY) ? 
            vec2_size(vec_ptr) : VEC2_INITIAL_CAPACITY;

        /* Move the data to the beginning if we're at an offset due to removals.
         * This might cause double copy if realloc allocates a new buffer for us instead
         * of shrinking the current one, but we can't just do a malloc and copy because
         * then we give up the potential of in-place shrinking, and we might run into OOM */
        if (vec2_start(vec_ptr) > 0)
        {
            unsigned char *mem = vec2_mem(vec_ptr, el_size);

            memmove(mem, vec2_data(vec_ptr), el_size * vec2_size(vec_ptr));
            vec_ptr->data = mem;
            vec2_start(vec_ptr) = 0;
        }

        new_mem = (unsigned char *)realloc(vec2_data(vec_ptr), new_capacity * el_size);

        if (new_mem == NULL)
        {
            return FALSE;
        }

        vec_ptr->data = new_mem;
        vec_ptr->capacity = vec2_size(vec_ptr);
    }

    return TRUE;
}

int _vec2_impl_create_hole(struct _vec2_impl_struct *vec_ptr, size_t idx, size_t len, size_t el_size)
{
    if (!_vec2_impl_valid(vec_ptr) || (len <= 0) || !el_size ||
        !_vec2_create_hole(vec_ptr, idx, len, el_size))
    {
        return FALSE;
    }


    vec_ptr->_idx[0] = idx;
    return TRUE;
}

int _vec2_impl_swap(struct _vec2_impl_struct *vec_ptr, size_t first, size_t second, size_t el_size)
{
    if (!_vec2_impl_valid(vec_ptr) ||
        (first >= vec2_size(vec_ptr)) || (second >= vec2_size(vec_ptr)) || !el_size)
    {
        return FALSE;
    }

    /* Check if we need to do anything */
    if (first != second)
    {
        union
        {
            size_t align_dummy;
            unsigned char bytes[VEC2_SWAP_SIZE];
        } swap;
        size_t left_bytes = el_size;

        for (; left_bytes >= sizeof(swap); left_bytes -= sizeof(swap))
        {
            memcpy(swap.bytes, VEC2_GET(vec_ptr, el_size, first), sizeof(swap));
            memcpy(VEC2_GET(vec_ptr, el_size, first), VEC2_GET(vec_ptr, el_size, second), sizeof(swap));
            memcpy(VEC2_GET(vec_ptr, el_size, second), swap.bytes, sizeof(swap));
        }

        if (left_bytes)
        {
            memcpy(swap.bytes, VEC2_GET(vec_ptr, el_size, first), left_bytes);
            memcpy(VEC2_GET(vec_ptr, el_size, first), VEC2_GET(vec_ptr, el_size, second), left_bytes);
            memcpy(VEC2_GET(vec_ptr, el_size, second), swap.bytes, left_bytes);
        }
    }

    return TRUE;
}

int _vec2_impl_sort(struct _vec2_impl_struct *vec_ptr, _vec2_impl_cmpfn cmpfn, size_t el_size)
{
    if (!_vec2_impl_valid(vec_ptr) || (cmpfn == NULL) || !el_size)
    {
        return FALSE;
    }

    /* Check if we need to sort anything */
    if (vec2_size(vec_ptr))
    {
        qsort(VEC2_GET(vec_ptr, el_size, 0), vec2_size(vec_ptr), el_size, cmpfn);
    }

    return TRUE;
}

int _vec2_impl_assign(struct _vec2_impl_struct *vec_ptr, size_t idx, const void *val, size_t len, size_t el_size)
{
    if (!_vec2_impl_valid(vec_ptr) || (val == NULL) || !el_size ||
        (idx >= vec2_size(vec_ptr)) || (vec2_size(vec_ptr) - idx < len))
    {
        return FALSE;
    }

    memmove(VEC2_GET(vec_ptr, el_size, idx), val, len * el_size);
    return TRUE;
}

int _vec2_impl_remove(struct _vec2_impl_struct *vec_ptr, size_t idx, size_t len, size_t el_size, void *out)
{
    if (!_vec2_impl_valid(vec_ptr) || !el_size)
    {
        return FALSE;
    }

    return _vec2_remove(vec_ptr, idx, len, el_size, out);
}

int _vec2_impl_insert(struct _vec2_impl_struct *vec_ptr, size_t idx, const void *val, size_t len, size_t el_size)
{
    if (!_vec2_impl_valid(vec_ptr) || (val == NULL) || !el_size)
    {
        return FALSE;
    }

    if (len > 0)
    {
        if (!_vec2_create_hole(vec_ptr, idx, len, el_size))
        {
            return FALSE;
        }

        /* Copy len elements from val into the free slots */
        memcpy(VEC2_GET(vec_ptr, el_size, idx), val, el_size * len);
        vec_ptr->size += len;
    }

    return TRUE;
}

void _vec2_impl_clear(struct _vec2_impl_struct *vec_ptr, size_t el_size)
{
    if (_vec2_impl_valid(vec_ptr))
    {
        _vec2_clear(vec_ptr, el_size);
    }
}

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */
