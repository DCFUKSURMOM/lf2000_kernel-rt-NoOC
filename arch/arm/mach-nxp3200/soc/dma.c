/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/hardirq.h>
#include <linux/slab.h>

/* 	Note>
 *	to enable assert, must be efine "PB_DEBUG"
 */

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/soc.h>

#if (0)
#define DBGOUT(msg...)		{ printk("dma: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*-----------------------------------------------------------------------------*/
struct dma_ch_stat {
	int				ch;
	bool			used;
	unsigned int	mode;
	unsigned int * 	handle;
#if (0)
	spinlock_t 		lock;
#else
	struct mutex	lock;
#endif
};

static struct dma_ch_stat	dma_stat[NUMBER_OF_DMA_MODULE];
static struct mutex		  	dma_lock;

#if (0)
#define	DMA_LOCK_INIT(ch)	{ spin_lock_init(&dma_stat[ch].lock); }
#define	DMA_LOCK(ch)		{ spin_lock  (&dma_stat[ch].lock); }
#define	DMA_UNLOCK(ch)		{ spin_unlock(&dma_stat[ch].lock); }
#else
#define	DMA_LOCK_INIT(ch)	{ mutex_init(&dma_stat[ch].lock); }
#define	DMA_LOCK(ch)		{ if (! preempt_count() && !in_interrupt()) mutex_lock  (&dma_stat[ch].lock); }
#define	DMA_UNLOCK(ch)		{ if (mutex_is_locked(&dma_stat[ch].lock))  mutex_unlock(&dma_stat[ch].lock); }
#endif

/*-----------------------------------------------------------------------------*/
void soc_dma_init(void)
{
	int i  = 0;
	DBGOUT("%s\n", __func__);

	mutex_init(&dma_lock);
	for (; i<NUMBER_OF_DMA_MODULE; i++) {
		dma_stat[i].ch     = i;
		dma_stat[i].used   = false;
		dma_stat[i].mode   = DMA_MODE_NORMAL;
		dma_stat[i].handle = NULL;
		DMA_LOCK_INIT(i);
	}
}

/*------------------------------------------------------------------------------
 * 	Description	: request DMA channel
 *	In[ch]		: DMA ch (0~15), set when request staic DMA ch
 *	In[dynamic]	: set true when request DMA ch dynamically. set false when static DMA ch.
 *	Return 		: NULL = failure else success.
 *				:
 */
struct dma_trans * soc_dma_request(int channel, bool dynamic)
{
	int i  =  0;
	int ch = -1;
	struct dma_trans *tr = NULL;

	DBGOUT("%s (ch:%d, %s)\n", __func__, channel, dynamic?"dynamic":"static");

	/* lock */
	mutex_lock(&dma_lock);

	if (! dynamic) {
		if (channel > 16 || 0 > channel ) {
			printk(KERN_ERR "fail, not support '%s' dma '%d'...\n", dynamic?"dynamic":"static", channel);
			goto err;
		}
		ch = dma_stat[channel].used ? -1 : channel;
	} else {
		for (; i<NUMBER_OF_DMA_MODULE; i++) {
			if (false == dma_stat[i].used) {
				ch = i;
				break;
			}
		}
	}

	if (-1 == ch) {
		dynamic ? printk(KERN_ERR "fail, no exist dma free channel ...\n"):
			printk(KERN_ERR "fail, dma %d channel is used ...\n", channel);
		goto err;
	}

	tr = kzalloc(sizeof(struct dma_trans), GFP_KERNEL);
	if (! tr) {
		printk(KERN_ERR "fail, allocate dma(%d) transfer data ...\n", i);
		goto err;
	}
	tr->channel = ch;

	/* set dma channel status */
	dma_stat[ch].used   = true;
	dma_stat[ch].handle = (unsigned int*)tr;
	dma_stat[ch].mode   = DMA_MODE_NORMAL;

	/* default: non buffered */
	NX_DMA_OpenModule(ch);
	NX_DMA_SetCommandBufferMode(ch, CFALSE);

err:
	/* unlock */
	mutex_unlock(&dma_lock);
	return tr;
}
EXPORT_SYMBOL(soc_dma_request);

/*------------------------------------------------------------------------------
 * 	Description	: release DMA channel
 *	In[tr]		: requested DMA handle
 *	Return 		: 0 = success.
 *				:-1 = unknown DMA handle.
 */
int	soc_dma_release(struct dma_trans *tr)
{
	int ch;
	NX_ASSERT(tr);

	ch = tr->channel;

	NX_ASSERT(dma_stat[ch].handle == (u_int*)tr);
	DBGOUT("%s (ch:%d)\n", __func__, ch);

	/* lock */
	mutex_lock(&dma_lock);

	kfree(tr);

	dma_stat[ch].used   = false;
	dma_stat[ch].handle = NULL;

	/* unlock */
	mutex_unlock(&dma_lock);
	return 0;
}
EXPORT_SYMBOL(soc_dma_release);

/*------------------------------------------------------------------------------
 * 	Description	: check whether DMA transfer is running or not.
 *	In[tr]		: requested DMA handle
 *	Return 		: 0 = DMA is ready.
 *				: 1 = DMA is running.
 *				:-1 = unknown DMA handle.
 */
int soc_dma_check_run(struct dma_trans *tr)
{
	int ch, run = 0;
	NX_ASSERT(tr);

	ch = tr->channel;

	NX_ASSERT(dma_stat[ch].handle == (u_int*)tr);
	DBGOUT("%s (ch:%d)\n", __func__, ch);

	/* lock */
	DMA_LOCK(ch);

	run = NX_DMA_CheckRunning(ch) ? 1 : 0;

	/* unlock */
	DMA_UNLOCK(ch);
	DBGOUT("%s (ch:%d, ret:%s, size:%5d)\n",
		__func__, ch, run?"BUSY":"READY", NX_DMA_GetDataSize(ch));
	return run;
}
EXPORT_SYMBOL(soc_dma_check_run);

/*------------------------------------------------------------------------------
 * 	Description	: wait for DMA ready status
 *	In[tr]		: requested DMA handle
 *	In[timeout]	: wait timer out (us)
 *	Return 		: 0 = time out.
 *				: n = wait time for ready.
 *				:-1 = unknown DMA handle.
 */
int soc_dma_wait_ready(struct dma_trans *tr, u_int timeout)
{
	int ch, tout;
	NX_ASSERT(tr);

	ch = tr->channel;

	NX_ASSERT(dma_stat[ch].handle == (u_int*)tr);
	DBGOUT("%s (ch:%d, timeout:%d)\n", __func__, ch, timeout);

	/* lock */
	DMA_LOCK(ch);

	tout = timeout;

	while ( tout-- &&
			NX_DMA_CheckRunning(ch) )
		udelay(1);

	/* unlock */
	DMA_UNLOCK(ch);
	DBGOUT("%s (ch:%d, ret:%s, wait:%6d)\n",
		__func__, ch, 0 >= tout?"Time out":"Ready", timeout-tout);

	return (0 >= tout ? 0 : timeout-tout);
}
EXPORT_SYMBOL(soc_dma_wait_ready);

/*------------------------------------------------------------------------------
 * 	Description	: Get a transfer size in bytes
 *				: this function informs a remained data size to be transferred while
 *			    : DMA transfer is running.
 *	In[tr]		: requested DMA handle
 *	Return 		: n = a remained data size.
 *				:-1 = unknown DMA handle.
 */
unsigned int soc_dma_get_length(struct dma_trans *tr)
{
	int ch, len = 0;
	u_long flags;
	NX_ASSERT(tr);

	ch = tr->channel;

	NX_ASSERT(dma_stat[ch].handle == (u_int*)tr);
	DBGOUT("%s (ch:%d)\n", __func__, ch);

	/* disable irq */
	local_irq_save(flags);

	len = NX_DMA_GetDataSize(ch);

	/* enable irq */
	local_irq_restore(flags);
	return len;
}
EXPORT_SYMBOL(soc_dma_get_length);

/*------------------------------------------------------------------------------
 * 	Description	: Transfer DMA
 *	In[tr]		: requested DMA handle
 *	Return 		: 0 = success.
 *				:-1 = unknown DMA handle
 *				:-2 = not support DMA transfer type
 */
int soc_dma_transfer(struct dma_trans *tr)
{
	int ch, ret = 0;
	NX_ASSERT(tr);

	ch = tr->channel;

	NX_ASSERT(dma_stat[ch].handle == (u_int*)tr);
	DBGOUT("+++%s (ch:%d, type:%d)\n", __func__, ch, tr->tr_type);

	/* lock */
	DMA_LOCK(ch);

	if (DMA_MODE_BUFFER == dma_stat[ch].mode) {
		volatile int wait = 1000;
		while ( wait-- > 0 &&
				CTRUE != NX_DMA_IsCommandBufferReady(ch)) {
			udelay(1);
		}

		if (0 >= wait) {
			printk(KERN_ERR "%s: EROR, command buffer ready \n", __func__);
			return -1;
		}
	}

	/* SET ENABLE varialbe, check prototype function */
	if (NX_DMA_GetInterruptEnable(ch, 0))
		NX_DMA_SetInterruptEnableAll(ch, CTRUE);

	switch (tr->tr_type) {
	case DMA_TRANS_MEM2IO:
		NX_DMA_TransferMemToIO(ch, (const void*)tr->srcbase, tr->dstbase,
					tr->requestid, tr->bitwidth,
					tr->length);
		break;
	case DMA_TRANS_IO2MEM:
		NX_DMA_TransferIOToMem(ch, tr->srcbase, tr->requestid,
					tr->bitwidth, (void*)tr->dstbase,
					tr->length);
		break;
	case DMA_TRANS_MEM2MEM:
		NX_DMA_TransferMemToMem(ch, (const void*)tr->srcbase,
					(void*)tr->dstbase, tr->length);
		break;
//	case DMA_TRANS_MEM2SIO:
//		NX_DMA_TransferMemToSIO(ch, (const void*)tr->srcbase, tr->dstbase,
//					tr->dst_id, tr->dst_bit, tr->length);
//		break;
//	case DMA_TRANS_SIO2MEM:
//		NX_DMA_TransferSIOToMem(ch, tr->srcbase, tr->src_id, tr->src_bit,
//					(void*)tr->dstbase, tr->length);
		break;
	default:
		ret = -2;
		break;
	}
	DBGOUT("---%s (ch:%d, type:%d, len:%d)\n", __func__,
		ch, tr->tr_type, NX_DMA_GetDataSize(ch));

	/* unlock */
	DMA_UNLOCK(ch);
	return ret;
}
EXPORT_SYMBOL(soc_dma_transfer);

/*------------------------------------------------------------------------------
 * 	Description	: Set transfer DMA mode
 *	In[tr]		: requested DMA handle
 *	In[mode]	: 0 = noraml mode
 *				: 1 = buffered mode
 *	Return 		: 0 = success.
 *				:-1 = unknown DMA handle
 *				:-2 = not support DMA transfer mode
 */
int soc_dma_set_mode(struct dma_trans *tr, int mode)
{
	int ch, ret = 0;
	u_long flags;

	NX_ASSERT(tr);

	ch = tr->channel;

	NX_ASSERT(dma_stat[ch].handle == (u_int*)tr);
	DBGOUT("%s (ch:%d, mode:%d)\n", __func__, ch, mode);

	/* disable irq */
	local_irq_save(flags);

	switch (mode) {
	case DMA_MODE_NORMAL:
		dma_stat[ch].mode = DMA_MODE_NORMAL;
		NX_DMA_SetCommandBufferMode(ch, CFALSE);
		break;
	case DMA_MODE_BUFFER:
		dma_stat[ch].mode = DMA_MODE_BUFFER;
		NX_DMA_SetCommandBufferMode(ch, CTRUE);
		break;
	default:
		ret = -2;
		break;
	}

	/* enable irq */
	local_irq_restore(flags);
	return ret;
}
EXPORT_SYMBOL(soc_dma_set_mode);

/*------------------------------------------------------------------------------
 * 	Description	: get transfer DMA mode
 *	In[tr]		: requested DMA handle
 *	Return		: 0 = noraml mode
 *				: 1 = buffered mode
 */
unsigned int soc_dma_get_mode(struct dma_trans *tr)
{
	int ch;
	NX_ASSERT(tr);

	ch = tr->channel;

	NX_ASSERT(dma_stat[ch].handle == (u_int*)tr);
	DBGOUT("%s (ch:%d)\n", __func__, ch);

	return (dma_stat[ch].mode);
}
EXPORT_SYMBOL(soc_dma_get_mode);

/*------------------------------------------------------------------------------
 * 	Description	: Stop/Cancel DMA Transfer
 *	In[tr]		: requested DMA handle
 *	Return 		: -1 = unknown DMA handle, 0 = success.
 */
int soc_dma_trans_stop(struct dma_trans *tr)
{
	int ch;
	NX_ASSERT(tr);

	ch = tr->channel;

	NX_ASSERT(dma_stat[ch].handle == (u_int*)tr);
	DBGOUT("%s (ch:%d)\n", __func__, ch);

	/* lock */
	DMA_LOCK(ch);

	if (DMA_MODE_BUFFER == dma_stat[ch].mode) {
		volatile int wait = 1000;
		NX_DMA_CommandBufferFlush(ch);
		while ( wait-- > 0 &&
				CTRUE != NX_DMA_IsCommandBufferFlush(ch))
			udelay(1);
	}
	NX_DMA_Stop(ch, CTRUE);

	/* unlock */
	DMA_UNLOCK(ch);
	return 0;
}
EXPORT_SYMBOL(soc_dma_trans_stop);
