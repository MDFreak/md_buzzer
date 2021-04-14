#ifndef _MD_BUZZER_H_
  #define _MD_BUZZER_H_

  #include <Arduino.h>
  #include "md_defines.h"

  //#ifdef USE_BUZZER
    //
    // --- public prototypes
    //
    // --- class buzzer
      class md_buzzer
        {
          public:  // methods
            md_buzzer(){};
            void initMusic(uint8_t buzzPin, uint8_t pwmChan);
            bool setSong(int16_t anzNotes, void* pnote0);
            bool setSong(int16_t anzNotes, void* pnote0, uint64_t beat_us);
            void playDingDong(uint8_t anz = 1);
            #ifndef songTask
              void playSong();
            #endif
          protected: // deklarations
            void playTone();

            uint8_t  _pwmChan;
            uint8_t  _buzzPin;
            note_t   _note = (note_t) PAUSE;     // NOTE_C .. NOTE_B
            int8_t   _octa = 0;     // oktave 0 .. 7
            uint64_t _beat = 0;     // MB1 = base
        };
    //
//  #endif // USE_BUZZER
#endif // _MD_BUZZER_H_