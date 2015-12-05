// Main.cpp
#include "system.h"

void* operator new(size_t i)
  {
    return _mm_malloc(i, 16);
  }

  void operator delete(void* p)
  {
    _mm_free(p);
  }

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
  System* m_System;

  // Create the system object.
  m_System = new System();

  // Initialize and run the system object.
  m_System->Run();

  // Shutdown and release the system object.
  delete m_System;
  m_System = 0;

  return 0;
}