#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

struct s1
{
    int a;
    char b;

    struct file arr[100];
};

struct s2
{
    int a;
    char b;
};

struct s3
{
    size_t a;
    size_t b;
    size_t c;
    size_t d;

    struct file f;
};

void s1_ctor(void *s1)
{}

void s2_ctor(void *s2)
{}

void s3_ctor(void *s3)
{}

static int __init psm_md_init(void)
{
    printk(KERN_INFO "exp_md: loading module (PID %d)\n", current->pid);

    msleep(20000);

    struct kmem_cache *s1_cache = kmem_cache_create("s1_cache", sizeof(struct s1), 0, 0, s1_ctor);
    struct kmem_cache *s2_cache = kmem_cache_create("s2_cache", sizeof(struct s2), 0, 0, s2_ctor);
    struct kmem_cache *s3_cache = kmem_cache_create("s3_cache", sizeof(struct s3), 0, 0, s3_ctor);

    printk(KERN_INFO "exp_md: created caches\n");

    struct s1 **s1_arr = vmalloc(1000 * sizeof(struct s1 *));
    struct s2 **s2_arr = vmalloc(1000 * sizeof(struct s2 *));
    struct s3 **s3_arr = vmalloc(1000 * sizeof(struct s3 *));

    for (size_t i = 0; i < 200; ++i)
    {
        s1_arr[i] = kmem_cache_alloc(s1_cache, GFP_KERNEL);
        s2_arr[i] = kmem_cache_alloc(s2_cache, GFP_KERNEL);
        s3_arr[i] = kmem_cache_alloc(s3_cache, GFP_KERNEL);
    }

    printk(KERN_INFO "exp_md: (1) allocated 200 s1, 200 s2, 200 s3\n");

    msleep(10000);

    for (size_t i = 200; i < 1000; ++i)
    {
        s1_arr[i] = kmem_cache_alloc(s1_cache, GFP_KERNEL);
        s2_arr[i] = kmem_cache_alloc(s2_cache, GFP_KERNEL);
        s3_arr[i] = kmem_cache_alloc(s3_cache, GFP_KERNEL);
    }

    printk(KERN_INFO "exp_md: (2) allocated 800 s1, 800 s2, 800 s3\n");

    msleep(10000);

    struct s2 **s2_km_arr = vmalloc(1500 * sizeof(struct s2 *));

    for (size_t i = 0; i < 1500; ++i)
        s2_km_arr[i] = kmalloc(sizeof(struct s2), GFP_KERNEL);

    printk(KERN_INFO "exp_md: 1500 s1 with kmalloc\n");

    msleep(10000);

    for (size_t i = 0; i < 1500; ++i)
        kfree(s2_km_arr[i]);

    printk(KERN_INFO "exp_md: free 1500 s1 with kfree\n");

    msleep(10000);

    for (size_t i = 0; i < 1000; ++i)
    {
        kmem_cache_free(s1_cache, s1_arr[i]);
        kmem_cache_free(s2_cache, s2_arr[i]);
        kmem_cache_free(s3_cache, s3_arr[i]);
    }

    printk(KERN_INFO "exp_md: free 1000 s1, 1000 s2, 1000 s3\n");

    msleep(10000);

    kmem_cache_destroy(s1_cache);
    kmem_cache_destroy(s2_cache);
    kmem_cache_destroy(s3_cache);

    vfree(s1_arr);
    vfree(s2_arr);
    vfree(s3_arr);
    vfree(s2_km_arr);

    printk(KERN_INFO "exp_md: module has been loaded\n");

    return 0;
}

static void __exit psm_md_exit(void)
{
    printk(KERN_INFO "exp_md: unloading module\n");

    printk(KERN_INFO "exp_md: module has been unloaded\n");
}

module_init(psm_md_init);
module_exit(psm_md_exit);
