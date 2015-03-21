AM_CFLAGS = -fstack-protector -Wall -Wextra -Werror \
	-Wstrict-prototypes -Wundef -fno-common \
	-Werror -Werror-implicit-function-declaration \
	-Wformat -Wformat-security -Werror=format-security \
	-Wno-conversion -Wunreachable-code -std=c99

AM_CPPFLAGS += \
	-I $(top_srcdir)/src
