#ifndef HEADER_SCRIPTS_C_META
#define HEADER_SCRIPTS_C_META

#ifdef __GNUC__
#define META_NONNULL __attribute__((nonnull))
#else /* ifdef __GNUC__ */
#define META_NONNULL
#endif /* ifndef __GNUC__ */

#endif /* ifndef HEADER_SCRIPTS_C_META */
