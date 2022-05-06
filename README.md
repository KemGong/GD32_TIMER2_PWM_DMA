/*!
    \file  readme.txt
    \brief description of the TIMER2 DMA demo for gd32e23x

    \version 2019-02-19, V1.0.0, firmware for GD32E23x
*/


  This demo is based on the GD32E230C-EVAL-V1.0 board, it shows how to use DMA with 
TIMER0 update request to transfer data from memory to TIMER2 capture compare register 0.

  TIMER0CLK is fixed to systemcoreclock, the TIMER2 prescaler is equal to 71 so the 
TIMER0 counter clock used is 1MHz.

  The objective is to configure TIMER2 channel 0(PB4) to generate PWM signal with a 
frequency equal to 1KHz and a variable duty cycle(25%-100%) that is changed by 
the DMA after a specific number of update DMA request.

  The number of this repetitive requests is defined by the TIMER2 repetition counter,
each 1 update requests, the TIMER2 channel 0 duty cycle changes to the next new value
defined by the buffer.
![1](https://user-images.githubusercontent.com/39556572/167073374-0e0a425e-99af-4368-99fa-d22eb5fc295d.png)
