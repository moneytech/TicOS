#include "types.h"
#include "screen.h"
#include "kbd.h"
#include "io.h"
#include "eio.h"

static unsigned int tic;
static unsigned int sec;
static unsigned int ms;

void isr_default_int(void)
{
	//print("interrupt\n");
}

void isr_clock_int(void)
{
	tic++;
	ms += 10;
	if (tic % 100 == 0) {
		sec++;
		tic = 0;
		//char buf[50];
		//numberToString(buf, 482);
		//print("number ");
		//print(buf);
		//println("ms has passed since poweron.");
	}
}

void isr_kbd_int(void)
{
	uchar i;
	static int lshift_enable;
	static int rshift_enable;
	static int alt_enable;
	static int ctrl_enable;
	
	do {
		i = inb(0x64);
	} while ((i & 0x01) == 0);
	i = inb(0x60);
	i--;
	
	//// putcar('\n'); dump(&i, 1); putcar(' ');
	if (i < 0x80) {
		int offset = 0;
		switch (i) {
			case 0x29:
				lshift_enable = 1;
				break;
			case 0x35:
				rshift_enable = 1;
				break;
			case 0x1C:
				ctrl_enable = 1;
				break;
			case 0x37:
				alt_enable = 1;
				break;
			default:
				if (lshift_enable || rshift_enable)
					offset = 1;
				if (ctrl_enable)
					offset = 2;
				if (alt_enable)
					offset = 3;
				putcar(kbdmap[i * 4 + offset]);
		}
	} else {                /* touche relachee */
                i -= 0x80;
                switch (i) {
                case 0x29:
                        lshift_enable = 0;
                        break;
                case 0x35:
                        rshift_enable = 0;
                        break;
                case 0x1C:
                        ctrl_enable = 0;
                        break;
                case 0x37:
                        alt_enable = 0;
                        break;
                }
        }

        show_cursor();
}