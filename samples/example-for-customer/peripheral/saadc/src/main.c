/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 *  @brief Nordic mesh light switch sample
 */
#include <stdio.h>
#include <stdlib.h>

#include <zephyr.h>
#include <nrfx_saadc.h>

static volatile bool is_ready;
static nrf_saadc_value_t sample;
 
nrfx_saadc_channel_t channel = NRFX_SAADC_DEFAULT_CHANNEL_SE(NRF_SAADC_INPUT_AIN1, 0);
 
static void handle_error(nrfx_err_t error_code)
{
    if (error_code!= NRFX_SUCCESS)
    {
        // error
        while(1){};
    }
}
 
static void event_handler(nrfx_saadc_evt_t const * p_event)
{
    if (p_event->type == NRFX_SAADC_EVT_DONE)
    {
        // Buffer with data is available here: p_event->data.done.p_buffer
        is_ready = true;
		printk("adc value is %d",sample);


    }
}

void main(void)
{
	int err;

    nrfx_err_t err_code;
 
    err_code = nrfx_saadc_init(NRFX_SAADC_DEFAULT_CONFIG_IRQ_PRIORITY);
    handle_error(err_code);
    err_code = nrfx_saadc_channels_config(&channel, 1);
    handle_error(err_code);

	/* Connect ADC IRQ to nrfx_saadc_irq_handler */
	IRQ_CONNECT(DT_IRQN(DT_NODELABEL(adc)),
		    DT_IRQ(DT_NODELABEL(adc), priority),
		    nrfx_isr, nrfx_saadc_irq_handler, 0);

	printk("Initializing...\n");

    while (1)
    {
        is_ready = false;
        err_code = nrfx_saadc_simple_mode_set((1<<0),
                                              NRF_SAADC_RESOLUTION_10BIT,
                                              NRF_SAADC_OVERSAMPLE_DISABLED,
                                              event_handler);
        handle_error(err_code);
        err_code = nrfx_saadc_buffer_set(&sample, 1);
        handle_error(err_code);
        err_code = nrfx_saadc_mode_trigger();		
        handle_error(err_code);
        while(!is_ready){};
		k_sleep(K_MSEC(1000));
    }

}
