#if !defined HOLDING
	#define HOLDING(%0) 	((newkeys & (%0)) == (%0))
#endif

#if !defined PRESSED
	#define PRESSED(%0) (((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))
#endif

#if !defined RELEASED
	#define RELEASED(%0) ((oldkeys & (%0)) && !(newkeys & (%0)))
#endif

#if !defined KEY_AIM
	#define KEY_AIM 128
#endif

#if !defined IsValidVehicle
	native IsValidVehicle(vehicleid);
#endif