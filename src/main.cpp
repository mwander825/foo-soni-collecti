#include "stdafx.h"

// Declaration of your component's version information
DECLARE_COMPONENT_VERSION("Soni Collecti","1.0","(C) 2019- Michael Wander\n\nOffline scrobbler of played tracks.\nOutputs logged track info to .csv. ");

// This will prevent users from renaming your component around (important for proper troubleshooter behaviors) or loading multiple instances of it.
VALIDATE_COMPONENT_FILENAME("foo_soni_collecti.dll");
