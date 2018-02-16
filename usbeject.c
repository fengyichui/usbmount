#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <linux/limits.h>

#include "config.h"

#ifdef DEBUG
# define D(...)         printf(__VA_ARGS__)
#else
# define D(...)         ((void)0)
#endif

struct cookie {
    char *name;
    char *fstype;
    char *devname;
    char *mountpoint;
    char mounted;
};

static struct cookie cookies[COOKIE_MAX];
static int cookie_cnt;

static char *strnstr(char const *str, int len, char const *pat)
{
    int i, j;
    for (i = 0; i < len; i++) {
        for (j = 0; i+j < len && '\0' != pat[j] && str[i+j] == pat[j]; j++)
            ;
        if ('\0' == pat[j])
            return (char*)str+i;
    }
    return NULL;
}

static int isinteger(char  const *num)
{
    for (; isdigit(*num) && '\0' != *num; num++)
        ;
    return ('\0' == *num);
}

static int parse_cookie(char const *path, char *fstype, char *devname, char *mountpoint)
{
    int ret = 0;
    int len;
    FILE *content = fopen(path, "r");

    struct string {
        char *str;
        int len;
    } strs[] = {
        {fstype, FSTYPE_LEN},
        {devname, PATH_MAX},
        {mountpoint, PATH_MAX},
    };

    for (int i = 0; i < 3; i++) {
        if (!fgets(strs[i].str, strs[i].len, content)) {
            ret = -1;
            goto close;
        }
        len = strlen(strs[i].str);
        if ('\n' != strs[i].str[len-1]) {
            ret = -2;
            goto close;
        }

        strs[i].str[len-1] = '\0';    /* remove '\n' */
    }

close:
    fclose(content);
    return ret;
}

static int init(void)
{
    int ret = 0;
    DIR *cookie_root = opendir(COOKIE);
    struct dirent *ent;
    if (!cookie_root) {
        printf("%s: %s\n", COOKIE, strerror(errno));
        return -1;
    }
    cookie_cnt = 0;
    while ((ent =  readdir(cookie_root)) != NULL) {
        if (ent->d_type != DT_REG)
            continue;
        /* fstype devname mountpoint */
        char fstype[FSTYPE_LEN], devname[PATH_MAX], mountpoint[PATH_MAX];
        FILE *content;
        char path[PATH_MAX] = COOKIE"/";
        strncat(path, ent->d_name, PATH_MAX-strlen(path)-1);

        if (0 != parse_cookie(path, fstype, devname, mountpoint)) {
            ret = -2;
            goto close;
        }
        cookies[cookie_cnt].name        = strdup(ent->d_name);
        cookies[cookie_cnt].fstype      = strdup(fstype);
        cookies[cookie_cnt].devname     = strdup(devname);
        cookies[cookie_cnt].mountpoint  = strdup(mountpoint);
        cookies[cookie_cnt].mounted     = 1;
        D("%s %s %s %s\n", ent->d_name, fstype, devname, mountpoint);

        cookie_cnt++;
    }

close:
    closedir(cookie_root);

    return ret;
}

static void quit(void)
{
    for (int i = 0; i < cookie_cnt; i++) {
        free(cookies[i].devname);
        free(cookies[i].mountpoint);
        free(cookies[i].fstype);
    }
}

static void eject(int idx)
{
    char name[PATH_MAX] = COOKIE"/";
    strncat(name, cookies[idx].name, PATH_MAX);
    umount(cookies[idx].mountpoint);
    rmdir(cookies[idx].mountpoint);
    remove(name);
    cookies[idx].mounted = 0;
    D("name: %s\n", name);
}

static void eject_all(void)
{
    for (int i = 0; i < cookie_cnt; i++)
        eject(i);
}

static void list(void)
{
    printf("no.\t%-20s %-20s %-20s\n", "devname", "mountpoint", "fstype");
    for (int i = 0; i < cookie_cnt; i++) {
        printf("%2d.\t%-20s %-20s %-20s\n", i+1,
                cookies[i].devname, cookies[i].mountpoint, cookies[i].fstype);
    }
}

static void help(void)
{
    printf("Usage: %s [-hl] [<mountpoint> or <no.>]\n", APP);
    printf("usbeject ejects usb device mounted via `usbmount`.\n");
    printf("     -h: show this page\n"
            "     -l: list all mountpoint\n"
            "     <mountpoint>: a mountpoint\n"
            "     <no.>: number of list order\n"
            "If no argment given, eject all usb device.\n");
    printf("%s version %s BSD-2\n", APP, APP_VER);
}

int main(int argc, char **argv)
{
    char opt;
    if (0 != init()) {
        printf("INIT ERROR!\n");
        return 1;
    }

    while (-1 != (opt = getopt(argc, argv, "hl"))) {
        switch (opt) {
        case 'h':
            help();
            exit(0);
            break;
        case 'l':
            list();
            exit(0);
            break;
        default:
            help();
            break;
        }
    }

    if (optind < argc) {
        for (int i = optind; i < argc; i++) {
            if (isinteger(argv[i])) {
                int no = atoi(argv[i]);
                D("<no.>: %d\n", no);
                no--;   /* begin from 0 */
                if (0 <= no && no < cookie_cnt && cookies[no].mounted)
                    eject(no);
            } else {
                for (int c = 0; c < cookie_cnt; c++) {
                    if ((strnstr(cookies[c].devname, strlen(cookies[c].devname), argv[i])
                                || strnstr(cookies[c].mountpoint, strlen(cookies[c].mountpoint), argv[i]))
                            && cookies[c].mounted) {
                        D("<mountpoint>: %s %s\n", argv[i], cookies[c].mountpoint);
                        eject(c);
                    }
                }
            }
        }
        exit(0);
    }

    /* no argument given */
    eject_all();

    quit();
    return 0;
}


