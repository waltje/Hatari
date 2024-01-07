/*
 * GEMDOS tests
 *
 * This file is distributed under the GNU General Public License, version 2
 * or at your option any later version. Read the file gpl.txt for details.
 */

#include <tos.h>
#include <string.h>

extern unsigned short get_sr(void);

static char buf1[512];
static char buf2[512];
static char buf3[512];

struct test
{
	char *name;
	int (*testfunc)(void);
};

static int tst_directories(void)
{
	int ret;

	ret = Dgetpath(buf1, 0);
	if (ret)
	{
		Cconws("Dgetpath(buf1)");
		return ret;
	}

	ret = strlen(buf1);
	if (ret > 0 && buf1[ret - 1] == '\\')
	{
		Cconws("buffer must not end with backslash");
		return -1;
	}

	ret = Dcreate("TESTDIR");
	if (ret)
	{
		Cconws("Dcreate(\"TESTDIR\")");
		return ret;
	}

	ret = Dcreate("TESTDIR");
	if (ret != -36)
	{
		Cconws("Dcreate(\"TESTDIR\" again)");
		return -1;
	}

	ret = Dsetpath("TESTDIR\\");
	if (ret)
	{
		Cconws("Dsetpath(\"TESTDIR\")");
		return ret;
	}

	ret = Dgetpath(buf2, 0);
	if (ret)
	{
		Cconws("Dgetpath(buf2)");
		return ret;
	}

	if (!strcmp(buf1, buf2))
	{
		Cconws("buf1 vs. buf2");
		return -1;
	}

	ret = Dsetpath("INVLDDIR");
	if (ret != -34)
	{
		Cconws("Dsetpath(\"INVLDDIR\")");
		return -1;
	}

	/* Empty string with Dsetpath should succeed but not change anything */
	ret = Dsetpath("");
	if (ret)
	{
		Cconws("Dsetpath(\"\")");
		return ret;
	}

	ret = Dgetpath(buf3, 0);
	if (ret)
	{
		Cconws("Dgetpath(buf3)");
		return ret;
	}

	if (strcmp(buf2, buf3))
	{
		Cconws("path immutability");
		return -1;
	}

	ret = Dsetpath("..");
	if (ret)
	{
		Cconws("Dsetpath(\"..\")");
		return ret;
	}

	ret = Dgetpath(buf3, 0);
	if (ret)
	{
		Cconws("Dgetpath(buf3)");
		return ret;
	}

	if (strcmp(buf1, buf3))
	{
		Cconws("buf1 vs. buf3");
		return -1;
	}

	ret = Ddelete("TESTDIR");
	if (ret)
	{
		Cconws("Ddelete(\"TESTDIR\")");
		return ret;
	}

	ret = Ddelete("TESTDIR");
	if (ret != -34)
	{
		Cconws("Ddelete(invalid directory)");
		return -1;
	}

	/* After switching to the root dir, we should get an empty path */
	ret = Dsetpath("\\");
	if (ret)
	{
		Cconws("Dsetpath(\"\\\")");
		return ret;
	}
	ret = Dgetpath(buf3, 0);
	if (ret || buf3[0] != '\0')
	{
		Cconws("empty Dgetpath()");
		return -1;
	}

	ret = Dsetpath(buf1);
	if (ret)
	{
		Cconws("Dsetpath(buf1)");
		return ret;
	}

	return 0;
}

static int tst_files(void)
{
	int fh;
	long ret;
	const char testdata[] = "Hello World!\n1234567890";

	fh = Fcreate("TESTFILE.DAT", 0);
	if (fh < 0)
	{
		Cconws("Fcreate(\"TESTFILE.DAT\")");
		return fh;
	}

	ret = Fwrite(fh, sizeof(testdata), testdata);
	if (ret != sizeof(testdata))
	{
		Cconws("Fwrite");
		return -1;
	}

	ret = Fclose(fh);
	if (ret)
	{
		Cconws("Fclose on created file");
		return ret;
	}

	fh = Fopen("TESTFILE.DAT", 0);
	if (fh < 0)
	{
		Cconws("Fopen(\"TESTFILE.DAT\")");
		return fh;
	}

	ret = Fread(fh, sizeof(testdata), buf1);
	if (ret != sizeof(testdata) || memcmp(testdata, buf1, sizeof(testdata)))
	{
		Cconws("Fread");
		return -1;
	}

	ret = Fread(fh, sizeof(testdata), buf2);
	if (ret)
	{
		Cconws("Fread EOF");
		return -1;
	}

	ret = Fclose(fh);
	if (ret)
	{
		Cconws("Fclose on read-only file");
		return ret;
	}

	ret = Fdelete("TESTFILE.DAT");
	if (ret)
	{
		Cconws("Fdelete(\"TESTFILE.DAT\")");
		return ret;
	}

	ret = Fdelete("TESTFILE.DAT");
	if (ret == 0)
	{
		Cconws("Fdelete(\"TESTFILE.DAT\") again");
		return -1;
	}

	return 0;
}

static int tst_sys(void)
{
	long old_ssp, ret;

	/* Initially we should be in user mode */
	ret = Super(1);
	if (ret)
	{
		Cconws("user mode by default");
		return -1;
	}

	old_ssp = Super(0);
	if (old_ssp <= 0)
	{
		Cconws("switch to supervisor mode");
		return -1;
	}

	ret = Super(1);
	if (ret != -1 || !(get_sr() & 0x2000))
	{
		Cconws("supervisor mode");
		return -1;
	}

	Super(old_ssp);    /* Back to user mode - no valid return value here */

	ret = Super(1);
	if (ret)
	{
		Cconws("user mode");
		return -1;
	}

	return 0;
}

struct test tests[] =
{
	{ "paths", tst_directories },
	{ "files", tst_files },
	{ "sys", tst_sys },

	{ 0L, 0L }
};

int main()
{
	int failures = 0;
	int idx;

	for (idx = 0; tests[idx].name != 0L; idx++)
	{
		Cconws("Test '");
		Cconws(tests[idx].name);
		Cconws("'\t: ");
		if (tests[idx].testfunc() != 0)
		{
			Cconws(" FAILED\r\n");
			failures++;
		}
		else
		{
			Cconws(" OK\r\n");
		}
	}

	// Crawcin();

	return !(failures == 0);
}
