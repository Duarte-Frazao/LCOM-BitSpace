#include <minix/syslib.h>
#include <minix/drivers.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <machine/int86.h>
#include <string.h>
#include <stdbool.h>

#include "GraphicsMacros.h"
#include "Graphics.h"
#include "vbe.h"


#define BIT(n) (0x01<<(n))
/* Private global variables */

char *video_mem;		/* Process (virtual) address to which VRAM is mapped */
unsigned h_res;		/* Horizontal screen resolution in pixels */
unsigned v_res;		/* Vertical screen resolution in pixels */
unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
char *second_buffer;

/* Get  Functions*/

unsigned getHorRes(){return h_res;}
unsigned getVerRes(){return v_res;}
char * getGraphicsBuf(){return second_buffer;}
unsigned getBitsPerPixel(){return bits_per_pixel;}
unsigned getBytsPerPixel(){return bits_per_pixel/8;}

void *vg_init(unsigned short mode){
	struct reg86u reg;

	reg.u.b.ah = VBE_FUNCTION; // VBE call,
	reg.u.b.al = SET_VBE_MODE; //Set VBE mode
	reg.u.w.bx = (BIT(14) | mode); // set bit 14: linear framebuffer
	reg.u.b.intno = BIOS_VIDEO_SERVICE;

	if (sys_int86(&reg) != OK) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return NULL;
	}


	vbe_mode_info_t vbe_mode;
	vbe_get_mode_info(mode, &vbe_mode);


	int r;
	struct mem_range mr;
	unsigned int vram_base = vbe_mode.PhysBasePtr;	//VRAM’s physical address
	unsigned int vram_size = (vbe_mode.XResolution * vbe_mode.YResolution * vbe_mode.BitsPerPixel) / 8; //VRAM’s size, but you can use the frame-buffer size, instead

	h_res = vbe_mode.XResolution;
	v_res = vbe_mode.YResolution;
	bits_per_pixel = vbe_mode.BitsPerPixel;

	//Allow memory mapping
	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;
	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);


	//Map memory
	video_mem = (char *) vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
	second_buffer= malloc(h_res*v_res*bits_per_pixel/8);

	if (video_mem == MAP_FAILED){
		panic("Couldn’t map video memory");
		return NULL;
	}

	return video_mem;
}

int vg_exit() {
  struct reg86u reg86;

  reg86.u.b.intno = 0x10; /* BIOS video services */
  reg86.u.b.ah = 0x00;    /* Set Video Mode function */
  reg86.u.b.al = 0x03;    /* 80x25 text mode*/

  if( sys_int86(&reg86) != OK ) {
      printf("\tvg_exit(): sys_int86() failed \n");
      return 1;
  }
  else return 0;
}



void clearScreen()
{
	memset(second_buffer,0,v_res*h_res*bits_per_pixel/8);
}

void blurScreen(){
	unsigned short * mem  = (unsigned short *) second_buffer;
	unsigned int i;
	unsigned int j;
	unsigned int x;
	unsigned int y;

	int blurSize = 7;

	for(i = 0; i < h_res; i++){
	    for(j = 0; j < v_res; j++){
	    	short avgB, avgG, avgR;
	        avgB = avgG = avgR = 0;
	        short ile = 0;

	        for(x = i; x < h_res && x < i + blurSize; x++){
	            for(y = j; y < v_res && y < j + blurSize; y++){
	            	short pixel = mem[x + y*h_res];
	        		short R = (pixel & 0xF800) >> 11;
	        		short G = (pixel & 0x07E0) >> 5;
	        		short B = (pixel & 0x001F);

	                avgB += B;
	                avgG += G;
	                avgR += R;
	                ile++;
	            }
	        }
	        avgB = avgB / ile;
	        avgG = avgG / ile;
	        avgR = avgR / ile;

			mem[i + j*h_res] = 0x0000 | (avgR << 11) | (avgG << 5) | avgB;
	    }
	}

}


void copyBufferToDisplay()
{
	memcpy(video_mem,second_buffer,v_res*h_res*bits_per_pixel/8);
}
