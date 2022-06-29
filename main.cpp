//gebruik gemaakt van de RC522 library van Bas van der Geer (2019) onder de Boost Software License, Version 1.0.

#include "hwlib.hpp"
#include "MFRC522.hpp"
#include "DS1307.hpp"

void biepen_goed(hwlib::target::pin_out & bieper_pin){
    bieper_pin.write( 1 );
    hwlib::wait_ms(500);
    bieper_pin.write(0);
}

void biepen_start(hwlib::target::pin_out & bieper_pin){
    for (int x = 0; x < 3; x++){
        bieper_pin.write( 1 );
        hwlib::wait_ms(500);
        bieper_pin.write(0);
        hwlib::wait_ms(500);
    }
    bieper_pin.write( 1 );
    hwlib::wait_ms(1000);
    bieper_pin.write(0);
}

int main(){
    //spi variabelen
    auto miso = hwlib::target::pin_in(hwlib::target::pins::d11);
    auto sclk = hwlib::target::pin_out(hwlib::target::pins::d9);
    auto ss = hwlib::target::pin_out(hwlib::target::pins::d8);
    auto mosi = hwlib::target::pin_out(hwlib::target::pins::d10);
    auto reset = hwlib::target::pin_out(hwlib::target::pins::d12);
    spiSetup spibus(sclk, mosi, miso);
    MFRC522 rfid(spibus, ss, reset);
    //opstarten RC522 kaartlezer
    rfid.initialize(); 
    
    //i2c variabelen
    auto scl = hwlib::target::pin_oc(hwlib::target::pins::scl);
    auto sda = hwlib::target::pin_oc(hwlib::target::pins::sda);
    hwlib::i2c_bus_bit_banged_scl_sda bus(scl, sda);
    DS1307 rtc(bus);

    //restvariabelen
    uint8_t UID[5] = {0x00};
    uint8_t receivedData[64];
    uint8_t * data_rtc;
    
    
    auto switch_select = hwlib::target::pin_in(hwlib::target::pins::d28);
    auto bieper_pin = hwlib::target::pin_out(hwlib::target::pins::d22);
    auto knop_uitlezen = hwlib::target::pin_in(hwlib::target::pins::d24); //hoog als ingedrukt
    auto knop_start = hwlib::target::pin_in(hwlib::target::pins::d26); //hoog als ingedrukt
    
    for (;;){    
        if (switch_select.read() == 0){
            hwlib::cout << "Postoperatie \n";
            hwlib::cout << "Wachten op nieuwe kaart \n";
            rfid.waitForUID(UID);

            rfid.read_card(); //leest kaart
            //uitlezen DS1307 real-time clock
            data_rtc = rtc.uitlezen_bytes();
            hwlib::cout << "default = "<<UID[0]<<"\n";
            //klaarmaken array om terug te schrijven naar de kaart
            int rtc_data_ptr = 0;
            uint8_t sendData[64]; 
            for (int x = 0; x < 64; x++){
                if (receivedData[x] == UID[0]){
                    if (rtc_data_ptr < 7){
                        sendData[x]=data_rtc[rtc_data_ptr];
                        rtc_data_ptr = rtc_data_ptr + 1;
                    }else{
                        sendData[x]=UID[0];
                    }
                }else{
                    sendData[x]=receivedData[x];
                }
                hwlib::cout<<sendData[x]<<" - ";
            }
            
            rfid.write(sendData, 64); //terugschrijven van array met data
            for (int r = 0; r < 64; r++){
                receivedData[r] = sendData[r];
            }
            hwlib::cout<<"Kaart geschreven!\n";
            biepen_goed(bieper_pin);
            //ontvangen/uitlezen data
            rtc.uitlezen();

            for (int i = 0; i < 64; i++){
                hwlib::cout <<"data: " << receivedData[i] <<"\n";
            }
        }else{
            hwlib::cout << "Basisstation \n";
            hwlib::cout << "Wachten op knop\n";
            while ((knop_start.read() == 0) && (knop_uitlezen.read() == 0)){
                hwlib::wait_ms(500);
            }
            if (knop_start.read() == 1){
                hwlib::cout << "Start\n";
                hwlib::cout << "Wachten op kaart \n";
                rfid.waitForUID(UID);
                uint8_t sendData[64]; 
                for (int x = 0; x < 64; x++){
                    sendData[x]=UID[0];
                }
                rfid.write(sendData, 64);
                for (int r = 0; r < 64; r++){
                    receivedData[r] = sendData[r];
                }
                biepen_start(bieper_pin);
                hwlib::cout << "START! \n";
            
            }else if (knop_uitlezen.read() == 1){
                hwlib::cout << "Uitlezen\n";
                hwlib::cout << "Wachten op nieuwe kaart \n";
                rfid.waitForUID(UID);
                for (int i = 0; i < 64; i++){
                    hwlib::cout <<"data: " << receivedData[i] <<"\n";
                }
        
            }else{
                hwlib::cout << "Er is iets fout gegaan!";
            }
        }
        hwlib::wait_ms(1500);
    }
}