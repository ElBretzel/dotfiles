#ifndef CONFIG_H
#define CONFIG_H

#include "slstatus.h"

/* See LICENSE file for copyright and license details. */

/* interval between updates (in ms) */
#define INTERVAL 1000

/* text to show if no value can be retrieved */
static const char unknown_str[] = "n/a";

/* maximum output string length */
#define MAXLEN 2048

// Number of bars
#define BARS 10
// Width of one bar in pixel
#define WIDTH 10
// Height of one bar in pixel
#define HEIGHT 25
// Minimum height of one bar in pixel
#define MINSIZE 5
// Number of refreshment per second (FPS) (ressource intensive)
#define FRAMERATE 12

/*
 * Advanced configuration (should not be usefull unless you are using an OS with
 * different Linux folder architecture
 */

// Where and how the tempfile will be located.
// DO NOT CHANGE THE XXXXXX
#define TMP_TEMPLATE "/tmp/cavadwm_XXXXXX"

// Where the binary is located.
// %s will be replaced by cava
// ex: /bin/cava -> /bin/%s, /usr/bin/cava -> /usr/bin/%s, ...
#define BINLOC "/bin/%s"
/*
 * function            description                     argument (example)
 *
 * battery_perc        battery percentage              battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * battery_remaining   battery remaining HH:MM         battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * battery_state       battery charging state          battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * cat                 read arbitrary file             path
 * cpu_freq            cpu frequency in MHz            NULL
 * cpu_perc            cpu usage in percent            NULL
 * datetime            date and time                   format string (%F %T)
 * disk_free           free disk space in GB           mountpoint path (/)
 * disk_perc           disk usage in percent           mountpoint path (/)
 * disk_total          total disk space in GB          mountpoint path (/)
 * disk_used           used disk space in GB           mountpoint path (/)
 * entropy             available entropy               NULL
 * gid                 GID of current user             NULL
 * hostname            hostname                        NULL
 * ipv4                IPv4 address                    interface name (eth0)
 * ipv6                IPv6 address                    interface name (eth0)
 * kernel_release      `uname -r`                      NULL
 * keyboard_indicators caps/num lock indicators        format string (c?n?)
 *                                                     see keyboard_indicators.c
 * keymap              layout (variant) of current     NULL
 *                     keymap
 * load_avg            load average                    NULL
 * netspeed_rx         receive network speed           interface name (wlan0)
 * netspeed_tx         transfer network speed          interface name (wlan0)
 * num_files           number of files in a directory  path
 *                                                     (/home/foo/Inbox/cur)
 * ram_free            free memory in GB               NULL
 * ram_perc            memory usage in percent         NULL
 * ram_total           total memory size in GB         NULL
 * ram_used            used memory in GB               NULL
 * run_command         custom shell command            command (echo foo)
 * swap_free           free swap in GB                 NULL
 * swap_perc           swap usage in percent           NULL
 * swap_total          total swap size in GB           NULL
 * swap_used           used swap in GB                 NULL
 * temp                temperature in degree celsius   sensor file
 *                                                     (/sys/class/thermal/...)
 *                                                     NULL on OpenBSD
 *                                                     thermal zone on FreeBSD
 *                                                     (tz0, tz1, etc.)
 * uid                 UID of current user             NULL
 * uptime              system uptime                   NULL
 * username            username of current user        NULL
 * vol_perc            OSS/ALSA volume in percent      mixer file (/dev/mixer)
 *                                                     NULL on OpenBSD/FreeBSD
 * wifi_essid          WiFi ESSID                      interface name (wlan0)
 * wifi_perc           WiFi signal in percent          interface name (wlan0)
 */

struct arg {
  const char *(*func)(const char *);
  const char *fmt;
  const char *args;
};

static const struct arg args[] = {
    /* function format          argument */

    {run_command, "^c#9ece6a^%s", "sh $HOME/.config/slstatus/scripts/xbps"},
    {run_command, "^c#a9b1d6^^b#222222^ %s ",
     "sh $HOME/.config/slstatus/scripts/brightness"},

    {run_command, "^c#a9b1d6^^b#222222^ %s ",
     "sh $HOME/.config/slstatus/scripts/battery"},

    {run_command, "^c#a9b1d6^^b#222222^ %s ",
     "sh $HOME/.config/slstatus/scripts/volume"},

    {run_command,
     "^b#1a1b26^ ^b#2a2b3d^^c#ad8ee6^  ^b#32344a^^c#ffffff^ %s ",
     "sh $HOME/.config/slstatus/scripts/ram"},

    {run_command, "^b#2a2b3d^^c#e0af68^ 󱛟 ^b#32344a^^c#ffffff^ %s ",
     "sh $HOME/.config/slstatus/scripts/disk"},

    //        { run_command, "^b#7aa2f7^^c#000000^|  : %s ", "sh
    //        /home/dluca/Programs/slstatus/shellmodules/kern" },

    {run_command, "^b#1a1b26^ %s ",
     "sh $HOME/.config/slstatus/scripts/connection"},
    {run_command,
     "^b#1a1b26^ "
     "^c#f7768e^%s ^b#1a1b26^ ",
     "sh $HOME/.config/slstatus/scripts/time"},
    {cava, "%s", ""},
};

#endif /* CONFIG_H */
