#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <endian.h>
#include <iostream>
#include <string>
#include "datagram.h"





void Datagram::clear_datagram(char* datagram) {
    memset(datagram, 0, DATAGRAM_SIZE);
    next_free_byte = 0;
    recv_length = 0;
}


void Datagram::pack_next_int8_bit_value_buffer(char* datagram, int8_t v) {
    datagram[next_free_byte] = ((uint8_t*)&v)[0];
    next_free_byte++;
}

void Datagram::pack_next_uint32_bit_value_buffer(char* datagram, uint32_t v) {
    uint32_t net_order = htobe32(v);
    for (int i=0; i<4 ;++i) {
        datagram[next_free_byte] = ((uint8_t*)&net_order)[i];
        next_free_byte++;
    }
}

void Datagram::pack_next_uint64_bit_value_buffer(char* datagram, uint64_t v) {
    uint64_t net_order = htobe64(v);
    for (int i=0; i<8 ;++i) {
        datagram[next_free_byte] = ((uint8_t*)&net_order)[i];
        next_free_byte++;
    }
}


void Datagram::pack_next_string_buffer(char* datagram, string s) {
    //mempcpy(datagram_buffer+next_free_byte, s.c_str(), s.length());
    cout<<s<<endl;
    for(int i = 0; i < s.length(); i++) {
        datagram[next_free_byte] = (char)s[i];
        next_free_byte++;
    }
    cout<<"next "<<next_free_byte<<endl;
}


void Datagram::get_int8_bit_value_fbuffer(const char* datagram, int8_t& v, int start_in_datagram) {
    uint8_t a[1];
    mempcpy(a, datagram + start_in_datagram, sizeof(uint8_t));
    v = *((uint8_t*) a);
}

void Datagram::get_int32_bit_value_fbuffer(const char* datagram, uint32_t& v, int start_in_datagram) {
    uint8_t a[4];
    mempcpy(a, datagram + start_in_datagram, sizeof(uint32_t));
    v = *((uint32_t*) a);
    v = be32toh(v);
}

void Datagram::get_int64_bit_value_fbuffer(const char* datagram, uint64_t& v, int start_in_datagram) {
    uint8_t a[8];
    mempcpy(a, datagram + start_in_datagram, sizeof(uint64_t));
    v = *((uint64_t*) a);
    v = be64toh(v);
}

void Datagram::get_string_fbuffer(const char* datagram, string& s, int start_in_datagram, int end_in_datagram) {
    size_t s_len = size_t (end_in_datagram - start_in_datagram + 1);
    char recv[s_len];
    mempcpy(recv, datagram + start_in_datagram, s_len);
    s = string(recv);
}




void Client_datagram::create_datagram_for_server(char* datagram,
                                                  const uint64_t &session_id, const int8_t &turn_direction,
                                                  const uint32_t &next_expected_event_no, const string& player_name) {
    clear_datagram(datagram);
    pack_next_uint64_bit_value_buffer(datagram, session_id);
    pack_next_int8_bit_value_buffer(datagram, turn_direction);
    pack_next_uint32_bit_value_buffer(datagram, next_expected_event_no);
    pack_next_string_buffer(datagram, player_name);
}




bool Client_datagram::read_datagram_from_client(const char* datagram,
                                                uint64_t& session_id, int8_t& turn_direction,
                                                uint32_t& next_expected_event_no, string& player_name) {
    if(recv_length < MIN_CLIENT_DATAGRAM_SIZE)
        return false;
    get_int64_bit_value_fbuffer(datagram, session_id, SESSION_ID_POS);
    get_int8_bit_value_fbuffer(datagram, turn_direction, TURN_DIRECTION_POS);
    get_int32_bit_value_fbuffer(datagram, next_expected_event_no, NEXT_EXP_EVENT_NUMBER_POS);
    get_string_fbuffer(datagram, player_name, PLAYER_NAME_POS, recv_length-1);

    if(player_name.length() > MAX_PLAYER_NAME_LEN)
        return false;

    for(char c : player_name){
        if(c < MIN_VALID_PLAYER_NAME_SYMBOL ||
           c > MAX_VALID_PLAYER_NAME_SYMBOL) {
            return false;
        }
    }

    return true;
}


bool Server_datagram::read_datagram_from_client(const char* datagram,
                                                uint64_t& session_id, int8_t& turn_direction,
                                                uint32_t& next_expected_event_no, string& player_name) {
    if(recv_length < MIN_CLIENT_DATAGRAM_SIZE)
        return false;
    get_int64_bit_value_fbuffer(datagram, session_id, SESSION_ID_POS);
    get_int8_bit_value_fbuffer(datagram, turn_direction, TURN_DIRECTION_POS);
    get_int32_bit_value_fbuffer(datagram, next_expected_event_no, NEXT_EXP_EVENT_NUMBER_POS);
    get_string_fbuffer(datagram, player_name, PLAYER_NAME_POS, recv_length-1);

    if(player_name.length() > MAX_PLAYER_NAME_LEN)
        return false;

    for(char c : player_name){
        if(c < MIN_VALID_PLAYER_NAME_SYMBOL ||
                c > MAX_VALID_PLAYER_NAME_SYMBOL) {
            return false;
        }
    }

    return true;
}
