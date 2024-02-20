/* See LICENSE file for copyright and license details. */

/* interval between updates (in ms) */
const unsigned int interval = 1000;

/* text to show if no value can be retrieved */
static const char unknown_str[] = "n/a";

/* maximum output string length */
#define MAXLEN 2048

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

static const struct arg args[] = {
	/* function format          argument */

        { run_command, "^c#9ece6a^%s", "sh $HOME/.config/slstatus/scripts/xbps" },
	{ run_command,    "^c#a9b1d6^^b#222222^ %s ", "sh $HOME/.config/slstatus/scripts/brightness" },

	{ run_command,    "^c#a9b1d6^^b#222222^ %s ", "sh $HOME/.config/slstatus/scripts/battery" },


	{ run_command,    "^c#a9b1d6^^b#222222^ %s ", "sh $HOME/.config/slstatus/scripts/volume" },


	{ run_command,    "^b#1a1b26^ ^b#2a2b3d^^c#ad8ee6^  ^b#32344a^^c#ffffff^ %s ", "sh $HOME/.config/slstatus/scripts/ram" },

        { run_command, "^b#2a2b3d^^c#e0af68^ 󱛟 ^b#32344a^^c#ffffff^ %s ", "sh $HOME/.config/slstatus/scripts/disk" },


//        { run_command, "^b#7aa2f7^^c#000000^|  : %s ", "sh /home/dluca/Programs/slstatus/shellmodules/kern" },

	{ run_command,    "^b#1a1b26^ %s ", "sh $HOME/.config/slstatus/scripts/connection" },
        { run_command, "^b#1a1b26^ ^c#f7768e^%s ^b#1a1b26^ ", "sh $HOME/.config/slstatus/scripts/time" },

};
