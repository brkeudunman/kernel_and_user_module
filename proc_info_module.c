#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/sched/signal.h>
#include <linux/errno.h>
#include <linux/string.h>

// Berke Udunman 270201046
// Sezin Cagla Serficeli 280201041

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Berke Udunman Sezin Cagla Serficeli G11");
MODULE_DESCRIPTION("Dynamic Kernel Module");

static int pid = -1;
static char *pname = "";

module_param(pid, int, 0);
MODULE_PARM_DESC(pid, "User process ID (PID)");

module_param(pname, charp, 0);
MODULE_PARM_DESC(pname, "User process name");

static void printProcessInfo(struct seq_file *m, struct task_struct *task) {
    seq_printf(m, "Name: %s\n", task->comm);
    seq_printf(m, "PID: %d\n", task->pid);
    seq_printf(m, "PPID: %d\n", task->real_parent->pid);
    seq_printf(m, "UID: %d\n", task->cred->uid.val);
    seq_printf(m, "Path: /proc/%d\n", task->pid);
    char stateLabel[20];
    switch (task->state) {
        case TASK_RUNNING:
            strlcpy(stateLabel, "Running", sizeof(stateLabel));
            break;
        case TASK_INTERRUPTIBLE:
            strlcpy(stateLabel, "Interruptible Sleep", sizeof(stateLabel));
            break;
        case TASK_UNINTERRUPTIBLE:
            strlcpy(stateLabel, "Uninterruptible Sleep", sizeof(stateLabel));
            break;
        case TASK_STOPPED:
            strlcpy(stateLabel, "Stopped", sizeof(stateLabel));
            break;
        default:
            strlcpy(stateLabel, "Unknown", sizeof(stateLabel));
            break;
    }
    seq_printf(m, "State: %s\n", stateLabel);
    if (task->mm) {
        unsigned long vm_size = task->mm->total_vm * PAGE_SIZE;
        seq_printf(m, "Memory Usage: %lu KB\n", vm_size / 1024);
    }
}

static int findProcess(struct seq_file *m) {
    struct task_struct *task;
    struct task_struct *selected_task = NULL;
    int found = 0;
    if (pid != -1 || strlen(pname) != 0) {
        for_each_process(task) {
            if ((pid != -1 && task->pid == pid) || (strlen(pname) != 0 && strcmp(task->comm, pname) == 0)) {
                selected_task = task;
                found = 1;
                break;
            }
        }
    } else {
	seq_printf(m, "Debug: pid=%d, pname=%s\n", pid, pname); // Debug print
        seq_printf(m, "Error: No process ID or name specified.\n");
        return 0;
    }

    if (!found) {
        seq_printf(m, "Error: No process found with the specified ID or name.\n");
        return 0;
    }

    printProcessInfo(m, selected_task);
    return 0;
}

static int showProcess(struct seq_file *m, void *v)
{
    return findProcess(m);
}

static int openProcess(struct inode *inode, struct file *file)
{
    return single_open(file, showProcess, NULL);
}

static const struct file_operations processInfo = {
    .owner = THIS_MODULE,
    .open = openProcess,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static int __init initKernelModule(void)
{
    struct proc_dir_entry *entry;

    entry = proc_create("proc_info_module", 0, NULL, &processInfo);
    if (!entry) {
        pr_err("Error creating /proc/%s\n", "proc_info_module");
        return -ENOMEM;
    }

    pr_info("/proc/%s created\n", "proc_info_module");
    return 0;
}

static void __exit exitKernelModule(void)
{
    remove_proc_entry("proc_info_module", NULL);
    pr_info("/proc/%s removed\n", "proc_info_module");
}

module_init(initKernelModule);
module_exit(exitKernelModule);



