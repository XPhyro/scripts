#ifndef HEADER_SCRIPTS_C_VECTOR_
#define HEADER_SCRIPTS_C_VECTOR_

#include <stddef.h>
#include <sys/param.h>

#include "stdutil.h"

/* if managed manually, the following functions might not work correctly. */
/* only works for members of constant size. if using strings, allocate a constant-size buffer for them. */
typedef struct {
    void *arr;
    size_t nmemb;
    size_t nptr;
    size_t size;
} vector_t;

vector_t vec_new(size_t nmemb, size_t size)
{
    return (vector_t){ .arr = acalloc(nmemb, size), .nmemb = nmemb, .nptr = 0, .size = size };
}

void vec_init(vector_t *vec, size_t nmemb, size_t size)
{
    if (vec->arr)
        free(vec->arr);
    *vec = vec_new(nmemb, size);
}

void vec_del(vector_t *vec)
{
    free(vec->arr);
    *vec = (vector_t){ .arr = NULL, .nmemb = 0, .nptr = 0, .size = 0 };
}

void vec_cpy(vector_t *dest, const vector_t *src)
{
    memcpy(dest->arr, src->arr, src->nmemb * src->size);
    dest->nmemb = src->nmemb;
    dest->nptr = src->nptr;
    dest->size = src->size;
}

vector_t vec_dup(const vector_t *vec)
{
    vector_t dup;
    vec_cpy(&dup, vec);
    return dup;
}

void vec_expandto(vector_t *vec, size_t nmemb)
{
    if (nmemb <= vec->nmemb)
        return;
    vec->arr = acrealloc(vec->arr, vec->nmemb, nmemb, vec->size);
    vec->nmemb = nmemb;
}

void vec_expandby(vector_t *vec, size_t nmemb)
{
    vec_expandto(vec, vec->nmemb + nmemb);
}

void vec_expand(vector_t *vec)
{
    vec_expandby(vec, vec->nmemb);
}

void vec_shrinkto(vector_t *vec, size_t nmemb)
{
    vec->arr = acrealloc(vec->arr, vec->nmemb, nmemb, vec->size);
    vec->nmemb = nmemb;
    if (vec->nptr < nmemb)
        vec->nptr = nmemb;
}

void vec_shrinkby(vector_t *vec, size_t nmemb)
{
    vec_shrinkto(vec, vec->nmemb - nmemb);
}

void vec_shrink(vector_t *vec)
{
    vec_shrinkto(vec, vec->nmemb);
}

#define VEC_AT(VEC, IDX, TYPE) (((TYPE *)((VEC)->arr))[(VEC)->IDX])

#define VEC_ADD(VEC, MEMB, TYPE)                      \
    {                                                 \
        if ((VEC)->nptr == (VEC)->nmemb) {            \
            vec_expand(VEC);                          \
        }                                             \
        ((TYPE *)((VEC)->arr))[(VEC)->nptr++] = MEMB; \
    }

/* no safety check for whether index is within range */
#define VEC_CPY(VEC, DESTIDX, SRCIDX, TYPE)                               \
    {                                                                     \
        ((TYPE *)((VEC)->arr))[DESTIDX] = ((TYPE *)((VEC)->arr))[SRCIDX]; \
    }

/* no safety check for whether index is within range */
#define VEC_RM(VEC, IDX, TYPE)                                                 \
    {                                                                          \
        size_t VEC_I = IDX;                                                    \
        for (; VEC_I < (VEC)->nptr - 1; VEC_I++) {                             \
            ((TYPE *)((VEC)->arr))[VEC_I] = ((TYPE *)((VEC)->arr))[VEC_I + 1]; \
        }                                                                      \
        (VEC)->nptr--;                                                         \
        if ((VEC)->nptr * 2 > (VEC)->nmemb)                                    \
            vec_shrink(VEC);                                                   \
    }

#define VEC_IT(VEC, FUNC, TYPE, ...)                                 \
    {                                                                \
        size_t VEC_I = 0;                                            \
        for (; VEC_I < (VEC)->nptr; VEC_I++) {                       \
            FUNC(VEC_I, ((TYPE *)((VEC)->arr))[VEC_I], __VA_ARGS__); \
        }                                                            \
    }

#define VEC_ITRANGE(VEC, FUNC, BEG, END, TYPE, ...)                  \
    {                                                                \
        size_t VEC_I = MAX(BEG, 0);                                  \
        for (; VEC_I < MIN(END, (VEC)->nptr); VEC_I++) {             \
            FUNC(VEC_I, ((TYPE *)((VEC)->arr))[VEC_I], __VA_ARGS__); \
        }                                                            \
    }

#define VEC_IT_SAFE(VEC, FUNC, TYPE, ...)                              \
    {                                                                  \
        vector_t VEC_DUP = vec_dup(VEC);                               \
        size_t VEC_I = 0;                                              \
        for (; VEC_I < VEC_DUP->nptr; VEC_I++) {                       \
            FUNC(VEC_I, ((TYPE *)(VEC_DUP->arr))[VEC_I], __VA_ARGS__); \
        }                                                              \
        vec_del(&VEC_DUP)                                              \
    }

#define VEC_ITRANGE_SAFE(VEC, FUNC, BEG, END, TYPE, ...)               \
    {                                                                  \
        vector_t VEC_DUP = vec_dup(VEC);                               \
        size_t VEC_I = MAX(BEG, 0);                                    \
        for (; VEC_I < MIN(END, VEC_DUP->nptr); VEC_I++) {             \
            FUNC(VEC_I, ((TYPE *)(VEC_DUP->arr))[VEC_I], __VA_ARGS__); \
        }                                                              \
        vec_del(&VEC_DUP)                                              \
    }

#endif /* HEADER_SCRIPTS_C_VECTOR_ */
