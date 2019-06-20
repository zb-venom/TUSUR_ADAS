/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   request.h
 * Author: касымхан
 *
 * Created on 18 мая 2019 г., 7:31
 */
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   request.h
 * Author: касымхан
 *
 * Created on 18 мая 2019 г., 7:25
 */
#include "../w5100.h"
#include "../Ethernet.h"
#include "../Dhcp.h"
#include "Arduino_w.h"

#ifndef REQUEST_H
#define REQUEST_H

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
class Ethernet_request{
public:
    bool intialazion_server(byte user_device[16],byte user_login [16]);
};
#endif

#endif /* REQUEST_H */

