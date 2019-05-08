#
# Regular cron jobs for the fort-hermes package
#
0 4	* * *	root	[ -x /usr/bin/fort-hermes_maintenance ] && /usr/bin/fort-hermes_maintenance
