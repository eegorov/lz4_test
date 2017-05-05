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
#define OUT_BUF_SIZE LZ4_COMPRESSBOUND(BUF_SIZE)

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


static int __init init_lz4_test(void)
{
	int ret;
	size_t out_len;
	size_t compressed_len;
	char * compressed_buf = vmalloc(OUT_BUF_SIZE);
	char * work_mem = vmalloc(LZ4_MEM_COMPRESS);
	char * decompressed_buf = vmalloc(BUF_SIZE);

	printk(KERN_INFO "TEST LZ4: Hello!\n");
	print_hex_dump(KERN_INFO, "TEST LZ4 INBUF:", DUMP_PREFIX_NONE,
                                                16, 1, buf, BUF_SIZE, true);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
	ret = LZ4_compress_default(buf, compressed_buf, BUF_SIZE, OUT_BUF_SIZE, work_mem);
	out_len = ret;
#else
	ret = lz4_compress(buf, BUF_SIZE, compressed_buf, OUT_BUF_SIZE, &out_len, work_mem);
#endif
	printk(KERN_INFO "TEST_LZ4: LZ4_compress returned %d\n", ret);
	
	if(ret == 0)
		return 0;
	compressed_len = out_len;
	
	print_hex_dump(KERN_INFO, "TEST LZ4 OUTBUF:", DUMP_PREFIX_NONE,
                                                16, 1, compressed_buf, out_len, true);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
	ret = LZ4_decompress_fast(compressed_buf, decompressed_buf, compressed_len);
	out_len = ret;
#else
	ret = lz4_decompress_unknownoutputsize(compressed_buf, compressed_len, decompressed_buf, &out_len);
#endif
        printk(KERN_INFO "TEST_LZ4: LZ4_decompress returned %d\n", ret);

        if(ret > 0)
		print_hex_dump(KERN_INFO, "TEST LZ4 OUTBUF:", DUMP_PREFIX_NONE,
	                                        16, 1, decompressed_buf, ret, true);

	return 0;
}

static void __exit exit_lz4_test(void)
{
	printk(KERN_INFO "TEST_LZ4: Bye!");
}

module_init( init_lz4_test );
module_exit( exit_lz4_test );

