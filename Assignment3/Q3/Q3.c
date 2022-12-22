#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saksham");
MODULE_DESCRIPTION("assmnt_3_OS");
MODULE_VERSION("0.01");

int x = 0;
module_param(x, int, 0);

void printKern(struct task_struct** task){
    printk(KERN_INFO "pid :%d\n", (*task)->pid);
    printk(KERN_INFO "uid : %d\n", (*task)->cred->uid.val);
    printk(KERN_INFO "pgid : %d\n", (*task)->group_leader->pid);
    printk(KERN_INFO "comm : %s\n", (*task)->comm);
}

static int __init init_os(void)
{
    struct task_struct *task;
    task = pid_task(find_vpid(x), PIDTYPE_PID);
    if (!task)
    {
        return -ESRCH;
    }
    printKern(&task);
    return 0;
}
static void __exit exit_os(void)
{
    printk(KERN_INFO "Leave\n");
}
module_init(init_os);
module_exit(exit_os);
