
#include <Arduino.h>
#include "md_defines.h"

#include "md_buzzer.h"

#ifdef USE_SONGTASK
  TaskHandle_t songTask;
#endif

#define PLAYER_IS_FREE false
#define OCUPIED        true

bool      _isfree;    // no song loaded
tone_t   *_psong;     // first note of song
uint16_t  _len;       // note count
double    _beatfac;       // OFF base beat = 1/4 length


#ifdef USE_SONGTASK
  void playSong(void * pvParameters)
    {
      note_t   _note = (note_t) PAUSE;     // NOTE_C .. NOTE_B
      int8_t   _octa = 0;     // oktave 0 .. 7
      uint64_t _beat = 0;     // MB1 = base
      while(true)  // endless loop
        {
          if (!_isfree)
            {
                  #if (DEBUG_MODE >= CFG_DEBUG_ACTIONS)
                    Serial.print(millis());
                    Serial.println(" playSong .. ");
                    Serial.println("for ii ");
                  #endif
              for (uint16_t ii = 0; ii < _len; ii++)
                {
                   _note = (note_t) _psong[ii].note;
                   _beat = _psong[ii].beat;
                   _octa = _psong[ii].octa;

                  if (_note == PAUSE)
                    {
                            #if (DEBUG_MODE >= CFG_DEBUG_DETAILS)
                              Serial.print(millis());
                              Serial.print(" ->PAUSE "); Serial.println(ii);
                            #endif
                      ledcWriteTone(PWM_BUZZ, 0.0);
                    }
                  else
                    {
                            #if (DEBUG_MODE >= CFG_DEBUG_DETAILS)
                              Serial.print(millis());
                              Serial.print(" ->NOTE "); Serial.print(_note);
                              Serial.print(" ->OCTA "); Serial.println(_octa);
                            #endif
                      ledcWriteNote(PWM_BUZZ, _note, _octa);
                    }
                  usleep( (uint64_t) ((_beat * MUSIC_RATIO_P100 /100) * _beatfac) );          // play tone
                  ledcWriteTone(PWM_BUZZ, 0.0);  // switch off
                  usleep( (uint64_t) ((_beat * (100 - MUSIC_RATIO_P100) / 100) * _beatfac) ); // pause
                }
              _isfree = true;
            }
          sleep(1);
        }
    }

  void startSongTask()
    {
      Serial.println("startSongTask");
      xTaskCreatePinnedToCore(
                          playSong,   /* Task function. */
                          "Task1",     /* name of task. */
                          10000,       /* Stack size of task */
                          NULL,        /* parameter of the task */
                          4 | portPRIVILEGE_BIT, /* priority of the task */
                          &songTask,      /* Task handle to keep track of created task */
                          1              );          /* pin task to core 0 */
    }

#else
  void md_buzzer::playSong()
    {
      if (!_isfree)
        {
              #if (DEBUG_MODE >= CFG_DEBUG_ACTIONS)
                Serial.print(millis());
                Serial.println(" playSong .. ");
                Serial.println("for ii ");
              #endif
          for (uint16_t ii = 0; ii < _len; ii++)
            {
               _note = (note_t) _psong[ii].note;
               _beat = _psong[ii].beat;
               _octa = _psong[ii].octa;
               playTone();
            }
          _isfree = true;
        }
    }
#endif // USE_SONGTASK

void md_buzzer::playDingDong(uint8_t _anz)
  {
        #if (DEBUG_MODE >= CFG_DEBUG_ACTIONS)
          Serial.print(millis());
          Serial.println(" playingDong .. ");
          Serial.println("for ii ");
        #endif
    _beat = MB4;
    _octa = OP0;
    _beatfac = 1;
    for (uint16_t ii = 0; ii < _anz; ii++)
      {
        _note = (note_t) NOTE_G;
        playTone();
        _note = (note_t) NOTE_E;
        playTone();
      }
  }

void md_buzzer::playTone()
  {
    if (_note == PAUSE)
      {
              #if (DEBUG_MODE >= CFG_DEBUG_DETAILS)
                Serial.print(millis());
                Serial.print(" ->PAUSE "); Serial.println(ii);
              #endif
        ledcWriteTone(_pwmChan, 0.0);
      }
    else
      {
              #if (DEBUG_MODE >= CFG_DEBUG_DETAILS)
                Serial.print(millis());
                Serial.print(" ->NOTE "); Serial.print(_note);
                Serial.print(" ->OCTA "); Serial.println(_octa);
              #endif
        ledcWriteNote(_pwmChan, _note, _octa);
      }
    usleep( (uint64_t) ((_beat * MUSIC_RATIO_P100 / 100) * _beatfac) );          // play tone
    ledcWriteTone(_pwmChan, 0.0);  // switch off
    usleep( (uint64_t) ((_beat * (100 - MUSIC_RATIO_P100) / 100) * _beatfac) ); // pause
  }

void md_buzzer::initMusic(uint8_t buzzPin, uint8_t pwmChan)
  {
    _buzzPin = buzzPin;
    _pwmChan = pwmChan;
    ledcAttachPin(buzzPin, pwmChan);
    Serial.print(millis());
    Serial.println(" initMusic .. ");
    #ifdef USE_SONGTASK
      startSongTask();
    #endif
    Serial.print(millis());
    Serial.println(" .. initMusic finished");
    _isfree = true;
    _psong  = NULL;
    usleep(500000);
  }

bool md_buzzer::setSong(int16_t anzNotes, void* pnote0)
  {
    return setSong(anzNotes, pnote0, MUSIC_BASEBEAT_US);
  }

bool md_buzzer::setSong(int16_t anzNotes, void* pnote0, uint64_t beat_us = MUSIC_BASEBEAT_US)
  {
    //tone_t  song[anzNotes];
            Serial.print(millis());
            Serial.print(" setSong anz = "); Serial.print(anzNotes);
    if (_isfree)
    {
      _psong   = (tone_t*) pnote0;
      _len     = anzNotes;
      _isfree  = false;
      _beatfac = beat_us / MUSIC_BASEBEAT_US;
            Serial.println(" ok");
      return ISOK;
    }
    else
    {
            Serial.println(" ERR Occupied");
    }
    return ISERR;
  }

