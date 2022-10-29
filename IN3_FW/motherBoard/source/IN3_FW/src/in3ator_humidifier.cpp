/*

	Arduino library for in3ator humidifier.

	MIT License

	Copyright (c) 2020 Medical Open World, Pablo Sánchez Bergasa

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

*/

#include "in3ator_humidifier.h"

void MAM_in3ator_Humidifier::_read(in3atorHum_param_t param, uint16_t *val) {
    _i2c->beginTransmission(_i2c_addr);
    _i2c->write(param); // parameter
    _i2c->endTransmission();

    _i2c->requestFrom(_i2c_addr, (uint8_t)2);

    if (_i2c->available())
    {
      *val = ((_i2c->read() << 8) | _i2c->read());
    }
}

void MAM_in3ator_Humidifier::_write(in3atorHum_param_t param, uint16_t *val) {
    _i2c->beginTransmission(_i2c_addr);
    _i2c->write(param);                 // parameter
    _i2c->write((*val >> 8) & 0xFF); // Upper 8-bits
    _i2c->write(*val & 0xFF);        // Lower 8-bits
    _i2c->endTransmission();
}

void MAM_in3ator_Humidifier::begin(TwoWire *theWire) {
    _i2c = theWire;
    _i2c->begin();
}

uint16_t MAM_in3ator_Humidifier::getParam(in3atorHum_param_t param){
    uint16_t val = 0;
    _read(param, &val);
    return val;
}

void MAM_in3ator_Humidifier::reset(){

}

void MAM_in3ator_Humidifier::turn(uint16_t mode)
{
int16_t val = false;
if(mode){
	val=true;
}
_write(IN3ATOR_HUM_ON, (uint16_t*)&val);
}
