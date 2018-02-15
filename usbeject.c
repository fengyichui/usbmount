#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
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
    char *fstype;
    char *devname;
    char *mountpoint;
    char mounted;
};

static struct cookie cookies[COOKIE_MAX];
static int cookie_cnt;

static int init(void)
{
    DIR *cookie_root = opendir(COOKIE);
    struct dirent *ent;
    if (!cookie_root) {
        return -1;
    }
    cookie_cnt = 0;
    while ((ent =  readdir(cookie_root)) != NULL) {
        if (ent->d_type != DT_REG)
            continue;
        /* fstype devname mountpoint */
        char fstype[RECORAD_LEN], devname[PATH_MAX], mountpoint[PATH_MAX];
        FILE *content;
        char path[1024] = COOKIE"/";
        strcat(path, ent->d_name);

        if (!(content = fopen(path, "r")))
            continue;
        fscanf(content, "%s %s %s", fstype, devname, mountpoint);
        D("%s %s %s\n", fstype, devname, mountpoint);
        cookies[cookie_cnt].fstype      = strdup(fstype);
        cookies[cookie_cnt].devname     = strdup(devname);
        cookies[cookie_cnt].mountpoint  = strdup(mountpoint);
        cookies[cookie_cnt].mounted       = 1;
        fclose(content);

        cookie_cnt++;
    }

    closedir(cookie_root);

    return 0;
}

static void quit(void)
{
    for (int i = 0; i < cookie_cnt; i++) {
        free(cookies[i].devname);
        free(cookies[i].mountpoint);
        free(cookies[i].fstype);
    }
}

static void eject_all(void)
{
    for (int i = 0; i < cookie_cnt; i++) {
        umount(cookies[i].mountpoint);
    }
}

static void list(void)
{
    printf("no.\t%-20s %-20s %-20s\n", "devname", "mountpoint", "fstype");
    for (int i = 0; i < cookie_cnt; i++) {
        printf("%2d.\t%-20s %-20s %-20s\n", i+1,
                cookies[i].devname, cookies[i].mountpoint, cookies[i].fstype);
    }
}

static void inter_run()
{
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
}

int main(int argc, char **argv)
{
    init();
    char is_inter = 0;
    char mountpoint[PATH_MAX];
    char opt;

    while (-1 != (opt = getopt(argc, argv, "hli"))) {
        switch (opt) {
        case 'h':
            help();
            exit(0);
            break;
        case 'l':
            list();
            exit(0);
            break;
        case 'i':
            is_inter = 1;
            break;
        default:
            help();
            break;
        }
    }

    if (optind < argc) {
        for (int i = optind; i < argc; i++) {
            if (isdigit(argv[i][0])) {
                int no = atoi(argv[i]);
                D("<no.>: %d\n", no);
                if (0 <= no && no < cookie_cnt && cookies[no].mounted) {
                    umount(cookies[no].mountpoint);
                    cookies[no].mounted = 0;
                }
            } else {
                for (int c = 0; c < cookie_cnt; c++) {
                    if ((strstr(cookies[c].devname, argv[i]) || strstr(cookies[c].mountpoint, argv[i]))
                            && cookies[c].mounted) {
                        D("<mountpoint>: %s %s\n", argv[i], cookies[c].mountpoint);
                        umount(cookies[c].mountpoint);
                        cookies[c].mounted = 0;
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


