//+-----------------------------------------------------------------------
//|	inclusion guard
//+-----------------------------------------------------------------------
#ifndef __DRV_CONTEXT_H__
#define __DRV_CONTEXT_H__

//+-----------------------------------------------------------------------
//|	define
//+-----------------------------------------------------------------------


//+-----------------------------------------------------------------------
//|	data types
//+-----------------------------------------------------------------------
typedef struct CONTEXT_ENTRY_t CONTEXT_ENTRY;

//+-----------------------------------------------------------------------
//|	funtion
//+----------------------------------------------------------------------- 
extern const char* __ogl_drv_VERSION_STRING__;

int             __drv_init        (volatile unsigned int * iobase); // include int  drv_command_init(void);
int             __drv_exit        (volatile unsigned int * iobase); // void drv_command_release(void);
int 			__drv_suspend     (void);
int 			__drv_resume      (void);
void            __drv_irq_handler (void);
void            __drv_loginfo     (void);

CONTEXT_ENTRY * __drv_create_context   (void);
int             __drv_destroy_context  (CONTEXT_ENTRY *pContext);
void            __drv_close_proc       (void);

int             __drv_ioctl_set_version (CONTEXT_ENTRY* pEntry, unsigned int arg); // set application library version string. arg is a user pointer(null terminated string).
int             __drv_ioctl_push        (CONTEXT_ENTRY* pEntry, unsigned int cmd_address);
int             __drv_ioctl_wait        (CONTEXT_ENTRY* pEntry, unsigned int arg);
int             __drv_ioctl_markswap   (CONTEXT_ENTRY* pEntry, unsigned int arg);
//int             __drv_ioctl_free        (CONTEXT_ENTRY* pEntry, unsigned int arg);
//int             __drv_ioctl_finish      (CONTEXT_ENTRY* pEntry, unsigned int arg);

//+-----------------------------------------------------------------------
//| end of inclusion guard
//+-----------------------------------------------------------------------
#endif //__DRV_CONTEXT_H__
