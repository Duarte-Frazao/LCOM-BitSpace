#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"
#include "video.h"
#include "video_test.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

//To-do por erros nos != OK


int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {

	//Specifications on page 16 of vbe guide
	struct reg86u r;

	//Buffer initialization
	if (lm_init() == NULL){printf("In vbe_get_mode_info_block(): Error in lm_init() call");return 1;}

	mmap_t m; //Struct that keeps info regarding the mapping of physical memory to virtual memory

	if(lm_alloc(sizeof(vbe_mode_info_t), &m) == NULL){ printf("In vbe_get_mode_info_block(): Error in lm_aloc() call");return 1;}

	r.u.b.ah = VBE_FUNCTION;
	r.u.b.al = RETURN_VBE_MODE_INFORMATION; /* VBE get mode info */
	r.u.w.cx = mode; //Mode number

	/* translate the buffer linear address to a far pointer */
	r.u.w.es = PB2BASE(m.phys); /* set a segment base */
	r.u.w.di = PB2OFF(m.phys); /* set the offset accordingly */
	r.u.b.intno = BIOS_VIDEO_SERVICE;

	if( sys_int86(&r) != OK ) {
		printf("In vbe_get_mode_info_block(): Error in sys_int86() call");
		lm_free(&m);
		return 1;}

	*vmi_p= *((vbe_mode_info_t *) m.virtual);

	lm_free(&m);

	return 0;
}

int vbe_get_info_block(vbe_info_block_t *vmi_p)
{
	//Specifications on page 12 of vbe guide
	struct reg86u r;

	//Buffer initialization
	if (lm_init() == NULL){printf("In vbe_get_info_block(): Error in lm_init() call");return 1;}

	mmap_t m; //Struct that keeps info regarding the mapping of physical memory to virtual memory

	if(lm_alloc(sizeof(vbe_info_block_t), &m) == NULL){printf("In vbe_get_info_block(): Error in lm_alloc() call");return 1;}



	r.u.b.ah = VBE_FUNCTION;
	r.u.b.al = RETURN_VBE_CONTROLLER_INFORMATION; /* VBE get controller info */

	/* translate the buffer linear address to a far pointer */
	r.u.w.es = PB2BASE(m.phys); /* set a segment base */
	r.u.w.di = PB2OFF(m.phys); /* set the offset accordingly */
	r.u.b.intno = BIOS_VIDEO_SERVICE;


	if( sys_int86(&r) != OK ) {
		printf("In vbe_get_info_block(): Error in sys_int86() call");
		lm_free(&m);
		return 1;}

	*vmi_p = *((vbe_info_block_t *) m.virtual);


	//vmi_p->VideoModePtr= ((vmi_p->VideoModePtr >>12) & 0xF0000) + (vmi_p->VideoModePtr & 0x0FFFF);

	lm_free(&m);

	return 0;
}




