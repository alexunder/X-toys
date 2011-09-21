#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "vty_interface.h"
#include "telnet_interface.h"

void *l23_ctx = NULL;
//struct llist_head ms_list;
static char *gsmtap_ip = 0;
//struct gsmtap_inst *gsmtap_inst = NULL;
unsigned short vty_port = 4247;
int debug_set = 0;
char *config_dir = NULL;


static struct vty_app_info vty_info = {
        .name = "OsmocomBB",
        .version = "0.0.1",
        .go_parent_cb = ms_vty_go_parent,
};

static void handle_options(int argc, char **argv)
{
	while (1) {
		int option_index = 0, c;
		static struct option long_options[] = {
			{"help", 0, 0, 'h'},
			{"gsmtap-ip", 1, 0, 'i'},
			{"vty-port", 1, 0, 'v'},
			{"debug", 1, 0, 'd'},
			{0, 0, 0, 0},
		};

		c = getopt_long(argc, argv, "hi:v:d:",
				long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			print_usage(argv[0]);
			print_help();
			exit(0);
			break;
		case 'v':
			vty_port = atoi(optarg);
			break;
//		case 'd':
//			log_parse_category_mask(stderr_target, optarg);
//			debug_set = 1;
//			break;
		default:
			break;
		}
	}
}


int main(int argc, char ** argv)
{
	struct telnet_connection dummy_conn;

	handle_options(argc, argv);
	vty_init(&vty_info);
	telnet_init(l23_ctx, NULL, vty_port);
	return 0;
}
