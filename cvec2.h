/**
 * @file    cvec2.h
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

#ifndef _GENERIC_CVEC_H_
#define _GENERIC_CVEC_H_

#include <stddef.h>

/****************************************************************************************
  Internal Macro Definitions
 ***************************************************************************************/
/**
 * @internal
 * Defines NULL in case it's not defined already
 */
#ifndef NULL
#    define NULL 0
#endif

/**
 * @internal
 * Defines FALSE in case it's not defined already
 */
#ifndef FALSE
#    define FALSE 0
#endif

/**
 * @internal
 * Defines TRUE in case it's not defined already
 */
#ifndef TRUE
#    define TRUE (!FALSE)
#endif

/**
 * @internal
 * Validates a pointer to a <code>vec</code> structure.
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure to validate.
 *
 * @return    TRUE if the pointer points to a valid <code>vec</code> structure.
 *            FALSE otherwise.
 */
#define _vec2_impl_valid(vec_ptr) \
    (((vec_ptr) != NULL) && \
     ((!vec2_capacity(vec_ptr) && vec2_data(vec_ptr) == NULL) || \
      (vec2_size(vec_ptr) <= vec2_capacity(vec_ptr))))

/****************************************************************************************
  Internal Type Definitions
 ***************************************************************************************/
/**
 * @internal
 * Forward declaration of the generic <code>vec</code> structure
 */
struct _vec2_impl_struct;

/**
 * @internal
 * Defines the generic comparer function.
 */
typedef int (*_vec2_impl_cmpfn)(const void *, const void *);

/****************************************************************************************
  Internal Function Declarations
 ***************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @internal
 * @brief   Initializes a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a gneric <code>vec</code> structure to initialize.
 *
 * @return    TRUE if the initialization succeeded.
 *            FALSE otherwise.
 */
extern int (_vec2_impl_init)(struct _vec2_impl_struct *vec_ptr);

/**
 * @internal
 * @brief   Reserves additional memory capacity in a <code>vec</code>
 *
 * @param[in] vec_ptr    Pointer to a generic <code>vec</code> structure.
 * @param[in] additional The additional capacity.
 * @param[in] el_size    The size of an element in the <code>vec</code>.
 *
 * @return    TRUE if the reservation succeeded.
 *            FALSE otherwise.
 */
extern int (_vec2_impl_reserve)(struct _vec2_impl_struct *vec_ptr, size_t additional, size_t el_size);

/**
 * @internal
 * @brief   Shrinks the underlying memory bufer of the <code>vec</code> to fit its size.
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in] el_size   The size of an element in the <code>vec</code>.
 *
 * @return    TRUE if the shrinking succeeded.
 *            FALSE otherwise.
 */
extern int (_vec2_impl_shrink)(struct _vec2_impl_struct *vec_ptr, size_t el_size);

/**
 * @internal
 * @brief   Create a hole to hold one or more elements at a specific index in a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a generic <code>vec</code> structure.
 * @param[in] idx       The index at which to create the hole.
 * @param[in] len       The size of the hole in elements.
 * @param[in] el_size   The size of an element in the <code>vec</code>.
 *
 * @note      This function does not change the size of the <code>vec</code>.
 *            The caller is responsible for adjusting the size.
 *
 * @return    TRUE if the hole was successfully created.
 *            FALSE otherwise.
 */
extern int (_vec2_impl_create_hole)(struct _vec2_impl_struct *vec_ptr, size_t idx, size_t len, size_t el_size);

/**
 * @internal
 * @brief   Swaps two elements in a <code>vec</code>
 *
 * @param[in]  vec_ptr  Pointer to a generic <code>vec</code> structure.
 * @param[in]  first    The index of the first element.
 * @param[in]  second   The index of the second element.
 * @param[in]  el_size  The size of an element in the <code>vec</code>.
 *
 * @return     TRUE if the swap suceeded.
 *             FALSE otherwise.
 */
extern int (_vec2_impl_swap)(struct _vec2_impl_struct *vec_ptr, size_t first, size_t second, size_t el_size);

/**
 * @internal
 * @brief   Sorts a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a generic <code>vec</code> structure.
 * @param[in] cmpfn     Pointer to comparer function.
 * @param[in] el_size   The size of an element in the <code>vec</code>.
 *
 * @return     TRUE if the sort succeeded.
 *             FALSE otherwise.
 */
extern int (_vec2_impl_sort)(struct _vec2_impl_struct *vec_ptr, _vec2_impl_cmpfn cmpfn, size_t el_size);

/**
 * @internal
 * @brief   Assigns one or more elements at a specific index in a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a generic <code>vec</code> structure.
 * @param[in] idx       The index at which to assign the elements.
 * @param[in] val       Pointer to the elements to assign.
 * @param[in] len       The amount of elements to assign.
 * @param[in] el_size   The size of an element in the <code>vec</code>.
 *
 * @return    TRUE if the assignment succeeded.
 *            FALSE otherwise.
 */
extern int (_vec2_impl_assign)(struct _vec2_impl_struct *vec_ptr, size_t idx, const void *val, size_t len, size_t el_size);

/**
 * @internal
 * @brief   Inserts one or more elements at a specific index in a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a generic <code>vec</code> structure.
 * @param[in] idx       The index at which to insert the elements.
 * @param[in] val       Pointer to the elements to insert.
 * @param[in] len       The amount of elements to insert.
 * @param[in] el_size   The size of an element in the <code>vec</code>.
 *
 * @return    TRUE if the insertion succeeded.
 *            FALSE otherwise.
 */
extern int (_vec2_impl_insert)(struct _vec2_impl_struct *vec_ptr, size_t idx, const void *val, size_t len, size_t el_size);

/**
 * @internal
 * @brief   Removes elements at a specific index from a <code>vec</code>
 *
 * @param[in]  vec_ptr   Pointer to a generic <code>vec</code> structure.
 * @param[in]  idx       The index of the element to remove.
 * @param[in]  len       The amount of elements to remove.
 * @param[in]  el_size   The size of an element in the <code>vec</code>.
 * @param[out] out       Optional pointer to store the removed elements in.
 *
 * @return    TRUE if the removal succeeded.
 *            FALSE otherwise.
 */
extern int (_vec2_impl_remove)(struct _vec2_impl_struct *vec_ptr, size_t idx, size_t len, size_t el_size, void *out);

/**
 * @internal
 * @brief   Clears a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a generic <code>vec</code> structure.
 * @param[in] el_size   The size of an element in the <code>vec</code>.
 *
 * @note    This function must be called when there's no more use for
 *          the initialized <code>vec</code>.
 */
extern void (_vec2_impl_clear)(struct _vec2_impl_struct *vec_ptr, size_t el_size);

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

/****************************************************************************************
  External Macro Definitions
 ***************************************************************************************/
/**
 * Defines the body of a <code>vec</code> struct of type <code>type</code>.
 */
#define VEC2_BODY(type) \
    { \
        size_t size; \
        size_t capacity; \
        /* The following member is defined as an array in order
         * to prevent passing of <code>void</code> as <code>type</code> */ \
        size_t _idx[sizeof(type) / sizeof(type)]; \
        size_t _start; \
        type  *data; \
    }

/**
 * Defines a static initializer for the <code>vec</code> struct defined using <code>VEC2_BODY</code>.
 */
#define VEC2_INITIALIZER { 0, 0, { 0 }, 0, NULL }

/**
 * @brief   Initializes a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 *
 * @return    TRUE if the initialization succeeded.
 *            FALSE otherwise.
 */
#define vec2_init(vec_ptr) \
    (_vec2_impl_init)((struct _vec2_impl_struct *)(vec_ptr))

/**
 * @brief   Reserves additional memory in a <code>vec</code>
 *
 * @param[in] vec_ptr    Pointer to a <code>vec</code> structure.
 * @param[in] additional The additional capacity to reserve.
 *
 * @return    TRUE if the reservation succeeded.
 *            FALSE otherwise.
 */
#define vec2_reserve(vec_ptr, additional) \
    (_vec2_impl_reserve)((struct _vec2_impl_struct *)(vec_ptr), additional, sizeof(*vec2_data(vec_ptr)))

/**
 * @brief   Shrinks the underlying memory bufer of the <code>vec</code> to fit its size.
 *
 * @param[in] vec_ptr    Pointer to a <code>vec</code> structure.
 *
 * @return    TRUE if the shrinking succeeded.
 *            FALSE otherwise.
 */
#define vec2_shrink_to_fit(vec_ptr) \
    (_vec2_impl_shrink)((struct _vec2_impl_struct *)(vec_ptr), sizeof(*vec2_data(vec_ptr)))

/**
 * @brief   Gets the amount of elements stored in a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 *
 * @return    The amount of elements stored in the <code>vec</code>.
 */
#define vec2_size(vec_ptr) \
    ((vec_ptr)->size | (size_t)0) /* Prevent accidental mutation of size */

/**
 * @brief   Gets the capacity of a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 *
 * @return    The capacity of the <code>vec</code>.
 */
#define vec2_capacity(vec_ptr) \
    ((vec_ptr)->capacity | (size_t)0) /* Prevent accidental mutation of capacity */

/**
 * @brief   Checks if a <code>vec</code> is empty.
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 *
 * @return  TRUE if the <code>vec</code> is empty.
 *          FALSE otherwise.
 */
#define vec2_empty(vec_ptr) \
    (vec2_size(vec_ptr) == 0)

/**
 * @brief   Gets the address of the underlying memory buffer of the <code>vec</code>.
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 *
 * @return  Pointer to the underlying memory buffer of the <code>vec</code>. Might be NULL.
 */
#define vec2_data(vec_ptr) \
    ((vec_ptr)->data + 0) /* Prevent accidental mutation of data */

/**
 * @brief   Gets an iterator to the beginning of the <code>vec</code>.
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 *
 * @return  An iterator to the beginning of the <code>vec</code>.
 */
#define vec2_begin(vec_ptr) \
    vec2_data(vec_ptr)

/**
 * @brief   Gets an iterator past the end of the <code>vec</code>.
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 *
 * @return  An iterator past the end of the <code>vec</code>.
 */
#define vec2_end(vec_ptr) \
    (vec2_data(vec_ptr) + \
        (_vec2_impl_valid(vec_ptr) ? vec2_size(vec_ptr) : 0))

/**
 * @brief   Gets an element from a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in] idx       The index of the element.
 *
 * @return    Pointer to the element if @p vec_ptr and @p idx are valid.
 *            NULL otherwise.
 */
#define vec2_get(vec_ptr, idx) \
    (_vec2_impl_valid(vec_ptr) && ((idx) < vec2_size(vec_ptr)) ? \
        &vec2_data(vec_ptr)[idx] : \
        NULL)

/**
 * @brief   Gets the first element in a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 *
 * @return    Pointer to the first element if any.
 *            NULL otherwise.
 */
#define vec2_first(vec_ptr) \
    vec2_get(vec_ptr, 0)

/**
 * @brief   Gets the last element in a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 *
 * @return    Pointer to the last element if any.
 *            NULL otherwise.
 */
#define vec2_last(vec_ptr) \
    (_vec2_impl_valid(vec_ptr) && (vec2_size(vec_ptr) > 0) ? \
        &vec2_data(vec_ptr)[vec2_size(vec_ptr) - 1] : \
        NULL)

/**
 * @brief   Swaps two elements in a <code>vec</code>
 *
 * @param[in]  vec_ptr  Pointer to a <code>vec</code> structure.
 * @param[in]  first    The index of the first element.
 * @param[in]  second   The index of the second element.
 *
 * @return     TRUE if the swap suceeded.
 *             FALSE otherwise.
 */
#define vec2_swap(vec_ptr, first, second) \
    (_vec2_impl_swap)((struct _vec2_impl_struct *)(vec_ptr), \
        first, second, sizeof(*vec2_data(vec_ptr)))

/**
 * @brief   Sorts a <code>vec</code>
 *
 * @param[in]  vec_ptr  Pointer to a <code>vec</code> structure.
 * @param[in]  cmpfn    Pointer to comparer function for type <code>type</code>.
 *
 * @return    TRUE if the sort succeeded.
 *            FALSE otherwise.
 */
#define vec2_sort(vec_ptr, cmpfn) \
    ((void)sizeof(cmpfn(vec2_data(vec_ptr), vec2_data(vec_ptr))), /* Type-safety enforcement */ \
        (_vec2_impl_sort)((struct _vec2_impl_struct *)(vec_ptr), \
            (_vec2_impl_cmpfn)cmpfn, sizeof(*vec2_data(vec_ptr))))

/**
 * @brief   Removes an element from the end of a <code>vec</code>
 *
 * @param[in]  vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[out] out       Optional pointer to store the removed element in.
 *
 * @return    TRUE if the removal succeeded.
 *            FALSE otherwise.
 */
#define vec2_pop(vec_ptr, out) \
    vec2_pop_multi(vec_ptr, 1, out)

/**
 * @brief   Removes multiple elements from the end of a <code>vec</code>
 *
 * @param[in]  vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in]  len       The amount of elements to pop from the <code>vec</code>.
 * @param[out] out       Optional pointer to store the removed elements in.
 *
 * @return    TRUE if the removal succeeded.
 *            FALSE otherwise.
 */
#define vec2_pop_multi(vec_ptr, len, out) \
    vec2_remove(vec_ptr, vec2_size(vec_ptr) - (len), len, out)

/**
 * @brief   Removes an element from the beginning of a <code>vec</code>
 *
 * @param[in]  vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[out] out       Optional pointer to store the removed element in.
 *
 * @return    TRUE if the removal succeeded.
 *            FALSE otherwise.
 */
#define vec2_shift(vec_ptr, out) \
    vec2_shift_multi(vec_ptr, 1, out)

/**
 * @brief   Removes multiple elements from the beginning of a <code>vec</code>
 *
 * @param[in]  vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in]  len       The amount of elements to unshift.
 * @param[out] out       Optional pointer to store the removed elements in.
 *
 * @return    TRUE if the removal succeeded.
 *            FALSE otherwise.
 */
#define vec2_shift_multi(vec_ptr, len, out) \
    vec2_remove(vec_ptr, 0, len, out)

/**
 * @brief   Inserts a value at a specific index in a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in] idx       The index at which to insert the value.
 * @param[in] val       The value to insert.
 *
 * @return    TRUE if the insertion succeeded.
 *            FALSE otherwise.
 */
#define vec2_insert(vec_ptr, idx, val) \
    ((_vec2_impl_create_hole)((struct _vec2_impl_struct *)(vec_ptr), \
            idx, 1, sizeof(*vec2_data(vec_ptr))) && \
        (vec2_data(vec_ptr)[(vec_ptr)->_idx[0]] = (val), ++(vec_ptr)->size, TRUE))

/**
 * @brief   Inserts a value passed by a pointer at a specific index in a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in] idx       The index at which to insert the value.
 * @param[in] val       Pointer to the value to insert.
 *
 * @return    TRUE if the insertion succeeded.
 *            FALSE otherwise.
 */
#define vec2_insert_ptr(vec_ptr, idx, val) \
    vec2_insert_multi(vec_ptr, idx, val, 1)

/**
 * @brief   Inserts multiple elements at a specific index in a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in] idx       The index at which to insert the value.
 * @param[in] val       The array of elements to insert.
 * @param[in] len       The amount of elements to insert.
 *
 * @return    TRUE if the insertion succeeded.
 *            FALSE otherwise.
 */
#define vec2_insert_multi(vec_ptr, idx, val, len) \
    ((void)sizeof(*vec2_data(vec_ptr) = (val)[0]), /* Type-safety enforcement */ \
     (_vec2_impl_insert)((struct _vec2_impl_struct *)(vec_ptr), \
        idx, val, len, sizeof(*vec2_data(vec_ptr))))

/**
 * @brief   Assigns a value to an element in a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in] idx       The index to assign the value to.
 * @param[in] val       The value to assign.
 *
 * @note      @p idx is valid only if its value is between 0 and the
 *            <code>vec</code>'s size (exclusive).
 *
 * @return    TRUE if the assignment succeeded.
 *            FALSE otherwise.
 */
#define vec2_assign(vec_ptr, idx, val) \
    (_vec2_impl_valid(vec_ptr) && \
        (((vec_ptr)->_idx[0] = (idx)) < vec2_size(vec_ptr)) && \
        (vec2_data(vec_ptr)[(vec_ptr)->_idx[0]] = (val), TRUE))

/**
 * @brief   Assigns a value passed by a pointer to an element in a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in] idx       The index to assign the value to.
 * @param[in] val       Pointer to the value to assign.
 *
 * @note      @p idx is valid only if its value is between 0 and the
 *            <code>vec</code>'s size (exclusive).
 *
 * @return    TRUE if the assignment succeeded.
 *            FALSE otherwise.
 */
#define vec2_assign_ptr(vec_ptr, idx, val) \
    vec2_assign_multi(vec_ptr, idx, val, 1)

/**
 * @brief   Assigns multiple elements to existing slots in a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in] idx       The index to assign the value to.
 * @param[in] val       Pointer to the elements to assign.
 * @param[in] len       THe amount of elements to assign.
 *
 * @note      @p idx and @ len are valid only if the range [idx, idx + len) fits
 *            in [0, vec2_size(vec_ptr)).
 *
 * @return    TRUE if the assignment succeeded.
 *            FALSE otherwise.
 */
#define vec2_assign_multi(vec_ptr, idx, val, len) \
    ((void)sizeof(vec2_data(vec_ptr) == (out)), /* Type-safety enforcement */ \
     (_vec2_impl_assign)((struct _vec2_impl_struct *)(vec_ptr), \
        idx, val, len, sizeof(*vec2_data(vec_ptr))))

/**
 * @brief   Pushes multiple elements to the end of a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in] val       The array of elements to push.
 * @param[in] len       The amount of elements from the array to push.
 *
 * @return    TRUE if the push succeeded.
 *            FALSE otherwise.
 */
#define vec2_push_multi(vec_ptr, val, len) \
    vec2_insert_multi(vec_ptr, vec2_size(vec_ptr), val, len)

/**
 * @brief   Pushes a value to the end of a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in] val       The value to push.
 *
 * @return    TRUE if the push succeeded.
 *            FALSE otherwise.
 */
#define vec2_push(vec_ptr, val) \
    vec2_insert(vec_ptr, vec2_size(vec_ptr), val)

/**
 * @brief   Pushes a value passed by a pointer to the end of a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in] val       Pointer to the value to push.
 *
 * @return    TRUE if the push succeeded.
 *            FALSE otherwise.
 */
#define vec2_push_ptr(vec_ptr, val) \
    vec2_push_multi(vec_ptr, val, 1)

/**
 * @brief   Insert a value to the beginning of a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in] val       The value to insert.
 *
 * @return    TRUE if the insertion succeeded.
 *            FALSE otherwise.
 */
#define vec2_unshift(vec_ptr, val) \
    vec2_insert(vec_ptr, 0, val)

/**
 * @brief   Insert a value passed by a pointer to the beginning of a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in] val       Pointer to the value to insert.
 *
 * @return    TRUE if the insertion succeeded.
 *            FALSE otherwise.
 */
#define vec2_unshift_ptr(vec_ptr, val) \
    vec2_insert_ptr(vec_ptr, 0, val)

/**
 * @brief   Insert multiple elements to the beginning of a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in] val       The array of elements to insert.
 * @param[in] len       The amount of elements to insert.
 *
 * @return    TRUE if the insertion succeeded.
 *            FALSE otherwise.
 */
#define vec2_unshift_multi(vec_ptr, val, len) \
    vec2_insert_multi(vec_ptr, 0, val, len)

/**
 * @brief   Removes elements at a specific index from a <code>vec</code>
 *
 * @param[in]  vec_ptr   Pointer to a <code>vec</code> structure.
 * @param[in]  idx       The index of the element to remove.
 * @param[in]  len       The amount of elements to remove.
 * @param[out] out       Optional pointer to store the removed elements in.
 *
 * @return    TRUE if the removal succeeded.
 *            FALSE otherwise.
 */
#define vec2_remove(vec_ptr, idx, len, out) \
    ((void)sizeof(vec2_data(vec_ptr) == (out)), /* Type-safety enforcement */ \
     (_vec2_impl_remove)((struct _vec2_impl_struct *)(vec_ptr), \
        idx, len, sizeof(*vec2_data(vec_ptr)), out))

/**
 * @brief   Clears a <code>vec</code>
 *
 * @param[in] vec_ptr   Pointer to a <code>vec</code> structure.
 *
 * @note    This function must be called when there's no more use for
 *          the initialized <code>vec</code>.
 */
#define vec2_clear(vec_ptr) \
    (_vec2_impl_clear)((struct _vec2_impl_struct *)(vec_ptr), sizeof(*vec2_data(vec_ptr)))

#endif /* !_GENERIC_CVEC_H_ */
