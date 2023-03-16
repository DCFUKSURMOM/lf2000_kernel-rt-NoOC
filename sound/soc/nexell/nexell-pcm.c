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

#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/export.h>
#include <linux/module.h>

#include <sound/core.h>
#include <sound/soc.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>

/* set dma channel */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/soc.h>

#include "nexell-pcm.h"

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "\npcm: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#if (0)
#define IRQOUT(msg...)		{ printk(KERN_INFO "pcm: " msg); }
#else
#define IRQOUT(msg...)		do {} while (0)
#endif

#define	DEBUG_RUNTINE		(0)
#define	DMA_DOUBLE_BUFFER	(0)
#define	DMA_BOTTOM_HANDLE	(1)

/*--------------------------------------------------------------------------------
 * sound pcm hardware parameters
 ---------------------------------------------------------------------------------*/
/* PAGE_SIZE 4096 */
#define	BUFFER_MAX_SIZE			(64*1024)

static const struct snd_pcm_hardware nx_pcm_hardware = {

	.info				= 	SNDRV_PCM_INFO_MMAP |
				    		SNDRV_PCM_INFO_MMAP_VALID |
				    		SNDRV_PCM_INFO_INTERLEAVED	|
				    		SNDRV_PCM_INFO_PAUSE |
				    		SNDRV_PCM_INFO_RESUME,	//  | SNDRV_PCM_INFO_BLOCK_TRANSFER

	.formats			= SNDRV_PCM_FMTBIT_S16,
	.channels_min		= 2,
	.channels_max		= 2,
	.buffer_bytes_max	= BUFFER_MAX_SIZE,
	.period_bytes_min	= PAGE_SIZE,
	.period_bytes_max	= BUFFER_MAX_SIZE/2,
	.periods_min		= 2,
	.periods_max		= BUFFER_MAX_SIZE/PAGE_SIZE,
	.fifo_size			= 32,
};

/*------------------------------------------------------------------------------
 * 	local data for DMA trans
 */
struct nx_runtime_data * g_rtd[2] = { NULL, };

static irqreturn_t pcm_play_handler(int irq, void *dev_id);
static irqreturn_t pcm_capt_handler(int irq, void *dev_id);

/* check support playback */
#if defined(PCM_DMA_PLAYBACK)
static int	playback_channel = PCM_DMA_PLAYBACK_CH;
#else
static int	playback_channel = -1;
#endif

/* check support capture */
#if defined(PCM_DMA_CAPTURE)
static int	capture_channel = PCM_DMA_CAPTURE_CH;
#else
static int	capture_channel = -1;
#endif

/*------------------------------------------------------------------------------
 * 	local functions to control DMA
 */
#if (DEBUG_RUNTINE)
static void	dump_runtime(struct snd_pcm_runtime *runtime)
{
	printk("\n--- RUNTIME ---\n");
	printk("Status>\n");
	printk("avail_max			=%d byte\n", frames_to_bytes(runtime, runtime->avail_max));
	printk("hw_ptr_base			=%d byte\n", frames_to_bytes(runtime, runtime->hw_ptr_base));
	printk("hw_ptr_interrupt	=%d byte\n", frames_to_bytes(runtime, runtime->hw_ptr_interrupt));
	printk("HW params>\n");
	printk("access              =0x%x\n", (u_int)runtime->access);
	printk("format              =0x%x\n", (u_int)runtime->format);
	printk("subformat           =0x%x\n", (u_int)runtime->subformat);
	printk("channels            =%d	\n", runtime->channels);
	printk("rate                =%d	\n", runtime->rate);
	printk("periods             =%d	\n", runtime->periods);
	printk("period_size         =%d byte\n", frames_to_bytes(runtime, runtime->period_size));
	printk("buffer_size         =%d byte\n", frames_to_bytes(runtime, runtime->buffer_size));
	printk("info                =%d	\n", runtime->info);
	printk("rate_num            =%d	\n", runtime->rate_num);
	printk("rate_den            =%d	\n", runtime->rate_den);
	printk("sample_bits         =%d	\n", runtime->sample_bits);
	printk("frame_bits          =%d	\n", runtime->frame_bits);
	printk("byte_align          =%d byte\n", runtime->byte_align);
	printk("min_align           =%d byte\n", frames_to_bytes(runtime, runtime->min_align));
	printk("SW params>\n");
	printk("tstamp_mode         =%d	\n", runtime->tstamp_mode);
	printk("period_step         =%d	\n", runtime->period_step);
	printk("control->avail_min  =%d byte\n", frames_to_bytes(runtime, runtime->control->avail_min));
	printk("start_threshold     =%d byte\n", frames_to_bytes(runtime, runtime->start_threshold));
	printk("stop_threshold      =%d byte\n", frames_to_bytes(runtime, runtime->stop_threshold));
	printk("silence_threshold   =%d byte\n", frames_to_bytes(runtime, runtime->silence_threshold));
	printk("silence_size        =%d byte\n", frames_to_bytes(runtime, runtime->silence_size));
	printk("boundary            =%d byte\n", frames_to_bytes(runtime, runtime->boundary));
}
#endif

static int pcm_alloc_dma_ch(struct snd_pcm_substream *substream)
{
	struct nx_runtime_data *rtd = NULL;
	struct dma_trans 	   *tr  = NULL;
	int  stream  = substream->stream;
	irqreturn_t (*handler)(int, void*) = NULL;
	int  ch, ret = -1;

	if ( substream->runtime &&
		substream->runtime->private_data)
		rtd = substream->runtime->private_data;
	else
		rtd = g_rtd[stream];

	if (SNDRV_PCM_STREAM_PLAYBACK == stream)
		ch = playback_channel;

	if (SNDRV_PCM_STREAM_CAPTURE == stream)
		ch = capture_channel;

	DBGOUT("%s (ch:%d)\n", __func__, ch);
	if (-1 == ch)
		return ret;

	tr = soc_dma_request(ch, false);
	if (! tr)
		return ret;

	if (SNDRV_PCM_STREAM_PLAYBACK == stream)	{

		tr->tr_type   = DMA_TRANS_MEM2IO;
		tr->dstbase   = PHY_BASEADDR_AUDIO;
		tr->requestid = DMAINDEX_OF_AUDIO_MODULE_PCMOUT;
		tr->bitwidth  = 16;
		handler       = pcm_play_handler;
	}

	if (SNDRV_PCM_STREAM_CAPTURE == stream) {

		tr->tr_type   = DMA_TRANS_IO2MEM;
		tr->srcbase   = PHY_BASEADDR_AUDIO;
		tr->requestid = DMAINDEX_OF_AUDIO_MODULE_PCMIN;
		tr->bitwidth  = 16;
		handler       = pcm_capt_handler;
	}

	rtd->irq 	= PB_DMA_IRQ(tr->channel);
	rtd->dma_tr = tr;
	ret = request_irq(rtd->irq, handler, IRQF_DISABLED, PCM_DEV_NAME, (void*)substream);

	if (ret)
		printk(KERN_ERR "pcm: fail, irq:%d ch:%d\n", rtd->irq, rtd->dma_tr->channel);

	DBGOUT("%s requestd dma ch(%d) done ...\n", __func__, tr->channel);
	return ret;
}

static void pcm_free_dma_ch(struct snd_pcm_substream *substream)
{
	struct nx_runtime_data *rtd = NULL;
	int  stream = substream->stream;

	if ( substream->runtime &&
		substream->runtime->private_data)
		rtd = substream->runtime->private_data;
	else
		rtd = g_rtd[stream];

	DBGOUT("%s\n", __func__);

	if (rtd->irq)
		free_irq(rtd->irq, (void*)substream);

	if (rtd->dma_tr)
		soc_dma_release(rtd->dma_tr);

	rtd->irq = 0;
	rtd->dma_tr = NULL;
}

static int pcm_start_dma_ch(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct nx_runtime_data *rtd = runtime->private_data;
	struct dma_trans 	   *tr  = rtd->dma_tr;
	int stream = substream->stream;

	DBGOUT("%s\n", __func__);
	if (! tr)
		return -1;

#if (DEBUG_RUNTINE)
	dump_runtime(runtime);
#endif

	/* set buffer */
	rtd->buf_point = 0;
	rtd->buf_base  = runtime->dma_addr;
	rtd->dma_run   = true;

	if (SNDRV_PCM_STREAM_PLAYBACK == stream) {
		tr->length  = snd_pcm_lib_period_bytes(substream);
		tr->srcbase = runtime->dma_addr;

#if (DMA_DOUBLE_BUFFER)
		soc_dma_set_mode(tr, DMA_MODE_BUFFER);
		soc_dma_transfer(tr);
		tr->srcbase += tr->length;
#endif
		soc_dma_transfer(tr);

	} else {
		tr->length  = snd_pcm_lib_period_bytes(substream);
		tr->dstbase = runtime->dma_addr;

		soc_dma_set_mode(tr, DMA_MODE_NORMAL);
		soc_dma_transfer(tr);
	}

	return 0;
}

static int pcm_stop_dma_ch(struct snd_pcm_substream *substream)
{
	struct nx_runtime_data *rtd = substream->runtime->private_data;
	struct dma_trans 	   *tr  = rtd->dma_tr;
	struct snd_dma_buffer  *buf = &substream->dma_buffer;

	DBGOUT("%s\n", __func__);
	if (! tr)
		return -1;

	rtd->dma_run = false;

#if (DEBUG_RUNTINE)
	if (buf->area) {
		int i, j;
		unsigned short* ptr = (unsigned short*)buf->area;
		printk(KERN_INFO "%s: stream=%d, dmabuf=%p, phys=%08x, len=%08x\n",
			__func__, substream->stream, buf->area, buf->addr, buf->bytes);
		for (i = j = 0; i < 64 ; i++, j+=16, ptr+=16) {
			printk(KERN_INFO "%04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x \n",
					ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7],
					ptr[8], ptr[9], ptr[10], ptr[11], ptr[12], ptr[13], ptr[14], ptr[15]);
		}
	}
#else
	(void)buf;
#endif

	return 0;
}

/*--------------------------------------------------------------------------------
 * sound pcm interrupt handler
 ---------------------------------------------------------------------------------*/
#if (DMA_BOTTOM_HANDLE)
static void pcm_play_bh_func(unsigned long data)
{
	struct snd_pcm_substream *substream = (struct snd_pcm_substream *)data;
	snd_pcm_period_elapsed(substream);
  	return;
}
DECLARE_TASKLET(pcm_play_tasklet, pcm_play_bh_func, 0);
#endif

#define	INC_POINT(p, l)		{ p++; if ((int)p > (int)(l-1)) p = 0;}

static irqreturn_t pcm_play_handler(int irq, void *dev_id)
{
	struct snd_pcm_substream *substream = (struct snd_pcm_substream *)dev_id;
	struct snd_pcm_runtime   *runtime = substream->runtime;
	struct nx_runtime_data   *rtd = substream->runtime->private_data;
	struct dma_trans 	     *tr  = rtd->dma_tr;

	snd_pcm_uframes_t avail;

	u_int length   = tr->length;
	u_int srcbase  = 0;
#if (DMA_DOUBLE_BUFFER)
	int	  dmapoint = 0;
#endif
	int   ret      = IRQ_HANDLED;

	/* get next buffer point */
	INC_POINT(rtd->buf_point, (frames_to_bytes(runtime, runtime->buffer_size)/length))

	/* check under run and change buffer */
	avail   = snd_pcm_playback_avail(runtime);
	avail  += (length/4);
	srcbase = rtd->buf_base + (rtd->buf_point * length);

	if (avail >= runtime->stop_threshold) {
		unsigned int dma_addr = (rtd->buf_mute.dma_phys & 0xFFFFFFF0);
		srcbase = (rtd->buf_mute.dma_phys & 0xF ? (dma_addr + 0x10): dma_addr);
	}

#if (DMA_DOUBLE_BUFFER)
	dmapoint = rtd->buf_point;
	INC_POINT(dmapoint, (frames_to_bytes(runtime, runtime->buffer_size)/length))
	srcbase = rtd->buf_base + (dmapoint * length);
#endif

	/* DMA transfer */
	tr->srcbase = srcbase;

	if (rtd->dma_run) {
		soc_dma_transfer(tr);
		ret = IRQ_HANDLED;
	} else {
		ret = IRQ_NONE;
	}

	DBGOUT("IRQ(type:%x, ch:%d, bufp:%2d, src:0x%x, dst:0x%x, len:%d) \n",
		tr->tr_type, tr->channel, rtd->buf_point, tr->srcbase, tr->dstbase, length);

	/* call back */
#if (DMA_BOTTOM_HANDLE)
	pcm_play_tasklet.data = (unsigned long)substream;
	tasklet_hi_schedule(&pcm_play_tasklet);
#else
	snd_pcm_period_elapsed(substream);
#endif
	return ret;
}

#if (DMA_BOTTOM_HANDLE)
static void pcm_capt_bh_func(unsigned long data)
{
	struct snd_pcm_substream *substream = (struct snd_pcm_substream *)data;
	snd_pcm_period_elapsed(substream);
  	return;
}
DECLARE_TASKLET(pcm_capt_tasklet, pcm_capt_bh_func, 0);
#endif

static irqreturn_t pcm_capt_handler(int irq, void *dev_id)
{
	struct snd_pcm_substream *substream = (struct snd_pcm_substream *)dev_id;
	struct snd_pcm_runtime   *runtime = substream->runtime;
	struct nx_runtime_data   *rtd = substream->runtime->private_data;
	struct dma_trans 	     *tr  = rtd->dma_tr;

	u_int length  = tr->length;
	u_int dstbase = 0;
	int   ret     = IRQ_HANDLED;

	/* get next buffer point */
	INC_POINT(rtd->buf_point, (frames_to_bytes(runtime, runtime->buffer_size)/length))

	dstbase = rtd->buf_base + (rtd->buf_point * length);

	/* DMA transfer */
	tr->dstbase = dstbase;

	if (rtd->dma_run) {
		soc_dma_transfer(tr);
		ret = IRQ_HANDLED;
	} else {
		ret = IRQ_NONE;
	}

	IRQOUT("IRQ (type:%x, ch:%d, bufp:%d, src:0x%x, dst:0x%x, len:%d) \n",
		tr->tr_type, tr->channel, rtd->buf_point, tr->srcbase, tr->dstbase, length);

	/* call back */
#if (DMA_BOTTOM_HANDLE)
	pcm_capt_tasklet.data = (unsigned long)substream;
	tasklet_hi_schedule(&pcm_capt_tasklet);
#else
	snd_pcm_period_elapsed(substream);
#endif
	return ret;
}

/*--------------------------------------------------------------------------------
 * sound pcm ops
 ---------------------------------------------------------------------------------*/
static snd_pcm_uframes_t nx_pcm_ops_pointer(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct nx_runtime_data *rtd  = runtime->private_data;
	struct dma_trans 	    *tr  = rtd->dma_tr;

	snd_pcm_uframes_t frames;
	unsigned int bytes;

	bytes  = tr->length * rtd->buf_point;
//	bytes += (tr->length - soc_dma_get_length(tr));

	frames = bytes_to_frames(runtime, bytes);

	if (frames >= runtime->buffer_size)
		frames = 0;

	DBGOUT("pointer (bufp=%d, bytes=%d buffer_size=%d)\n",
		rtd->buf_point, bytes, frames_to_bytes(runtime, runtime->buffer_size));

	return frames;
}

static int nx_pcm_ops_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	int stream = substream->stream;

	DBGOUT("%s\n", __func__);

	if (! g_rtd[stream]) {
		printk("Error, snd runtime data not exist !!!\n");
		return -1;
	}

	runtime->private_data = g_rtd[stream];

	return snd_soc_set_runtime_hwparams(substream, &nx_pcm_hardware);
}

static int nx_pcm_ops_close(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	DBGOUT("%s\n", __func__);

	runtime->private_data = NULL;
	return 0;
}

static int nx_pcm_ops_hw_params(struct snd_pcm_substream *substream,
					struct snd_pcm_hw_params *params)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	unsigned int totbytes = params_buffer_bytes(params);

	DBGOUT("%s\n", __func__);

	snd_pcm_set_runtime_buffer(substream, &substream->dma_buffer);

	/* used DMA bytes by application */
	runtime->dma_bytes = totbytes;
	return 0;
}

static int nx_pcm_ops_free(struct snd_pcm_substream *substream)
{
	DBGOUT("%s\n", __func__);

	snd_pcm_set_runtime_buffer(substream, NULL);

	return 0;
}

static int nx_pcm_ops_prepare(struct snd_pcm_substream *substream)
{
	struct nx_runtime_data *rtd = substream->runtime->private_data;
	struct dma_trans 	   *tr  = rtd->dma_tr;
	volatile int wait = 0;

	DBGOUT("%s\n", __func__);

	while (soc_dma_check_run(tr) &&
			DMA_STOP_TIMEOUT > wait) {
			wait++, msleep(1);
	}

	if (wait >= DMA_STOP_TIMEOUT) {
		printk(KERN_ERR "%s, dma stop wait time out %d\n", __func__, wait);
		soc_dma_trans_stop(tr);
	}

	// clear dummy buffer
	if (rtd->buf_mute.buf_base)
		memset((void*)rtd->buf_mute.buf_base, 0, rtd->buf_mute.buf_size);

	return 0;
}

static int nx_pcm_ops_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct nx_runtime_data *rtd;
	unsigned long flags;
	int ret = 0;

	DBGOUT("%s(cmd:0x%x)\n", __func__, cmd);
	rtd = substream->runtime->private_data;

	spin_lock_irqsave(&rtd->lock, flags);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_RESUME:
		PM_DBGOUT("+%s\n", __func__);
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:

		ret = pcm_start_dma_ch(substream);
		if (SNDRV_PCM_TRIGGER_RESUME == cmd)
			PM_DBGOUT("-%s\n", __func__);
		break;

	case SNDRV_PCM_TRIGGER_SUSPEND:
		PM_DBGOUT("+%s\n", __func__);
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:

		ret = pcm_stop_dma_ch(substream);
		if (SNDRV_PCM_TRIGGER_RESUME == cmd)
			PM_DBGOUT("-%s\n", __func__);
		break;

	default:
		ret = -EINVAL;
		break;
	}

	spin_unlock_irqrestore(&rtd->lock, flags);
	return ret;
}

static int nx_pcm_ops_mmap(struct snd_pcm_substream *substream, struct vm_area_struct *vma)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	DBGOUT("%s\n", __func__);

	return dma_mmap_writecombine(substream->pcm->card->dev, vma,
				     runtime->dma_area,
				     runtime->dma_addr,
				     runtime->dma_bytes);
}

static struct snd_pcm_ops nx_pcm_ops = {
	.open		= nx_pcm_ops_open,
	.close		= nx_pcm_ops_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= nx_pcm_ops_hw_params,
	.hw_free	= nx_pcm_ops_free,
	.prepare	= nx_pcm_ops_prepare,
	.trigger	= nx_pcm_ops_trigger,
	.pointer	= nx_pcm_ops_pointer,
	.mmap		= nx_pcm_ops_mmap,
};

/*--------------------------------------------------------------------------------
 * sound pcm platform
 ---------------------------------------------------------------------------------*/
static int nx_pcm_allocate(struct snd_pcm *pcm, int stream)
{
	struct snd_pcm_substream *substream = pcm->streams[stream].substream;
	struct snd_pcm_runtime 	 *runtime   = substream->runtime;
	struct snd_dma_buffer 	 *buf  		= &substream->dma_buffer;
	size_t 					  size 		= nx_pcm_hardware.buffer_bytes_max;
	struct nx_runtime_data 	 *rtd;

	DBGOUT("+%s\n", __func__);

	/* allocate private data */
	rtd = kzalloc(sizeof(struct nx_runtime_data), GFP_KERNEL);
	if (NULL == rtd) {
		printk(KERN_ERR "fail, snd runtime data ...\n");
		goto fail_alloc;
	}
	g_rtd[stream] = rtd;

	/* allocate pcm buffer */
	buf->dev.type 		= SNDRV_DMA_TYPE_DEV;
	buf->dev.dev 	  	= pcm->card->dev;
	buf->private_data 	= NULL;
	buf->bytes 			= size;
	buf->area 			= dma_alloc_writecombine(
								pcm->card->dev,
								size,
								&buf->addr,
								GFP_KERNEL);
	if (! buf->area) {
		printk(KERN_ERR "fail, snd dma buffer ...\n");
		goto fail_alloc;
	}

	/* allocate mute buffer */
	rtd->buf_mute.buf_size = BUFFER_MAX_SIZE;
	rtd->buf_mute.buf_base = (u_int)dma_alloc_writecombine(
								pcm->card->dev,
								rtd->buf_mute.buf_size,
								(dma_addr_t*)&rtd->buf_mute.dma_phys,
								GFP_KERNEL);

	if (! rtd->buf_mute.buf_base) {
		printk(KERN_ERR "fail, snd mute buffer ...\n");
		goto fail_alloc;
	}

	/* allocate DMA channel */
	if (0 > pcm_alloc_dma_ch(substream))
		goto fail_alloc;

	spin_lock_init(&rtd->lock);

	DBGOUT("-%s\n", __func__);
	return 0;

fail_alloc:

	if (buf->area)
		dma_free_writecombine(pcm->card->dev, buf->bytes, buf->area, buf->addr);

	if (rtd && rtd->buf_mute.buf_base)
		dma_free_writecombine( pcm->card->dev,
					rtd->buf_mute.buf_size,
					(dma_addr_t*)rtd->buf_mute.buf_base,
					rtd->buf_mute.dma_phys );

	if (rtd)
		kfree(rtd);

	buf->area = NULL;

	return -ENOMEM;
}

static void nx_pcm_release(struct snd_pcm *pcm, int stream)
{
	struct snd_pcm_substream *substream = pcm->streams[stream].substream;
	struct snd_pcm_runtime 	 *runtime   = substream->runtime;
	struct nx_runtime_data 	 *rtd		= runtime->private_data;
	struct snd_dma_buffer 	 *buf  	    = &substream->dma_buffer;

	if (runtime && runtime->private_data)
		rtd = runtime->private_data;
	else
		rtd	= g_rtd[stream];

	/* free pcm buffer */
	if (buf->area) {
		dma_free_writecombine(pcm->card->dev, buf->bytes, buf->area, buf->addr);
		buf->area = NULL;
	}

	if (rtd) {
		/* free mute buffer */
		 if (rtd->buf_mute.buf_base)
			dma_free_writecombine( pcm->card->dev,
						rtd->buf_mute.buf_size,
						(dma_addr_t*)rtd->buf_mute.buf_base,
						rtd->buf_mute.dma_phys );

		/* free dma channel */
		pcm_free_dma_ch(substream);

		/* free private data */
		kfree(rtd);
	}

	g_rtd[stream] = NULL;

	DBGOUT("%s\n", __func__);
}

static void nx_pcm_free(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *substream;
	int stream;

	DBGOUT("%s\n", __func__);

	for (stream = 0; stream < 2; stream++) {
		substream = pcm->streams[stream].substream;
		if (! substream)
			continue;

		nx_pcm_release(pcm, stream);
	}
}

static u64 nx_pcm_dmamask = DMA_BIT_MASK(32);

static int nx_pcm_new(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_card    *card = rtd->card->snd_card;
	struct snd_soc_dai *dai  = rtd->cpu_dai;
	struct snd_pcm     *pcm  = rtd->pcm;

	DBGOUT("%s\n", __func__);

	if (!card->dev->dma_mask)
		card->dev->dma_mask = &nx_pcm_dmamask;

	if (! card->dev->coherent_dma_mask)
		card->dev->coherent_dma_mask = 0xffffffff;

	/* allocate playback buffer */
	if (dai->driver->playback.channels_min)	{
		if (nx_pcm_allocate(pcm, SNDRV_PCM_STREAM_PLAYBACK))
			return -ENOMEM;
	}

	/* allocate capture buffer */
	if (dai->driver->capture.channels_min) {
		if (nx_pcm_allocate(pcm, SNDRV_PCM_STREAM_CAPTURE))
			return -ENOMEM;
	}
	return 0;
}

struct snd_soc_platform_driver nx_snd_pcm_plat_drv = {
	.ops 		= &nx_pcm_ops,
	.pcm_new	=  nx_pcm_new,
	.pcm_free	=  nx_pcm_free,
};

/*--------------------------------------------------------------------------------
 * sound pcm platform
 ---------------------------------------------------------------------------------*/
static __devinit int nx_snd_pcm_probe(struct platform_device *pdev)
{
	DBGOUT("%s\n", __func__);
	return snd_soc_register_platform(&pdev->dev, &nx_snd_pcm_plat_drv);
}

static __devexit int nx_snd_pcm_remove(struct platform_device *pdev)
{
	DBGOUT("%s\n", __func__);
	snd_soc_unregister_platform(&pdev->dev);
	return 0;
}

static struct platform_driver nx_snd_pcm_driver = {
	.probe  = nx_snd_pcm_probe,
	.remove = nx_snd_pcm_remove,
	.driver = {
		.name  = PCM_DEV_NAME,
		.owner = THIS_MODULE,
	},
};

static int __init nx_snd_pcm_init(void)
{
	DBGOUT("%s\n", __func__);
	return platform_driver_register(&nx_snd_pcm_driver);
}

static void __exit nx_snd_pcm_exit(void)
{
	DBGOUT("%s\n", __func__);
	platform_driver_unregister(&nx_snd_pcm_driver);
}

module_init(nx_snd_pcm_init);
module_exit(nx_snd_pcm_exit);

MODULE_AUTHOR("jhkim <jhkim@nexell.co.kr>");
MODULE_DESCRIPTION("Sound pcm driver for the Nexell");
MODULE_LICENSE("GPL");
//MODULE_ALIAS("platform:"PCM_DEV_NAME);
MODULE_ALIAS("platform:nx-pcm");
