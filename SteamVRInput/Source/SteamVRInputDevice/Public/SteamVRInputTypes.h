#pragma once

#include "openvr.h"
#include "GameFramework/InputSettings.h"
#include "InputCoreTypes.h"

using namespace vr;

#define CONTROLLERS_PER_PLAYER			2
#define GENERIC_TRACKER_PLAYER_NUM		0
#define TOUCHPAD_AXIS					0
#define TRIGGER_AXIS					1
#define KNUCKLES_TOTAL_HAND_GRIP_AXIS	2
#define KNUCKLES_UPPER_HAND_GRIP_AXIS	3
#define KNUCKLES_LOWER_HAND_GRIP_AXIS	4
#define DOT_45DEG						0.707f
#define TOUCHPAD_DEADZONE				0.0f

/* SteamVR Input System Constants */
#define CONTROLLER_BINDING_PATH			"SteamVRBindings"
#define ACTION_MANIFEST					"steamvr_manifest.json"
#define ACTION_MANIFEST_UE				"steamvr_actions.json"
#define ACTION_SET						"/actions/main"
#define ACTION_PATH_IN					"/actions/main/in"
#define ACTION_PATH_SKELETON_LEFT		"/actions/main/in/skeletonleft"
#define ACTION_PATH_SKELETON_RIGHT		"/actions/main/in/skeletonright"
#define ACTION_PATH_SKEL_HAND_LEFT		"/skeleton/hand/left"
#define ACTION_PATH_SKEL_HAND_RIGHT		"/skeleton/hand/right"
#define ACTION_PATH_OPEN_CONSOLE		"/actions/main/in/open_console"
#define ACTION_PATH_VIBRATE_LEFT		"/actions/main/out/vibrateleft"
#define ACTION_PATH_VIBRATE_RIGHT		"/actions/main/out/vibrateright"
#define ACTION_PATH_TRIGGER_LEFT		"/user/hand/left/input/trigger"
#define ACTION_PATH_TRIGGER_RIGHT		"/user/hand/right/input/trigger"
#define ACTION_PATH_THUMBSTICK_LEFT		"/user/hand/left/input/thumbstick"
#define ACTION_PATH_THUMBSTICK_RIGHT	"/user/hand/right/input/thumbstick"
#define ACTION_PATH_TRACKPAD_LEFT		"/user/hand/left/input/trackpad"
#define ACTION_PATH_TRACKPAD_RIGHT		"/user/hand/right/input/trackpad"
#define ACTION_PATH_GRIP_LEFT			"/user/hand/left/input/grip"
#define ACTION_PATH_GRIP_RIGHT			"/user/hand/right/input/grip"
#define ACTION_PATH_BTN_A_LEFT			"/user/hand/left/input/a"
#define ACTION_PATH_BTN_A_RIGHT			"/user/hand/right/input/a"
#define ACTION_PATH_BTN_B_LEFT			"/user/hand/left/input/b"
#define ACTION_PATH_BTN_B_RIGHT			"/user/hand/right/input/b"
#define ACTION_PATH_USER_SKEL_LEFT		"/user/hand/left/input/skeleton/left"
#define ACTION_PATH_USER_SKEL_RIGHT		"/user/hand/right/input/skeleton/right"
#define ACTION_PATH_USER_VIB_LEFT		"/user/hand/left/output/haptic"
#define ACTION_PATH_USER_VIB_RIGHT		"/user/hand/right/output/haptic"

namespace SteamVRInputDeviceConstants 
{
	/* The maximum number of Unreal controllers is 8, two motion controllers per Unreal controller */
	static const int32 MaxUnrealControllers = 4;

	/* Total number of supported tracked devices */
	static const int32 MaxControllers = k_unMaxTrackedDeviceCount;

	/* The maximum number of "Special" hand designations */
	static const int32 MaxSpecialDesignations = ((int32)EControllerHand::Special_9 - (int32)EControllerHand::Special_1) + 1;
}

/* Buttons available on a SteamVR controller */
struct ESteamVRInputButton
{
	enum Type
	{
		System = 0,
		ApplicationMenu = 1,
		Grip = 2,
		DPadLeft = 3,
		DPadUp = 4,
		DPadRight = 5,
		DPadDown = 6,
		ButtonA = 7,

		ProximitySensor = 31,

		Axis0 = 32,
		Axis1 = 33,
		Axis2 = 34,
		Axis3 = 35,
		Axis4 = 36,

		Touchpad = Axis0,
		Trigger =Axis1,
		DashboardBack =Grip,
		KnucklesA =Grip,
		KnucklesB =ApplicationMenu,
		KnucklesJoyStick =Axis3,
		TotalButtonCount = 64
	};
};

/* Available SteamVR Input Action Types */
enum EActionType
{
	Boolean,
	Vector1,
	Vector2,
	Vector3,
	Vibration,
	Pose,
	Skeleton,
	Invalid
};

const FString SActionTypes[] = {
			TEXT("boolean"),
			TEXT("vector1"),
			TEXT("vector2"),
			TEXT("vector3"),
			TEXT("vibration"),
			TEXT("pose"),
			TEXT("skeleton"),
			TEXT("")
};

struct FControllerType
{
	bool	bIsGenerated;
	FName	Name;
	FString	Description;

	FControllerType() {}
	FControllerType(const FName& inName, const FString& inDescription)
		: bIsGenerated(false)
		, Name(inName)
		, Description (inDescription)
	{}
};

struct FActionPath
{
	FString	Path;

	FActionPath() {}
	FActionPath(const FString& inPath)
		: Path(inPath)
	{}
};

struct FActionSource
{
	FName			Mode;
	FString			Path;

	FActionSource() {}
	FActionSource(const FName& inMode, const FString& inPath)
		: Mode(inMode),
		Path(inPath)
	{}
};

struct FInputMapping
{
	FName	InputKey;
	TArray<FString> Actions;

	FInputMapping() {}
};

struct FSteamVRInputAction
{
	FName		Name;
	FString		Path;
	EActionType	Type;
	FName		KeyX;
	FName		KeyY;
	FName		KeyZ;
	FVector		Value;
	FString		StringPath;
	bool		bState;
	bool		bRequirement;

	VRActionHandle_t Handle;
	EVRInputError LastError;

	FString GetActionTypeName()
	{
		return SActionTypes[(int)Type];
	}

	FSteamVRInputAction(const FString& inPath, EActionType inType, bool inRequirement, const FName& inName, const FString& inStringPath)
		: Path(inPath)
		, Name(inName)
		, Type(inType)
		, KeyX()
		, KeyY()
		, KeyZ()
		, Value()
		, StringPath(inStringPath)
		, bRequirement(inRequirement)
		, Handle()
		, LastError(VRInputError_None)
	{}

	FSteamVRInputAction(const FString& inPath, const FName& inName, const FName& inKeyName, bool inState)
		: Path(inPath)
		, Name(inName)
		, Type(Boolean)
		, KeyX(inKeyName)
		, KeyY()
		, KeyZ()
		, Value()
		, bState(inState)
		, bRequirement(false)
		, Handle()
		, LastError(VRInputError_None)
	{}

	FSteamVRInputAction(const FString& inPath, const FName& inName, const FName& inKeyName, float inValue1D)
		: Path(inPath)
		, Name(inName)
		, Type(Vector1)
		, KeyX(inKeyName)
		, KeyY()
		, KeyZ()
		, Value(inValue1D, 0.f, 0.f)
		, bRequirement(false)
		, Handle()
		, LastError(VRInputError_None)
	{}

	FSteamVRInputAction(const FString& inPath, const FName& inName, const FName& inKeyName_X, const FName& inKeyName_Y, const FVector2D& inValue2D)
		: Path(inPath)
		, Name(inName)
		, Type(Vector2)
		, KeyX(inKeyName_X)
		, KeyY(inKeyName_Y)
		, KeyZ()
		, Value(inValue2D.X, inValue2D.Y, 0.f)
		, bRequirement(false)
		, Handle()
		, LastError(VRInputError_None)
	{}

	FSteamVRInputAction(const FString& inPath, const FName& inName, const FName& inKeyName_X, const FName& inKeyName_Y, const FName& inKeyName_Z, const FVector& inValue3D)
		: Path(inPath)
		, Name(inName)
		, Type(Vector3)
		, KeyX(inKeyName_X)
		, KeyY(inKeyName_Y)
		, KeyZ(inKeyName_Z)
		, Value(inValue3D.X, inValue3D.Y, inValue3D.Z)
		, bRequirement(false)
		, Handle()
		, LastError(VRInputError_None)
	{}

	FSteamVRInputAction(const FString& inPath, const EActionType& inActionType, const bool& inRequirement, const FName& inName)
		: Path(inPath)
		, Name(inName)
		, Type(inActionType)
		, KeyX()
		, KeyY()
		, KeyZ()
		, Value()
		, bRequirement(inRequirement)
		, Handle()
		, LastError(VRInputError_None)
	{}

};

namespace KnucklesVRControllerKeyNames
{
	// Knuckles Finger Curls
	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_Index_Curl("Knuckles_Left_Finger_Index_Curl");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_Index_Curl("Knuckles_Right_Finger_Index_Curl");

	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_Middle_Curl("Knuckles_Left_Finger_Middle_Curl");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_Middle_Curl("Knuckles_Right_Finger_Middle_Curl");

	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_Ring_Curl("Knuckles_Left_Finger_Ring_Curl");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_Ring_Curl("Knuckles_Right_Finger_Ring_Curl");

	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_Pinky_Curl("Knuckles_Left_Finger_Pinky_Curl");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_Pinky_Curl("Knuckles_Right_Finger_Pinky_Curl");

	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_Thumb_Curl("Knuckles_Left_Finger_Thumb_Curl");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_Thumb_Curl("Knuckles_Right_Finger_Thumb_Curl");

	// Knuckles Finger Splays
	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_ThumbIndex_Splay("Knuckles_Left_Finger_ThumbIndex_Splay");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_ThumbIndex_Splay("Knuckles_Right_Finger_ThumbIndex_Splay");

	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_IndexMiddle_Splay("Knuckles_Left_Finger_IndexMiddle_Splay");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_IndexMiddle_Splay("Knuckles_Right_Finger_IndexMiddle_Splay");

	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_MiddleRing_Splay("Knuckles_Left_Finger_MiddleRing_Splay");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_MiddleRing_Splay("Knuckles_Right_Finger_MiddleRing_Splay");

	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_RingPinky_Splay("Knuckles_Left_Finger_RingPinky_Splay");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_RingPinky_Splay("Knuckles_Right_Finger_RingPinky_Splay");

}

namespace KnucklesVRControllerKeys
{
	// Knuckles CapSense
	const FKey SteamVR_Knuckles_Left_A_CapSense("Knuckles_Left_A_CapSense");
	const FKey SteamVR_Knuckles_Right_A_CapSense("Knuckles_Right_A_CapSense");
	const FKey SteamVR_Knuckles_Left_B_CapSense("Knuckles_Left_B_CapSense");
	const FKey SteamVR_Knuckles_Right_B_CapSense("Knuckles_Right_B_CapSense");

	const FKey SteamVR_Knuckles_Left_Trigger_CapSense("Knuckles_Left_Trigger_CapSense");
	const FKey SteamVR_Knuckles_Right_Trigger_CapSense("Knuckles_Right_Trigger_CapSense");

	const FKey SteamVR_Knuckles_Left_Thumbstick_CapSense("Knuckles_Left_Thumbstick_CapSense");
	const FKey SteamVR_Knuckles_Right_Thumbstick_CapSense("Knuckles_Right_Thumbstick_CapSense");

	const FKey SteamVR_Knuckles_Left_Trackpad_CapSense("Knuckles_Left_Trackpad_CapSense");
	const FKey SteamVR_Knuckles_Right_Trackpad_CapSense("Knuckles_Right_Trackpad_CapSense");

	const FKey SteamVR_Knuckles_Left_Trackpad_GripForce("Knuckles_Left_Trackpad_GripForce");
	const FKey SteamVR_Knuckles_Right_Trackpad_GripForce("Knuckles_Right_Trackpad_GripForce");

	// Knuckles Trackpad
	const FKey SteamVR_Knuckles_Left_Trackpad_X("Knuckles_Left_Trackpad_X");
	const FKey SteamVR_Knuckles_Right_Trackpad_X("Knuckles_Right_Trackpad_X");
	const FKey SteamVR_Knuckles_Left_Trackpad_Y("Knuckles_Left_Trackpad_Y");
	const FKey SteamVR_Knuckles_Right_Trackpad_Y("Knuckles_Right_Trackpad_Y");

	// Knuckles Curls
	const FKey SteamVR_Knuckles_Left_Finger_Index_Curl("Knuckles_Left_Finger_Index_Curl");
	const FKey SteamVR_Knuckles_Right_Finger_Index_Curl("Knuckles_Right_Finger_Index_Curl");

	const FKey SteamVR_Knuckles_Left_Finger_Middle_Curl("Knuckles_Left_Finger_Middle_Curl");
	const FKey SteamVR_Knuckles_Right_Finger_Middle_Curl("Knuckles_Right_Finger_Middle_Curl");

	const FKey SteamVR_Knuckles_Left_Finger_Ring_Curl("Knuckles_Left_Finger_Ring_Curl");
	const FKey SteamVR_Knuckles_Right_Finger_Ring_Curl("Knuckles_Right_Finger_Ring_Curl");

	const FKey SteamVR_Knuckles_Left_Finger_Pinky_Curl("Knuckles_Left_Finger_Pinky_Curl");
	const FKey SteamVR_Knuckles_Right_Finger_Pinky_Curl("Knuckles_Right_Finger_Pinky_Curl");

	const FKey SteamVR_Knuckles_Left_Finger_Thumb_Curl("Knuckles_Left_Finger_Thumb_Curl");
	const FKey SteamVR_Knuckles_Right_Finger_Thumb_Curl("Knuckles_Right_Finger_Thumb_Curl");

	// Knuckles Splays
	const FKey SteamVR_Knuckles_Left_Finger_ThumbIndex_Splay("Knuckles_Left_Finger_ThumbIndex_Splay");
	const FKey SteamVR_Knuckles_Right_Finger_ThumbIndex_Splay("Knuckles_Right_Finger_ThumbIndex_Splay");

	const FKey SteamVR_Knuckles_Left_Finger_IndexMiddle_Splay("Knuckles_Left_Finger_IndexMiddle_Splay");
	const FKey SteamVR_Knuckles_Right_Finger_IndexMiddle_Splay("Knuckles_Right_Finger_IndexMiddle_Splay");

	const FKey SteamVR_Knuckles_Left_Finger_MiddleRing_Splay("Knuckles_Left_Finger_MiddleRing_Splay");
	const FKey SteamVR_Knuckles_Right_Finger_MiddleRing_Splay("Knuckles_Right_Finger_MiddleRing_Splay");

	const FKey SteamVR_Knuckles_Left_Finger_RingPinky_Splay("Knuckles_Left_Finger_RingPinky_Splay");
	const FKey SteamVR_Knuckles_Right_Finger_RingPinky_Splay("Knuckles_Right_Finger_RingPinky_Splay");
}