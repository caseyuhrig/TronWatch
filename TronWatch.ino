/**
 * Retro::Watch by Casey Uhrig.
 * The Simple clock display interface was originally
 * written by lewishe and used as a basis for the Retro
 * style this watch application uses.
 * */

#include "config.h"
#include "RetroWatch.hpp"

auto* watch = Retro::Watch::Create();

void setup()
{
  watch->Init();
}

void loop()
{
  watch->TaskHandler();
  if (watch->IsNextSecond())
  {
    watch->UpdateAll();
    watch->UpdateGUI();
  }
  watch->Delay();
}
