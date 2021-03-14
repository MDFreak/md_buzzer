#ifndef _MD_BUZZER_H_
  #define _MD_BUZZER_H_

  #include <Arduino.h>
  #include "config.h"

  #ifdef USE_BUZZER
    //
    // --- public prototypes
    //
    // --- class buzzer
      class md_buzzer
        {
          public:  // methods
            md_buzzer(){};
            void initMusic();
            bool setSong(int16_t anzNotes, void* pnote0);
            bool setSong(int16_t anzNotes, void* pnote0, uint64_t beat_us);
            #ifndef songTask
              void playSong();
            #endif
          protected: // deklarations
        };
    //
  #endif // USE_BUZZER
#endif // _MD_BUZZER_H_