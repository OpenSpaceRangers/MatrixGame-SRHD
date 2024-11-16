#pragma once

#include "BaseDef.hpp"
#include "CException.hpp"
#include "CMain.hpp"
#include "CHeap.hpp"
#include "CStr.hpp"
#include "CWStr.hpp"
#include "CFile.hpp"
#include "CBuf.hpp"
#include "CBlockPar.hpp"
#include "Registry.hpp"
#include "CReminder.hpp"
#include "CDWORDMap.hpp"
#include "CRC32.hpp"
#include "CStorage.hpp"
#include "Pack.hpp"

#include "Tracer.hpp"

#define IS_UNICODE() (GetVersion()<0x80000000)
