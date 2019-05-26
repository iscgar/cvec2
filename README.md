# cvec2 #

A generic, type-safe resizable array implementation in ANSI C, inspired by C++'s `std::vector`, Rust's `Vec`,
and JavaScript's `Array`. This is a rewrite of a [weird earlier attemp](https://github.com/iscgar/cvec)
that I did a few years ago (I hope that explains the 2 in the name).

## Table of Contents ##

 * [Goal](#goal)
 * [Usage](#usage)
 * [License](#license)
 * [Caveats](#caveats)
 * [Introduction](#introduction)
 * [API Reference](#api-reference)

## Goal ##

I just wanted a generic resizable array that I could use in my ANSI C projects (yes, I needed to
write ANSI C for environments that don't support anything beyond C89/90). The goal was to have
a safe and correct implementation, a nice interface, small memory footprint (for constrained
environments), and resonable performance (in that order of priority).

## Usage ##

Just drop the [cvec2.c](cvec2.c?raw=1) and [cvec2.h](cvec2.h?raw=1) files anywhere in your project
and compile along.

## License ##

This library is licensed under the MIT license. See [LICENSE](LICENSE) for details.

## Caveats ##

In order to be generic this library relies heavily on macros. While that creates a nice interface,
unfortunately it also requires that the first argument of every "function" (the pointer to the vector
structure) be an expression that is free of side effects. That means that you should take care not
to do the following:

```c
vec2_push(vptr++, 0); /* Bad. Might result in memory corruption */
```

Nor should you do the following if calling the function `foo()` has side effects:

```c
vec2_push(foo(), 0); /* Woops. foo() might be called more than once */
```

This restriction applies to almost the entire API; and while care was taken to keep this restriction
to the vector pointer argument, unfortunately some of the API "functions" have additional side effects
restrictions on their arguments, so be sure to check the reference below if in doubt.

## Introduction ##

Since C doesn't have a way to define a type that accepts a type argument and is generic over it,
this library can't define a generic vector type. What you need to do, then, is to define such a type
for every type that you want to store in a `cvec2` vector. This, however, doesn't mean that you need to
reimplement all the logic yourself (otherwise what's the point of this library?). You just need to use
the `VEC2_BODY` macro to define your type:

```c
struct int_vector VEC2_BODY(int);

/* Or, if you prefer not to keep it in the struct namespace: */
typedef int_vector_tag /* <- optional tag */ VEC2_BODY(int) int_vector;

/* I prefer the explicit struct namespacing, so all of the examples will use the first form */
```

The type argument to `VEC2_BODY` can be any type. Well, almost any type. Function pointer types cannot be passed
directly to `VEC2_BODY` due to C's declaration syntax. You'd need to `typedef` them and then pass the type alias.
The same goes, if for some unknown reason you find yourself wanting to use `cvec2` from C++ with a template type
that has more than a single template argument -- you'd need to `typedef` it before passing it to the macro,
because otherwise the preprocessor would just treat the comma as an argument separator and error out (but I really
hope that you won't consider using this library from C++. It's not built for C++, and it will not run destructors
on removal of items).

In any case, after defining the type you can just declare and define variables of type `int_vector` and pass
them to `cvec2`'s API functions.

Speaking of defining a vector variable, in order to use any of the API functions you first need to initialize
the vector structure. You do it by calling the `vec2_init` function, or simply by using the static initializer
constant:

```c
struct int_vector v = VEC2_INITIALIZER;

/* Or, if you prefer to do it dynamically (for vector structures allocated using malloc, etc.): */
struct int_vector *pv = (struct int_vector *)malloc(sizeof(struct int_vector));
assert(vec2_init(pv));
```

Having an empty initialized vector isn't that useful on its own, so you can use the insertion functions to add
items to the vector:

```c
assert(vec2_push(&v, 1)); /* Insert 1 at the end of the vector */
assert(vec2_unshift(&v, -1)); /* Insert -1 at the beginning of the vector (inspired by JavaScript, told you) */
assert(vec2_insert(&v, 1, 0)); /* Insert 0 at index 1 */
```

If you're working with pointers to `int`s, you can use the `_ptr` versions instead of dereferencing:

```c
assert(vec2_push_ptr(&v, one)); /* Insert 1 at the end of the vector */
assert(vec2_unshift_ptr(&v, minus_one)); /* Insert -1 at the beginning of the vector (again, blame JavaScript) */
assert(vec2_insert_ptr(&v, 1, zero)); /* Insert 0 at index 1 */
```

(assuming `one`, `minus_one`, and `zero` are variables that are pointers to `int`, of course).

And, if you're already working with pointers, you can just insert multiple items at once:

```c
assert(vec2_push_multi(&v, arr0, 5)); /* Insert 5 items from arr0 at the end of the vector */
assert(vec2_unshift_multi(&v, arr1, 2)); /* Insert 2 items from arr1 at the beginning of the vector (you know the drill) */
assert(vec2_insert_multi(&v, 3, arr2, 11)); /* Insert 11 items from arr2 somewhere in the middle */
```

You can query the size and capacity of the vector at any time using `vec2_size` and `vec2_capacity`:

```c
printf("The size of the vector is %zu and the capacity is %zu\n", vec2_size(&v), vec2_capacity(&v));
```

If you just want to check if the vector is empty, `vec2_empty` might come in handy:

```c
printf("The vector is %s\n", vec2_empty(&v) ? "empty" : "not empty");
```

Now you probably didn't put all of these items in the vector just to query the size of the vector. You can access them
using one of the accessor functions:

```c
printf("The first element is %d, The last is %d, and the one at index 1 is %d\n",
       *vec2_first(&v),
       *vec2_last(&v),
       *vec2_get(&v, 1));
```

If you're wondering why the code above dereferences the result of the call to the accessor functions, that's
because these function do bound-checking to enure that out of bounds access doesn't occur (remember that safety
and correctness is the first goal of this library?). In case of failure they return `NULL`, which is why you
should probably check the return value before dereferencing it.

If you don't want to incur the cost of bound checks on every item subscript (e.g. due of performace concerns),
you can just access the underlying memory buffer directly using `vec2_data`:

```c
printf("Unsafely accessing the element at index 1: %d\n", vec2_data(&v)[1]);
```

Use `vec2_data` wisely. Remember that bound-checking is there for a reason, so instead of having to decide between checking
the result of `vec2_get` for every access and using `vec2_data`, when iterating over the vector you can use an iterator
with `vec2_begin` and `vec2_end`:

```c
/* Assuming a C99+ compiler */
for (int *current = vec2_begin(&v), *end = vec2_end(&v); current != end; ++current)
{
    printf("Current item is %d\n", *current);
}
```

Now that you know all of that, assigning to an existing cell in the vector should be easy by dereferencing the pointer returned
from the accessor functions, but there's an even easier way using one of the assignment functions:

```c
assert(vec2_assign(&v, 1, 42)); /* Assign 42 at index 1 */

/* Or using a pointer (useful for struct assignments) */
int a = 5;
assert(vec2_assign_ptr(&v, 0, &a));

/* You can also assign multiple values at once, assuming the destination cells are all currently used in the vector */
assert(vec2_assign_multi(&v, 2, arr0, 5));
```

Removing items is also possible. This is how you'd remove items from the edges:

```c
assert(vec2_pop(&v, NULL)); /* Remove an item from the end of the vector, optionally copying it to an output pointer */
assert(vec2_shift(&v, NULL)); /* Remove an item from the beginning of the vector, optionally copying it to an output pointer */
```

You can also remove multiple items at once:

```c
assert(vec2_pop_multi(&v, 4, NULL)); /* Remove 4 items from the end of the vector */
assert(vec2_shift_multi(&v, 3, NULL)); /* Remove 3 items from the beginning of the vector */
```

Or you can use `vec2_remove` to arbitrarily remove however many items you want from anywhere in the vector (as long as
the range is still inside the vector bounds, of course):

```c
assert(vec2_remove(&v, 2, 3, NULL)); /* Remove 3 items from index 2 */
```

Note that vector will not release allocated memory when items are removed. To reclain free memory after removals you
should call the `vec2_shrink_to_fit` function:

```c
assert(vec2_shrink_to_fit(&v));
```

Lastly, when there's no more use for the vector, you must call the `vec2_clear` function in order to free the
allocated memory used by the vector:

```c
int i;
struct int_vector v = VEC2_INITIALIZER;

for (i = 0; i < 10; ++i)
{
    vec2_push(&v, i + 1);
}

vec2_clear(&v);
```

Now go check the API reference below. There are a few goodies that haven't been mentioned in this intro.

## API Reference ##

All vector functions are called through the macros defined int `cvec2.h`. See [Caveats](#caveats) above for certain things
to watch out for.

#### `VEC2_BODY(T)`
A macro that defines the structure body for vector of type `T`.
```c
struct filep_vec VEC2_BODY(FILE*);
```

#### `VEC2_INITIALIZER`
A macro that defines the static initialization value for a structure defined using `VEC2_BODY`..
```c
struct filep_vec v = VEC2_INITIALIZER;
```

#### `vec2_init(vec_ptr)`
Initializes a vector. This function must be called before any other operation on the vector if `VEC2_INITIALIZER`
isn't used. Otherwise the other functions will either fail or your application will segfault. Returns `FALSE` if
passed a NULL pointer.

#### `vec2_clear(vec_ptr)`
Clears the elements in the vector and frees the memory allocated for them. To prevent memory leaks this
function must be called when there's no more use for the vector.

#### `vec2_size(vec_ptr)`
Return the size of the vector.

#### `vec2_empty(vec_ptr)`
Returns `TRUE` when the vector is empty. `FALSE` otherwise.

#### `vec2_capacity(vec_ptr)`
Returns the current capacity of the vector. 0 if no memory is currently allocated.

#### `vec2_get(vec_ptr, idx)`
Returns a pointer to the element at `idx`. NULL if `vec_ptr` points to an invalid vector structure or `idx` is outside
the vector's bounds. Note that this pointer is invalid after a call to any function which mutates the vector. Also note
that `idx` must be an expression that is free from side effects.

#### `vec2_first(vec_ptr)`
Returns a pointer to the first element in the vector. NULL if `vec_ptr` points to an invalid vector structure or if it's empty.
Note that this pointer is invalid after a call to any function which mutates the vector.

#### `vec2_last(vec_ptr)`
Returns a pointer to the last element in the vector. NULL if `vec_ptr` points to an invalid vector structure or if it's empty.
Note that this pointer is invalid after a call to any function which mutates the vector.

#### `vec2_data(vec_ptr)`
Returns a pointer to the underlying memory buffer of the vector. Might be NULL. Note that this pointer is invalid after a call
to any function which mutates the vector.

#### `vec2_begin(vec_ptr)`
Returns a pointer to the beginning of the vector. Note that this pointer is invalid if it's pointing to `vec2_end()` or beyond it.
This pointer is also invalid after a call to any function which mutates the vector.

#### `vec2_end(vec_ptr)`
Returns a pointer past the end of the vector. Note that this pointer is invalid after a call to any function which mutates the vector.

#### `vec2_reserve(vec_ptr, n)`
Requests that the vector capacity be at least enough to contain `n` additional elements. If `n` is 0 or if the
current capacity already satisfies the request, the function does nothing. Returns `TRUE` if the reservation
succeeded. `FALSE` otherwise.

```c
struct VEC2_BODY(int) v;
assert(vec2_init(&v));
uint capacity = vec2_capacity(&v); /* 0 */
if (vec2_reserve(&v, 25)) {
    capacity = vec2_capacity(&v); /* >= 25 */
}
```

#### `vec2_push(vec_ptr, v)`
Inserts a value `v` to the end of the vector. Return `TRUE` if `vec_ptr` points to a valid vector structure and insertion
succeeded. `FALSE` otherwise.

#### `vec2_push_ptr(vec_ptr, v_ptr)`
Inserts a value pointed to by the pointer `v_ptr` to the end of the vector. Return `TRUE` if `vec_ptr` points to a valid
vector structure, `v_ptr` is not NULL, and insertion succeeded. `FALSE` otherwise.

#### `vec2_push_multi(vec_ptr, arr, len)`
Inserts `len` elements from the array `arr` to the end of the vector. Return `TRUE` if `vec_ptr` points to a valid
vector structure, `arr` is not NULL, and insertion succeeded. `FALSE` otherwise.

#### `vec2_unshift(vec_ptr, v)`
Inserts a value `v` to the beginning of the vector. Return `TRUE` if `vec_ptr` points to a valid vector structure, and
insertion succeeded. `FALSE` otherwise.

#### `vec2_unshift_ptr(vec_ptr, v_ptr)`
Inserts a value pointed to by the pointer `v_ptr` to the beginning of the vector. Return `TRUE` if `vec_ptr` points to
a valid vector structure, `v_ptr` is not NULL, and insertion succeeded. `FALSE` otherwise.

#### `vec2_unshift_multi(vec_ptr, arr, len)`
Inserts `len` elements from the array `arr` to the beginning of the vector. Return `TRUE` if `vec_ptr` points to a valid
vector structure, `arr` is not NULL, and insertion succeeded. `FALSE` otherwise.

#### `vec2_insert(vec_ptr, idx, v)`
Inserts a value `v` at the specified `idx` in the vector. Returns `TRUE` if `vec_ptr` points to a valid vector structure,
`idx` is not greater than the vector's size, and insertion suceeded. `FALSE` otherwise.

#### `vec2_insert_ptr(vec_ptr, idx, v_ptr)`
Inserts a value passed by the reference `v_ptr` at the specified `idx` in the vector. Returns `TRUE` if `vec_ptr` points
to a valid vector structure, `v_ptr` is not NULL, `idx` is not greater than the vector's size, and insertion suceeded.
`FALSE` otherwise.

#### `vec2_insert_multi(vec_ptr, idx, arr, len)`
Inserts `len` elements from the array `arr` at the specified `idx` in the vector. Returns `TRUE` if `vec_ptr` points
to a valid vector structure, `arr` is not NULL, the range [`idx`, `idx+len`) is not outside than the vector's bounds,
and insertion suceeded. `FALSE` otherwise.

#### `vec2_swap(vec_ptr, first, second)`
Performs a swap on the elements at indices `first` and `second` in `v_ptr`. Returns `TRUE` if `vec_ptr` points
to a valid vector structure and `first` and `second` are not greater than the vector's size. `FALSE` otherwise.

#### `vec2_sort(vec_ptr, cmpfn_ptr)`
Sorts a vector using the function pointed by `cmpfn_ptr`. Returns `TRUE` if `vec_ptr` points to a valid vector structure
and `cmpfn_ptr` is not NULL. `FALSE` otherwise.

#### `vec2_assign(vec_ptr, idx, v)`
Assigns a value `v` at `idx`. Returns `TRUE` if `vec_ptr` points to a valid vector structure and `idx` is not outside the
vector's bounds. `FALSE` otherwise. Also note that `idx` must be an expression that is free from side effects

#### `vec2_assign_ptr(vec_ptr, idx, v_ptr)`
Assigns a value pointed to by the pointer `v_ptr` at `idx`. Returns `TRUE` if `vec_ptr` points to a valid vector structure,
`idx` is not outside the vector's bounds, and `v_ptr` is not NULL. `FALSE` otherwise.

#### `vec2_assign_multi(vec_ptr, idx, v_ptr, len)`
Assigns `len` items from `v_ptr` starting at `idx`. Returns `TRUE` if `vec_ptr` points to a valid vector structure, the range
[`idx`, `idx+len`) is not outside the vector's bounds, and `v_ptr` is not NULL. `FALSE` otherwise.

#### `vec2_pop(vec_ptr, o_ptr)`
Removes an element from the end of the vector and stores it in `o_ptr` if it's not NULL. Returns `TRUE` if `vec_ptr` points to a
valid vector structure that is not empty. `FALSE` otherwise.

#### `vec2_pop_multi(vec_ptr, len, o_ptr)`
Removes `len` elements from the end of the vector and stores them in `o_ptr` if it's not NULL. Returns `TRUE` if `vec_ptr` points
to a valid vector structure that holds at least `len` elements. `FALSE` otherwise.

#### `vec2_shift(vec_ptr, o_ptr)`
Removes an element from the beginning of the vector and stores it in `o_ptr` if it's not NULL. Returns `TRUE` if `vec_ptr` points
to a valid vector structure that is not empty. `FALSE` otherwise.

#### `vec2_shift_multi(vec_ptr, len, o_ptr)`
Removes `len` elements from the beginning of the vector and stores them in `o_ptr` if it's not NULL. Returns `TRUE` if `vec_ptr`
points to a valid vector structure that holds at least `len` elements. `FALSE` otherwise.

#### `vec2_remove(vec_ptr, idx, len, o_ptr)`
Removes `len` items starting at `idx` from the vector and stores them in `o_ptr` if it's not NULL. Returns `TRUE` if `vec_ptr` points
to a valid vector structure that is not empty and the range [`idx`, `idx+len`) is inside the vector's bounds. `FALSE` otherwise.

