#ifdef __BORLANDC__
  #include "OS_StructuredExceptions.h"
#endif
#include "Standard_InitialiseFinalise.h"


Standard_InitialiseFinalise::Standard_InitialiseFinalise()
{
  #ifdef __BORLANDC__
    OS_StructuredException::SetExceptionHandler();
  #endif
}


Standard_InitialiseFinalise::~Standard_InitialiseFinalise()
{
  #ifdef __BORLANDC__
    OS_StructuredException::RevertExceptionHandler();
  #endif
}


void Standard_InitialiseFinalise::CaughtException()
{
}

