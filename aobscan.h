#include<Windows.h>
#include<iostream>
#include <fstream>
#include<vector>
#include<time.h>

#include "driver.h"
#include "driverloader.h"

using namespace std;

#define BLOCKMAXSIZE 409600
BYTE* MemoryData;
short Next[260];