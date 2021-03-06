/********************************************************************/
/**                     Microsoft LAN Manager                      **/
/**               Copyright(c) Microsoft Corp., 1990-2000          **/
/********************************************************************/
/* :ts=4 */

//** ARP.H - Exports from ARP.
//
// This file contains the public definitons from ARP.
int ARPInit(void);

#pragma warning(push)
#pragma warning(disable:4115) // named type definition in parenthesis
int ARPRegister(PNDIS_STRING Adapter, uint *Flags,
                struct ARPInterface **Interface);
#pragma warning(pop)
