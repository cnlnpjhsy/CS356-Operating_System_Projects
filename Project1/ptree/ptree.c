#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/unistd.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <asm-generic/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

#define __NR_ptreecall 356

struct prinfo
{
    pid_t parent_pid;       /* parent pid */
    pid_t pid;              /* process id */
    pid_t first_child_pid;  /* pid of youngest child */
    pid_t next_sibling_pid; /* pid of older sbling */
    long state;             /* current state of process */
    long uid;               /* user id of process owner */
    char comm[64];          /* name of progres executed */
};

static int (*oldcall)(void);

static void copy(struct prinfo *buf, struct task_struct *task){ 
    buf->parent_pid = task->parent->pid;
    buf->pid = task->pid;
    if (list_empty(&(task->children)))
    {
        buf->first_child_pid = 0;
    }
    else{
        buf->first_child_pid = list_entry(
            (task->children).next, struct task_struct, sibling)->pid;
    }
    if(list_empty(&(task->sibling))){
        buf->next_sibling_pid = 0;
    }
    else{
        buf->next_sibling_pid = list_entry(
            (task->sibling).next, struct task_struct, sibling)->pid;
    }
    buf->state = task->state;
    buf->uid = task->cred->uid;
    get_task_comm(buf->comm, task);
}

void DFS(struct prinfo *buf, struct task_struct *task, int *cnt, const int max){   
    if(*cnt == max){
        printk("Copy stopped - process count overflow\n");
        return;
    }    
    if(task==NULL){
        printk("Error: task list empty\n");
        return;
    }
    copy(buf+(*cnt), task);
    (*cnt)++;

    struct task_struct *cursor;
    /* cursor as an iterator: will be assigned values in the marco */
    list_for_each_entry(cursor, &(task->children), sibling){
        DFS(buf, cursor, cnt, max);
    }
}

static int sys_ptreecall(struct prinfo *buf, int *nr){
    int *process_count = kmalloc(sizeof(int), GFP_KERNEL);
    *process_count = 0;
    int max;
    get_user(max, nr);
    /* Allocate *nr size of buffer first. If the number of
     * processes is bigger than *nr, stop copying and warn
     * the caller. Otherwise, change *nr as the actual
     * number of processes.
     */

    struct prinfo *buf_kernel = kmalloc(max * sizeof(struct prinfo), GFP_KERNEL);

    struct task_struct *init = &init_task;
    struct prinfo *buf_cursor = buf_kernel;

    read_lock(&tasklist_lock);
    printk("DFS start...\n");    
    DFS(buf_cursor, init, process_count, max);
    printk("DFS done.\n");    
    read_unlock(&tasklist_lock);

    copy_to_user(buf, buf_kernel, sizeof(struct prinfo) * *process_count);
    put_user(*process_count, nr);

    kfree(buf_kernel);
    kfree(process_count);
    return 0;
}

static int addsyscall_init(void){
    long *syscall = (long*)0xc000d8c4;
    oldcall = (int(*)(void))(syscall[__NR_ptreecall]);
    syscall[__NR_ptreecall] = (unsigned long)sys_ptreecall;
    printk(KERN_INFO "module load!\n");
    return 0;
}
static void addsyscall_exit(void){
    long *syscall = (long*)0xc000d8c4;
    syscall[__NR_ptreecall] = (unsigned long)oldcall;
    printk(KERN_INFO "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);

// emulator -avd Android -kernel ~/MyFile/kernel/goldfish/arch/arm/boot/zImage -show-kernel

/* 3/27: ptree.c finished */

/**
 * 3/27: Process swapper return a negative children pid.
 * Also, kernel crashed while copying 2nd process info
 * (requesting at addr 74696e6d).
 * Solution: for "list_for_each" kind of marcos, 
 * @member should be "sibling" but not "children" (Why??)
 * Solved: children.next points to the sibling member in Linux kernel!!
 */

/**
 * 3/22: Process uid = -1 or 0 while printing.
 * Seems that task->loginuid is not the uid we want.
 * However, task_struct doesn't include uid.
 * Solution: process credential *cred
 */

/**
 * 3/21: Error occur as:
 * ----------
 * Unable to handle kernel paging request at virtual address aa34b000
 * ----------
 * This is a user address. Kernel cannot visit it and thus return an Oops.
 * Solution: get_user(var, addr) / put_user(var, addr)
 * For structs, use copy_from/to_user()
 */

/* 3/20: ptree.c completed */