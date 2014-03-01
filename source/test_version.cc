
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include "version.h"

int main() {
	bool is_ok(true);

#ifdef PACKAGE_VERSION
	is_ok = strcmp(PACKAGE_VERSION, VERSION_STRING) == 0;
#endif

	if (!is_ok)
		fprintf(stderr, "versions in configure.ac and version.h do not match!\n"
		        "configure.ac: %s\nversion.h: %s\n", PACKAGE_VERSION,
		        VERSION_STRING);

	return is_ok ? 0 : 1;
}
