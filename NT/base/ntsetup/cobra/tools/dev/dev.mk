# compiler options
USE_MSVCRT=1

# include path
INCLUDES=$(COBRA_ROOT)\inc;                         \
         $(COBRA_ROOT)\utils\inc;                   \
         $(BASE_INC_PATH);                          \
         $(WINDOWS_INC_PATH);                       \


# SDK link libraries

TARGETLIBS=$(TARGETLIBS) \
        $(SDK_LIB_PATH)\kernel32.lib    \
        $(SDK_LIB_PATH)\user32.lib      \
        $(SDK_LIB_PATH)\gdi32.lib       \
        $(SDK_LIB_PATH)\advapi32.lib    \
        $(SDK_LIB_PATH)\version.lib     \
        $(SDK_LIB_PATH)\imagehlp.lib    \
        $(SDK_LIB_PATH)\shell32.lib     \
        $(SDK_LIB_PATH)\ole32.lib       \
        $(SDK_LIB_PATH)\comdlg32.lib    \
        $(SDK_LIB_PATH)\comctl32.lib    \
        $(SDK_LIB_PATH)\uuid.lib        \
        $(SDK_LIB_PATH)\winmm.lib       \
        $(SDK_LIB_PATH)\mpr.lib         \
        $(SDK_LIB_PATH)\userenv.lib     \
        $(SDK_LIB_PATH)\netapi32.lib    \
        $(SHELL_LIB_PATH)\shell32p.lib              \
        $(COBRA_ROOT)\lib\*\utils.lib               \
        $(COBRA_ROOT)\utils\pch\obj\*\pch.obj       \
        $(SDK_LIB_PATH)\setupapi.lib

# LINT options, do not end in a semicolon
LINT_TYPE=all
LINT_EXT=lnt
LINT_ERRORS_TO_IGNORE=$(LINT_ERRORS_TO_IGNORE);43;729;730;534;526;552;714;715;749;750;751;752;753;754;755;756;757;758;759;765;768;769;

LINT_OPTS=-v -cmsc -zero +fcu +fwu +fan +fas +fpc +fie -e$(LINT_ERRORS_TO_IGNORE:;= -e)
LINT_OPTS=$(LINT_OPTS:-e =)
