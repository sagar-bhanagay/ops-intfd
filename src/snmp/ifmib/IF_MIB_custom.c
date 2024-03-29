#include "openswitch-idl.h"
#include "ovsdb-idl.h"
#include "vswitch-idl.h"
#include "openvswitch/vlog.h"
#include "IF_MIB_custom.h"
#include "IF_MIB_scalars_ovsdb_get.h"
#include <netinet/ether.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>


static char *interface_statistics_keys [] = {
        "rx_packets",
        "rx_bytes",
        "tx_packets",
        "tx_bytes",
        "rx_dropped",
        "rx_frame_err",
        "rx_over_err",
        "rx_crc_err",
        "rx_errors",
        "tx_dropped",
        "collisions",
        "tx_errors"
};

int ifTable_skip_function(const struct ovsdb_idl *idl,
                          const struct ovsrec_interface *interface_row) {
    if(strchr(interface_row->name, '-') != NULL)
        return 1;
    if (atoi(interface_row->name) == 0)
        return 1;
    return 0;
}


int ifXTable_skip_function(const struct ovsdb_idl *idl,
                           const struct ovsrec_interface *interface_row) {
    if(strchr(interface_row->name, '-') != NULL)
        return 1;
    if (atoi(interface_row->name) == 0)
        return 1;
    return 0;
}


void ifTableifIndex_custom_function(const struct ovsdb_idl *idl,
                                    const struct ovsrec_interface *interface_row,
                                    long *ifIndex_val_ptr) {
    char * temp = interface_row->name;
    *ifIndex_val_ptr = atoi(temp);
}


void ifMtu_custom_function(const struct ovsdb_idl *idl,
                           const struct ovsrec_interface *interface_row,
                           long* ifMtu_val_ptr){
    *ifMtu_val_ptr = (long) interface_row->mtu;
}

void ifAdminStatus_custom_function(const struct ovsdb_idl *idl,
                                   const struct ovsrec_interface *interface_row,
                                   long *ifAdminStatus_val_ptr) {
    if (strncmp(interface_row->admin_state,
                OVSREC_INTERFACE_ADMIN_STATE_UP,
                MAX_ADMIN_STATE_LENGTH) == 0 ) {
        *ifAdminStatus_val_ptr = 1;
    }
    else if (strncmp(interface_row->admin_state,
                     OVSREC_INTERFACE_ADMIN_STATE_DOWN,
                     MAX_ADMIN_STATE_LENGTH) == 0 ) {
        *ifAdminStatus_val_ptr = 2;
    }
    else if (strncmp(interface_row->admin_state, "testing",
                     MAX_ADMIN_STATE_LENGTH) == 0 ) {
        *ifAdminStatus_val_ptr = 3;
    }
}


void ifOperStatus_custom_function(const struct ovsdb_idl *idl,
                                  const struct ovsrec_interface *interface_row,
                                  long *ifOperStatus_val_ptr) {
    if (strncmp(interface_row->admin_state,
                OVSREC_INTERFACE_ADMIN_STATE_UP,
                MAX_ADMIN_STATE_LENGTH) == 0 ) {
        *ifOperStatus_val_ptr = 1;
    }
    else if (strncmp(interface_row->admin_state,
                     OVSREC_INTERFACE_ADMIN_STATE_DOWN,
                     MAX_ADMIN_STATE_LENGTH) == 0 ) {
        *ifOperStatus_val_ptr = 2;
    }
    else if (strncmp(interface_row->admin_state,
                     "testing", MAX_ADMIN_STATE_LENGTH) == 0 ) {
        *ifOperStatus_val_ptr = 3;
    }
}


void ifPhysAddress_custom_function(const struct ovsdb_idl *idl,
                                   const struct ovsrec_interface *interface_row,
                                   char *ifPhysAddress_val_ptr,
                                   size_t *ifPhysAddress_val_ptr_len) {

    char *key = interface_row->mac_in_use ;
    char hexbyte[3] = {0} ;
    int octets[(strlen(key)+1) / 3] ;
    char hexstg[7];

    int d = 0, i = 0;

    for( d = 0; d < strlen(key); d += 3 )
    {
        hexbyte[0] = key[d] ;
        hexbyte[1] = key[d+1] ;

        sscanf( hexbyte, "%X", &octets[d/3] ) ;

        hexstg[i++] = octets[d/3];
    }
    hexstg[i] = '\0';


    *ifPhysAddress_val_ptr_len = MAC_ADDRESS_OCTATES;
    memcpy(ifPhysAddress_val_ptr,hexstg, MAC_ADDRESS_OCTATES+1);

}

void ifInOctets_custom_function(const struct ovsdb_idl *idl,
                                const struct ovsrec_interface *interface_row,
                                unsigned long *ifInOctets_val_ptr){
    const struct ovsdb_datum *datum;
    union ovsdb_atom atom;
    unsigned int index;

    datum = ovsrec_interface_get_statistics(interface_row,
                OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    if (NULL==datum)
        *ifInOctets_val_ptr = 0;
    else {
        atom.string = interface_statistics_keys[1];
        index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
        *ifInOctets_val_ptr = (index == UINT_MAX)? 0 :
                               datum->values[index].integer;
    }
}

void ifInUcastPkts_custom_function(const struct ovsdb_idl *idl,
                                   const struct ovsrec_interface *interface_row,
                                   unsigned long *ifInUcastPkts_val_ptr){
    const struct ovsdb_datum *datum;
    union ovsdb_atom atom;
    unsigned int index;

    datum = ovsrec_interface_get_statistics(interface_row,
                 OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    if (NULL==datum)
        *ifInUcastPkts_val_ptr = 0;
    else {
        atom.string = interface_statistics_keys[0];
        index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
        *ifInUcastPkts_val_ptr = (index == UINT_MAX)? 0 :
                                  datum->values[index].integer;
    }

}

void ifInNUcastPkts_custom_function(const struct ovsdb_idl *idl,
                                    const struct ovsrec_interface *interface_row,
                                    unsigned long *ifInNUcastPkts_val_ptr){
    *ifInNUcastPkts_val_ptr = 0;
}

void ifInDiscards_custom_function(const struct ovsdb_idl *idl,
                                  const struct ovsrec_interface *interface_row,
                                  unsigned long *ifInDiscards_val_ptr){
    const struct ovsdb_datum *datum;
    union ovsdb_atom atom;
    unsigned int index;

    datum = ovsrec_interface_get_statistics(interface_row,
                OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    if (NULL==datum)
        *ifInDiscards_val_ptr = 0;
    else {
        atom.string = interface_statistics_keys[4];
        index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
        *ifInDiscards_val_ptr = (index == UINT_MAX)? 0 :
                                 datum->values[index].integer;
    }


}

void ifInErrors_custom_function(const struct ovsdb_idl *idl,
                                const struct ovsrec_interface *interface_row,
                                unsigned long *ifInErrors_val_ptr){
    const struct ovsdb_datum *datum;
    union ovsdb_atom atom;
    unsigned int index;

    datum = ovsrec_interface_get_statistics(interface_row,
                OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    if (NULL==datum)
        *ifInErrors_val_ptr = 0;
    else {
        atom.string = interface_statistics_keys[8];
        index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
        *ifInErrors_val_ptr = (index == UINT_MAX)? 0 :
                               datum->values[index].integer;
    }


}

void ifInUnknownProtos_custom_function(
    const struct ovsdb_idl *idl,
    const struct ovsrec_interface *interface_row,
    unsigned long * ifInUnknownProtos_val_ptr){
    *ifInUnknownProtos_val_ptr = 0;
}

void ifOutOctets_custom_function(const struct ovsdb_idl *idl,
                                 const struct ovsrec_interface *interface_row,
                                 unsigned long *ifOutOctets_val_ptr){
    const struct ovsdb_datum *datum;
    union ovsdb_atom atom;
    unsigned int index;

    datum = ovsrec_interface_get_statistics(interface_row,
                OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    if (NULL==datum)
        *ifOutOctets_val_ptr = 0;
    else {
        atom.string = interface_statistics_keys[3];
        index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
        *ifOutOctets_val_ptr = (index == UINT_MAX)? 0 :
                                datum->values[index].integer;
    }



}

void ifOutUcastPkts_custom_function(const struct ovsdb_idl *idl,
                                    const struct ovsrec_interface *interface_row,
                                    unsigned long *ifOutUcastPkts_val_ptr){
    const struct ovsdb_datum *datum;
    union ovsdb_atom atom;
    unsigned int index;

    datum = ovsrec_interface_get_statistics(interface_row,
                OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    if (NULL==datum)
        *ifOutUcastPkts_val_ptr = 0;
    else {
        atom.string = interface_statistics_keys[2];
        index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
        *ifOutUcastPkts_val_ptr = (index == UINT_MAX)? 0 :
                                   datum->values[index].integer;
    }



}

void ifOutNUcastPkts_custom_function(const struct ovsdb_idl *idl,
                                     const struct ovsrec_interface *interface_row,
                                     unsigned long *ifOutNUcastPkts_val_ptr){
    *ifOutNUcastPkts_val_ptr = 0;
}

void ifOutDiscards_custom_function(const struct ovsdb_idl *idl,
                                   const struct ovsrec_interface *interface_row,
                                   unsigned long *ifOutDiscards_val_ptr){
    const struct ovsdb_datum *datum;
    union ovsdb_atom atom;
    unsigned int index;

    datum = ovsrec_interface_get_statistics(interface_row,
                OVSDB_TYPE_STRING,
                OVSDB_TYPE_INTEGER);
    if (NULL==datum)
        *ifOutDiscards_val_ptr = 0;
    else {
        atom.string = interface_statistics_keys[9];
        index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
        *ifOutDiscards_val_ptr = (index == UINT_MAX)? 0 :
                                  datum->values[index].integer;
    }


}

void ifOutErrors_custom_function(const struct ovsdb_idl *idl,
                                 const struct ovsrec_interface *interface_row,
                                 unsigned long *ifOutErrors_val_ptr){
    const struct ovsdb_datum *datum;
    union ovsdb_atom atom;
    unsigned int index;

    datum = ovsrec_interface_get_statistics(interface_row,
                OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    if (NULL==datum)
        *ifOutErrors_val_ptr = 0;
    else {
        atom.string = interface_statistics_keys[11];
        index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
        *ifOutErrors_val_ptr = (index == UINT_MAX)? 0 :
                                datum->values[index].integer;
    }
}


void ifInMulticastPkts_custom_function(const struct ovsdb_idl *idl,
                                       const struct ovsrec_interface *interface_row,
                                       unsigned long *ifInMulticastPkts_val_ptr){
    *ifInMulticastPkts_val_ptr = 0;
}

void ifInBroadcastPkts_custom_function(
    const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    unsigned long *ifInBroadcastPkts_val_ptr){
    *ifInBroadcastPkts_val_ptr = 0;
}

void ifOutMulticastPkts_custom_function(
    const struct ovsdb_idl *idl, const struct ovsrec_interface *interface_row,
    unsigned long *ifOutMulticastPkts_val_ptr){
    *ifOutMulticastPkts_val_ptr = 0;
}

void ifOutBroadcastPkts_custom_function(
    const struct ovsdb_idl *idl,
    const struct ovsrec_interface *interface_row,
    unsigned long *ifOutBroadcastPkts_val_ptr){
    *ifOutBroadcastPkts_val_ptr = 0;
}

void ifHCInOctets_custom_function(
    const struct ovsdb_idl *idl,
    const struct ovsrec_interface *interface_row,
    unsigned long long *ifHCInOctets_val_ptr){
    const struct ovsdb_datum *datum;
    union ovsdb_atom atom;
    unsigned int index;

    datum = ovsrec_interface_get_statistics(interface_row,
                OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    if (NULL==datum)
        *ifHCInOctets_val_ptr = 0;
    else {
        atom.string = interface_statistics_keys[1];
        index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
        *ifHCInOctets_val_ptr = (index == UINT_MAX)? 0 :
                                 datum->values[index].integer;
    }

}

void ifHCInUcastPkts_custom_function(
    const struct ovsdb_idl *idl,
    const struct ovsrec_interface *interface_row,
    unsigned long long *ifHCInUcastPkts_val_ptr){
    const struct ovsdb_datum *datum;
    union ovsdb_atom atom;
    unsigned int index;

    datum = ovsrec_interface_get_statistics(interface_row, OVSDB_TYPE_STRING,
                                            OVSDB_TYPE_INTEGER);
    if (NULL==datum)
        *ifHCInUcastPkts_val_ptr = 0;
    else {
        atom.string = interface_statistics_keys[0];
        index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
        *ifHCInUcastPkts_val_ptr = (index == UINT_MAX)? 0 :
                                    datum->values[index].integer;
    }
}

void ifHCInMulticastPkts_custom_function(
    const struct ovsdb_idl *idl,
    const struct ovsrec_interface *interface_row,
    unsigned long long *ifHCInMulticastPkts_val_ptr){
    *ifHCInMulticastPkts_val_ptr = 0;
}

void ifHCInBroadcastPkts_custom_function(
    const struct ovsdb_idl *idl,
    const struct ovsrec_interface *interface_row,
    unsigned long long *ifHCInBroadcastPkts_val_ptr){
    *ifHCInBroadcastPkts_val_ptr = 0;
}

void ifHCOutOctets_custom_function(
    const struct ovsdb_idl *idl,
    const struct ovsrec_interface *interface_row,
    unsigned long long *ifHCOutOctets_val_ptr){
    const struct ovsdb_datum *datum;
    union ovsdb_atom atom;
    unsigned int index;

    datum = ovsrec_interface_get_statistics(interface_row,
                OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    if (NULL==datum)
        *ifHCOutOctets_val_ptr = 0;
    else {
        atom.string = interface_statistics_keys[3];
        index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
        *ifHCOutOctets_val_ptr = (index == UINT_MAX)? 0 :
                                  datum->values[index].integer;
    }

}

void ifHCOutUcastPkts_custom_function(
    const struct ovsdb_idl *idl,
    const struct ovsrec_interface *interface_row,
    unsigned long long *ifHCOutUcastPkts_val_ptr){
    const struct ovsdb_datum *datum;
    union ovsdb_atom atom;
    unsigned int index;

    datum = ovsrec_interface_get_statistics(interface_row,
                OVSDB_TYPE_STRING, OVSDB_TYPE_INTEGER);
    if (NULL==datum)
        *ifHCOutUcastPkts_val_ptr = 0;
    else {
        atom.string = interface_statistics_keys[2];
        index = ovsdb_datum_find_key(datum, &atom, OVSDB_TYPE_STRING);
        *ifHCOutUcastPkts_val_ptr = (index == UINT_MAX)? 0 :
                                     datum->values[index].integer;
    }

}

void ifHCOutMulticastPkts_custom_function(
    const struct ovsdb_idl *idl,
    const struct ovsrec_interface *interface_row,
    unsigned long long *ifHCOutMulticastPkts_val_ptr){
    *ifHCOutMulticastPkts_val_ptr = 0;
}

void ifHCOutBroadcastPkts_custom_function(
    const struct ovsdb_idl *idl,
    const struct ovsrec_interface *interface_row,
     unsigned long long *ifHCOutBroadcastPkts_val_ptr){
    *ifHCOutBroadcastPkts_val_ptr = 0;
}

void ifLinkUpDownTrapEnable_custom_function(
    const struct ovsdb_idl *idl,
    const struct ovsrec_interface *interface_row,
    long *ifLinkUpDownTrapEnable_val_ptr){
    *ifLinkUpDownTrapEnable_val_ptr = 0;
}


void ifConnectorPresent_custom_function(
    const struct ovsdb_idl *idl,
    const struct ovsrec_interface *interface_row,
    long *ifConnectorPresent_val_ptr){
    if(smap_get(&interface_row->pm_info, "connector") != NULL)
        *ifConnectorPresent_val_ptr = 1;
    else
        *ifConnectorPresent_val_ptr = 2;
}
