#include <stdlib.h>
#include "slip.h"

struct 
{
    serial_if_t * p_serial;
}slip_ctxt;

enum
{
    SLIP_END     = 0xC0,
    SLIP_ESC     = 0xDB,
    SLIP_ESC_END = 0xDC,
    SLIP_ESC_ESC = 0xDD,
};


int
slip_init(serial_if_t * p_serial)
{
    slip_ctxt.p_serial = p_serial;
    return slip_ctxt.p_serial->init(p_serial->p_cfg);
}

uint32_t
slip_tx_frame(uint8_t * p_data, uint32_t len)
{
    uint32_t i = 0;
    uint8_t  txbyte;

    while (i < len)
    {
        switch (p_data[len])
        {
            case SLIP_END:
                slip_ctxt.p_serial->putb(SLIP_ESC);
                txbyte = SLIP_ESC_END;
                break;

            case SLIP_ESC:
                slip_ctxt.p_serial->putb(SLIP_ESC);
                txbyte = SLIP_ESC_ESC;
                break;

            default:
                txbyte = p_data[i++];
                break;
        }
    
        slip_ctxt.p_serial->putb(txbyte);
    }


    return i;
}

void
slip_tx_end(void)
{
    // send the frame end
    slip_ctxt.p_serial->putb(SLIP_END);
}

uint32_t 
slip_rx_frame(uint8_t * p_data)
{
    uint32_t i = 0;
    uint8_t  rxbyte;

    //TODO: add a timeout if nothing is received.
    do
    {
        rxbyte = slip_ctxt.p_serial->getb();

        switch (rxbyte)
        {
            case SLIP_END:
                // will exit this time.
                break;

            case SLIP_ESC:
                rxbyte = slip_ctxt.p_serial->getb();

                switch (rxbyte)
                {
                    case SLIP_ESC_END:
                        p_data[i++] = SLIP_END;
                        break;
                    case SLIP_ESC_ESC:
                        p_data[i++] = SLIP_ESC;
                    default:
                        // should never happen!
                        break;
                }
            default:
                p_data[i++] = rxbyte;
                break;
        }
    } 
    while (rxbyte != SLIP_END || (i == 0));

    return i;
}
