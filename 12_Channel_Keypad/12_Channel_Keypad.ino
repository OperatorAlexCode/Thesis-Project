/*  12_Channel_Keypad.ino
/*
 *  
 * Copyright (c) 2019 Seeed Technology Co., Ltd.
 * Website    : www.seeed.cc
 * Create Time: February 2019
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <Arduino.h>

void setup() {
	Serial.begin(9600); 

    Serial1.begin(9600);  // start serial for output
}

void loop() {
    printData();
}

/*
* data mapping:E1---1；E2---2；E3---3；E4---4；E5---5；E6---6；
*              E7---7；E8---8；E9---9；EA---*；EB---0；EC---#；
*/
void printData() {
    while(Serial1.available()) {
        uint8_t data = Serial1.read();
        switch(data) {
                case 0xE1 :
                    Serial1.println("1");
                    break;
                case 0xE2 :
                    Serial1.println("2");
                    break;
                case 0xE3 :
                    Serial1.println("3");
                    break;
                case 0xE4 :
                    Serial1.println("4");
                    break;
                case 0xE5 :
                    Serial1.println("5");
                    break;
                case 0xE6 :
                    Serial1.println("6");
                    break;
                case 0xE7 :
                    Serial1.println("7");
                    break;
                case 0xE8 :
                    Serial1.println("8");
                    break;
                case 0xE9 :
                    Serial1.println("9");
                    break;
                case 0xEA :
                    Serial1.println("*");
                    break;
                case 0xEB :
                    Serial1.println("0");
                    break;
                case 0xEC :
                    Serial1.println("#");
                    break;
                default:
                    break;
            }
    }

}


