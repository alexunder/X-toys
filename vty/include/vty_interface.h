#ifndef OSMOCOM_VTY_H
#define OSMOCOM_VTY_H

//#include "osmocom_data.h"
#include "vty.h"
#include "buffer.h"
#include "command.h"

enum ms_vty_node {
	MS_NODE = _LAST_OSMOVTY_NODE + 1,
	TESTSIM_NODE,
	SUPPORT_NODE,
};

enum node_type ms_vty_go_parent(struct vty *vty);
int ms_vty_init(void);
extern void vty_notify(struct osmocom_ms *ms, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

#endif

