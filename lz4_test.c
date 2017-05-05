#include <linux/module.h>
#include <linux/init.h>
#include <linux/vmalloc.h>
#include <linux/lz4.h>

#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Egor Y. Egorov");
MODULE_DESCRIPTION("Module to test new LZ4 inplementation");
MODULE_VERSION("1.0");


#define BUF_SIZE 573
//#define OUT_BUF_SIZE LZ4_COMPRESSBOUND(BUF_SIZE)

const char buf[BUF_SIZE] = "[General]\nlang=ru_RU\nDefault%20template=/var/tmp/GuitarPro6/Data/Templates/Steel Guitar.gpt\nDefault%20style=:/styles/Classic Style.gps\n\n[RSE]\nMultiThreading=true\nenabled=true"
"[Dialogs]\n"
"Preferences\\ShowErrorNoteOutOfRange=false\n"
"Preferences\\ShowErrorInvalidTuplet=true\n"
"Preferences\\ShowErrorInvalidSimile=true\n"
"Preferences\\ShowErrorInvalidBarLength=true\n"
"Preferences\\ShowErrorIncompleteLegato=true\n"
"Preferences\\ShowErrorUnreachableBar=true\n"
"InstrumentPanel\\Position=@Point(0 0)\n"
"\n"
"[TransportBar]\n"
"SpeedTrainerActivated=false\n"
"\n"
"[MainWindow]\n"
"Position=@Point(50 50)\n"
"Size=@Size(800 640)\n"
"\n"
;

char * compressed_buf;
char * work_mem;
char * decompressed_buf;

static int __init init_lz4_test(void)
{
	int ret;
	size_t out_len;
	size_t compressed_len;
	size_t out_buf_max_size;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
	out_buf_max_size = LZ4_compressBound(BUF_SIZE);
#else
        out_buf_max_size = lz4_compressbound(BUF_SIZE);
#endif

	compressed_buf = vmalloc(out_buf_max_size);
	work_mem = vmalloc(LZ4_MEM_COMPRESS);
	decompressed_buf = vmalloc(BUF_SIZE);

	if(!(compressed_buf && work_mem && decompressed_buf )) 
	{
		printk(KERN_INFO "TEST_LZ4: Unable to allocate memory!\n");
		return -ENOMEM;
	}

	printk(KERN_INFO "TEST LZ4: Hello!\n");
	print_hex_dump(KERN_INFO, "TEST LZ4 INBUF: ", DUMP_PREFIX_NONE,
                                                16, 1, buf, BUF_SIZE, true);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
	ret = LZ4_compress_default(buf, compressed_buf, BUF_SIZE, out_buf_max_size, work_mem);
	out_len = ret;
#else
	out_len = out_buf_max_size;
	ret = lz4_compress(buf, BUF_SIZE, compressed_buf, &out_len, work_mem);
#endif
	printk(KERN_INFO "TEST_LZ4: LZ4_compress returned %d, out_len = %lu\n", ret, out_len);
	
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
	if(ret == 0)
#else
	if(ret < 0)
#endif
		return 0;
	compressed_len = out_len;
	
	print_hex_dump(KERN_INFO, "TEST LZ4 OUTBUF: ", DUMP_PREFIX_NONE,
                                                16, 1, compressed_buf, out_len, true);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
	ret = LZ4_decompress_fast(compressed_buf, decompressed_buf, compressed_len);
	out_len = ret;
#else
	out_len = compressed_len;
	ret = lz4_decompress(compressed_buf, &compressed_len, decompressed_buf, BUF_SIZE);
	out_len = compressed_len;
#endif
        printk(KERN_INFO "TEST_LZ4: LZ4_decompress returned %d, out_len = %lu\n", ret, out_len);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
	if(ret > 0)
#else
	if(ret == 0)
#endif
	print_hex_dump(KERN_INFO, "TEST LZ4 DECBUF: ", DUMP_PREFIX_NONE,
                                           16, 1, decompressed_buf, BUF_SIZE, true);

	return 0;
}

static void __exit exit_lz4_test(void)
{
	vfree(compressed_buf);
	vfree(work_mem);
	vfree(decompressed_buf);
	printk(KERN_INFO "TEST_LZ4: Bye!");
}

module_init( init_lz4_test );
module_exit( exit_lz4_test );

