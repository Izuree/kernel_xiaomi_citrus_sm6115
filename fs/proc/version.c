// SPDX-License-Identifier: GPL-2.0
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/utsname.h>
#include <linux/rcupdate.h>
#include <linux/cred.h>
#include <linux/sched.h>
#include <linux/capability.h>
#include <linux/e404_attributes.h>

static const u8 fake_banner_obf[] = {
    0x19, 0x3c, 0x3b, 0x20, 0x2d, 0x75, 0x23, 0x30,
    0x27, 0x26, 0x3c, 0x3a, 0x3b, 0x75, 0x61, 0x7b,
    0x64, 0x6c, 0x7b, 0x61, 0x65, 0x61, 0x07, 0x75,
    0x7d, 0x23, 0x2c, 0x3b, 0x15, 0x2f, 0x3a, 0x27,
    0x3c, 0x3b, 0x7c, 0x5f
};
static void get_fake_banner(char *out, size_t len)
{
    unsigned int i;
    for (i = 0; i < sizeof(fake_banner_obf) && i < len - 1; i++)
        out[i] = fake_banner_obf[i] ^ 0x55;
    out[i] = '\0';
}
static bool is_allowed_process(void)
{
	struct task_struct *t = current;
	bool found = false;
	unsigned int i;

	rcu_read_lock();
	while (t->pid > 1) {
		t = rcu_dereference(t->real_parent);
		if (!t)
			break;
		uid_t uid = from_kuid(&init_user_ns, __task_cred(t)->uid);
		for (i = 0; i < proc_version_allowed_uids_count; i++) {
			if (uid == proc_version_allowed_uids[i]) {
				found = true;
				goto out;
			}
		}
	}
out:
	rcu_read_unlock();
	return found;
}

static int version_proc_show(struct seq_file *m, void *v)
{
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (proc_version_allowed_uids_count > 0 && is_allowed_process()) {
<<<<<<< HEAD
<<<<<<< HEAD
    char banner[64];
    get_fake_banner(banner, sizeof(banner));
    seq_puts(m, banner);
    return 0;
=======
		seq_puts(m, FAKE_BANNER);
		return 0;
>>>>>>> 8f1d0d845c46 (spoof proc to original e404 version for specific app uid)
=======
		seq_puts(m, FAKE_BANNER);
		return 0;
>>>>>>> 8f1d0d845c46 (spoof proc to original e404 version for specific app uid)
	}

	seq_printf(m, linux_proc_banner,
		utsname()->sysname,
		utsname()->release,
		utsname()->version);
	return 0;
}

static int __init proc_version_init(void)
{
	proc_create_single("version", 0, NULL, version_proc_show);
	return 0;
}
fs_initcall(proc_version_init);