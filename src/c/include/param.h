#ifndef HEADER_SCRIPTS_C_PARAM_
#define HEADER_SCRIPTS_C_PARAM_

#define STRINGIFY_IMPL_(VAL) #VAL
#define STRINGIFY(VAL) STRINGIFY_IMPL_(VAL)

#define ARRLEN(ARR) (sizeof(ARR) / sizeof((ARR)[0]))

extern int _iso_c_forbids_an_empty_translation_unit;

#endif /* ifndef HEADER_SCRIPTS_C_PARAM_ */
