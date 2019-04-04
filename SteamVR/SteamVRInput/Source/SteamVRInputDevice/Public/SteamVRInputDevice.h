/*
Copyright 2019 Valve Corporation under https://opensource.org/licenses/BSD-3-Clause

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/


#pragma once

#include "IInputDevice.h"
#include "Core.h"
#include "IMotionController.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformProcess.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "SteamVRInputTypes.h"
#include "SteamVRKnucklesKeys.h"
#include "SteamVRInputPublic.h"

class STEAMVRINPUTDEVICE_API FSteamVRInputDevice : public IInputDevice, public IMotionController
{
public:
	FSteamVRInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler);

	~FSteamVRInputDevice();

	// IInput Device Interface
	virtual void Tick(float DeltaTime) override;
	virtual void SendControllerEvents() override;
	virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values) override;
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	// End of IInput Device Interface

	// IMotionController Interface
	virtual bool GetControllerOrientationAndPosition(const int32 ControllerIndex, const FName MotionSource, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale) const;
	virtual ETrackingStatus GetControllerTrackingStatus(const int32 ControllerIndex, const FName MotionSource) const;
	virtual void EnumerateSources(TArray<FMotionControllerSource>& SourcesOut) const;
	virtual float GetCustomParameterValue(const FName MotionSource, FName ParameterName, bool& bValueFound) const { bValueFound = false;  return 0.f; }
	virtual FName GetMotionControllerDeviceTypeName() const override;
	// End IMotionController interface

	// End of IMotionController Interface

	/** Initialize the SteamVR System. Will cause a reconnect if one is already active  */
	void InitSteamVRSystem();

	/**
	* Retrieve the skeletal input from SteamVR
	* @param bLeftHand - Whether or not retrieve values for the Left Hand instead of the Right Hand
	* @param bMirror - Will mirror the pose read from SteamVR to fit the skeleton of the opposite hand
	* @param MotionRange - Whether to retrieve skeletal anim values with or without controllers
	* @param OutBoneTransform - The transform for each bone as defined in the SteamVR Skeleton
	* @param OutBoneTransformCount - The number of bones in the SteamVR Skeleton as reported by the SteamVR Input system
	* @return Whether or not we successfully retrieved the transform values
	*/
	bool GetSkeletalData(bool bLeftHand, bool bMirror, EVRSkeletalMotionRange MotionRange, FTransform* OutBoneTransform, int32 OutBoneTransformCount);

	/**
	* Retrieve the left hand pose information - position, orientation and velocities
	* @return Position - Translation from the pose data matrix in UE coordinates
	* @return Orientation - Orientation derived from the pose data matrix in UE coordinates
	* @return AngularVelocity - The angular velocity of the hand this frame
	* @return Velocity - The velocity of the hand this frame
	*/
	void GetLeftHandPoseData(FVector& Position, FRotator& Orientation, FVector& AngularVelocity, FVector& Velocity); 

	/**
	* Retrieve the right hand pose information - position, orientation and velocities
	* @return Position - Translation from the pose data matrix in UE coordinates
	* @return Orientation - Orientation derived from the pose data matrix in UE coordinates
	* @return AngularVelocity - The angular velocity of the hand this frame
	* @return Velocity - The velocity of the hand this frame
	*/
	void GetRightHandPoseData(FVector& Position, FRotator& Orientation, FVector& AngularVelocity, FVector& Velocity);
	
	/**
	* Calculate the UE equivalent orientation and position from a SteamVR PoseData matrix 
	* @param PoseData - The pose data returned by SteamVR
	* @return OutPosition - Translation from the pose data
	* @return OutOrientation - Orientation from the pose data
	*/
	void GetUETransform(InputPoseActionData_t PoseData, FVector& OutPosition, FRotator& OutOrientation);

	/** Retrieve skeletal tracking level for all controllers */
	void GetControllerFidelity();

#if WITH_EDITOR
	/** Have the action manifest regenerated. Used by the plugin Editor UI */
	void RegenerateActionManifest();

	/** Have the controller bindings regenerated. Used by the plugin Editor UI  */
	void RegenerateControllerBindings();

	/** Reload the action manifest to the SteamVR system. Use if you changed the action manifest manually */
	void ReloadActionManifest();
#endif

	/** The SteamVR System Handler, will be valid if there's an active SteamVR session */
	IVRSystem* SteamVRSystem = nullptr;

	/** Whether or not Curls and Splay values for the LEFT HAND are fed to the game every frame */
	bool bCurlsAndSplaysEnabled_L = true;

	/** Whether or not Curls and Splay values for the RIGHT HAND are fed to the game every frame */
	bool bCurlsAndSplaysEnabled_R = true;

	/** @deprecated Number of controllers that are currently active in a session */
	int32 NumControllersMapped;

	/** @deprecated Number of trackers that are currently active in a session */
	int32 NumTrackersMapped;

	/** @deprecated Mapping between SteamVR Legacy Device Ids and UE4s controller mappings */
	int32 DeviceToControllerMap[k_unMaxTrackedDeviceCount];

	/** @deprecated Mapping of UE4 devices, hand they are associated with (including "special" for trackers) and legacy SteamVR device Ids */
	int32 UnrealControllerIdAndHandToDeviceIdMap[SteamVRInputDeviceConstants::MaxUnrealControllers][k_unMaxTrackedDeviceCount];

	/** @deprecated Maximum number of allowed active controllers available to a player */
	int32 MaxUEHandCount[CONTROLLERS_PER_PLAYER];

	/** Holds the current frame values of a controller, including any SteamVR summary skeletal values */
	struct FInputDeviceState
	{
		EControllerHand Hand;	// The hand this controller is associated with (left or right)
		uint32 PacketNum;		// The unique packet number the values in this struct relate to

		float TouchPadXAnalog;	// X Axis values of the touchpad
		float TouchPadYAnalog;	// Y Axis values of the touchpad

		float TriggerAnalog;	// Float value of how much the trigger has been pressed

		float HandGripAnalog;	// Float value of how much the grip has been pressed
		float IndexGripAnalog;	// Float value of the index finger's curl
		float MiddleGripAnalog;	// Float value of the middle finger's curl
		float RingGripAnalog;	// Float value of the ring finger's curl
		float PinkyGripAnalog;	// Float value of the pinky finger's curl
		float ThumbGripAnalog;	// Float value of the thumb's curl

		float ThumbIndexSplayAnalog;	// Float value of the thumb-index splay
		float IndexMiddleSplayAnalog;	// Float value of the index-middle splay
		float MiddleRingSplayAnalog;	// Float value of the middle-ring splay
		float RingPinkySplayAnalog;		// Float value of the ring-pinky splay
	};

	/** Current input values of the active player controllers  */
	FInputDeviceState ControllerStates[SteamVRInputDeviceConstants::MaxControllers];

	/** Holds the actions that will be handled by the SteamVR Input Systems  */
	TArray<FSteamVRInputAction> Actions;

	/** A list of supported controller types that Controller Binding files will be generated for  */
	TArray<FControllerType> ControllerTypes;

	/** The handle to the action set that will be used in generating the Action Manifest and Controller Bindings  */
	VRActionSetHandle_t MainActionSet;

	/** The raw pose handle for the left controller  */
	VRActionHandle_t VRControllerHandleLeft;

	/** The raw pose handle for the right controller  */
	VRActionHandle_t VRControllerHandleRight;

	/** The tracker pose tagged as Special1 in UE  */
	VRActionHandle_t VRSpecial1;

	/** The tracker pose tagged as Special2 in UE  */
	VRActionHandle_t VRSpecial2;

	/** The tracker pose tagged as Special3 in UE  */
	VRActionHandle_t VRSpecial3;

	/** The tracker pose tagged as Special4 in UE  */
	VRActionHandle_t VRSpecial4;

	/** The tracker pose tagged as Special5 in UE  */
	VRActionHandle_t VRSpecial5;

	/** The tracker pose tagged as Special6 in UE  */
	VRActionHandle_t VRSpecial6;

	/** The tracker pose tagged as Special7 in UE  */
	VRActionHandle_t VRSpecial7;

	/** The tracker pose tagged as Special8 in UE  */
	VRActionHandle_t VRSpecial8;

	/** The handle for the skeletal input of the left hand  */
	VRActionHandle_t VRSkeletalHandleLeft;

	/** The handle for the skeletal input of the right hand  */
	VRActionHandle_t VRSkeletalHandleRight;

	/** Current skeletal summary input values of the left hand  */
	VRSkeletalSummaryData_t VRSkeletalSummaryDataLeft;

	/** Current skeletal summary input values of the right hand  */
	VRSkeletalSummaryData_t VRSkeletalSummaryDataRight;

	/** The last input error that was encountered. Used for preventing spamming the output log  */
	EVRInputError LastInputError = VRInputError_None;

	/** Delay that will be applied for buttons  */
	float InitialButtonRepeatDelay;

	/** Delay that will be applied to detect button press repeats  */
	float ButtonRepeatDelay;

	/** Mapping of buttons and controllers  */
	FGamepadKeyNames::Type Buttons[k_unMaxTrackedDeviceCount][ESteamVRInputButton::TotalButtonCount];

	/** The filename for this project  */
	FString GameFileName;

	/** The name of the project as defined in the Project Settings' Title field */
	FString GameProjectName;

	/** The version of this project as defined in the Project Settings' Version field  */
	FString GameProjectVersion;

	/** The unique app key used by SteamVR to distinguish an Editor Session  */
	FString EditorAppKey;

	/** Whether or not skeletal input is supported and present in the controller in the player's left hand  */
	bool bIsSkeletalControllerLeftPresent = false;

	/** Whether or not skeletal input is supported and present in the controller in the player's right hand  */
	bool bIsSkeletalControllerRightPresent = false;

	/** Whether to use the Skeleton Pose for the Orientation and Position of the motion controller  */
	bool bUseSkeletonPose = false;

	/** The skeletal tracking level for the controller in the player's left hand  */
	EVRSkeletalTrackingLevel LeftControllerFidelity;

	/** The skeletal tracking level for the controller in the player's right hand  */
	EVRSkeletalTrackingLevel RightControllerFidelity;

	/** Device identifier used to check for class validity */
	FString DeviceSignature;

private:
#if WITH_EDITOR
	/** 
	 * Generate the controller bindings for SteamVR supported controllers
	 * @param BindingsPath - Where the controller input profile json files will be generated (by default, this is Config\SteamVRBindings)
	 * @param InOutControllerTypes - Common SteamVR controller types for which the bindings will be generated (e.g. knuckles)
	 * @param InOutDefaultBindings - Meadata associated to each controller binding
	 * @param InActionsArray - The list of SteamVR actions that needs to be generated for each controller
	 * @param InInputMapping - The mapping of SteamVR actions and their controller inputs
	 * @param bDeleteIfExists - Flag of whether or not to overwrite an existing controller binding
	 */
	void GenerateControllerBindings(const FString& BindingsPath, TArray<FControllerType>& InOutControllerTypes, TArray<TSharedPtr<FJsonValue>>& InOutDefaultBindings, TArray<FSteamVRInputAction>& InActionsArray, TArray<FInputMapping>& InInputMapping, bool bDeleteIfExists = false);

	/**
	* Generate the SteamVR specific action bindings that will generated for a controller
	* @param InInputMapping - The mapping of SteamVR actions and their controller inputs
	* @param JsonValuesArray - The generated mappings in json format
	*/
	void GenerateActionBindings(TArray<FInputMapping> &InInputMapping, TArray<TSharedPtr<FJsonValue>> &JsonValuesArray);

	/** Delegate called when an action mapping has been modified in the editor  */
	FDelegateHandle ActionMappingsChangedHandle;
#endif

	/** Provides a user-friendly version of the results of a SteamVR Input call in English to the Output Log */
	void GetInputError(EVRInputError InputError, FString InputAction);

	/** @deprecated Initialize the SteamVR device Ids to UE Controller Id mappings */
	void InitControllerMappings();

	/** Setup the keys used by the Skeletal Input System  */
	void InitSkeletalControllerKeys();

	/** Grab the inputs from SteamVR and process them so the active game can consume the actions as needed  */
	void SendSkeletalInputEvents();

	/**
	* Create the action manifest used by the SteamVR Input System
	* @param GenerateActions - Whether to generate main actions from the project input settings
	* @param GenerateBindings - Whether to generate the controller bindings for all supported SteamVR controllers
	* @param RegisterApp - Whether to register currently running application as an Editor session
	* @param DeleteBindings - Whether to overwrite current controller bindings (if any)
	*/
	void GenerateActionManifest(bool GenerateActions=true, bool GenerateBindings=true, bool RegisterApp=true, bool DeleteBindings=false);

	/**
	* Create the application manifest for an Editor session
	* @param ManifestPath - Where the action manifest is located. By default this is under Config/SteamVRBindings
	* @param ProjectName - The name of the project from the project settings' description field
	* @param OutAppKey - The generated unique appkey for this application
	* @param OutAppManifestPath - Where the application manifest will be generated to. Default is under the project's Config folder
	* @return Whether or not the application manifest was generated successfully and the running Editor session has been properly registered to SteamVR
	*/
	bool GenerateAppManifest(FString ManifestPath, FString ProjectName, FString& OutAppKey, FString& OutAppManifestPath);

	/**
	* Utility function that builds a json object from a string of fields. Input strings should be paired (e.g fieldname1, fieldvalue1, fieldname2, fieldvalue2 ...)
	* @param StringFields - The paired strings to generate a json object for (e.g fieldname1, fieldvalue1, fieldname2, fieldvalue2 ...)
	* @param OutJsonObject - The resulting json object
	* @return Whether or not we generated a valid json object
	*/
	bool BuildJsonObject(TArray<FString> StringFields, TSharedRef<FJsonObject> OutJsonObject);

	/**
	* Convert UE4 style key-based action bindings to SteamVR/OpenXR format
	* @param InputSettings - The engine's input settings
	* @param InOutUniqueInputs - The input mappings that needs to be processed. Will also hold the processed mappings
	*/
	void ProcessKeyInputMappings(const UInputSettings* InputSettings, TArray<FName> &InOutUniqueInputs);

	/**
	* Convert UE4 style axis-based action bindings to SteamVR/OpenXR format
	* @param InputSettings - The engine's input settings
	* @param InOutUniqueInputs - The input mappings that needs to be processed. Will also hold the processed mappings
	*/
	void ProcessKeyAxisMappings(const UInputSettings* InputSettings, TArray<FName> &InOutUniqueInputs);

	/** Remove any invalid action to Axis mappings so they won#t be generated by the plugin */
	void SanitizeActions();

	/**
	* Registers an Editor session to the SteamVR system
	* @param ManifestPath - Path to the Application Manifest that will be generated. By default this will be under the Config folder
	*/
	void RegisterApplication(FString ManifestPath);

	/** 
	* Get the Skeletal Input Handle for a give controller input path
	* @param ActionPath - The SteamVR user path for the controller hand we want the skeletal handle for
	* @param SkeletalHandle - Will hold the skeletal handle if found. k_InvalidHandleValue if an error is encountered
	* @return Whether or not Whether the Skeletal Handle was successfully read
	*/	
	bool SetSkeletalHandle(char* ActionPath, VRActionHandle_t& SkeletalHandle);

	/**
	* Look for any Key Mappings defined in the project's DefaultInput.ini 
	* @param InputSettings - The engine's input settings
	* @param ActionName - The name of the action where we want to find controller inputs for
	* @param OutMappings - Will hold the Action to controller axis mapping result of the search
	*/
	void FindAxisMappings(const UInputSettings* InputSettings, const FName AxisName, TArray<FInputAxisKeyMapping>& OutMappings) const;

	/**
	* Look for any Key Mappings defined in the project's DefaultInput.ini 
	* @param InputSettings - The engine's input settings
	* @param ActionName - The name of the action where we want to find controller inputs for
	* @param OutMappings - Will hold the Action to controller key mapping result of the search
	*/
	void FindActionMappings(const UInputSettings* InputSettings, const FName ActionName, TArray<FInputActionKeyMapping>& OutMappings) const;

	/**
	* Send the axis value for the given controller input
	* @param TrackedControllerRole - The tracking type/role for this controller
	* @param AxisButton - The type of button the value is associated with
	* @param AnalogValue - The input value (float) that is being sent
	*/
	void SendAnalogMessage(const ETrackedControllerRole TrackedControllerRole, const FGamepadKeyNames::Type AxisButton, float AnalogValue);

	/**
	* Remove any unsupported special characters on a provide string
	* @param InOutString - The text to process
	* @return A copy of the processed text
	*/
	FString SanitizeString(FString& InOutString);

	/**
	* Transform the bone transforms for one hand to fit the skeleton of the opposite hand
	* @param BoneTransformsLS - The bone transforms to mirror, in local (parent) space
	* @param BoneTransformCount - The number of elements in the BoneTransformsLS array.  Must equal the number of bones in the SteamVR skeleton
	*/
	void MirrorSteamVRSkeleton(VRBoneTransform_t* BoneTransformsLS, int32 BoneTransformCount) const;

	/** Our Message handler to direct input from the SteamVRInput System to the game runtime */
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;

	/** Currently active SteamVRInput Error. Used to compare with the last error to avoid spamming the output log. */
	EVRInputError SteamVRError = VRInputError_None;

	/** Previous SteamVRInput Error. Used to compare against the current SteamVR Error to avoid spamming logs */
	EVRInputError PrevSteamVRError = VRInputError_None;

	/** Input Axis (Analog/Float) action mappings defined for this project. Available in DefaultInput.ini or via the Editor UI ProjectSettings > Engine > Input  */
	TArray<FInputAxisKeyMapping> KeyAxisMappings;

	/** Input Key (Digital/Boolean) action mappings defined for this project. Available in DefaultInput.ini or via the Editor UI ProjectSettings > Engine > Input  */
	TArray<FInputActionKeyMapping> KeyMappings;
};
