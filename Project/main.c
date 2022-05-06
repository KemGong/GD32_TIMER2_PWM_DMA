/*!
    \file  main.c
    \brief TIMER0 dma demo for gd32e23x

    \version 2019-02-19, V1.0.0, firmware for GD32E23x
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32e23x.h"
#include <stdio.h>
#include "gd32e230c_eval.h"


#define TIMER0_CH0CV_OFFSET             (0x34)
#define TIMER0_CH0CV                    ((uint32_t)0x40012C34)
#define TIMER2_CH0CV                    ((uint32_t)(TIMER2 + TIMER0_CH0CV_OFFSET))
//#define TIMER2_CH0CV                    ((uint32_t)0x40000434)



uint16_t buffer[18] = {249, 499, 749};
uint16_t dshoot_data[18] = {250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000};

/* configure the GPIO ports */
void gpio_config(void);
/* configure the TIMER peripheral */
void timer_config(void);
/* configure the DMA peripheral */
void dma_config(void);

/**
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gpio_config(void)
{
    /* enable the GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOB);

    /*configure PA8(TIMER0 CH0) as alternate function*/
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_4);
    gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_4);
}

/**
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void dma_config(void)
{
    dma_parameter_struct dma_init_struct;

    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);

    /* initialize DMA channel4 */
    dma_deinit(DMA_CH2);
    /* DMA channel4 initialize */
    dma_init_struct.periph_addr  = (uint32_t)TIMER2_CH0CV;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_addr  = (uint32_t)buffer;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.number       = 18;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH2, &dma_init_struct);
    
    /* enable DMA circulation mode */
    dma_circulation_enable(DMA_CH2);
    /* enable DMA channel4 */
    //dma_channel_enable(DMA_CH2);
}

/**
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer_config(void)
{
    /* TIMER0 DMA Transfer example -------------------------------------------------
    TIMER0CLK = 72MHz, Prescaler = 71 
    TIMER0 counter clock = systemcoreclock/72 = 1MHz.

    the objective is to configure TIMER0 channel 0 to generate PWM
    signal with a frequency equal to 1KHz and a variable duty cycle(25%, 50%, 75%) that
    is changed by the DMA after a specific number of update DMA request.

    the number of this repetitive requests is defined by the TIMER0 repetition counter,
    each 2 update requests, the TIMER0 Channel 0 duty cycle changes to the next new 
    value defined by the buffer . 
    -----------------------------------------------------------------------------*/
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;
    /* enable TIMER clock */
    rcu_periph_clock_enable(RCU_TIMER2);

    timer_deinit(TIMER2);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER0 configuration */
    timer_initpara.prescaler         = 71;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2, &timer_initpara);

    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* CH0 configuration in PWM1 mode */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_HIGH;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER2, TIMER_CH_0, &timer_ocinitpara);

    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_0, 0);//0 todo buffer[0]
    /* configure TIMER channel output compare mode */
    timer_channel_output_mode_config(TIMER2, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    /* disable TIMER channel output shadow function */
    timer_channel_output_shadow_config(TIMER2, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /* TIMER0 primary output enable */
    timer_primary_output_config(TIMER2, ENABLE);
    /* TIMER0 update DMA request enable */
    timer_dma_enable(TIMER2, TIMER_DMA_UPD);//TIMER_DMA_CH0D//TIMER_DMA_UPD
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER2);

    /* TIMER0 counter enable */
    //timer_enable(TIMER2);
}


void send_dshoot(uint16_t *data)
{
	for(uint8_t i = 0; i < 16 ; i++)
	buffer[i] = data[i];
	buffer[16] = 0;
	buffer[17] = 0;
    //__disable_irq();
    dma_transfer_number_config(DMA_CH2, 18);
	dma_channel_enable(DMA_CH2);
	timer_enable(TIMER2);
	while(!dma_flag_get(DMA_CH2, DMA_FLAG_FTF));
	timer_disable(TIMER2);
	dma_channel_disable(DMA_CH2);
	dma_flag_clear(DMA_CH2, DMA_FLAG_FTF);
    //__enable_irq();
	}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure the GPIO ports */
    gpio_config();
    /* configure the DMA peripheral */
    dma_config();
    /* configure the TIMER peripheral */
    timer_config();

    while (1) {
        send_dshoot(dshoot_data);
        for(uint32_t i=0 ; i<100000 ; i++)
        {
            __NOP();
        }
    }
}
