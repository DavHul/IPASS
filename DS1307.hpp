//Copyright David Hulsebosch 2022.
// Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE_1_0.txt or copy at
//https://www.boost.org/LICENSE_1_0.txt)

#ifndef DS1307_HPP
#define DS1307_HPP
#include <hwlib.hpp>

/// @file

/// \brief
/// Library for the DS1307 Real Time Clock
/// \details
/// This is the library for controlling and reading the DS1307 RTC (Real Time Clock). It uses the library hwlib made by (c) Wouter van Ooijen (wouter@voti.nl) 2017.
/// Without hwlib this library does not work. 

class DS1307{
protected:
     /// @brief
    /// All the registers of the DS1307 Chip
    /// @detail
    /// All the registers have there right address.
    hwlib::i2c_bus_bit_banged_scl_sda & bus; ///@brief Software i2c bus using hwlib.
    uint_fast8_t adres_secondes = 0x00; ///@brief Register address for seconds.
    uint_fast8_t adres_minuten = 0x01; ///@brief Register address for minutes.
    uint_fast8_t adres_uren = 0x02; /// @brief Register address for hours.
    uint_fast8_t adres_dagen_week = 0x03; ///@brief Register address for daynames.
    uint_fast8_t adres_dagen_getal = 0x04; /// @brief Register address for daynumbers.
    uint_fast8_t adres_maanden = 0x05; ///@brief Register address for months.
    uint_fast8_t adres_jaren = 0x06; ///@brief Register address for years.
    uint_fast8_t adres_control = 0x07; /// @brief Register address for controlling the SQW/OUT pin.
    uint_fast8_t adres = 0x68; ///@brief 7 bit address \details Total write adres is 0xD0 and total read adres is 0xD1
    
    uint8_t secondes;
    uint8_t minuten;
    uint8_t uren;
    int uren_modus; ///@brief Stores hour mode. \details If it is an 0 then it is AM, if it is an 1 it is PM. if it is an 2 then it is in the 24 hour mode
    uint8_t dag_nummer;
    uint8_t dag_getal;
    uint8_t maand;
    uint8_t jaren;
    
    /// \brief   
    /// Decimal to BCD
    /// \details
    /// This function converts decimal integers to the binary-coded decimal (BCD) format
    uint8_t dec2bcd(uint8_t getal){
    return ((getal/10 * 16) + (getal % 10));
}

    /// \brief   
    /// BCD to decimal
    /// \details
    /// This function converts from the binary-coded decimal (BCD) format to decimal integers
    uint8_t bcd2dec(uint8_t getal){
    return ((getal/16 * 10) + (getal % 16));
}
    
    /// \brief   
    /// Read seconds
    /// \details
    /// This function reads the seconds and returns an 8 bit long unsigned integer. Range from 0 to 59
    uint8_t lezen_secondes(){
    { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
        wtrans.write(adres_secondes);}
    { hwlib::i2c_read_transaction rtrans = ((hwlib::i2c_bus*)(&bus))->read(adres);
        rtrans.read(secondes);}
    if ((secondes >> 7 & 0x01)==1){ //als oscilator uitstaat kan je nog steeds de secondes aflezen door het meest linker bit op 0 te zetten
        secondes = secondes ^ 0x80; //meest linker bit op 0 te zetten
    }
    return bcd2dec(secondes);
}

    /// \brief   
    /// Read minutes
    /// \details
    /// This function reads the minutes and returns an 8 bit long unsigned integer. Range from 0 to 59
    uint8_t lezen_minuten(){
    { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
        wtrans.write(adres_minuten);}
    { hwlib::i2c_read_transaction rtrans = ((hwlib::i2c_bus*)(&bus))->read(adres);
        rtrans.read(minuten);}
    return bcd2dec(minuten);
}

    /// \brief   
    /// Read hours
    /// \details
    /// This function reads the hours and returns an 8 bit long unsigned integer. Range from 0 to 23
    uint8_t lezen_uren(){
    { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
        wtrans.write(adres_uren);}
    { hwlib::i2c_read_transaction rtrans = ((hwlib::i2c_bus*)(&bus))->read(adres);
        rtrans.read(uren);}
    if ((uren >> 6 & 0x01)==1){ //controleren of de uren in een 12 uurs format zit en dus of bit 5 AM/PM is
        if ((uren >> 5 & 0x01)==1){ //controleren of het PM is
            uren_modus = 1;
            uren = uren ^ 0x60;
        }else{
            uren_modus = 0;
            uren = uren ^ 0x40;
        }
        
    }else{
        uren_modus = 2;
    }
    
    return bcd2dec(uren);
}

    /// \brief   
    /// Read dayname
    /// \details
    /// This function reads the daynames and returns the number (0 for sunday, 1 for monday etc.). Range from 0 to 6
    uint8_t lezen_dagnaam(){
    { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
    wtrans.write(adres_dagen_week);}
    { hwlib::i2c_read_transaction rtrans = ((hwlib::i2c_bus*)(&bus))->read(adres);
        rtrans.read(dag_nummer);}
    return dag_nummer;
}

    /// \brief   
    /// Read day number
    /// \details
    /// This function reads the day number and returns an 8 bit long unsigned integer. Range from 1 to 31
    uint8_t lezen_daggetal(){
    { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
        wtrans.write(adres_dagen_getal);}
    { hwlib::i2c_read_transaction rtrans = ((hwlib::i2c_bus*)(&bus))->read(adres);
        rtrans.read(dag_getal);}
    return bcd2dec(dag_getal);
}

    /// \brief   
    /// Read month number
    /// \details
    /// This function reads the month number and returns an 8 bit long unsigned integer. Range from 1 to 12
    uint8_t lezen_maand(){
    { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
        wtrans.write(adres_maanden);}
    { hwlib::i2c_read_transaction rtrans = ((hwlib::i2c_bus*)(&bus))->read(adres);
        rtrans.read(maand);}
    return bcd2dec(maand);
}

    /// \brief   
    /// Read year number
    /// \details
    /// This function reads the year number and returns an 8 bit long unsigned integer. 
    /// Don't forget to add 1952 to the return value of this function to get the right year
    uint8_t lezen_jaar(){
    { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
        wtrans.write(adres_jaren);}
    { hwlib::i2c_read_transaction rtrans = ((hwlib::i2c_bus*)(&bus))->read(adres);
        rtrans.read(jaren);}
    return bcd2dec(jaren);
}

public:
    /// \brief   
    /// Constructor
    /// \details
    /// Constructs the class. It needs an i2c_bus_bit_banged from the library hwlib in order to work. 
    DS1307(hwlib::i2c_bus_bit_banged_scl_sda & bus):
    bus(bus){}
    
    /// \brief   
    /// Read date and time in string
    /// \details
    /// Reads the date and time and returns it in an string.
    /// The string's format is: dayname daynumber/monthnumber/year hour:minutes:seconds
    void uitlezen(){
        switch (lezen_dagnaam()){
            case 1: hwlib::cout << "Zondag "; break;
            case 2: hwlib::cout << "Maandag "; break;
            case 3: hwlib::cout << "Dinsdag "; break;
            case 4: hwlib::cout << "Woensdag "; break;
            case 5: hwlib::cout << "Donderdag "; break;
            case 6: hwlib::cout << "Vrijdag "; break;
            case 7: hwlib::cout << "Zaterdag "; break;
        }
        hwlib::cout << lezen_daggetal() << "/";
        hwlib::cout << lezen_maand() << "/";
        hwlib::cout << lezen_jaar()+1952 << " ";
        
        lezen_uren();
        if (uren_modus == 0){
            hwlib::cout << uren << " AM";  
        }else if (uren_modus == 1){
            hwlib::cout << uren << " PM";
        }else{
            hwlib::cout << uren;
        }
        hwlib::cout << ":";
        hwlib::cout << lezen_minuten() << ":";
        hwlib::cout << lezen_secondes() << "\n";
    }

    /// \brief   
    /// Read date and time in array
    /// \details
    /// Reads the date and time and returns it in an array.
    /// The array format is {dayname(int), daynumber, monthnumber, year, hour, minutes, seconds}
    uint8_t * uitlezen_bytes(){ //add 1952 to data[3] to get actual year
        static uint8_t data[7];
        data[0] = lezen_dagnaam();
        data[1] = lezen_daggetal();
        data[2] = lezen_maand();
        data[3] = lezen_jaar();
        data[4] = lezen_uren();
        data[5] = lezen_minuten();
        data[6] = lezen_secondes();
        return data;
    }

    /// \brief   
    /// Fuction to turn on the oscilator
    /// \details
    /// Turns on the oscilator, if it is already on then it gives a message through the terminal
    void aanzetten_oscilator(){
    { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
        wtrans.write(adres_secondes);}
    { hwlib::i2c_read_transaction rtrans = ((hwlib::i2c_bus*)(&bus))->read(adres);
        rtrans.read(secondes);}

    if ((secondes >> 7 & 0x01)==1){ //als oscilator uitstaat kan je nog steeds de secondes aflezen door het meest linker bit op 0 te zetten
        uint8_t code = secondes ^ 0x80; //zorgt voor behoud aantal seconde
        { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
        wtrans.write(adres_secondes);
        wtrans.write(code);}
    }else{
        hwlib::cout << "Oscilator staat al aan! \n";
    }
}
    
    /// \brief   
    /// Fuction to turn off the oscilator
    /// \details
    /// Turns the oscilator off, if it is already off then it gives a message through the terminal
    void uitzetten_oscilator(){
        { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
            wtrans.write(adres_secondes);}
        { hwlib::i2c_read_transaction rtrans = ((hwlib::i2c_bus*)(&bus))->read(adres);
            rtrans.read(secondes);}
            
        if ((secondes >> 7 & 0x01)==0){ //als oscilator uitstaat kan je nog steeds de secondes aflezen door het meest linker bit op 0 te zetten
            uint8_t code = secondes ^ 0x80; //zorgt voor behoud aantal seconde
            { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
            wtrans.write(adres_secondes);
            wtrans.write(code);}
        }else{
            hwlib::cout << "Oscilator staat al uit! \n";
        }
    }
    
    /// \brief   
    /// Fuction to control the SQW/out pin
    /// \details
    /// Controls the SQW/out pin. The parameter output_control controls the pin when square_wave is disabled. The parameter square_wave turns the oscilator output on or off. 
    /// The rate_select parameter controls the rate. If rate_select is 0 then the rate is 1 Hz; if it is 1 then the rate is 4.096 kHz; 2 equals 8.192 kHz and 3 equals 32.768 kHz. 
    void control(const int & output_control, const int & square_wave, const int & rate_select){
        uint8_t pakket = 0x00;
        if (output_control == 1){ //output pin hoog zetten als square wave uit staat
            pakket = pakket | 0x80;
        }
        if (square_wave == 1){ //square wave aanzetten
            pakket = pakket | 0x10;
        }
        switch (rate_select){
            case 0: //1 Hz
                pakket = pakket | 0x00;
                break;
            case 1: //4.096kHz
                pakket = pakket | 0x01;
                break;
            case 2: //8.192kHz
                pakket = pakket | 0x02;
                break;
            case 3: //32.768kHz
                pakket = pakket | 0x03;
                break;
            default:
                hwlib::cout << "Onbekende modus \n";
        }
        { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
            wtrans.write(adres_control);
            wtrans.write(pakket);}
    }
    
    /// \brief   
    /// Fuction to switch between 12 or 24 hour format
    /// \details
    /// This function switches between the 12 hour and the 24 hour format. In the 12 hour format the get_uur function also returns if it is AM or PM by calling the uur_modus variable
    void toggle_12_24(){
        { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
            wtrans.write(adres_uren);}
        { hwlib::i2c_read_transaction rtrans = ((hwlib::i2c_bus*)(&bus))->read(adres);
            rtrans.read(uren);}
        uren = uren ^ 0x40;
        { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
            wtrans.write(adres_uren);
            wtrans.write(uren);}
    }

    
    /// \brief   
    /// Sets date and time
    /// \details
    /// Sets the date and time. Use this once at the beginning of your project and then comment it out. 
    /// The oscilator is turned on in this function and thanks to the battery it will continue to track time even if the power is disconnected
    void set_datetime(int seconde_inv, int minuut_inv, int uur_inv, const int weekdag_inv, const int dag_inv, const int maand_inv, int jaar_inv){
        uitzetten_oscilator();
            
        uint8_t seconde = unsigned(seconde_inv);
        uint8_t minuut = unsigned(minuut_inv); 
        uint8_t uur = unsigned(uur_inv);
        uint8_t weekdag = unsigned(weekdag_inv);
        uint8_t dag = unsigned(dag_inv); 
        uint8_t maand = unsigned(maand_inv); 
        uint8_t jaar = unsigned(jaar_inv);
                    
        seconde = dec2bcd(seconde) | 0x80;
            
        lezen_uren();
        if (uren_modus != 2){
            uren = dec2bcd(uren) ^ 0x20;
            uren_modus = 2;
        } 
            
        { hwlib::i2c_write_transaction wtrans = ((hwlib::i2c_bus*)(&bus))->write(adres);
            wtrans.write(adres_secondes);
            wtrans.write(seconde);
            wtrans.write(dec2bcd(minuut));
            wtrans.write(uur);
            wtrans.write(weekdag);
            wtrans.write(dec2bcd(dag));
            wtrans.write(dec2bcd(maand));
            wtrans.write(dec2bcd(jaar));
        }
        aanzetten_oscilator();
    }


    /// \brief   
    /// Tests the library
    /// \details
    /// Tests the library by getting the current RTC data. Then it sets the date and time to Maandag 1/1/1952 1:1:1. After that it loops for 20 seconds where it displays the data every two seconds
    /// Finally it sets the RTC obtained earlier back to the RTC.
    void test_script(){
        hwlib::cout << "This is a testscript for the library. First it sets the date to Maandag 1/1/1952 1:1:1. Then it prints the date and time every two seconds for 20 seconds. If it doesn't then something is wrong.\n";
        hwlib::cout << "At the end the date and time gets put back to it original state.\n";
        
        uint8_t * data_rtc;
        data_rtc = uitlezen_bytes();
        
        set_datetime(1,1,1,1,1,1,1);
        for (int y = 0; y < 10; y++){
            uitlezen();
            hwlib::wait_ms(2000);
        }
        set_datetime(data_rtc[6], data_rtc[5], data_rtc[4], data_rtc[0], data_rtc[1], data_rtc[2], data_rtc[3]);
    }
    
    uint8_t get_secondes(){
        return lezen_secondes();
    }
};

#endif