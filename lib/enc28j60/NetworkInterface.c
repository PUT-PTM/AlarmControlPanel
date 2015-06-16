#include "enc28j60.h"

typedef long BaseType_t;

#include "FreeRTOS.h"
#include "FreeRTOS_IP.h"
#include "list.h"
#include "debug.hpp"

void vReleaseNetworkBufferAndDescriptor( xNetworkBufferDescriptor_t * const pxNetworkBuffer );

BaseType_t xNetworkInterfaceInitialise( void ) 
{
    debug("enc28j60: initializing...\n");
    extern uint8_t ucMACAddress[ 6 ];
    if ( enc28j60_init(ucMACAddress) == 0 ) {
        debug("enc28j60: initialization completed successfully!\n");
        uint8_t revision_id = 0;
        revision_id = enc28j60_rcr(EREVID);
        debug("enc28j60: revision %#x\n", revision_id);
        debug("enc28j60: checked MAC address %x:%x:%x:%x:%x:%x filter: %x\n",
            enc28j60_rcr(MAADR5), enc28j60_rcr(MAADR4), enc28j60_rcr(MAADR3),
            enc28j60_rcr(MAADR2), enc28j60_rcr(MAADR1), enc28j60_rcr(MAADR0),
            enc28j60_rcr(ERXFCON));
        return pdPASS;
    } else {
        debug("......FAILED!\n");
        return pdFAIL;
    }
}

BaseType_t xNetworkInterfaceOutput( xNetworkBufferDescriptor_t * const pxDescriptor,
                                    BaseType_t xReleaseAfterSend )
{
    enc28j60_send_packet(pxDescriptor->pucEthernetBuffer, pxDescriptor->xDataLength );
    
    debug("FreeRTOS: Packet forwarded to driver for transmiting...\n");
    /* Call the standard trace macro to log the send event. */
    iptraceNETWORK_INTERFACE_TRANSMIT();

    if( xReleaseAfterSend != pdFALSE )
    {
        vReleaseNetworkBufferAndDescriptor( pxDescriptor );
    }

    return pdTRUE;
}

