#ifndef _COMMON_H_
#define _COMMON_H_
/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/
/* Define a Boolean data type. (from linux kernel 2.6.0) */
typedef enum { false, true } __attribute__ ((packed)) boolean;

#define SUCCESS         0
#define PARA_ERROR      1
#define RTN_ERROR       2
#define MEM_ERROR       3

#define REL_MEMORY(ptr)\
do\
{\
    if (NULL != ptr)\
    {\
        free(ptr);\
    }\
}while(0)

#define GET_MEMORY(ptr, type, size, pos)\
do\
{\
    ptr = (type*)malloc(size);\
    if (NULL == ptr)\
    {\
        printf("Malloc error.\n");\
        goto pos;\
    }\
    memset(ptr, 0, size);\
}while(0)

#define GET_MEMORY_RTN(ptr, type, size)\
do\
{\
    ptr = (type*)malloc(size);\
    if (NULL == ptr)\
    {\
        printf("Malloc error.\n");\
        return MEM_ERROR;\
    }\
    memset(ptr, 0, size);\
}while(0)

#define CHECK_RETURN(ret, sucess, str)\
do\
{\
    if (sucess != ret)\
    {\
        printf(str);\
        ret = RTN_ERROR;\
    }\
}while(0)

#define CHECK_RETURN_ERR(ret, error, str)\
do\
{\
    if (error == ret)\
    {\
        printf(str);\
        ret = RTN_ERROR;\
    }\
}while(0)

#define CHECK_RETURN_GOTO(ret, sucess, str, pos)\
do\
{\
    if (sucess != ret)\
    {\
        printf(str);\
        goto pos;\
    }\
}while(0)

#define CHECK_POINT(ptr)\
do\
{\
    if (NULL == ptr)\
    {\
        return MEM_ERROR;\
    }\
}while(0)

#define CHECK_POINT_NORTN(ptr)\
do\
{\
    if (NULL == ptr)\
    {\
        return;\
    }\
}while(0)

#endif