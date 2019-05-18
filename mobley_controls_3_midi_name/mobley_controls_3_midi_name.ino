#include <FastLED.h>

#include <Encoder.h>

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability

Encoder myenc0(28, 24);
Encoder myenc5(10, 26);
Encoder myenc7(6, 5);
Encoder myenc3(4, 7);
Encoder myenc4(12, 25);
Encoder myenc1(27, 11);
Encoder myenc6(8, 3);
Encoder myenc2(2, 9);



byte button_pins[8] = {34, 36, 14, 38, 35, 33, 37, 39};
byte switch_pins[2] = {31, 32};
byte led_pins[8] = {18, 16, 20, 21, 17, 19, 23, 22};
byte button_cc[2][8] = {{12, 14, 16, 17, 18, 20, 22, 24}, {12, 15, 16, 17, 19, 21, 23, 25}};
byte enc_cc [2][8] = {{26, 28, 30, 32, 34, 36, 38, 40}, {27, 29, 31, 33, 35, 37, 39, 41}};
byte button_cc_latch[8] = {255, 0, 255, 255, 255, 255, 0, 0};
byte cc_layer, cc_clutch;
byte layer_sw, clutch_sw;
byte bread[8], pbread[8];
int cread[9], pread[9];
int swread[2];
int pswread[2];
int midi_enc[2][8];
int midi_butt[20];
float osc[4];
int osc_latch[4];
byte channel = 1;
byte layerbutt = 13;
float sync_led = 1;
byte sync_c;
byte ccin[2] = {103, 102};
byte ccin_read[2];
byte cc_led[2], cc_color[2];
byte enc_step;
byte enc_step_high=2;
#define LED_PIN  6

#define COLOR_ORDER RGB
#define CHIPSET     WS2812B
CRGB apaleds[ 3];


void setup() {

  FastLED.addLeds<CHIPSET, 29, COLOR_ORDER>(apaleds, 2).setCorrection(TypicalSMD5050);
  apaleds[0].setHSV(180, 255, 0);
  apaleds[1].setHSV(0, 255, 0);
  FastLED.show();

  pinMode(30, OUTPUT);
  pinMode(31, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);


  for (byte f = 0; f < 8; f++) {
    pinMode(button_pins[f], INPUT_PULLUP);
    pinMode(led_pins[f], OUTPUT);
    digitalWrite(led_pins[f], 1);
    delay(100);
    digitalWrite(led_pins[f], 0);
    apaleds[0].setHSV(0, 0, f * 16);
    apaleds[1].setHSV(0, 0, f * 16);
    analogWrite(30, f * 16);

    FastLED.show();
  }
  apaleds[0].setHSV(0, 0, 16);
  apaleds[1].setHSV(0, 0, 16);
  FastLED.show();

}

uint32_t ct, pt[8];

void loop() {

  ct = millis();

  if (ct - pt[1] > 10) {
    pt[1] = ct;

    pread[0] = cread[0];
    cread[0] = myenc0.read();

    pread[1] = cread[1];
    cread[1] = myenc1.read();

    pread[2] = cread[2];
    cread[2] = myenc2.read();

    pread[3] = cread[3];
    cread[3] = myenc3.read();

    pread[4] = cread[4];
    cread[4] = myenc4.read();

    pread[5] = cread[5];
    cread[5] = myenc5.read();

    pread[6] = cread[6];
    cread[6] = myenc6.read();

    pread[7] = cread[7];
    cread[7] = myenc7.read();

    pswread[0] = swread[0];
    swread[0] = digitalRead(switch_pins[0]);
    clutch_sw = !swread[0];

    pswread[1] = swread[1];
    swread[1] = digitalRead(switch_pins[1]);
    layer_sw = !swread[1];
    byte disp_latch = 0;

    if (pswread[0] == 1 && swread[0] == 0) {
      for (byte j = 0; j < 8; j++) {
        digitalWrite(led_pins[j], 0);
      }
      digitalWrite(led_pins[0], 1);

    }

    if (pswread[0] == 0 && swread[0] == 1) {
      for (byte j = 0; j < 8; j++) {
        digitalWrite(led_pins[j], 1);
      }
      disp_latch = 1;
    }

    if (pswread[1] == 0 && swread[1] == 1) {
      for (byte j = 0; j < 8; j++) {
        digitalWrite(led_pins[j], 1);
      }
    }

    if (pswread[1] == 1 && swread[1] == 0) {
      disp_latch = 1;
    }
    if (disp_latch == 1 && layer_sw == 1) {
      for (byte j = 0; j < 8; j++) {
        if (button_cc_latch[j] != 255) {
          digitalWrite(led_pins[j], !button_cc_latch[j]);
        }
        if (button_cc_latch[j] == 255) {
          digitalWrite(led_pins[j], 1);
        }
      }
    }

    for (byte j = 0; j < 8; j++) {
      pbread[j] = bread[j];
      bread[j] = digitalRead(button_pins[j]);
      // Serial.print(j);    Serial.print("-");    Serial.print(bread[j]);    Serial.print("  ");

      if (clutch_sw == 1) {
        enc_step=1;
        if (j == 0) {
          if (pbread[j] == 0 && bread[j] == 1) {
            usbMIDI.sendControlChange(layerbutt, 1, channel);
            digitalWrite(led_pins[0], 0);
          }
          if (pbread[j] == 1 && bread[j] == 0) {
            usbMIDI.sendControlChange(layerbutt, 0, channel);
            digitalWrite(led_pins[0], 1);
          }
        }
      }

      if (clutch_sw == 0) {
        enc_step=enc_step_high;
        if (pbread[j] == 0 && bread[j] == 1) {
          if (layer_sw == 0) {
            usbMIDI.sendControlChange(button_cc[0][j], 1, channel);
            digitalWrite(led_pins[j], 0);

          }
          if (layer_sw == 1) {
            if (button_cc_latch[j] == 255) {
              usbMIDI.sendControlChange(button_cc[1][j], 1, channel);
              digitalWrite(led_pins[j], 0);
            }
            if (button_cc_latch[j] != 255) {
              button_cc_latch[j] = !button_cc_latch[j];
              usbMIDI.sendControlChange(button_cc[1][j], button_cc_latch[j], channel);
              digitalWrite(led_pins[j], !button_cc_latch[j]);
            }
          }
          //midi_butt[j] = 1;
          //Serial.print(j);
          //Serial.println(" -bon");
        }

        if (pbread[j] == 1 && bread[j] == 0) {
          if (layer_sw == 0) {
            usbMIDI.sendControlChange(button_cc[0][j], 0, channel);
            digitalWrite(led_pins[j], 1);
          }
          if (layer_sw == 1) {
            if (button_cc_latch[j] == 255) {
              usbMIDI.sendControlChange(button_cc[1][j], 0, channel);
              digitalWrite(led_pins[j], 1);
            }
          }
          //digitalWrite(led_pins[j], 0);
          // midi_butt[j] = 0;
        }
      }
    }
    /*
        for (byte j = 0; j < 2; j++) {
          pswread[j] = swread[j];
          swread[j] = digitalRead(switch_pins[j]);

          if (pswread[j] == 0 && swread[j] == 1) {
            midi_butt[j + 8] = 1;
            //   Serial.print(j);
            //  Serial.println(" -sw on");

          }

          if (pswread[j] == 1 && swread[j] == 0) {
            //    Serial.print(j);
            //    Serial.println(" -sw off");
            midi_butt[j + 8] = 0;
          }
        }
    */
    for (byte i = 0; i < 8; i++) {
      byte ccc = 0;

      if (cread[i] < pread[i]) {
        midi_enc[layer_sw][i]-=enc_step;
        ccc = 1;
        if (midi_enc[layer_sw][i] < 0) {
          midi_enc[layer_sw][i] = 0;
        }
        //Serial.print(i);        Serial.print("- ");        Serial.println(midi_enc[layer_sw][i]);
      }
      if (cread[i] > pread[i]) {
        midi_enc[layer_sw][i]+=enc_step;
        ccc = 1;

        if (midi_enc[layer_sw][i] > 127) {
          midi_enc[layer_sw][i] = 127;
        }

        //  Serial.print(i);        Serial.print("- ");        Serial.println(midi_enc[layer_sw][i]);
      }
      if (ccc == 1) {
        usbMIDI.sendControlChange(enc_cc[layer_sw][i], midi_enc[layer_sw][i], channel);

      }
    }


  }

  if (ct - pt[3] > 200 && 1 == 0) {
    pt[3] = ct;
    //swread[0] = digitalRead(switch_pins[0]);
    ///swread[1] = digitalRead(switch_pins[1]);
    /*
      Serial.print("clutch- ");
      Serial.println(clutch_sw);
      Serial.print("layer- ");
      Serial.println(layer_sw);
      Serial.println();
    */
    Serial.println(sync_c);

  }

  if (ct - pt[2] > 20) {
    pt[2] = ct;

    if (osc_latch[0] == 1) {
      osc[0] *= 1.03;
    }
    if (osc_latch[0] == 0) {
      osc[0] *= .98;
    }

    if (osc[0] > 100) {
      osc_latch[0] = 0;
    }
    if (osc[0] < 10) {
      osc[0] = 10;
      osc_latch[0] = 1;
    }
    analogWrite(30, osc[0]*sync_led);
  }



  if (usbMIDI.read()) {
    byte t = usbMIDI.getType();
    byte d1 = usbMIDI.getData1();
    byte d2 = usbMIDI.getData2();

    if (1 == 0) {
      Serial.println("M " );
      Serial.println(t);
      Serial.println(d1);
      Serial.println(d2);
      Serial.println();
    }

    if (t == 176) {
      if (d1 == ccin[0]) {
        ccin_read[0] = d2;
      }
      if (d1 == ccin[1]) {
        ccin_read[1] = d2;
      }
    }
    if (t == 250) { // start
      sync_c = 0;
    }

    if (t == 252) { //stop
      sync_led = 1;
      sync_c = 0;
    }

    if (t == 248) {
      // if (d1 == 76) {}
      sync_led = 1;
      sync_c++;
      if (sync_c > (24) - 1) { //24ppq
        sync_c = 0;
      }
      if (sync_c < 2) {
        sync_led = .3;
      }

    }
  }


  if (ct - pt[4] > 40) {
    pt[4] = ct;
    static byte sat = 230;
    static byte bright = 80;

    for (byte h = 0; h < 2; h++) {

      if (ccin_read[h] == 0) {
        apaleds[h].setHSV(0, 0, 0);
      }
      if (ccin_read[h] == 1) {//red
        apaleds[h].setHSV(0, sat, bright);
      }
      if (ccin_read[h] == 2) {//blue
        apaleds[h].setHSV(156, sat, bright * 1.5);
      }
      if (ccin_read[h] == 3) {//green
        apaleds[h].setHSV(100, sat, bright);
      }
      if (ccin_read[h] == 4) {//white
        apaleds[h].setRGB(35 * (bright / 90.00), 50 * (bright / 90.00), 50 * (bright / 90.00));
      }
      if (ccin_read[h] > 4) { //purple
        apaleds[h].setHSV(190,  sat, bright);
      }
    }

    FastLED.show();

  }


}
