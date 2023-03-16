//+-----------------------------------------------------------------------
//|	including files
//+-----------------------------------------------------------------------
#include <linux/list.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/slab.h> // kmalloc, kfree
#include <asm/io.h>
#include <asm/atomic.h>
#include <asm/uaccess.h>
#include <mach/devices.h>

#include <linux/fb.h>

#include "driver.h"

#define VERSION_STRING  MAKE_VERSION_STRING(ogl_vr5)
//#define VERSION_STRING  MAKE_VERSION_STRING(nxp2120_ogl)
const char* __ogl_vr5_VERSION_STRING__ = "ogl_vr5 version 1.0.55";

//+-----------------------------------------------------------------------------
//| kernel functions
//+-----------------------------------------------------------------------------
  static void*        __knl_malloc   (unsigned int size)                        { return kmalloc(size, GFP_KERNEL); }
//static void*        __knl_zmalloc  (unsigned int size)                        { return kzalloc(size, GFP_KERNEL); }
  static void         __knl_free     (void * ptr)                               { kfree(ptr); }
//static void         __knl_memcpy   (void * dest, const void * src, u32 count) { memcpy(dest, src, count);}
  static void         __knl_memset   (void * dest, u32 c, u32 count)            { memset(dest,   c, count);}
  static unsigned int __knl_getpid   (void)                                     { return (unsigned int)task_tgid_vnr(current); }
//static unsigned int __knl_gettid   (void)                                     { return (unsigned int)current->pid ; }
  static unsigned int __knl_ioread32 (unsigned int base, unsigned int addr )                       { return ioread32(base+addr); }
  static void         __knl_iowrite32(unsigned int base, unsigned int addr,unsigned int data )     { iowrite32(data,base+addr); }
  static unsigned int __knl_copy_from_user( void * to, const void __user * from, unsigned long n ) { return (unsigned int)copy_from_user (to, from, n); }

//+-----------------------------------------------------------------------------
//| local
//+-----------------------------------------------------------------------------
struct CONTEXT_ENTRY_t
{
	struct CONTEXT_ENTRY_t* pNext;
	struct semaphore		m_CmdWait  ;
	struct semaphore		m_CmdRefcnt;
	unsigned int			m_CmdReserved_Address;
	int						m_PID;
	unsigned int            m_Activated;
	int						m_Swap;
	struct fb_var_screeninfo m_SwapVar;
};

struct swap_work_struct
{
	struct work_struct m_work_struct;
	struct fb_var_screeninfo	m_var_info;
};

typedef struct
{
	CONTEXT_ENTRY*	pEntry;					// contex entry
	unsigned int	cmd_address;			// command buffer physical address
	int				release_count;			// irq release count
	int				swap;
	struct swap_work_struct	swap_struct;
} COMMAND_DATA;
static struct workqueue_struct		*g_swap_workqueue;

#define COMMAND_FIFO_BITS		5
#define COMMAND_FIFO_SIZE		(1<<COMMAND_FIFO_BITS)	// 32 entries
#define COMMAND_FIFO_MASK		(COMMAND_FIFO_SIZE - 1)
#define COMMAND_INDEX_INC(idx)	(idx) = (((idx) + 1)&COMMAND_FIFO_MASK)

static char __ogl_lib_VERSION_STRING__[128] = {"library version ?.?.?? : library had not been loaded..."};
static unsigned int             g_iobase            = 0;
static unsigned int             g_clock_state       = 0;
static CONTEXT_ENTRY*           g_pprev_entry	    = 0;
static CONTEXT_ENTRY*           g_pcontex_link_head = 0;

static struct semaphore			g_sema_fifo;			// fifo sync.
static struct semaphore			g_sema_seq ;			// command sync.
static struct semaphore			g_sema_ctx ;			// context management.
static volatile int				g_iIndexIn		= 0;	// push index
static volatile int				g_iIndexOut		= 0;	// pop index
static volatile int				g_wait_pan = 0;
static volatile int				g_fifo_empty	= 1;	// fifo is empty.
static COMMAND_DATA				g_command_fifo[COMMAND_FIFO_SIZE];	// command fifo.
static unsigned int				g_irq_pendding;			// interrupt pending bit

static unsigned int				g_activated_counter=0;

#define REG_STATUS           0x0000
#define REG_POWER_CONTROL    0x0004
#define REG_COMMAND_CONTROL  0x0008
#define REG_CLOCK_CONTROL    0x02C0
#define REG_TSE_BASE         0x0900

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
static void __clock_enable( void )
{
	if( g_clock_state ){ return; }
	g_clock_state = 1;
	__knl_iowrite32(g_iobase, REG_CLOCK_CONTROL, (1 << 3) | 3);
}

static void __clock_disable( void )
{
	if( ! g_clock_state ){ return; }
	g_clock_state = 0;
	__knl_iowrite32(g_iobase, REG_CLOCK_CONTROL, (0 << 3) | 0);
}

static void __sram_power_on( void )
{
	u32 i;
	/* sram power on */
	__knl_iowrite32(g_iobase, REG_POWER_CONTROL, 0x00000001); // power on[0]
	__knl_iowrite32(g_iobase, REG_POWER_CONTROL, 0x00010001); // sleep off[0]
	__knl_iowrite32(g_iobase, REG_POWER_CONTROL, 0x00030001); // power on[1]  // h/w bug
	__knl_iowrite32(g_iobase, REG_POWER_CONTROL, 0x00030003); // sleep off[1] // h/w bug

	for(i = 2; i < 16; ++i)
	{
		__knl_iowrite32( g_iobase, REG_POWER_CONTROL, (((1<<(i  ))-1)<<16) | ((1<<(i+1))-1) ); // power on
		__knl_iowrite32( g_iobase, REG_POWER_CONTROL, (((1<<(i+1))-1)<<16) | ((1<<(i+1))-1) ); // sleep off
	}

	__knl_iowrite32(g_iobase, REG_TSE_BASE+4,  1 );
	__knl_iowrite32(g_iobase, REG_TSE_BASE+4,  3 );
	__knl_iowrite32(g_iobase, REG_TSE_BASE+4,  7 );
	__knl_iowrite32(g_iobase, REG_TSE_BASE+4, 15 );
	__knl_iowrite32(g_iobase, REG_TSE_BASE+0,  1 );
}

static void __sram_power_off( void )
{
	/* sram power off */
	__knl_iowrite32( g_iobase, REG_POWER_CONTROL, 0x0002fffd ); // sleep
	__knl_iowrite32( g_iobase, REG_POWER_CONTROL, 0x00000000 ); // power off

	__knl_iowrite32(g_iobase, REG_TSE_BASE+1,  0 );
}

//#define TRACK_ENABLE_DISABLE_TIMES
#ifdef TRACK_ENABLE_DISABLE_TIMES
static struct timeval start = {0,0};
static struct timeval now = {0,0};
static struct timeval enable_time = {0,0};
static struct timeval disable_time  = {0,0};
static int enable_count = 0;
static int disable_count=0;
static int total_count = 0;
#endif

static void issue_command(void)
{
#ifdef TRACK_ENABLE_DISABLE_TIMES
	struct timeval d_time;
	do_gettimeofday(&now);
	while(start.tv_usec > now.tv_usec)
	{
		now.tv_sec--;
		now.tv_usec += 1000000;
	}
	d_time.tv_sec = now.tv_sec - start.tv_sec;
	d_time.tv_usec = now.tv_usec - start.tv_usec;
	start = now;
	if( g_fifo_empty )
	{
		disable_time.tv_usec += d_time.tv_usec;
		if(disable_time.tv_usec >= 1000000)
		{
			disable_time.tv_sec++;
			disable_time.tv_usec -= 1000000;
		}
		disable_time.tv_sec += d_time.tv_sec;
	} else {
		enable_time.tv_usec += d_time.tv_usec;
		if(enable_time.tv_usec >= 1000000)
		{
			enable_time.tv_sec++;
			enable_time.tv_usec -= 1000000;
		}
		enable_time.tv_sec += d_time.tv_sec;
	}
#endif
	g_fifo_empty = (g_iIndexIn == g_iIndexOut);
	if( g_fifo_empty || g_wait_pan)
	{
		#ifdef TRACK_ENABLE_DISABLE_TIMES
		disable_count++;
		#endif
		__clock_disable();
	}
	else
	{
		#ifdef TRACK_ENABLE_DISABLE_TIMES
		enable_count++;
		#endif
		__clock_enable();
		__knl_iowrite32(g_iobase, REG_COMMAND_CONTROL, g_command_fifo[g_iIndexOut].cmd_address );
	}

	#ifdef TRACK_ENABLE_DISABLE_TIMES
	total_count++;
	if(total_count >= 1000)
	{
		printk(KERN_CRIT "%s:%s:%d enable=%d, %u.%06u, disable=%u, %u.%06u\n", __FILE__, __FUNCTION__, __LINE__,
				enable_count, (int)enable_time.tv_sec, (int)enable_time.tv_usec, disable_count, (int)disable_time.tv_sec, (int)disable_time.tv_usec);
		enable_time.tv_sec = enable_time.tv_usec = disable_time.tv_sec =
				disable_time.tv_usec = enable_count = disable_count = total_count = 0;
	}
	#endif
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
static void __drv_swap_work(struct work_struct *work)
{
	struct swap_work_struct *swap_work = (struct swap_work_struct *)work;
	swap_work->m_var_info.sync = false;
	fb_pan_display(registered_fb[1], &swap_work->m_var_info);
	if( g_wait_pan )
    {
        down(&g_sema_seq);        // sequencial lock (refer to __drv_ioctl_push)
        g_wait_pan = 0;
        issue_command();
        up(&g_sema_seq);        // sequencial unlock (refer to __drv_ioctl_push)
    }
}

int __drv_init(volatile u32 * iobase)
{
	int i;
	g_swap_workqueue = create_workqueue("vr5-wq");

	g_iobase = (unsigned int)iobase;

	//__clock_enable();	
	__sram_power_on();
	
	//// command pump thread initialize
	sema_init(&g_sema_fifo,	COMMAND_FIFO_SIZE);
	sema_init(&g_sema_seq,	1);
	sema_init(&g_sema_ctx,	1);	
	__knl_memset(g_command_fifo, -1, sizeof(COMMAND_DATA)*COMMAND_FIFO_SIZE);
	for(i = 0; i < COMMAND_FIFO_SIZE; ++i)
		INIT_WORK(&g_command_fifo[i].swap_struct.m_work_struct, __drv_swap_work);

	//// interrupt initialize
	g_irq_pendding = __knl_ioread32(g_iobase, REG_STATUS) | 3;	// setup pending bit

	// enable interrupt & clear pending
	__knl_iowrite32(g_iobase, REG_STATUS, g_irq_pendding );
	return 1;
}
int __drv_exit(volatile u32 * iobase)
{
	//// command pump thread release
	{
		int i;
		for(i=0;i<COMMAND_FIFO_SIZE;i++) down(&g_sema_fifo);
		for(i=0;i<COMMAND_FIFO_SIZE;i++) up(&g_sema_fifo);
	}

	// disable interrupt
	__knl_iowrite32( g_iobase, REG_STATUS, g_irq_pendding & (~2));
	
	__sram_power_off();
	//__clock_disable();
	
	g_iobase = 0;
	return 1;
}

int __drv_suspend(void)
{
	int i;
	down(&g_sema_seq);		// block command input
	for(i=0;i<COMMAND_FIFO_SIZE;i++) down(&g_sema_fifo);	// wait fifo empty
	for(i=0;i<COMMAND_FIFO_SIZE;i++) up(&g_sema_fifo);
	g_pprev_entry	= 0;	// set next command must do context switching
	
	__sram_power_off();
	return 1;
}

int __drv_resume (void)
{
	__sram_power_on();

	// enable interrupt & clear pending
	__knl_iowrite32(g_iobase, REG_STATUS, g_irq_pendding );
	g_wait_pan = 0;
		
	up(&g_sema_seq);		// allow command input

	return 1;
}

void __drv_irq_handler(void)
{
	// interrupt pending.
	__knl_iowrite32(g_iobase, REG_STATUS, g_irq_pendding);
	{
		int iIndex = g_iIndexOut;
		COMMAND_DATA* pCmd	= &(g_command_fifo[iIndex]);
		// next pop indext
		COMMAND_INDEX_INC(g_iIndexOut);
		// issue next command
		if(pCmd->swap) {
			//if(pCmd->swap_struct.m_var_info.bits_per_pixel == 32)
			g_wait_pan = 1;
			pCmd->swap = 0;
			queue_work(g_swap_workqueue, &pCmd->swap_struct.m_work_struct);
		}
		issue_command();
		// wake up command pump thread
		{
			CONTEXT_ENTRY* pContext = g_command_fifo[iIndex].pEntry;
			for(;g_command_fifo[iIndex].release_count>0;g_command_fifo[iIndex].release_count--)
				up(&pContext->m_CmdWait  );
			up(&pContext->m_CmdRefcnt);
		}
	}
	// fifo allow
	up(&g_sema_fifo);
}

void __drv_loginfo(void)
{
	#define MAKE_VERSION_STRING(module_name) __## module_name ##_VERSION_STRING__
    extern const char* VERSION_STRING;
	if(g_iobase){
		/*printk( KERN_CRIT "%s = 0x%08x 0x%08x \n", "STATUS", __knl_ioread32(g_iobase,REG_STATUS), __knl_ioread32(g_iobase,12));*/
		printk( KERN_CRIT "0x%08x g_wait_pan\n", g_wait_pan);
		printk( KERN_CRIT "0x%08x STATUS\n", __knl_ioread32(g_iobase, 0x0));
		printk( KERN_CRIT "0x%08x POWER_CONTROL\n", __knl_ioread32(g_iobase, 0x4));
		printk( KERN_CRIT "0x%08x COMMAND_CONTROL\n", __knl_ioread32(g_iobase, 0x8));
		printk( KERN_CRIT "0x%08x COMMAND_EVENT\n", __knl_ioread32(g_iobase, 0xc));
		/*printk( KERN_CRIT "0x%08x STREAM_ADDR_ELEMENT_ARRAY\n", __knl_ioread32(g_iobase, 0x10));
		printk( KERN_CRIT "0x%08x DRAW_FIRST\n", __knl_ioread32(g_iobase, 0x14));
		printk( KERN_CRIT "0x%08x DRAW_COUNT\n", __knl_ioread32(g_iobase, 0x18));
		printk( KERN_CRIT "0x%08x DRAW_CONTROL\n", __knl_ioread32(g_iobase, 0x1c));
		printk( KERN_CRIT "0x%08x DRAW_MODE00\n", __knl_ioread32(g_iobase, 0x20));
		printk( KERN_CRIT "0x%08x DRAW_MODE01\n", __knl_ioread32(g_iobase, 0x24));
		printk( KERN_CRIT "0x%08x DRAW_MODE02\n", __knl_ioread32(g_iobase, 0x28));
		printk( KERN_CRIT "0x%08x BLEND_COLOR\n", __knl_ioread32(g_iobase, 0x2c));
		printk( KERN_CRIT "0x%08x STENCIL_PARAM00\n", __knl_ioread32(g_iobase, 0x30));
		printk( KERN_CRIT "0x%08x STENCIL_PARAM01\n", __knl_ioread32(g_iobase, 0x34));
		printk( KERN_CRIT "0x%08x STENCIL_PARAM02\n", __knl_ioread32(g_iobase, 0x38));
		printk( KERN_CRIT "0x%08x VIEWPORT_MIN\n", __knl_ioread32(g_iobase, 0x3c));
		printk( KERN_CRIT "0x%08x VIEWPORT_MAX\n", __knl_ioread32(g_iobase, 0x40));
		printk( KERN_CRIT "0x%08x SCISSOR_BOX_MIN\n", __knl_ioread32(g_iobase, 0x44));
		printk( KERN_CRIT "0x%08x SCISSOR_BOX_MAX\n", __knl_ioread32(g_iobase, 0x48));
		printk( KERN_CRIT "0x%08x POLYGON_OFFSET_FACTOR\n", __knl_ioread32(g_iobase, 0x4c));
		printk( KERN_CRIT "0x%08x POLYGON_OFFSET_UNITS\n", __knl_ioread32(g_iobase, 0x50));
		printk( KERN_CRIT "0x%08x RCP_LINE_WIDTH\n", __knl_ioread32(g_iobase, 0x54));
		printk( KERN_CRIT "0x%08x -TEXTURE_2D-\n", __knl_ioread32(g_iobase, 0x58));*/
	}
    printk( "%s\n", VERSION_STRING );
	printk( "%s\n", __ogl_lib_VERSION_STRING__ );
}


CONTEXT_ENTRY * __drv_create_context   (void)
{
	CONTEXT_ENTRY* pContext = (CONTEXT_ENTRY *)__knl_malloc(sizeof(CONTEXT_ENTRY));
	if(pContext)
	{
		__knl_memset(pContext, 0, sizeof(CONTEXT_ENTRY));
		down(&g_sema_ctx);
		pContext->pNext = (void*)g_pcontex_link_head;
		g_pcontex_link_head = pContext;
		sema_init(&pContext->m_CmdWait  , 0);
		sema_init(&pContext->m_CmdRefcnt, COMMAND_FIFO_SIZE);
		pContext->m_CmdReserved_Address = 0;
		pContext->m_PID                 = __knl_getpid();
		pContext->m_Activated           = g_activated_counter++;
		
		up(&g_sema_ctx);
		return pContext;
	}
	return 0;
}

static void __drv_cleanup_context(CONTEXT_ENTRY *pContext)
{
	int i;
	if(!pContext || !pContext->m_PID ) return;
	pContext->m_PID = 0;
	pContext->m_Activated = 0;
	for(i=0;i<COMMAND_FIFO_SIZE;i++) down(&pContext->m_CmdRefcnt);
	for(i=0;i<COMMAND_FIFO_SIZE;i++) up  (&pContext->m_CmdRefcnt);
}

int __drv_destroy_context  (CONTEXT_ENTRY *pContext)
{
	// �ڽ��� ���� ���ؽ��� �����Ѵ�.
	if(!pContext) return -1;
	{
		CONTEXT_ENTRY* pCur;
		__drv_cleanup_context( pContext );
		down(&g_sema_ctx);
		pCur = g_pcontex_link_head;
		if(pCur==pContext){
			g_pcontex_link_head = pCur->pNext;
		}else{
			while(pCur){
				if(pCur->pNext == pContext){
					pCur->pNext = pContext->pNext;
					break;
				}
				pCur = pCur->pNext;
			}
		}
		up(&g_sema_ctx);
		if(pCur){
			__knl_free(pContext);
			return 0;
		}
	}
	return -1;
}

void __drv_close_proc( void )
{
	int closed_pid = __knl_getpid();
	CONTEXT_ENTRY* pEntry = g_pcontex_link_head;
	while( pEntry )
	{
		if(pEntry->m_PID==closed_pid)
		{
			__drv_cleanup_context( pEntry );
		}
		pEntry = pEntry->pNext;
	}	
}

int __drv_ioctl_set_version (CONTEXT_ENTRY* pEntry, unsigned int arg)
{
	__knl_copy_from_user((void *)__ogl_lib_VERSION_STRING__, (const void __user *)arg, (unsigned long)sizeof(__ogl_lib_VERSION_STRING__));
	__ogl_lib_VERSION_STRING__[sizeof(__ogl_lib_VERSION_STRING__)-1] = 0;
	return 0;
}

int __drv_ioctl_push(CONTEXT_ENTRY* pEntry, unsigned int cmd_address)
{
	int				release_count = 0;
	if(pEntry->m_PID==0){ return 1; }

	down(&g_sema_seq);		// sequencial lock for index increase

	// check context switching
	// if context is different and not forced command, run
	if(g_pprev_entry!=pEntry && (!pEntry->m_CmdReserved_Address)){
		pEntry->m_CmdReserved_Address = cmd_address | 1;
		up(&g_sema_seq);
		return 0;
	}

	// add queue
	pEntry->m_Activated 	= g_activated_counter++;
ADD_QUEUE:
	if(cmd_address)
	{
		COMMAND_DATA* pCmd	= &(g_command_fifo[g_iIndexIn]);
		down(&g_sema_fifo);		// request fifo
		down(&pEntry->m_CmdRefcnt);
		pCmd->pEntry		= pEntry;
		pCmd->cmd_address	= cmd_address | 1;	// 1 = run signal.
		pCmd->release_count	= release_count + (pEntry->m_CmdReserved_Address ? 0 : 1);
		if( ! pEntry->m_CmdReserved_Address )
		{
			pCmd->swap = pEntry->m_Swap;
			pCmd->swap_struct.m_var_info = pEntry->m_SwapVar;
			pEntry->m_Swap = 0;
		}
		else
		{
			pCmd->swap = 0;
		}

		COMMAND_INDEX_INC(g_iIndexIn);
		if(g_fifo_empty)
		{
			issue_command();
		}
	}

	if(pEntry->m_CmdReserved_Address){
		cmd_address		= pEntry->m_CmdReserved_Address;
		release_count	= 1;
		pEntry->m_CmdReserved_Address = 0;
		goto ADD_QUEUE;
	}

	g_pprev_entry	= pEntry;

	up(&g_sema_seq);

	return 1;
}

int __drv_ioctl_wait(CONTEXT_ENTRY* pEntry, unsigned int arg)
{
	if(pEntry->m_PID==0){ return 0; }
	//down(&pEntry->m_CmdWait);
	return down_trylock(&pEntry->m_CmdWait);
}

int __drv_ioctl_markswap(CONTEXT_ENTRY* pEntry, unsigned int arg)
{
	copy_from_user(&pEntry->m_SwapVar, (int*)arg, sizeof(struct fb_var_screeninfo));
	pEntry->m_Swap = 1;
	return 1;
}

/*
int __drv_ioctl_free(CONTEXT_ENTRY* pEntry, unsigned int arg)
{
	up(&pEntry->m_CmdWait);
	return 0;
}

int __drv_ioctl_finish(CONTEXT_ENTRY* pEntry, unsigned int arg)
{
	int i;
	for(i=0;i<COMMAND_FIFO_SIZE;i++) down(&pEntry->m_CmdRefcnt);
	for(i=0;i<COMMAND_FIFO_SIZE;i++) up  (&pEntry->m_CmdRefcnt);
	return 0;
}
*/

//----------------------------------------------------------------------------
//	for vmem(vmem_killer)
//----------------------------------------------------------------------------
int KnlGetPrimCandidatePIDToRemove( void )
{
	CONTEXT_ENTRY* oldest_activated = 0;
	CONTEXT_ENTRY* pEntry = g_pcontex_link_head;
	printk( "%s(%d):pid(%d)\n", __FUNCTION__,__LINE__,__knl_getpid() );
	while( pEntry )
	{
		if( pEntry->m_PID )
		{
			if( (!oldest_activated) || 
				((oldest_activated->m_Activated > pEntry->m_Activated) && ((oldest_activated->m_Activated - pEntry->m_Activated) < 0x7FFFFFFF)) )
			{
				oldest_activated = pEntry;
			}
		}
		pEntry = pEntry->pNext;
	}
	printk( "%s(%d):pid(%d):result is %d\n", __FUNCTION__,__LINE__,__knl_getpid(), oldest_activated ? oldest_activated->m_PID : -1 );
	return oldest_activated ? oldest_activated->m_PID : -1;
}
EXPORT_SYMBOL(KnlGetPrimCandidatePIDToRemove);


