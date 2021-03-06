#ifndef __GLOBAL_H__
#define __GLOBAL_H__

typedef PVOID MARTA_CONTEXT, *PMARTA_CONTEXT;
#define NULL_MARTA_CONTEXT ((MARTA_CONTEXT) 0)

typedef struct _MARTA_OBJECT_PROPERTIES {
    DWORD cbSize;
    DWORD dwFlags;
} MARTA_OBJECT_PROPERTIES, *PMARTA_OBJECT_PROPERTIES;

#define MARTA_OBJECT_IS_CONTAINER   0x1

typedef struct _MARTA_OBJECT_TYPE_PROPERTIES {
    DWORD           cbSize;
    DWORD           dwFlags;
    GENERIC_MAPPING GenMap;
} MARTA_OBJECT_TYPE_PROPERTIES, *PMARTA_OBJECT_TYPE_PROPERTIES;

#define MARTA_OBJECT_TYPE_MANUAL_PROPAGATION_NEEDED_FLAG   0x1
#define MARTA_OBJECT_TYPE_INHERITANCE_MODEL_PRESENT_FLAG   0x2

#define CONDITIONAL_EXIT(a, b) if (ERROR_SUCCESS != (a)) { goto b; }
#define CONDITIONAL_RETURN(a)  if (ERROR_SUCCESS != (a)) { return (a); }
#define CONDITIONAL_ACE_SIZE_ERROR(a)                                         \
            if ((a) > 0xFFFF) { return ERROR_BAD_INHERITANCE_ACL; }

#endif
